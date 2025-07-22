#ifndef XRTCSDK_XRTC_RTC_PC_TASK_QUEUE_PACED_SENDER_H_
#define XRTCSDK_XRTC_RTC_PC_TASK_QUEUE_PACED_SENDER_H_


#include <system_wrappers/include/clock.h>
#include <api/task_queue/task_queue_factory.h>
#include <rtc_base/task_queue.h>
#include <xrtc/rtc/modules/pacing/pacing_controller.h>





namespace xrtc {
    class TaskQueuePacedSender {
    public:
        TaskQueuePacedSender(webrtc::Clock* clock,
            webrtc::TaskQueueFactory* task_queue_factory);

        ~TaskQueuePacedSender();

        void EnsureStarted();

        void EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet);

        void MaybeProcessPackets(webrtc::Timestamp scheduled_process_time);

    private:
        webrtc::Clock* clock_;
        rtc::TaskQueue task_queue_;
        PacingController pacing_controller_;
        //// 初始值是负的无穷大，表示暂时还没有调度任何任务
        webrtc::Timestamp next_process_time_ = webrtc::Timestamp::MinusInfinity();
        // // 最小的调度周期
        webrtc::TimeDelta hold_back_window_;





    };
}








#endif