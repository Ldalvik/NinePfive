# METER — Mode 2E (Write DID)

Target: 2017 US Accord meter 78100-T2F-A140-M1. Unknown values are retained explicitly.

| Command | Name | Source | Header | Payload | Request bytes | Response layout | Response bytes | SecurityAccess mode | Field source | Field name | Field ID | Offset / bit | Mask | Parse / decode |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 2E/7012 | Write equipment replacement data | METER A140 HDS/runtime research | 2E 70 12 | <saved_data...> | VARIABLE | 6E 70 12 | UNKNOWN | TYPE_V 27 07/08 | HDS/runtime analysis | saved_data | — | request payload | — | opaque bytes and exact saved length from GaugeMemory2.bin |
| 2E/7090 | Write odometer/replacement data | METER A140 HDS/runtime research | 2E 70 90 | <saved_data...> | VARIABLE | 6E 70 90 | UNKNOWN | TYPE_V 27 07/08 | HDS/runtime analysis | saved_data | — | request payload | — | opaque bytes and exact saved length from GaugeMemory2.bin |
| 2E/70A0 | Write EUS replacement data 1 | METER A140 HDS/runtime research | 2E 70 A0 | <saved_data...> | VARIABLE | 6E 70 A0 | UNKNOWN | TYPE_V 27 07/08 | HDS/runtime analysis | saved_data | — | request payload | — | opaque bytes and exact saved length from GaugeMemory2.bin |
| 2E/70A1 | Write EUS replacement data 2 | METER A140 HDS/runtime research | 2E 70 A1 | <saved_data...> | VARIABLE | 6E 70 A1 | UNKNOWN | TYPE_V 27 07/08 | HDS/runtime analysis | saved_data | — | request payload | — | opaque bytes and exact saved length from GaugeMemory2.bin |
| 2E/70A4 | Write maintenance-reset replacement data | METER A140 HDS/runtime research | 2E 70 A4 | <saved_data...> | VARIABLE | 6E 70 A4 | UNKNOWN | TYPE_V 27 07/08 | HDS/runtime analysis | saved_data | — | request payload | — | opaque bytes and exact saved length from GaugeMemory2.bin |
| 2E/70A8 | Write USS replacement data 1 | METER A140 HDS/runtime research | 2E 70 A8 | <saved_data...> | VARIABLE | 6E 70 A8 | UNKNOWN | TYPE_V 27 07/08 | HDS/runtime analysis | saved_data | — | request payload | — | opaque bytes and exact saved length from GaugeMemory2.bin |
| 2E/70A9 | Write USS replacement data 2 | METER A140 HDS/runtime research | 2E 70 A9 | <saved_data...> | VARIABLE | 6E 70 A9 | UNKNOWN | TYPE_V 27 07/08 | HDS/runtime analysis | saved_data | — | request payload | — | opaque bytes and exact saved length from GaugeMemory2.bin |
