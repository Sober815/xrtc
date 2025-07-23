#include "xrtc/rtc/pc/rtp_transport_controller_send.h"

namespace xrtc {

RtpTransportControllerSend::RtpTransportControllerSend(webrtc::Clock* clock,
    webrtc::TaskQueueFactory* task_queue_factory) :
    clock_(clock),
    task_queue_pacer_(std::make_unique<TaskQueuePacedSender>(clock, task_queue_factory,
        webrtc::TimeDelta::Millis(1)))
{
    task_queue_pacer_->EnsureStarted();
}

RtpTransportControllerSend::~RtpTransportControllerSend() {
}

void RtpTransportControllerSend::EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet) {
    task_queue_pacer_->EnqueuePacket(std::move(packet));
}

} // namespace xrtc