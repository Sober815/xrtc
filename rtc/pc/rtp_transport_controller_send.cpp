#include "xrtc/rtc/pc/rtp_transport_controller_send.h"




namespace xrtc {
    RtpTransportControllerSend::RtpTransportControllerSend(webrtc::Clock* clock,
        //PacingController::PacketSender* packet_sender,
        webrtc::TaskQueueFactory* task_queue_factory)
        : clock_(clock)
        {
            
        }


        RtpTransportControllerSend::~RtpTransportControllerSend()
        {
            
        }

        void RtpTransportControllerSend::EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet)
        {
            //task_queue_pacer_->EnqueuePacket(std::move(packet));
        }







}