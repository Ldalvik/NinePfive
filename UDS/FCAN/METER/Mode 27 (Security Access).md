# METER — Mode 27 (Security Access)

Target: 2017 US Accord meter 78100-T2F-A140-M1. Unknown values are retained explicitly.

| Command | Name | Source | Header | Payload | Request bytes | Response layout | Response bytes | SecurityAccess mode | Field source | Field name | Field ID | Offset / bit | Mask | Parse / decode |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 27/01 | SecurityAccess level 0x01 seed | METER A140 HDS/runtime research | 27 01 | — | 2 | 67 01 <seed...> | UNKNOWN | seed request | HDS/runtime analysis | seed | — | see response layout | — | length and byte order unresolved |
| 27/07 | TYPE_V replacement seed | METER A140 HDS/runtime research | 27 07 | — | 2 | 67 07 <seed_hi> <seed_lo> <type_hi> <type_lo> | 6 | seed request | HDS/runtime analysis | seed | — | see response layout | — | big-endian u16 |
| 27/07 | TYPE_V replacement seed | METER A140 HDS/runtime research | 27 07 | — | 2 | 67 07 <seed_hi> <seed_lo> <type_hi> <type_lo> | 6 | seed request | HDS/runtime analysis | type | — | see response layout | — | big-endian u16; CalcTypeVKey table index type-0x7000 |
| 27/07 | TYPE_V replacement seed | METER A140 HDS/runtime research | 27 07 | — | 2 | 67 07 <seed_hi> <seed_lo> <type_hi> <type_lo> | 6 | seed request | Recovered Meterapp | seed | — | response bytes 2..3 | FFFF | uint16 big-endian |
| 27/07 | TYPE_V replacement seed | METER A140 HDS/runtime research | 27 07 | — | 2 | 67 07 <seed_hi> <seed_lo> <type_hi> <type_lo> | 6 | seed request | Recovered Meterapp | type | — | response bytes 4..5 | FFFF | uint16 big-endian; 0x7000..0x700F |
| 27/08 | TYPE_V replacement key | METER A140 HDS/runtime research | 27 08 | <key_hi> <key_lo> <type_hi> <type_lo> | VARIABLE | 67 08 | UNKNOWN | paired with 27 07 | HDS/runtime analysis | key | — | request payload | — | big-endian u16; CalcTypeVKey(seed,type) |
| 27/08 | TYPE_V replacement key | METER A140 HDS/runtime research | 27 08 | <key_hi> <key_lo> <type_hi> <type_lo> | VARIABLE | 67 08 | UNKNOWN | paired with 27 07 | HDS/runtime analysis | type | — | request payload | — | big-endian u16; echo from 67 07 |
| 27/08 | TYPE_V replacement key | METER A140 HDS/runtime research | 27 08 | <key_hi> <key_lo> <type_hi> <type_lo> | VARIABLE | 67 08 | UNKNOWN | paired with 27 07 | Recovered Meterapp | key | CalcTypeVKey | request bytes 2..3 | FFFF | ROL16(seed+SFC1[i],ROL[i]) XOR ROR16(seed+SFC2[i],ROR[i]), then +FINAL_ADD[i], modulo 2^16; i=type-0x7000 |
| 27/41 | SecurityAccess 0x41 seed / selector | METER A140 HDS/runtime research | 27 41 | — | 2 | 67 41 <seed32_be> D1 | 7 | seed request | HDS/runtime analysis | seed | — | see response layout | — | 4-byte big-endian |
| 27/41 | SecurityAccess 0x41 seed / selector | METER A140 HDS/runtime research | 27 41 | — | 2 | 67 41 <seed32_be> D1 | 7 | seed request | HDS/runtime analysis | selector | — | see response layout | — | 0xD1 |
