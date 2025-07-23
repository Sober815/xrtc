#ifndef XRTCSDK_XRTC_RTC_PC_RTP_TRANSPORT_CONTROLLER_SEND_H_
#define XRTCSDK_XRTC_RTC_PC_RTP_TRANSPORT_CONTROLLER_SEND_H_

#include <system_wrappers/include/clock.h>

#include "xrtc/rtc/modules/rtp_rtcp/rtp_packet_to_send.h"
#include <xrtc/rtc/modules/pacing/task_queue_paced_sender.h>

namespace xrtc {

class RtpTransportControllerSend {
public:
    RtpTransportControllerSend(webrtc::Clock* clock,
        webrtc::TaskQueueFactory* task_queue_factory);
    ~RtpTransportControllerSend();

    void EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet);

private:
    webrtc::Clock* clock_;
    std::unique_ptr<TaskQueuePacedSender> task_queue_pacer_;
};

} // namespace xrtc

#endif // XRTCSDK_XRTC_RTC_PC_RTP_TRANSPORT_CONTROLLER_SEND_H_