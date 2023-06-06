# Edge Synchronization

The edge synchronizer is a 2-bit synchronizer and an edge detector combined into
a single module.

## Contents
* [Inputs](#inputs)
* [Outputs](#outputs)
* [Functionality](#functionality)

## Inputs
|    Name    |  Width  |          Description          |
|    :--:    |  :---:  |          :---------:          |
|   `clk`    |    1    | Input clock                   |
|   `nReset` |    1    | Asynchronous active-low reset |
|   `in`     |    1    | Asynchronous input signal     |

## Outputs

|    Name    |  Width  |          Description          |
|    :--:    |  :---:  |          :---------:          |
|   `out`    |    1    | Synchronized output signal    |
|   `rise`   |    1    | Rising edge detected          |
|   `fall`   |    1    | Falling edge detected         |

## Functionality

Combining a synchronizer and an edge detector creates a three-bit shift
register. For convenience, each bit has a name:


|   2   |   1   |    0   |
|  :-:  |  :-:  |   :-:  |
| `cmp` | `out` | `sync` |

Each clock cycle, the shift register is shifted left one bit and the `in` signal
is read into the `sync` bit.

The `out` bit is the synchronized output from the module.

The `cmp` bit is used to detect a rising or falling edge according to the
following logic:
```verilog
rise =  out && !cmp;
fall = !out &&  cmp;
```
