#ifndef TEST_CUSTOMIZATIONS_HPP
#define TEST_CUSTOMIZATIONS_HPP

#include <NyuCatch2TestUtil.hpp>

template <typename T, typename R = int>
concept has_en_assign_from = requires(T t, R r) { t.en = r; };

template <typename T, typename R = int>
concept nothrow_has_en_assign_from = requires(T t, R r) {
  { t.en = r } noexcept;
};

template <typename Dut, typename R = int>
requires has_en_assign_from<Dut, R> && nyu::reset_default_ok<Dut>
void tag_invoke(nyu::reset_t, Dut& dut) noexcept(
    nothrow_has_en_assign_from<Dut, R> && nyu::nothrow_reset_default_ok<Dut>) {
  dut.en = R {1};
  nyu::reset_default(dut);
}

template <typename T, typename R = bool>
concept has_phase_assign_from =
    requires(T t, R r) { t.phase = static_cast<bool>(r); };

template <typename T, typename R = bool>
concept nothrow_has_phase_assign_from = requires(T t, R r) {
  { t.phase = static_cast<bool>(r) } noexcept;
};

template <typename T, typename R = int>
concept has_rate_assign_from = requires(T t, R r) { t.rate = r; };

template <typename T, typename R = int>
concept nothrow_has_rate_assign_from = requires(T t, R r) {
  { t.rate = r } noexcept;
};

template <typename T, typename R = int>
concept has_syncReset_assign_from = requires(T t, R r) { t.syncReset = r; };

template <typename T, typename R = int>
concept nothrow_has_syncReset_assign_from = requires(T t, R r) {
  { t.syncReset = r } noexcept;
};

template <typename Dut, typename Phase = bool, typename Rate = int,
    typename SyncReset = int>
concept can_phase_reset =
    has_phase_assign_from<Dut, Phase> && has_rate_assign_from<Dut, Rate> &&
    has_syncReset_assign_from<Dut, SyncReset>;

template <typename Dut, typename Phase = bool, typename Rate = int,
    typename SyncReset = int>
concept nothrow_can_phase_reset = nothrow_has_phase_assign_from<Dut, Phase> &&
    nothrow_has_rate_assign_from<Dut, Rate> &&
    nothrow_has_syncReset_assign_from<Dut, SyncReset>;

#endif // TEST_CUSTOMIZATIONS_HPP
