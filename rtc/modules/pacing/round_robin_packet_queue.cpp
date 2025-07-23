#include "xrtc/rtc/modules/pacing/round_robin_packet_queue.h"

namespace xrtc {

RoundRobinPacketQueue::QueuedPacket::QueuedPacket(int priority, 
    webrtc::Timestamp enqueue_time, 
    uint64_t enqueue_order, 
    std::unique_ptr<RtpPacketToSend> packet) :
    priority_(priority),
    enqueue_time_(enqueue_time),
    enqueue_order_(enqueue_order),
    owned_packet_(packet.release())
{
}

RoundRobinPacketQueue::QueuedPacket::~QueuedPacket() {
}

RoundRobinPacketQueue::Stream::Stream() :
    size(webrtc::DataSize::Zero()),
    ssrc(0)
{
}

RoundRobinPacketQueue::Stream::~Stream() {
}

RoundRobinPacketQueue::RoundRobinPacketQueue() {
}

RoundRobinPacketQueue::~RoundRobinPacketQueue() {
}

void RoundRobinPacketQueue::Push(int priority, 
    webrtc::Timestamp enqueue_time, 
    uint64_t enqueue_order, 
    std::unique_ptr<RtpPacketToSend> packet) 
{
    Push(QueuedPacket(priority, enqueue_time, enqueue_order,
        std::move(packet)));
}

void RoundRobinPacketQueue::Push(const QueuedPacket& packet) {
    // 查找packet是否存在对应的stream
    auto stream_iter = streams_.find(packet.Ssrc());
    if (stream_iter == streams_.end()) { // Stream第一个数据包
        stream_iter = streams_.emplace(packet.Ssrc(), Stream()).first;
        // 新的数据尚未列入优先级排序
        stream_iter->second.priority_it = stream_priorities_.end();
        stream_iter->second.ssrc = packet.Ssrc();
    }

    Stream* stream = &stream_iter->second;
    // 调整流的优先级
    if (stream->priority_it == stream_priorities_.end()) {
        // 在map当中还没有进行该流的优先级排序
        stream_priorities_.emplace(StreamPrioKey(packet.Priority(), stream->size),
            packet.Ssrc());
    }
    else if (packet.Priority() < stream->priority_it->first.priority) {
        stream_priorities_.erase(stream->priority_it);
        // 重新插入，更新优先级
        stream_priorities_.emplace(StreamPrioKey(packet.Priority(), stream->size),
            packet.Ssrc());
    }

    stream->packet_queue.emplace(packet);
}

} // namespace xrtc