#ifndef XRTCSDK_XRTC_RTC_MODULES_PACING_PACING_CONTROLLER_H
#define XRTCSDK_XRTC_RTC_MODULES_PACING_PACING_CONTROLLER_H

#include <system_wrappers/include/clock.h>
#include <api/units/data_rate.h>
#include <memory>
#include "xrtc/rtc/modules/rtp_rtcp/rtp_packet_to_send.h"
#include "xrtc/rtc/modules/pacing/round_robin_packet_queue.h"
#include "xrtc/rtc/modules/pacing/interval_budget.h"
namespace xrtc {

class PacingController {
public:
    PacingController(webrtc::Clock* clock);
    ~PacingController();

    void EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet);
    void ProcessPackets();
    webrtc::Timestamp NextSendTime();

private:
    void EnqueuePacketInternal(int priority,
        std::unique_ptr<RtpPacketToSend> packet);
    void UpdateBudgetWithElapsedTime(webrtc::TimeDelta elapsed_time);
    webrtc::TimeDelta UpdateTimeGetElapsed(webrtc::Timestamp now);
    std::unique_ptr<RtpPacketToSend> GetPendingPacket();
private:
    webrtc::Clock* clock_;
    RoundRobinPacketQueue packet_queue_;
    uint64_t packet_counter_ = 0;
    webrtc::Timestamp last_process_time_;
    webrtc::TimeDelta min_packet_limit_;

    IntervalBudget media_budget_;
    webrtc::DataRate pacing_bitrate_;
};

} // namespace xrtc

#endif // XRTCSDK_XRTC_RTC_MODULES_PACING_PACING_CONTROLLER_H