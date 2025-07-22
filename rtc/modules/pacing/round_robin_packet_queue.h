#ifndef XRTCSDK_RTC_MODULES_PACING_ROUND_ROBIN_PACKET_QUEUE_H_
#define XRTCSDK_RTC_MODULES_PACING_ROUND_ROBIN_PACKET_QUEUE_H_

#include <api/units/timestamp.h>
#include <api/units/data_size.h>
#include <queue>
#include <map>
#include <unordered_map>
#include <xrtc/rtc/modules/rtp_rtcp/rtp_packet_to_send.h>




namespace xrtc {
	class RoundRobinPacketQueue {
	public:
		RoundRobinPacketQueue();
		~RoundRobinPacketQueue();

		void Push(int priority,
			webrtc::Timestamp enqueue_time,
			uint64_t enquue_order,
			std::unique_ptr<RtpPacketToSend>packet);


	private:
		struct QueuedPacket {
		public:
			QueuedPacket(int priority,
				webrtc::Timestamp enqueue_time,
				uint64_t enqueue_order,
				std::unique_ptr<RtpPacketToSend> packet);
			~QueuedPacket();

			bool operator< (const QueuedPacket& other)const {
				if (priority_ != other.priority_)
				{
					return priority_ > other.priority_;
				}
				return enqueue_order_ > other.enqueue_order_;
			}

			int Priority() const
			{
				return priority_;
			}
			uint32_t Ssrc()const { return owned_packet_->ssrc(); }
		private:
			int priority_;
			webrtc::Timestamp enqueue_time_; // 入队列时间
			uint64_t enqueue_order_; // 入队列顺序
			RtpPacketToSend* owned_packet_; //原始数据包
		};
		struct StreamPrioKey {
			

			StreamPrioKey(int priority, webrtc::DataSize size) :
				priority(priority),
				size(size) {
			}
			bool operator<(const StreamPrioKey& other) const {
				if (priority != other.priority) {
					return priority < other.priority;
				}
				return size < other.size;
			}
			int priority;
			webrtc::DataSize size;
		};
		struct Stream {
			Stream();
			~Stream();

			webrtc::DataSize size;
			uint32_t ssrc;
			std::priority_queue<QueuedPacket> packet_queue;
			std::multimap<StreamPrioKey, uint32_t>::iterator priority_it_;
		};
	private:
		void Push(const QueuedPacket& packet);
	private:
		std::unordered_map<uint32_t, Stream> streams_;
		std::multimap<StreamPrioKey, uint32_t> stream_priorities_;
;

	};
}

#endif