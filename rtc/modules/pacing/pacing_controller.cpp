#include "xrtc/rtc/modules/pacing/pacing_controller.h"

#include "xrtc/rtc/modules/rtp_rtcp/rtp_packet_to_send.h"


namespace xrtc {
	namespace {
		//ֵԽС�����ȼ�Խ��
		const int kFirstPriority = 0;

		int GetPriorityForType(RtpPacketMediaType type)
		{
			switch (type) {
			case RtpPacketMediaType::kAudio:
				return kFirstPriority + 1;
			case RtpPacketMediaType::kRetransmission://�ش���
				return kFirstPriority + 2;
			case RtpPacketMediaType::kVideo:
			case RtpPacketMediaType::kForwardErrorCorrection:
				return kFirstPriority + 3;
			case RtpPacketMediaType::kPadding:
				return kFirstPriority + 4;
			}
		}
	}//namespace



	PacingController::PacingController(webrtc::Clock* clock) :
		clock_(clock)
	{

	}

	PacingController::~PacingController(webrtc::Clock* clock)
	{

	}



	void PacingController::EnqueuePacket(std::unique_ptr<RtpPacketToSend>packet)
	{
		//1���rtp packet�����ȼ�

		int priority = GetPriorityForType(*packet->packet_type());

		//2.����packet


	}
}

