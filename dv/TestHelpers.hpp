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

template <typename T, typename R = int>
concept has_data_assign_from = requires(T t, R r) { t.data = r; };

template <typename T, typename R = int>
concept nothrow_has_data_assign_from = requires(T t, R r) {
  { t.data = r } noexcept;
};

template <typename T, typename R = int>
concept has_valid_assign_from = requires(T t, R r) { t.valid = r; };

template <typename T, typename R = int>
concept nothrow_has_valid_assign_from = requires(T t, R r) {
  { t.valid = r } noexcept;
};

template <typename Dut, typename Data = int, typename Valid = int>
concept can_send = has_data_assign_from<Dut, Data> &&
    has_valid_assign_from<Dut, Data> && nyu::can_call_tick<Dut>;

template <typename Dut, typename Data = int, typename Valid = int>
concept nothrow_can_send = nothrow_has_data_assign_from<Dut, Data> &&
    nothrow_has_valid_assign_from<Dut, Data> && nyu::nothrow_can_call_tick<Dut>;

template <typename Dut, typename Data, typename Valid = int>
requires can_send<Dut, Data, Valid>
void send(Dut& tx, Data val) noexcept(nothrow_can_send<Dut, Data, Valid>) {
  tx.data = val;
  tx.valid = Valid {1};
  nyu::tick(tx);
  tx.valid = Valid {0};
}

#endif // TEST_CUSTOMIZATIONS_HPP
