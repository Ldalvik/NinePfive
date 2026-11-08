# LIN bus
19,200 bit/s

| Sensors | Flow |
|---|---|
| Cabin temperature sensor (left side of radio unit) | Wired to HVAC for temperature control |
| Outside-air temperature sensor (behind the front bumper) | Wired to HVAC for cluster LCD readout |
| Automatic-light sensor | Analog sunlight output to HVAC for sun-load compensation |
| 12 V battery sensor (negative terminal) | `0x30`, `0x31`, `0x32`, `0x33`, `0x39`, and `0x3A`. Cluster relays data to the PCM |

## 0x18 — Configuration and run state

**PID `D8` · 8 bytes**

| Payload | Signal | Decode |
|---|---|---|
| `L0` | Battery Sensor ID · `BATID` | Raw configuration ID supplied by the PCM |
| `L1..L6` | Configuration data | Raw bytes |
| `L7[0]` | Cluster run state | `0` off, `1` on |
| `L7[7:1]` | Fixed bits | `0x7F` |

## 0x23 — Unknown response

**PID `A3` · 8 bytes.** `L0..L7` raw.

## 0x24 — Unknown response

**PID `64` · 8 bytes.** `L0..L7` raw.

## 0x25 — Illumination

**PID `25` · 4 bytes**

| Payload | Signal | Decode |
|---|---|---|
| `L0[4:0]` | Illumination level | Raw level, normally `1`–`22` |
| `L0[6:5]` | Illumination mode | `0` primary preset, `1` dimmed preset, `2` other, `3` unknown. The dimmed preset can be overridden. |
| `L0[7]` | MICU status bit | Raw from `F810` byte 2 bit 7 |
| `L1..L3` | Initial bytes | `FF FF FF` |

## 0x26 — Unknown response

**PID `A6` · 8 bytes.** `L0..L7` raw.

## 0x2B — Receive status

**PID `2B` · 8 bytes**

| Payload | Signal | Decode |
|---|---|---|
| `L0` | Climate control data | Raw from `F858` byte 0. Defaults to `FF` when the cluster rejects the source. |
| `L1[1:0]` | Climate control code | Raw from `F853` byte 7 bits 2:1. Defaults to `0` when the cluster rejects the source. |
| `L1[4:2]` | `F8B0` receive status | `0` invalid, `1` valid, `7` timeout. Other values unknown. |
| `L1[7:5]` | Status code 2 | Raw from `F8B1` byte 1 bits 7:5 |
| `L2[4:0]` | Initial bits | `0x1F` |
| `L2[5]` | Status bit | Raw from `F894` byte 2 bit 5 |
| `L2[7:6]` | Initial bits | `3` |
| `L3..L7` | Initial bytes | `FF FF FF FF FF` |

## 0x30 — Battery current and voltage

**PID `F0` · 8 bytes**

| Payload | Signal | Decode |
|---|---|---|
| `L0..L2` | Battery Current (Battery Sensor) · `IPBAT` | `clamp(trunc((raw − 2000000) / 10), −32768, 32767) / 100` A |
| `L3..L4` | Battery Voltage From Battery Sensor · `VBPBAT` | `raw × 0.001` V |
| `L5[6:0]` | Status bits | Raw. Bit 1 is an optional P154B input, disabled in L640. |
| `L5[7]` | Current Detection Status · `IBATSTATUS` / Voltage Detection Status · `VBATSTATUS` | `0` normal, `1` circuit open failure |
| `L6..L7` | Unknown bytes | Raw |

`trunc` rounds toward zero.

## 0x31 — Battery temperature and management

**PID `B1` · 8 bytes**

The battery sensor estimates battery temperature. The PCM uses battery-management codes to track battery state and adapt its resistance estimate.

