#include "task_queue_paced_sender.h"
#include "task_queue_paced_sender.h"
#include "xrtc/rtc/modules/pacing/task_queue_paced_sender.h"

namespace xrtc {

    TaskQueuePacedSender::TaskQueuePacedSender(webrtc::Clock* clock,
        webrtc::TaskQueueFactory* task_queue_factory
        )
        : clock_(clock)
        , task_queue_(task_queue_factory->CreateTaskQueue(
            "TaskQueuePacedSender",
            webrtc::TaskQueue::Priority::NORMAL))

        {

        }


        TaskQueuePacedSender::~TaskQueuePacedSender()
        {

        }

     
   

        /*
        调用线程                    任务队列线程
            |                           |
        EnqueuePacket() ────PostTask───→ lambda 执行
            |                           |
        返回 (不阻塞)                    ├→ pacing_controller_.EnqueuePacket()
                                        |
                                        └→ 数据包被处理
        
        */
        // webrtc::Timestamp::MinusInfinity()指示此调用尚未被pacing_controller调度
        void TaskQueuePacedSender::EnsureStarted()
        {
            task_queue_.PostTask([this]() {
                MaybeProcessPackets(webrtc::Timestamp::MinusInfinity());
                });
        }

        void TaskQueuePacedSender::EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet)
        {
            //将 packet 的所有权转移给 lambda 内的 packet_
            task_queue_.PostTask([this, packet_ = std::move(packet)]() {
                pacing_controller_.EnqueuePacket(std::move(packet_));
            });
        }
        // 检查是不是达到发送数据包的时间，如果不是，则调度一个延迟任务
        void TaskQueuePacedSender::MaybeProcessPackets(webrtc::Timestamp scheduled_process_time)
        {
            webrtc::Timestamp next_process_time = pacing_controller_.NextSendTime();
            // 如果传入的时间等于计划要调度的时间，立即调度任务
            bool is_scheduled_call = (next_process_time_ == scheduled_process_time);
            if (is_scheduled_call) {
                // 上一个计划准备执行，下一个计划待创建
                next_process_time_ = webrtc::Timestamp::MinusInfinity();
                pacing_controller_.ProcessPackets();
                next_process_time = pacing_controller_.NextSendTime();
            }
            webrtc::Timestamp now = clock_->CurrentTime();

            absl::optional<webrtc::TimeDelta> time_to_next_process;
            // 已经没有下一个任务，需要创建一个新任务
            if (next_process_time_.IsMinusInfinity()) {
                time_to_next_process = std::max(next_process_time - now,
                    hold_back_window_);
            }
            if (time_to_next_process) {
                next_process_time_ = next_process_time;
                task_queue_.PostDelayedTask([this, next_process_time]() {
                    MaybeProcessPackets(next_process_time);
                    }, time_to_next_process->ms<uint32_t>());
            }
            
        }

     














