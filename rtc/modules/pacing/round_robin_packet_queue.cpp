#include "round_robin_packet_queue.h"


namespace xrtc {
	RoundRobinPacketQueue::RoundRobinPacketQueue()
	{
	}
	RoundRobinPacketQueue::~RoundRobinPacketQueue()
	{
	}
	void RoundRobinPacketQueue::Push(int priority, 
		webrtc::Timestamp enqueue_time, 
		uint64_t enquue_order, 
		std::unique_ptr<RtpPacketToSend> packet)
	{
		Push(QueuedPacket(priority, enqueue_time, enquue_order, std::move(packet)));
	}
	void RoundRobinPacketQueue::Push(const QueuedPacket& packet)
	{
		//����packet�Ƿ���ڶ�Ӧ��stream
		auto stream_iter = streams_.find(packet.Ssrc());
		if (stream_iter == streams_.end())//û�ҵ���stream��һ����
		{
			stream_iter = streams_.emplace(packet.Ssrc(), Stream()).first;
			// �µ�������δ�������ȼ�����
			stream_iter->second.priority_it_ = stream_priorities_.end();
			stream_iter->second.ssrc = packet.Ssrc();
		}

		Stream* stream = &stream_iter->second;
		//�����������ȼ�
		if (stream->priority_it_ == stream_priorities_.end())
		{
			//��map���л�û�н��и��������ȼ�����
				stream_priorities_.emplace(StreamPrioKey(packet.Priority(), stream->size),
					packet.Ssrc());
		}
		else if (packet.Priority() < stream->priority_it_->first.priority) {
			stream_priorities_.erase(stream->priority_it_);
			// ���²��룬�������ȼ�
			stream_priorities_.emplace(StreamPrioKey(packet.Priority(), stream->size),
				packet.Ssrc());
		}

		stream->packet_queue.emplace(packet);

	}
	RoundRobinPacketQueue::QueuedPacket::QueuedPacket(int priority, 
		webrtc::Timestamp enqueue_time, uint64_t enqueue_order, 
		std::unique_ptr<RtpPacketToSend> packet):
		priority_(priority),
		enqueue_time_(enqueue_time),
		enqueue_order_(enqueue_order),
		owned_packet_(packet.release())
	{
	}
	RoundRobinPacketQueue::QueuedPacket::~QueuedPacket()
	{
	}
	RoundRobinPacketQueue::Stream::Stream():
		size(webrtc::DataSize::Zero()),
		ssrc(0)
	{
	}
	RoundRobinPacketQueue::Stream::~Stream()
	{
	}
}