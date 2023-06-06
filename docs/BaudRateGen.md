# Baud Rate Generator

A baud rate generator is a form of digital clock divider which generates a
target clock rate based on the input clock and the desired baudrate. PurdNyUart
provides three such generators:
* **BaudRateGen**: Produces a single pair of clocks based on parameterized input
  clock, target baud, and oversample rate
* **BaudRateGen4**: Produces one-of-four pairs of clocks based on parameters
* **BaudRateGenVar**: Produces a configurable pair of clocks based on a counter
  input

## Contents
* [Parameters](#parameters)
* [Inputs](#inputs)
* [Outputs](#outputs)
* [Functionality](#functionality)
  * [Basics](#basics)
  * [Synchronization](#synchronization)

## Parameters
|      Name      | Default | Modules  |          Description            |
|      :--:      | :-----: | :-----:  |          :---------:            |
| `ClockRate`    |   50 * 10<sup>6</sup> | Gen/Gen4 | The input system clock rate |
| `MaxClockRate` |  100 * 10<sup>6</sup> | GenVar | Maximum input system clock rate |
| `BaudRate`     | 115200  |   Gen    | Target output baud rate         |
| `MinBaudRate`  |   9600  |  GenVar  | Minimum output baud rate        |
| `Oversample`   |    16   |   All    | Receiver clock multiplier       |
| `Baud1`        |   9600  |   Gen4   | Target baud rate for `sel == 0` |
| `Baud2`        |  38400  |   Gen4   | Target baud rate for `sel == 1` |
| `Baud3`        |  57600  |   Gen4   | Target baud rate for `sel == 2` |
| `Baud4`        | 115200  |   Gen4   | Target baud rate for `sel == 3` |

## Inputs
|    Name    |  Width  | Modules |          Description          |
|    :--:    |  :---:  | :-----: |          :---------:          |
|   `clk`    |    1    |   All   | Input clock                   |
|   `nReset` |    1    |   All   | Asynchronous active-low reset |
|   `phase`  |    1    |   All   | Start phase of output clocks  |
|   `sel`    |    2    |   Gen4  | Baud rate selection           |
|   `count`  | var[^1] |  GenVar | Clock division counter        |

[^1]: `$clog2(MaxClockRate / (2 * MinBaudRate))`

## Outputs

|    Name    |  Width  | Modules |          Description          |
|    :--:    |  :---:  | :-----: |          :---------:          |
|   `rxClk`  |    1    |   All   | Receiver clock                |
|   `txClk`  |    1    |   All   | Transmitter clock             |

## Functionality

### Basics

Consider a baud generator with a clock of 10Mhz, target rate of 115200 Baud,
and an oversample of 16x.

The length of each half-period of the receiver clock can be calculated with the
following formula:

```
RxCycles = (ClockRate // (2 * BaudRate * Oversample)) + 1 = 3
```

**Note**: This is essential a ceiling operation, but even a perfectly divisible
`ClockRate` will result in the extra +1 cycle due to how the divider is
implemented.

Similarly, the length of the half-period of the transmitter clock can be
calculated with:

```
TxCycles = (ClockRate // (2 * BaudRate)) + 1 = 44
```

The resulting `rxClk` is illustrated below:

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://svg.wavedrom.com/github/NYU-Processor-Design/PurdNyUart/main/docs/waveforms/rxClkDark.json">
  <img src="https://svg.wavedrom.com/github/NYU-Processor-Design/PurdNyUart/main/docs/waveforms/rxClkLight.json">
</picture>

`txClk` is the same except with an 88 cycle period.

### Synchronization

`rxClk` is synchronized to `txClk`, it will hold on half-periods to ensure that
its overall period is an even division of `txClk`.

For example, if `rxCycles` is 3 and `txCycles` is 7 the resulting waveform
would be:

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://svg.wavedrom.com/github/NYU-Processor-Design/PurdNyUart/main/docs/waveforms/syncDark.json">
  <img src="https://svg.wavedrom.com/github/NYU-Processor-Design/PurdNyUart/main/docs/waveforms/syncLight.json">
</picture>

Note that the positive half-period of `rxClk` is held for an extra `clk` cycle
to match the half-period of `txClk`. This ensures the overall baud rate of
the receiver and transmitter are the same.
