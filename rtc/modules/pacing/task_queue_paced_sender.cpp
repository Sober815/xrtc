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
        �����߳�                    ��������߳�
            |                           |
        EnqueuePacket() ��������PostTask�������� lambda ִ��
            |                           |
        ���� (������)                    ���� pacing_controller_.EnqueuePacket()
                                        |
                                        ���� ���ݰ�������
        
        */
        // webrtc::Timestamp::MinusInfinity()ָʾ�˵�����δ��pacing_controller����
        void TaskQueuePacedSender::EnsureStarted()
        {
            task_queue_.PostTask([this]() {
                MaybeProcessPackets(webrtc::Timestamp::MinusInfinity());
                });
        }

        void TaskQueuePacedSender::EnqueuePacket(std::unique_ptr<RtpPacketToSend> packet)
        {
            //�� packet ������Ȩת�Ƹ� lambda �ڵ� packet_
            task_queue_.PostTask([this, packet_ = std::move(packet)]() {
                pacing_controller_.EnqueuePacket(std::move(packet_));
            });
        }
        // ����ǲ��Ǵﵽ�������ݰ���ʱ�䣬������ǣ������һ���ӳ�����
        void TaskQueuePacedSender::MaybeProcessPackets(webrtc::Timestamp scheduled_process_time)
        {
            webrtc::Timestamp next_process_time = pacing_controller_.NextSendTime();
            // ��������ʱ����ڼƻ�Ҫ���ȵ�ʱ�䣬������������
            bool is_scheduled_call = (next_process_time_ == scheduled_process_time);
            if (is_scheduled_call) {
                // ��һ���ƻ�׼��ִ�У���һ���ƻ�������
                next_process_time_ = webrtc::Timestamp::MinusInfinity();
                pacing_controller_.ProcessPackets();
                next_process_time = pacing_controller_.NextSendTime();
            }
            webrtc::Timestamp now = clock_->CurrentTime();

            absl::optional<webrtc::TimeDelta> time_to_next_process;
            // �Ѿ�û����һ��������Ҫ����һ��������
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

     














