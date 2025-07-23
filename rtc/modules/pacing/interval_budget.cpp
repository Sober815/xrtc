#include "xrtc/rtc/modules/pacing/interval_budget.h"

#include <algorithm>

namespace xrtc {
	namespace {

		int kWindowMs = 500;

	} // namespace


	IntervalBudget::IntervalBudget(int initial_target_bitrate_kbps, bool can_build_up_underuse):
		target_bitrate_kbps_(initial_target_bitrate_kbps),
		can_build_up_underuse_(can_build_up_underuse)
	{
		set_target_bitrate_bps(target_bitrate_kbps_);
	}

	IntervalBudget::~IntervalBudget()
	{
	}

	void IntervalBudget::set_target_bitrate_bps(int target_bitrate_kbps)
	{
		target_bitrate_kbps_ = target_bitrate_kbps;
		max_bytes_in_budget_ = (target_bitrate_kbps * kWindowMs) / 8;
		// [-max_bytes_in_budget, max_bytes_in_bydget]
		bytes_remaining_ = std::min(
			std::max(-max_bytes_in_budget_, bytes_remaining_),
			max_bytes_in_budget_);

	}

	void IntervalBudget::IncreaseBudget(int64_t elapsed_time)
	{
		int64_t bytes = (elapsed_time * target_bitrate_kbps_) / 8;
		if (bytes_remaining_ < 0 || can_build_up_underuse_) {
			// ��һ��Ԥ��ʹ�ó��꣬��Ҫ�ڱ��ֲ���
			// ���can_build_up_underuse_ΪTRUE����һ�ֵ�Ԥ������ں�������ʹ��
			bytes_remaining_ = std::min(bytes_remaining_ + bytes, max_bytes_in_budget_);
		}
		else {
			// ��һ��Ԥ�㻹�и��࣬����
			bytes_remaining_ = std::min(bytes, max_bytes_in_budget_);
		}
	}

	void IntervalBudget::UseBudget(size_t bytes)
	{
		bytes_remaining_ = std::max(bytes_remaining_ - static_cast<int64_t>(bytes),
			-max_bytes_in_budget_);
	}

	size_t IntervalBudget::bytes_remaining()
	{
		return std::max<int64_t>(0, bytes_remaining_);
	}



}
