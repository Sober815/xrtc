﻿#ifndef XRTCSDK_XRTC_RTC_MODULES_RTP_RTCP_RTP_RTCP_DEFINES_H_
#define XRTCSDK_XRTC_RTC_MODULES_RTP_RTCP_RTP_RTCP_DEFINES_H_

#include <stdint.h>

namespace xrtc {

class RtpPacket;

#define IP_PACKET_SIZE 1500

enum RTCPPacketType : uint32_t {
    kRtcpReport = 0x0001,
    kRtcpSr = 0x0002,
    kRtcpRr = 0x0004,
};

class RtpPacketCounter {
public:
    RtpPacketCounter() = default;
    explicit RtpPacketCounter(const RtpPacket& packet);

    void Add(const RtpPacketCounter& other);
    void Subtract(const RtpPacketCounter& other);
    void AddPacket(const RtpPacket& packet);

    size_t header_bytes = 0;
    size_t payload_bytes = 0;
    size_t padding_bytes = 0;
    uint32_t packets = 0;
};

class StreamDataCounter {
public:
    RtpPacketCounter transmmited;
    RtpPacketCounter retransmitted;
};

} // namespace xrtc

#endif // XRTCSDK_XRTC_RTC_MODULES_RTP_RTCP_RTP_RTCP_DEFINES_H_