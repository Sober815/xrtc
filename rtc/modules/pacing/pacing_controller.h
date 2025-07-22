#ifndef XRTCSDK_XRTC_RTC_MODULES_PACING_PACING_CONTROLLER_H
#define XRTCSDK_XRTC_RTC_MODULES_PACING_PACING_CONTROLLER_H

#include <system_wrappers/include/clock.h>


#include "xrtc/rtc/modules/pacing/task_queue_paced_sender.h"


namespace xrtc {
class PacingController
{
public:
	PacingController(webrtc::Clock* clock);
	~PacingController();

	void EnqueuePacket(std::unique_ptr<RtpPacketToSend>packet);

private:
	webrtc::Clock* clock_;



};

}

#endif //XRTCSDK_XRTC_RTC_MODULES_PACING_PACING_CONTROLLER_H




