#include "xrtc/rtc/modules/pacing/pacing_controller.h"

#include <rtc_base/logging.h>

namespace xrtc {

namespace {

const webrtc::TimeDelta kDefaultMinPacketLimit = webrtc::TimeDelta::Millis(5);
const webrtc::TimeDelta kMaxElapsedTime = webrtc::TimeDelta::Seconds(2);//流逝时间
const webrtc::TimeDelta kMaxProcessingInterval = webrtc::TimeDelta::Millis(30);//设置一个最大的处理间隔时间限制
// 值越小，优先级越高
const int kFirstPriority = 0;

int GetPriorityForType(RtpPacketMediaType type) {
    switch (type) {
    case RtpPacketMediaType::kAudio:
        return kFirstPriority + 1;
    case RtpPacketMediaType::kRetransmission:
        return kFirstPriority + 2;
    case RtpPacketMediaType::kVideo:
    case RtpPacketMediaType::kForwardErrorCorrection:
        return kFirstPriority + 3;
    case RtpPacketMediaType::kPadding:
        return kFirstPriority + 4;
    }
}

} // namespace

PacingController::PacingController(webrtc::Clock* clock) :
    clock_(clock),
    last_process_time_(clock_->CurrentTime()),
    min_packet_limit_(kDefaultMinPacketLimit),
    media_budget_(0),
    pacing_bitrate_(webrtc::DataRate::Zero())
{
}

PacingController::~PacingController() {
}

void PacingController::EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet) {
    // 1. 获得RTP packet的优先级
    int priority = GetPriorityForType(*packet->packet_type());
    // 2. 插入packet
    EnqueuePacketInternal(priority, std::move(packet));
}

void PacingController::ProcessPackets() {
    webrtc::Timestamp now = clock_->CurrentTime();
    last_process_time_ = now;

    RTC_LOG(LS_WARNING) << "========ProcessPackets()";

    //计算流逝的时间(当前时间距离上一次发送过去了多长时间）
    webrtc::TimeDelta elapsed_time = UpdateTimeGetElapsed(now);

    if (elapsed_time > webrtc::TimeDelta::Zero())
    {
        //更新预算
        media_budget_.set_target_bitrate_bps(pacing_bitrate_.kbps());

        UpdateBudgetWithElapsedTime(elapsed_time);
    }

    while (true)
    {
        //从队列中获取rtp数据进行发送
        std::unique_ptr<RtpPacketToSend>rtp_packet = GetPendingPacket();
    }


}

webrtc::Timestamp PacingController::NextSendTime() {
    return last_process_time_ + min_packet_limit_;
}

void PacingController::EnqueuePacketInternal(int priority, 
    std::unique_ptr<RtpPacketToSend> packet) 
{
    webrtc::Timestamp now = clock_->CurrentTime();
    packet_queue_.Push(priority, now, packet_counter_++, std::move(packet));
}

void PacingController::UpdateBudgetWithElapsedTime(webrtc::TimeDelta elapsed_time)
{
    webrtc::TimeDelta delta = std::min(elapsed_time, kMaxProcessingInterval);
    media_budget_.IncreaseBudget(delta.ms());
}

webrtc::TimeDelta PacingController::UpdateTimeGetElapsed(webrtc::Timestamp now)
{
    if (now < last_process_time_)
    {
        return webrtc::TimeDelta::Zero();
    }

    webrtc::TimeDelta elapsed_time = now - last_process_time_;

    last_process_time_ = now;

    if (elapsed_time > kMaxElapsedTime)
    {
        elapsed_time = kMaxElapsedTime;
        RTC_LOG(LS_WARNING) << "elapsed_time " << elapsed_time.ms()
            << " is longer than the kMaxElapsedTime, limitting to " << kMaxElapsedTime.ms();
    }
    return elapsed_time;

}

std::unique_ptr<RtpPacketToSend> PacingController::GetPendingPacket()
{
    //如果队列为空
    if (packet_queue_.Empty())
    {
        return nullptr;
    }
   //如果本轮预算已经耗尽
    if (media_budget_.bytes_remaining())
    {
        return nullptr;
    }

    return packet_queue_.Pop();
    
    
}

} // namespace xrtc