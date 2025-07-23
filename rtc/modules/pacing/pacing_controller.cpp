#include "xrtc/rtc/modules/pacing/pacing_controller.h"

#include <rtc_base/logging.h>

namespace xrtc {

namespace {

const webrtc::TimeDelta kDefaultMinPacketLimit = webrtc::TimeDelta::Millis(5);

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

} // namespace xrtc