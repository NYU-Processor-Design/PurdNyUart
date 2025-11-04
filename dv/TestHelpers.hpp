#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include <cstdint>

#include <NyuTestUtil.hpp>

consteval unsigned flog2(unsigned x) {
  return x == 1 ? 0 : flog2(x >> 1) + 1;
}

consteval unsigned clog2(unsigned x) {
  return x == 1 ? 0 : flog2(x - 1) + 1;
}

inline constexpr unsigned MaxClockRate = 100 * 1000000;
inline constexpr unsigned MinBaudRate = 9600;
inline constexpr unsigned Oversample = 16;

inline constexpr unsigned rxRate = MaxClockRate / (MinBaudRate * Oversample);
inline constexpr unsigned txRate = MaxClockRate / MinBaudRate;

inline constexpr unsigned txWidth = clog2(txRate);
inline constexpr unsigned rxShift = clog2(Oversample);
inline constexpr unsigned rxWidth = txWidth - rxShift;


NYU_META_ASSIGNABLE_CONCEPTS(has_en_assign_from, en)
NYU_META_ASSIGNABLE_CONCEPTS(has_in_assign_from, in)
NYU_META_ASSIGNABLE_CONCEPTS(has_phase_assign_from, phase)
NYU_META_ASSIGNABLE_CONCEPTS(has_rate_assign_from, rate)
NYU_META_ASSIGNABLE_CONCEPTS(has_syncreset_assign_from, syncReset)
NYU_META_ASSIGNABLE_CONCEPTS(has_data_assign_from, data)
NYU_META_ASSIGNABLE_CONCEPTS(has_valid_assign_from, valid)


template <typename Dut, typename R = std::uint8_t>
requires has_en_assign_from<Dut, R> && nyu::reset_default_ok<Dut>
void tag_invoke(nyu::reset_t, Dut& dut) noexcept(
    nothrow_has_en_assign_from<Dut, R> && nyu::nothrow_reset_default_ok<Dut>) {
  dut.en = R {1};
  nyu::reset_default(dut);
}


template <typename Dut, typename Phase, typename Rate, typename SyncReset>
concept can_phase_reset =
    has_phase_assign_from<Dut, Phase> && has_rate_assign_from<Dut, Rate> &&
    has_syncreset_assign_from<Dut, SyncReset>;

template <typename Dut, typename Phase, typename Rate, typename SyncReset>
concept nothrow_can_phase_reset = nothrow_has_phase_assign_from<Dut, Phase> &&
    nothrow_has_rate_assign_from<Dut, Rate> &&
    nothrow_has_syncreset_assign_from<Dut, SyncReset>;

template <typename Dut, typename Phase = int, typename Rate = decltype(txRate),
    typename SyncReset = std::uint8_t>
requires can_phase_reset<Dut, Phase, Rate, SyncReset> &&
    nyu::reset_default_ok<Dut>
void tag_invoke(nyu::reset_t, Dut& dut, Phase phase = 0,
    Rate rate =
        txRate) noexcept(nothrow_can_phase_reset<Dut, Phase, Rate, SyncReset> &&
    nyu::nothrow_reset_default_ok<Dut>) {
  dut.phase = static_cast<bool>(phase);
  dut.rate = rate;
  dut.syncReset = SyncReset {0};
  nyu::reset_default(dut);
}


template <typename Dut, typename Data, typename Valid>
concept can_send = has_data_assign_from<Dut, Data> &&
    has_valid_assign_from<Dut, Data> && nyu::can_call_tick<Dut>;

template <typename Dut, typename Data, typename Valid>
concept nothrow_can_send = nothrow_has_data_assign_from<Dut, Data> &&
    nothrow_has_valid_assign_from<Dut, Data> && nyu::nothrow_can_call_tick<Dut>;

template <typename Dut, typename Data, typename Valid = std::uint8_t>
requires can_send<Dut, Data, Valid>
void send(Dut& dut, Data val) noexcept(nothrow_can_send<Dut, Data, Valid>) {
  dut.data = val;
  dut.valid = Valid {1};
  nyu::tick(dut);
  dut.valid = Valid {0};
}


template <typename Dut, typename In>
concept can_start_transmit =
    has_in_assign_from<Dut, In> && nyu::can_call_tick<Dut>;

template <typename Dut, typename In>
concept nothrow_can_start_transmit =
    nothrow_has_in_assign_from<Dut, In> && nyu::nothrow_can_call_tick<Dut>;

template <typename Dut, typename In = std::uint8_t>
requires can_start_transmit<Dut, In>
void start(Dut& dut, unsigned ticks = Oversample) noexcept(
    nothrow_can_start_transmit<Dut, In>) {
  dut.in = In {1};
  nyu::tick(dut);
  dut.in = In {0};
  nyu::tick(dut, ticks);
}

template <typename Dut, typename In = std::uint8_t>
requires can_start_transmit<Dut, In>
void transmit(Dut& dut, std::uint8_t val, unsigned ticks = Oversample) noexcept(
    nothrow_can_start_transmit<Dut, In>) {
  for(unsigned i {0}; i < 8; ++i) {
    dut.in = static_cast<In>(val & 0x1);
    nyu::tick(dut, ticks);
    val >>= 1;
  }
}

template <typename Dut, typename In = std::uint8_t>
requires can_start_transmit<Dut, In>
void start_transmit(Dut& dut, std::uint8_t val) {
  start<Dut, In>(dut);
  transmit<Dut, In>(dut, val);
}


#endif // TEST_HELPERS_HPP