| Payload | Signal | Decode |
|---|---|---|
| `L0..L1` | Unknown bytes | Raw |
| `L2` | Estimated Battery Temperature · `TPBAT` | `raw − 40` °C, estimated by the battery sensor |
| `L3` | Survey Infomation Battery Management(1) · `SOCPBAT` | Raw value used by the PCM's battery state logic. A Honda diesel uses `SOCPBAT` for estimated charge. Accord percent scale unverified. |
| `L4[2:0]` | Battery management mode | Raw mode. On a valid update, mode `1` sets the PCM's `SOCCL` flag (Survey Infomation Battery Management(2)). |
| `L4[4:3]` | Battery resistance adaptation change code | Raw code the PCM tracks for resistance adaptation |
| `L4[7:5]` | Unknown status bits | Raw |
| `L5[2:0]` | Battery sensor status code | Raw, forwarded by the cluster on B-CAN `F978` |
| `L5[7:3]` | Unknown bits | Raw |
| `L6..L7` | Unknown bytes | Raw |

## 0x32 — Battery resistance and counters

**PID `32` · 8 bytes**

The sensor estimates internal resistance, which affects voltage drop under load. Separate counters track total charge into and out of the battery. The PCM uses changes in those counters for diagnostics.

| Payload | Signal | Decode |
|---|---|---|
| `L0..L1` | Estimated Battery Resistance (Battery Sensor) · `RCLPBAT` | `min(floor(min(raw, 32767) / 10), 255) × 0.1` mΩ in HDS |
| `L2..L3` | Charge counter | `raw × 3.5172` A·s, based on captures |
| `L4..L5` | Discharge counter | `raw × 3.5172` A·s, based on captures |
| `L6..L7` | Unknown bytes | Raw |

## 0x33 — Battery current history

**PID `73` · 8 bytes**

HDS uses this ignition-off history to check for parasitic draw.

| Payload | Signal | Decode |
|---|---|---|
| `L0..L4` | Ignition-off current history · `Imon0-1` through `Imon0-4` | Four 10-bit codes at bit offsets `0`, `10`, `20`, `30`. Code `1023` is invalid. |
| `L5..L7` | Unknown bytes | Raw |

HDS phase 1 uses `(floor(code × 999474 / 1023) − 999474) / 100` mA. Phase 2 uses `20 × code − 20460` mA for the first code and `20 × code − 15340` mA for the other three. The payload alone cannot select the HDS phase.

In HDS phase 2, the four codes are current just after ignition off, average over the first hour, average over the second hour, and average over the last hour before ignition on.

## 0x39 — Battery estimates and reset status

**PID `39` · 8 bytes**

The battery sensor reports estimated resting voltage, battery health under load, and predicted voltage under a heavy load such as cranking. HDS also uses this reply for the ignition-off history duration.

| Payload | Signal | Decode |
|---|---|---|
| `L0..L1` | Latest - current history - Duration · `Duration0` | Dedicated HDS displays `raw & 0x3FFF` hours. Generic CANFI scales the same code by `0.01` hours. |
| `L2` | Estimated Battery Quiescent (or Open Circuit) Voltage · `VBATCO` | `4 × raw + 4600` code. Volts scale unknown. |
| `L3` | Estimated Battery Ability to Deliver Current/Voltage (SOH) · `SOHQPBAT` | `floor(raw / 2)` code. Percent scale unknown. |
| `L4` | Estimated Battery Voltage at the Specified High Load · `SOFVPBAT` | `25 × raw` code. Volts scale unknown. |
| `L5[0]` | Reset on Battery Sensor · `BATSRST` | Raw reset status bit used by PCM charge counter logic |
| `L5[1]` | Survey Infomation Battery Management(3) · `BDCHGBAT` | Raw status bit. Honda diesel  uses this symbol for external charging detection. |
| `L5[7:2]` | Unknown bits | Raw |
| `L6..L7` | Unknown bytes | Raw |

## 0x3A — Thermal and adaptation inputs

**PID `BA` · 8 bytes**

| Payload | Signal | Decode |
|---|---|---|
| `L0` | Battery temperature check input | `raw − 40` °C |
| `L1..L3` | Unknown bytes | Raw |
| `L4..L5` | Battery resistance adaptation input | Raw 16-bit sample used for resistance comparison and averaging |
| `L6..L7` | Unknown bytes | Raw |
