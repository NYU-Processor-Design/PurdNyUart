# Baud Rate Generator

A baud rate generator is a form of digital clock divider which generates a
target transmitter and receiver clock rate based on the input clock,
desired baudrate, and desired oversample.

## Contents
* [Parameters](#parameters)
* [Inputs](#inputs)
* [Outputs](#outputs)
* [Functionality](#functionality)
  * [Basics](#basics)
  * [Synchronization](#synchronization)

## Parameters
|      Name      |         Default       |          Description            |
|      :--:      |         :-----:       |          :---------:            |
| `MaxClockRate` |  100 * 10<sup>6</sup> | Maximum input system clock rate |
| `MinBaudRate`  |  9600                 | Minimum output baud rate        |
| `Oversample`   |  16                   | Receiver clock multiplier       |

## Inputs
|    Name     |  Width  |          Description          |
|    :--:     |  :---:  |          :---------:          |
| `clk`       |    1    | Input clock                   |
| `nReset`    |    1    | Asynchronous active-low reset |
| `syncReset` |    1    | Synchronous active-high reset |
| `phase`     |    1    | Start phase of output clocks  |
| `rate`      | var[^1] | Clock division counter        |

[^1]: `$clog2(MaxClockRate / MinBaudRate)`

## Outputs

|    Name     |  Width  |          Description          |
|    :--:     |  :---:  |          :---------:          |
| `rxClk`     |    1    | Receiver clock                |
| `txClk`     |    1    | Transmitter clock             |

## Functionality

### Basics

The `rate` input represents the period of the `txClk`. Put another way, it's a
simple counter that sets the `txClk` to `[~phase]` every `[rate]` clock ticks.

The `rxClk` will have `[Oversample]` number of `[~phase]` edges every `txClk`
period. The period of the `rxClk` is referred to as the `rxRate`. These edges
will be evenly spaced within the `txClk` period, with the goal of the
`[Oversample / 2]`<sup>th</sup> edge being directly in the center of the
`txClk` period.

The `rxClk` is kept synchronized with the `txClk` via two "wait" zones, the
`preWait` zone and the `postWait` zone, which account for the indivisble
remainder of the `rate` divided by the `Oversample`. Each wait zone is about
half of the remainder.

Additionally, the `rxClk` is shifted a half-period (ie, `rxRate / 2`) within
the overall `txClk` period, to further center the sampling. This shift is
referred to as the `offset` within the rate calculations.
