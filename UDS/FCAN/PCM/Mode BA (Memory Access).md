| mode/header | Request payload (hex) | Full name | shortname | Layer |
| --- | --- | --- | --- | --- |
| BA/11 | `BA 11 00 00 <addr_hi> <addr_lo>` | Read external serial EEPROM word | — | PCM |
| BA/15 | `BA 15 00 00 <addr_hi> <addr_lo> <word_lo> <word_hi>` | Write external serial EEPROM word | — | PCM |
| BA/90 | `BA 90 00 00 <id_hi> <id_lo>` | Read persistent record set | — | PCM |
| BA/91 | `BA 91 00 <len> <id_hi> <id_lo> <words_BE16...> <trailer>` | Write persistent record set | — | PCM |
| BA/F4 | `BA F4 00 00 <start_BE32> <last_word_BE32> <marker_addr_BE32>` | Validate execution descriptor | — | PCM |
| BA/F5 | `BA F5 00 00 <entry_BE32> <marker_addr_BE32> <marker[0]> <marker[1]> <marker[2]> <marker[3]>` | Validate/execute RAM image | — | PCM |

| Command | Request payload (hex) | Reply payload (hex) | Reply bytes | Parsing |
| --- | --- | --- | --- | --- |
| BA/11 | `BA 11 00 00 <addr_hi> <addr_lo>` | `FA 11 11 07 lo hi checksum` | 7 | Word = `U16LE(r[4..5])`. Address comes from the request. |
| BA/90 | `BA 90 00 00 <id_hi> <id_lo>` | `FA 90 90 25 <32 data bytes> <checksum>` | 37 | Record data = `r[4..35]`. ID comes from the request. |
| BA/15 | `BA 15 00 00 <addr_hi> <addr_lo> <word_lo> <word_hi>` | `FA 15 15 05 D7` | 5 | LE16 write word. Reply contains only the acknowledgment. |
| BA/91 | `BA 91 00 <len> <id_hi> <id_lo> <words_BE16...> <trailer>` | `FA 91 91 05 DF` | 5 | `len = 7 + 2 * word_count`. Dispatcher uses the length byte to count words. Trailer semantics unknown. |
| BA/F4 | `BA F4 00 00 <start_BE32> <last_word_BE32> <marker_addr_BE32>` | `FA F4 F4 05 19` | 5 | Descriptor fields at request bytes 4..15. |
| BA/F5 | `BA F5 00 00 <entry_BE32> <marker_addr_BE32> <marker[0]> <marker[1]> <marker[2]> <marker[3]>` | `FA F5 F5 05 17` | 5 | Acknowledgment is queued before RAM entry. |
| BA positive reply | — | `FA selector selector total_length data... checksum` | `r[3]` | `(sum(r) & 0xFF) == 0`. Offsets are zero-based after ISO-TP reassembly. BA CAN endpoint and entry are unresolved. |
| BA negative reply | — | `7F BA NRC` | 3 | NRC = `r[2]`. |

| Command | Request payload (hex) | Extraction (r = full reply) | Field | shortname | Conversion / unit |
| --- | --- | --- | --- | --- | --- |
| `BA/11/00` | `BA 11 00 00 00 00` | `U16LE(r[4..5])` | F100 source word 0 | — | Raw U16; exact counter/field assignment UNKNOWN |
| `BA/11/01` | `BA 11 00 00 00 01` | `r[5]` | Number of rewrite events | NREWR | raw events |
| `BA/11/01` | `BA 11 00 00 00 01` | `r[4]` | Number of erasing process | NERAS | raw processes |
| `BA/11/03` / `BA/11/43` | `BA 11 00 00 00 03` / `BA 11 00 00 00 43` | `r[5] & 0x7F` | VIN payload length | — | Length in bytes. Documented length is 17. |
| `BA/11/03` / `BA/11/43` | `BA 11 00 00 00 03` / `BA 11 00 00 00 43` | `(r[5] >> 7) & 1` | VIN record marker | — | 1 = marked present. Check length and checksum separately. |
| `BA/11/03` / `BA/11/43` | `BA 11 00 00 00 03` / `BA 11 00 00 00 43` | `r[4]` | VIN payload checksum | — | `(raw + sum(all 20 stored payload bytes)) & 0xFF == 0xFF`. |
| `BA/11/04..0D` / `BA/11/44..4D` | `BA 11 00 00 00 <index:04..0D>` / `BA 11 00 00 00 <index:44..4D>` | `[r[5], r[4]] per word` | VIN | VIN | Concatenate words in ascending index order. Take the first length bytes as VIN characters. Include all 20 stored bytes in the checksum. |
| `BA/11/0E` / `BA/11/4E` | `BA 11 00 00 00 0E` / `BA 11 00 00 00 4E` | `r[5] & 0x7F` | E800 payload length | — | Length in bytes. Documented length is 11. |
| `BA/11/0E` / `BA/11/4E` | `BA 11 00 00 00 0E` / `BA 11 00 00 00 4E` | `(r[5] >> 7) & 1` | E800 record marker | — | 1 = marked present. Check length and checksum separately. |
| `BA/11/0E` / `BA/11/4E` | `BA 11 00 00 00 0E` / `BA 11 00 00 00 4E` | `r[4]` | E800 payload checksum | — | `(raw + sum(all 12 stored payload bytes)) & 0xFF == 0xFF`. |
| `BA/11/0F..14` / `BA/11/4F..54` | `BA 11 00 00 00 <index:0F..14>` / `BA 11 00 00 00 <index:4F..54>` | `[r[5], r[4]] per word` | E800 payload | — | Concatenate words in ascending index order. The documented payload uses 11 bytes. Include all 12 stored bytes in the checksum. |
| `BA/11/15` / `BA/11/55` | `BA 11 00 00 00 15` / `BA 11 00 00 00 55` | `U16LE(r[4..5])` | VIN-bank repair count | — | Raw U16 counter |
| `BA/11/16` / `BA/11/56` | `BA 11 00 00 00 16` / `BA 11 00 00 00 56` | `U16LE(r[4..5])` | E800-bank repair count | — | Raw U16 counter |
| `BA/11/17` / `BA/11/57` | `BA 11 00 00 00 17` / `BA 11 00 00 00 57` | `(r[4] & 0xF0) != 0` | Codec-bank repair eligibility | — | 0 = ineligible, 1 = eligible. |
| `BA/11/18..19` / `BA/11/58..59` | `BA 11 00 00 00 <index:18..19>` / `BA 11 00 00 00 <index:58..59>` | `U16LE(r[4..5]) per word` | Auxiliary fault-registry generation pair | — | Two raw U16 generation values |
| `BA/11/1A` / `BA/11/5A` | `BA 11 00 00 00 1A` / `BA 11 00 00 00 5A` | `U16LE(r[4..5])` | Saved auxiliary fault code 1 | — | Raw fault-code word |
| `BA/11/1B` / `BA/11/5B` | `BA 11 00 00 00 1B` / `BA 11 00 00 00 5B` | `U16LE(r[4..5])` | Saved auxiliary fault code 2 | — | Raw fault-code word |
| `BA/11/1C` / `BA/11/5C` | `BA 11 00 00 00 1C` / `BA 11 00 00 00 5C` | `U16LE(r[4..5])` | Saved auxiliary fault code 3 | — | Raw fault-code word |
| `BA/11/1D` / `BA/11/5D` | `BA 11 00 00 00 1D` / `BA 11 00 00 00 5D` | `U16LE(r[4..5])` | Saved auxiliary fault code 4 | — | Raw fault-code word |
| `BA/11/1E` / `BA/11/5E` | `BA 11 00 00 00 1E` / `BA 11 00 00 00 5E` | `U16LE(r[4..5])` | Auxiliary fault-registry checksum | — | Two generations + four codes + checksum sum to FFFF modulo 65536 |
| `BA/11/1F` / `BA/11/5F` | `BA 11 00 00 00 1F` / `BA 11 00 00 00 5F` | `U16LE(r[4..5])` | Bank-selection state | — | Raw U16 selection state; retain all bits |
| `BA/11/30` / `BA/11/70` | `BA 11 00 00 00 30` / `BA 11 00 00 00 70` | `r[5]` | S-NET serial codec major | — | Raw U8 selector. |
| `BA/11/30` / `BA/11/70` | `BA 11 00 00 00 30` / `BA 11 00 00 00 70` | `r[4]` | S-NET serial codec minor | — | Raw U8 selector. |
| `BA/11/31` / `BA/11/71` | `BA 11 00 00 00 31` / `BA 11 00 00 00 71` | `U16LE(r[4..5])` | Codec-selector check word | — | Selector + check word = 0 modulo 65536 |
| `BA/11/32` / `BA/11/72` | `BA 11 00 00 00 32` / `BA 11 00 00 00 72` | `U16LE(r[4..5])` | Codec-bank repair count | — | U16 repair count |
| `BA/11/7E` | `BA 11 00 00 00 7E` | `U16LE(r[4..5])` | Monitor-snapshot validity | — | Raw U16 validity word |

| Command | Request payload (hex) | Record | Active words | Active reply bytes | Slot tail |
| --- | --- | --- | --- | --- | --- |
| `BA/90/8000` | `BA 90 00 00 80 00` | CVT solenoid current-feedback scales, offsets and checksum | 9 | `r[4..21]` | `r[22..35]` |
| `BA/90/8100` | `BA 90 00 00 81 00` | Three retained serial authentication-comparison bytes and checksum | 2 | `r[4..7]` | `r[8..35]` |
| `BA/90/8200` | `BA 90 00 00 82 00` | PCM part-number data for 22/F110 and checksum metadata | 8 | `r[4..19]` | `r[20..35]` |
| `BA/90/8300` | `BA 90 00 00 83 00` | PCM serial-number data for 22/F112 and checksum metadata | 7 | `r[4..17]` | `r[18..35]` |
| `BA/90/8400` | `BA 90 00 00 84 00` | Unknown record data | 16 | `r[4..35]` | — |
| `BA/90/8500` | `BA 90 00 00 85 00` | Unknown record data | 16 | `r[4..35]` | — |
| `BA/90/9000` | `BA 90 00 00 90 00` | Learned and runtime snapshot header | 10 | `r[4..23]` | `r[24..35]` |
| `BA/90/9100` | `BA 90 00 00 91 00` | CKP period-correction factors 1–16 | 16 | `r[4..35]` | — |
| `BA/90/9110` | `BA 90 00 00 91 10` | CKP period-correction factors 17–32 | 16 | `r[4..35]` | — |
| `BA/90/9120` | `BA 90 00 00 91 20` | CKP period-correction factors 33–48 | 16 | `r[4..35]` | — |
| `BA/90/9130` | `BA 90 00 00 91 30` | CKP factors 49–60; idle/A-F/request adaptations; peak RPM | 16 | `r[4..35]` | — |
| `BA/90/9140` | `BA 90 00 00 91 40` | Overspeed count; learning flags; idle/knock references; oil-model state | 16 | `r[4..35]` | — |
| `BA/90/9150` | `BA 90 00 00 91 50` | Oil history; pedal/cam/throttle references; starter total | 16 | `r[4..35]` | — |
| `BA/90/9160` | `BA 90 00 00 91 60` | Starter service total; battery-history bytes | 16 | `r[4..35]` | — |
| `BA/90/9170` | `BA 90 00 00 91 70` | Battery history/resistance; inclination/distance/EOP state; checksum | 13 | `r[4..29]` | `r[30..35]` |
| `BA/90/9400` | `BA 90 00 00 94 00` | Diagnostic monitor snapshot header | 6 | `r[4..15]` | `r[16..35]` |
| `BA/90/9500` | `BA 90 00 00 95 00` | Diagnostic monitor snapshot chunk 1 | 16 | `r[4..35]` | — |
| `BA/90/9510` | `BA 90 00 00 95 10` | Diagnostic monitor snapshot chunk 2 | 16 | `r[4..35]` | — |
| `BA/90/9520` | `BA 90 00 00 95 20` | Diagnostic monitor snapshot chunk 3 | 16 | `r[4..35]` | — |
| `BA/90/9530` | `BA 90 00 00 95 30` | Diagnostic monitor snapshot checksum | 1 | `r[4..5]` | `r[6..35]` |
| `BA/90/9700` | `BA 90 00 00 97 00` | Fatal-reset and exception snapshot header | 4 | `r[4..11]` | `r[12..35]` |
| `BA/90/9800` | `BA 90 00 00 98 00` | Fatal-reset reason and exception snapshot chunk 1 | 16 | `r[4..35]` | — |
| `BA/90/9810` | `BA 90 00 00 98 10` | Exception snapshot chunk 2 | 16 | `r[4..35]` | — |

| Command | Request payload (hex) | Extraction (r = full reply) | Field (identity, headers and raw records) | shortname | Conversion / unit (raw = extraction) |
| --- | --- | --- | --- | --- | --- |
| `BA/90/8000` | `BA 90 00 00 80 00` | `U16BE(r[4..5])` | Current-loop parameter checksum | — | `(sum(U16BE(r[4+2*i..5+2*i]), i=0..8) & 0xFFFF) == 0` |
| `BA/90/8000` | `BA 90 00 00 80 00` | `U16BE(r[6..7])` | Drive pulley current-sense divisor | — | Calibration divisor. Effective gain = `32768 / max(raw, 1)`. |
| `BA/90/8000` | `BA 90 00 00 80 00` | `S16BE(r[8..9])` | Drive pulley current-sense offset | — | Effective offset = `clamp(2*raw, -32768, 32767)` feedback counts. |
| `BA/90/8000` | `BA 90 00 00 80 00` | `U16BE(r[10..11])` | Driven pulley current-sense divisor | — | Calibration divisor. Effective gain = `32768 / max(raw, 1)`. |
| `BA/90/8000` | `BA 90 00 00 80 00` | `S16BE(r[12..13])` | Driven pulley current-sense offset | — | Effective offset = `clamp(2*raw, -32768, 32767)` feedback counts. |
| `BA/90/8000` | `BA 90 00 00 80 00` | `U16BE(r[14..15])` | Pressure control current-sense divisor | — | Calibration divisor. Effective gain = `32768 / max(raw, 1)`. |
| `BA/90/8000` | `BA 90 00 00 80 00` | `S16BE(r[16..17])` | Pressure control current-sense offset | — | Effective offset = `clamp(2*raw, -32768, 32767)` feedback counts. |
| `BA/90/8000` | `BA 90 00 00 80 00` | `U16BE(r[18..19])` | LCC linear current-sense divisor | — | Calibration divisor. Effective gain = `32768 / max(raw, 1)`. |
| `BA/90/8000` | `BA 90 00 00 80 00` | `S16BE(r[20..21])` | LCC linear current-sense offset | — | Effective offset = `clamp(2*raw, -32768, 32767)` feedback counts. |
| `BA/90/8100` | `BA 90 00 00 81 00` | `r[4]` | Retained serial comparison byte 0 | — | Raw U8. |
| `BA/90/8100` | `BA 90 00 00 81 00` | `r[5]` | Retained serial comparison byte 1 | — | Raw U8. |
| `BA/90/8100` | `BA 90 00 00 81 00` | `r[6]` | Retained serial comparison byte 2 | — | Raw U8. |
| `BA/90/8100` | `BA 90 00 00 81 00` | `r[7]` | Serial comparison checksum | — | `(sum(r[4..7]) & 0xFF) == 0` |
| `BA/90/8200` | `BA 90 00 00 82 00` | `r[4]` | PCM part number high metadata byte | — | Raw U8. |
| `BA/90/8200` | `BA 90 00 00 82 00` | `r[5]` | PCM part number checksum | — | `(sum(r[5..19]) & 0xFF) == 0xFF` |
| `BA/90/8200` | `BA 90 00 00 82 00` | `r[6..19]` | PCM part number | PARTNO | 14 stored character bytes. |
| `BA/90/8300` | `BA 90 00 00 83 00` | `r[4]` | PCM serial number high metadata byte | — | Raw U8. |
| `BA/90/8300` | `BA 90 00 00 83 00` | `r[5]` | PCM serial number checksum | — | `(sum(r[5..17]) & 0xFF) == 0xFF` |
| `BA/90/8300` | `BA 90 00 00 83 00` | `r[6..16]` | PCM serial number | — | 11 stored character bytes. |
| `BA/90/8300` | `BA 90 00 00 83 00` | `r[17]` | PCM serial number trailing stored byte | — | Raw U8 included in the checksum. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `r[4]` | Commit header marker | — | 01 for the learning/runtime format. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `r[5]` | Common child bank number | — | Raw U8 physical data-flash bank. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[6..7])` | Learning/runtime format version | — | 0503 for this layout. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[8..9])` | Physical slot for 9100 | — | Raw U16 slot index. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[10..11])` | Physical slot for 9110 | — | Raw U16 slot index. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[12..13])` | Physical slot for 9120 | — | Raw U16 slot index. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[14..15])` | Physical slot for 9130 | — | Raw U16 slot index. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[16..17])` | Physical slot for 9140 | — | Raw U16 slot index. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[18..19])` | Physical slot for 9150 | — | Raw U16 slot index. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[20..21])` | Physical slot for 9160 | — | Raw U16 slot index. |
| `BA/90/9000` | `BA 90 00 00 90 00` | `U16BE(r[22..23])` | Physical slot for 9170 | — | Raw U16 slot index. |
| `BA/90/9400` | `BA 90 00 00 94 00` | `r[4]` | Monitor snapshot format marker | — | 01 for this layout. |
| `BA/90/9400` | `BA 90 00 00 94 00` | `r[5]` | Common child bank number | — | Raw bank number. |
| `BA/90/9400` | `BA 90 00 00 94 00` | `U16BE(r[6..7])` | Diagnostic-monitor header prefix word 1 | — | Raw U16. |
| `BA/90/9400` | `BA 90 00 00 94 00` | `U16BE(r[8..9])` | Physical slot for 9500 | — | Raw U16 slot index. |
| `BA/90/9400` | `BA 90 00 00 94 00` | `U16BE(r[10..11])` | Physical slot for 9510 | — | Raw U16 slot index. |
| `BA/90/9400` | `BA 90 00 00 94 00` | `U16BE(r[12..13])` | Physical slot for 9520 | — | Raw U16 slot index. |
| `BA/90/9400` | `BA 90 00 00 94 00` | `U16BE(r[14..15])` | Physical slot for 9530 | — | Raw U16 slot index. |
| `BA/90/8400` | `BA 90 00 00 84 00` | `U16BE(r[4+2*i..5+2*i]), i=0..15` | Unknown record data | — | 16 raw words. Individual field layout unknown. |
| `BA/90/8500` | `BA 90 00 00 85 00` | `U16BE(r[4+2*i..5+2*i]), i=0..15` | Unknown record data | — | 16 raw words. Individual field layout unknown. |
| `BA/90/9500` | `BA 90 00 00 95 00` | `U16BE(r[4..5])` | Ignition Cycle Counter(Engine Start) | IGCYCLE | raw cycles. Rolls from 65534 to 0. |
| `BA/90/9500` | `BA 90 00 00 95 00` | `U16BE(r[6..7])` | Qualified monitor cycle count | — | raw cycles. Rolls from 65534 to 0. |
| `BA/90/9500` | `BA 90 00 00 95 00` | `U16BE(r[8+2*i..9+2*i]), i=0..13` | Diagnostic monitor snapshot chunk 1 | — | Preserve undecoded bytes. |
| `BA/90/9510` | `BA 90 00 00 95 10` | `r[4..15]` | Diagnostic monitor snapshot chunk 2 | — | Preserve undecoded bytes. |
| `BA/90/9510` | `BA 90 00 00 95 10` | `U16BE(r[16..17])` | P0125 ECT slow-response result count | — | raw counts. 0xFFFF when monitor is disabled. |
| `BA/90/9510` | `BA 90 00 00 95 10` | `U16BE(r[18..19])` | P0125 ECT slow-response eligibility count | CDRB86G | raw counts. 0xFFFF when monitor is disabled. |
| `BA/90/9510` | `BA 90 00 00 95 10` | `U16BE(r[20..21])` | P0116 ECT range/performance result count | — | raw counts. 0xFFFF when monitor is disabled. |
| `BA/90/9510` | `BA 90 00 00 95 10` | `U16BE(r[22..23])` | P0116 ECT range/performance eligibility count | — | raw counts. 0xFFFF when monitor is disabled. |
| `BA/90/9510` | `BA 90 00 00 95 10` | `r[24..35]` | Diagnostic monitor snapshot chunk 2 | — | Preserve undecoded bytes. |
| `BA/90/9520` | `BA 90 00 00 95 20` | `S32BE(r[32..35])` | Retained monitor distance | — | raw km in the normal nonnegative range. |
| `BA/90/9520` | `BA 90 00 00 95 20` | `U16BE(r[4+2*i..5+2*i]), i=0..13` | Diagnostic monitor snapshot chunk 3 | — | Preserve undecoded bytes. |
| `BA/90/9700` | `BA 90 00 00 97 00` | `r[4]` | Fatal snapshot format marker | — | 01 for this layout. |
| `BA/90/9700` | `BA 90 00 00 97 00` | `r[5]` | Common child bank number | — | Raw bank number. |
| `BA/90/9700` | `BA 90 00 00 97 00` | `U16BE(r[6..7])` | Fatal snapshot header word 1 | — | Raw word. |
| `BA/90/9700` | `BA 90 00 00 97 00` | `U16BE(r[8..9])` | Physical slot for 9800 | — | Raw slot index. |
| `BA/90/9700` | `BA 90 00 00 97 00` | `U16BE(r[10..11])` | Physical slot for 9810 | — | Raw slot index. |
| `BA/90/9800` | `BA 90 00 00 98 00` | `r[4]` | Fatal reset reason | — | Hexadecimal selector for the saved context. |
| `BA/90/9800` | `BA 90 00 00 98 00` | `U32BE(r[6..9])` | RAM ECC fault address | — | Raw byte address when reason is C1. |
| `BA/90/9800` | `BA 90 00 00 98 00` | `U32BE(r[10..13])` | RAM word read at ECC fault | — | Raw 32-bit word when reason is C1. |
| `BA/90/9800` + `BA/90/9810` | `BA 90 00 00 98 00` + `BA 90 00 00 98 10` | `r1[6..35] + r2[4..5]` | Saved 32-byte fault context | — | Raw bytes when reason is C2. r1 = BA/90/9800, r2 = BA/90/9810. |
| `BA/90/9800` | `BA 90 00 00 98 00` | `U32BE(r[6..9])` | Exception stack word 0 | — | Raw 32-bit word when reason is D1, D3-D5, DE, DF, E2, or E3. |
| `BA/90/9800` | `BA 90 00 00 98 00` | `U32BE(r[10..13])` | Exception stack word 1 | — | Raw 32-bit word when reason is D1, D3-D5, DE, DF, E2, or E3. |
| `BA/90/9800` | `BA 90 00 00 98 00` | `r[5..35]` | Fatal-reset reason and exception snapshot chunk 1 | — | Preserve undecoded bytes. |
| `BA/90/9810` | `BA 90 00 00 98 10` | `U16BE(r[4+2*i..5+2*i]), i=0..15` | Exception snapshot chunk 2 | — | 16 raw words. Individual field layout unknown. |
| `BA/90/9530` | `BA 90 00 00 95 30` | `U16BE(r[4..5])` | Diagnostic-monitor snapshot checksum | — | Sum of the 48 words from 9500, 9510 and 9520 plus this word is FFFF modulo 65536. |

| Command | Request payload (hex) | Extraction (r = full reply) | Field (CKP correction) | shortname | Conversion / unit (raw = extraction) |
| --- | --- | --- | --- | --- | --- |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[4..5])` | CKP period-correction factor 1 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[6..7])` | CKP period-correction factor 2 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[8..9])` | CKP period-correction factor 3 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[10..11])` | CKP period-correction factor 4 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[12..13])` | CKP period-correction factor 5 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[14..15])` | CKP period-correction factor 6 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[16..17])` | CKP period-correction factor 7 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[18..19])` | CKP period-correction factor 8 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[20..21])` | CKP period-correction factor 9 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[22..23])` | CKP period-correction factor 10 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[24..25])` | CKP period-correction factor 11 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[26..27])` | CKP period-correction factor 12 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[28..29])` | CKP period-correction factor 13 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[30..31])` | CKP period-correction factor 14 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[32..33])` | CKP period-correction factor 15 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9100` | `BA 90 00 00 91 00` | `U16BE(r[34..35])` | CKP period-correction factor 16 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[4..5])` | CKP period-correction factor 17 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[6..7])` | CKP period-correction factor 18 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[8..9])` | CKP period-correction factor 19 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[10..11])` | CKP period-correction factor 20 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[12..13])` | CKP period-correction factor 21 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[14..15])` | CKP period-correction factor 22 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[16..17])` | CKP period-correction factor 23 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[18..19])` | CKP period-correction factor 24 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[20..21])` | CKP period-correction factor 25 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[22..23])` | CKP period-correction factor 26 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[24..25])` | CKP period-correction factor 27 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[26..27])` | CKP period-correction factor 28 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[28..29])` | CKP period-correction factor 29 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[30..31])` | CKP period-correction factor 30 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[32..33])` | CKP period-correction factor 31 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9110` | `BA 90 00 00 91 10` | `U16BE(r[34..35])` | CKP period-correction factor 32 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[4..5])` | CKP period-correction factor 33 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[6..7])` | CKP period-correction factor 34 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[8..9])` | CKP period-correction factor 35 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[10..11])` | CKP period-correction factor 36 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[12..13])` | CKP period-correction factor 37 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[14..15])` | CKP period-correction factor 38 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[16..17])` | CKP period-correction factor 39 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[18..19])` | CKP period-correction factor 40 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[20..21])` | CKP period-correction factor 41 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[22..23])` | CKP period-correction factor 42 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[24..25])` | CKP period-correction factor 43 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[26..27])` | CKP period-correction factor 44 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[28..29])` | CKP period-correction factor 45 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[30..31])` | CKP period-correction factor 46 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[32..33])` | CKP period-correction factor 47 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9120` | `BA 90 00 00 91 20` | `U16BE(r[34..35])` | CKP period-correction factor 48 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[4..5])` | CKP period-correction factor 49 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[6..7])` | CKP period-correction factor 50 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[8..9])` | CKP period-correction factor 51 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[10..11])` | CKP period-correction factor 52 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[12..13])` | CKP period-correction factor 53 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[14..15])` | CKP period-correction factor 54 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[16..17])` | CKP period-correction factor 55 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[18..19])` | CKP period-correction factor 56 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[20..21])` | CKP period-correction factor 57 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[22..23])` | CKP period-correction factor 58 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[24..25])` | CKP period-correction factor 59 | — | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[26..27])` | CKP period-correction factor 60 | — | UQ15; `raw/32768.0` ratio |

| Command | Request payload (hex) | Extraction (r = full reply) | Field (learned values and oil model) | shortname | Conversion / unit (raw = extraction) |
| --- | --- | --- | --- | --- | --- |
| `BA/90/9130` | `BA 90 00 00 91 30` | `S16BE(r[28..29])` | Idle Air Block Lean | IAIRBL | S16; `raw/10.0` L/min |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[30..31])` | KAFREFX | KAFREFX | UQ15; `raw/32768.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[32..33])` | DBW Stuck Ratio | KTHC_B | UQ16; `raw/65536.0` ratio |
| `BA/90/9130` | `BA 90 00 00 91 30` | `U16BE(r[34..35])` | Max Eng Spd | NEORMAX | Whole RPM in valid producer domain; raw U16 storage |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[4..5])` | Over Eng Tm | CF119S | U16 qualifying calls; nominal seconds = `raw/100.0` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[6..7])` | packed learning status | — | bits 0 CKP, 1 A/F, 2 idle-air completion; preserve `raw & 0xFFF8` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `S16BE(r[8..9])` | filtered idle air axis | — | S16; `raw/10.0` L/min |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[10..11])` | Knock Ctrl | KIGKREF | UQ15; `raw/32768.0` ratio |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[12..13])` | Knock Ctrl EGR | KIGKREFE | UQ15; `raw/32768.0` ratio |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[14..15])` | tbn thermal loss view | — | U16 internal model count; physical unit UNKNOWN |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[16..17])` | tbn nox squared loss view | — | U16 internal model count; physical unit UNKNOWN |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[18..19])` | tbn additive loss view | — | U16 internal model count; physical unit UNKNOWN |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[20..21])` | oit antioxidant thermal exposure view | — | U16; internal exposure seed = `raw/64.0` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[22..23])` | oit antioxidant nox squared exposure view | — | U16; internal exposure seed = `raw/64.0` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[24..25])` | oit antioxidant fuel squared exposure view | — | U16; internal exposure seed = `raw/64.0` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[26..27])` | oit peroxide thermal exposure view | — | U16; internal exposure seed = `raw/64.0` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[28..29])` | oit peroxide nox squared exposure view | — | U16; internal exposure seed = `raw/64.0` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[30..31])` | oit peroxide fuel squared exposure view | — | U16; internal exposure seed = `raw/64.0` |
| `BA/90/9140` | `BA 90 00 00 91 40` | `S16BE(r[32..33])` | modeled fuel dilution view | — | S16 Q10; `raw/1024.0` internal model coordinate |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[34..35])` | oil model distance | — | U16; `raw` km |
| `BA/90/9140` | `BA 90 00 00 91 40` | `(r[7] >> 0) & 1` | CKP Pulser F/B Learn | KCREND | Boolean (0 or 1). |
| `BA/90/9140` | `BA 90 00 00 91 40` | `(r[7] >> 1) & 1` | af reference complete | — | Boolean (0 or 1). |
| `BA/90/9140` | `BA 90 00 00 91 40` | `(r[7] >> 2) & 1` | Idle Learn (Idle air) | IALSTAT | Boolean (0 or 1). |
| `BA/90/9140` | `BA 90 00 00 91 40` | `U16BE(r[6..7]) & 0xFFF8` | learning status unpopulated bits | — | raw bits; unpopulated by builder |

| Command | Request payload (hex) | Extraction (r = full reply) | Field (oil history, references and starter) | shortname | Conversion / unit (raw = extraction) |
| --- | --- | --- | --- | --- | --- |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[4..5])` | oil history overdue distance and minimum tbn | — | two U8 codes: overdue distance, minimum TBN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[6..7])` | oil history minimum oit and maximum dilution | — | two U8 codes: minimum OIT, maximum dilution |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[8..9])` | oil history before reset tbn and oit | — | two U8 codes: before-reset TBN, OIT |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[10..11])` | oil history before reset distance and reset count | — | `low16((u8_distance << 8) + s8_reset_count)`; bytes 6/7 are independent unsigned codes only for reset count 0..127 |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[12..13])` | app closed a reference | — | U16 native ADC/reference counts; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[14..15])` | app closed b reference | — | U16 native ADC/reference counts; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[16..17])` | app initial a reference | — | U16 native ADC/reference counts; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[18..19])` | app initial b reference | — | U16 native ADC/reference counts; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[20..21])` | app companion a reference | — | U16 native ADC/reference counts; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[22..23])` | app companion b reference | — | U16 native ADC/reference counts; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[24..25])` | app learn valid word | — | bit 0 valid; preserve `raw & 0xFFFE` |
| `BA/90/9150` | `BA 90 00 00 91 50` | `S16BE(r[26..27])` | VTCZP | VTCZP | S16; nominal reference degrees = `raw/10.0` |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[28..29])` | closed throttle an12 reference | — | U16 storage; normal 12-bit ADC counts; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[30..31])` | closed throttle an45 reference | — | U16 storage; normal 10-bit ADC counts after ADC >> 2; volts UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[32..33])` | starter event total high | — | high 16 bits of U32 gated release-event count |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[34..35])` | starter event total low | — | low 16 bits of U32 gated release-event count |
| `BA/90/9150` | `BA 90 00 00 91 50` | `(r[25] >> 0) & 1` | app learn valid | — | Boolean (0 or 1). |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U16BE(r[24..25]) & 0xFFFE` | app valid unpopulated bits | — | raw bits; unpopulated by builder |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[4]` | oil overdue distance coarse | — | 256 km/count |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[5]` | oil minimum tbn high | — | raw high-byte TBN model code |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[6]` | oil minimum oit high | — | raw high-byte OIT model code |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[7]` | oil maximum dilution coarse | — | raw coarse code from signed model view / 128; percent UNKNOWN |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[8]` | oil before reset tbn high | — | raw high-byte TBN model code |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[9]` | oil before reset oit high | — | raw high-byte OIT model code |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[10]` | oil before reset distance stored | — | 256 km/count in normal reset-count domain |
| `BA/90/9150` | `BA 90 00 00 91 50` | `r[11]` | oil accepted reset count raw | — | raw U8; normal 0..127 |
| `BA/90/9150` | `BA 90 00 00 91 50` | `U32BE(r[32..35])` | Idle Stop Starter Counter | ISINFSRCNT | U32 saturated gated event total |

| Command | Request payload (hex) | Extraction (r = full reply) | Field (starter, battery and inclination) | shortname | Conversion / unit (raw = extraction) |
| --- | --- | --- | --- | --- | --- |
| `BA/90/9160` | `BA 90 00 00 91 60` | `U16BE(r[4..5])` | starter relay service total high | — | high 16 bits of U32 gated release-event count |
| `BA/90/9160` | `BA 90 00 00 91 60` | `U16BE(r[6..7])` | starter relay service total low | — | low 16 bits of U32 gated release-event count |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[8..9]` | Battery-history bytes 0..1 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[10..11]` | Battery-history bytes 2..3 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[12..13]` | Battery-history bytes 4..5 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[14..15]` | Battery-history bytes 6..7 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[16..17]` | Battery-history bytes 8..9 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[18..19]` | Battery-history bytes 10..11 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[20..21]` | Battery-history bytes 12..13 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[22..23]` | Battery-history bytes 14..15 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[24..25]` | Battery-history bytes 16..17 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[26..27]` | Battery-history bytes 18..19 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[28..29]` | Battery-history bytes 20..21 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[30..31]` | Battery-history bytes 22..23 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[32..33]` | Battery-history bytes 24..25 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `r[34..35]` | Battery-history bytes 26..27 | — | 2 raw bytes |
| `BA/90/9160` | `BA 90 00 00 91 60` | `U32BE(r[4..7])` | starter relay service total | — | U32 saturated gated event total |
| `BA/90/9170` | `BA 90 00 00 91 70` | `r[4..5]` | Battery-history bytes 28..29 | — | 2 raw bytes |
| `BA/90/9170` | `BA 90 00 00 91 70` | `r[6..7]` | Battery-history bytes 30..31 | — | 2 raw bytes |
| `BA/90/9170` | `BA 90 00 00 91 70` | `r[8..9]` | Battery-history bytes 32..33 | — | 2 raw bytes |
| `BA/90/9170` | `BA 90 00 00 91 70` | `r[10..11]` | Battery-history bytes 34..35 | — | 2 raw bytes |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[12..13])` | battery resistance reference | — | U16; nominal mΩ = `raw/100.0` |
| `BA/90/9170` | `BA 90 00 00 91 70` | `S16BE(r[14..15])` | inclination initial zero reference | — | `raw / 4096.0` g (multiples of standard gravitational acceleration). |
| `BA/90/9170` | `BA 90 00 00 91 70` | `S16BE(r[16..17])` | inclination correction reference | — | `raw / 4096.0` g (multiples of standard gravitational acceleration). |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[18..19])` | integrated distance tally | — | U16; nominal km = `(uint32_t)raw*10u` |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[20..21])` | packed inclination eop state | — | bits 0 initial learned, 1 correction valid, 2 EOPFULL; preserve `raw & 0xFFF8` |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[22..23])` | clutch event first distance | — | Nominal 5*raw km relative to the event-distance reset |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[24..25])` | pulley ratio event first distance | — | Nominal 5*raw km relative to the event-distance reset |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[26..27])` | pulley ratio event latest distance | — | Nominal 5*raw km relative to the event-distance reset |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[28..29])` | snapshot checksum | — | (0xFFFF - sum(BE16 words 0x9100..0x9160 w0..w15 and 0x9170 w0..w11)) modulo 65536 |
| `BA/90/9170` | `BA 90 00 00 91 70` | `(r[21] >> 0) & 1` | Status of Initial Inclination Sensor Learn | SLOPE0 | Boolean (0 or 1). |
| `BA/90/9170` | `BA 90 00 00 91 70` | `(r[21] >> 1) & 1` | inclination correction valid | — | Boolean (0 or 1). |
| `BA/90/9170` | `BA 90 00 00 91 70` | `(r[21] >> 2) & 1` | The internal oil filling state of an electric oil pump. (For factories) | EOPFULL | Boolean (0 or 1). |
| `BA/90/9170` | `BA 90 00 00 91 70` | `U16BE(r[20..21]) & 0xFFF8` | inclination eop unpopulated bits | — | raw bits; unpopulated by builder |

| History row (oldest first) | Command | Request payloads (hex, in r1/r2 order) | Extraction (r = full reply) | Field | shortname | Conversion / unit |
| --- | --- | --- | --- | --- | --- | --- |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `r[8] + ((r[9] & 0x03) << 8)` | Current A | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 20460 mA |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[9] >> 2) + ((r[10] & 0x0F) << 6)` | Current B | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[10] >> 4) + ((r[11] & 0x3F) << 4)` | Current C | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[11] >> 6) + ((r[12] & 0xFF) << 2)` | Current D | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[13] + (r[14] << 8)) & 0x3FFF` | Duration | — | HDS duration scales disagree. Dedicated display uses q h. Generic CANFI uses q / 100 h. |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[14] >> 6) & 1` | Duration bit 14 | — | Raw bit; sender meaning UNKNOWN |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[14] >> 7) & 1` | Duration bit 15 | — | Raw bit; sender meaning UNKNOWN |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `((r[15] & 0x7F) << 8) + r[16]` | Discharge | — | HDS phase 2 uses q / 100 Ah. Sender meaning unknown. |
| 0 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[15] >> 7) & 1` | Discharge bit 15 | — | Raw bit. Meaning unknown. |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `r[17] + ((r[18] & 0x03) << 8)` | Current A | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 20460 mA |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[18] >> 2) + ((r[19] & 0x0F) << 6)` | Current B | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[19] >> 4) + ((r[20] & 0x3F) << 4)` | Current C | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[20] >> 6) + ((r[21] & 0xFF) << 2)` | Current D | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[22] + (r[23] << 8)) & 0x3FFF` | Duration | — | HDS duration scales disagree. Dedicated display uses q h. Generic CANFI uses q / 100 h. |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[23] >> 6) & 1` | Duration bit 14 | — | Raw bit; sender meaning UNKNOWN |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[23] >> 7) & 1` | Duration bit 15 | — | Raw bit; sender meaning UNKNOWN |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `((r[24] & 0x7F) << 8) + r[25]` | Discharge | — | HDS phase 2 uses q / 100 Ah. Sender meaning unknown. |
| 1 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[24] >> 7) & 1` | Discharge bit 15 | — | Raw bit. Meaning unknown. |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `r[26] + ((r[27] & 0x03) << 8)` | Current A | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 20460 mA |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[27] >> 2) + ((r[28] & 0x0F) << 6)` | Current B | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[28] >> 4) + ((r[29] & 0x3F) << 4)` | Current C | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[29] >> 6) + ((r[30] & 0xFF) << 2)` | Current D | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[31] + (r[32] << 8)) & 0x3FFF` | Duration | — | HDS duration scales disagree. Dedicated display uses q h. Generic CANFI uses q / 100 h. |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[32] >> 6) & 1` | Duration bit 14 | — | Raw bit; sender meaning UNKNOWN |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[32] >> 7) & 1` | Duration bit 15 | — | Raw bit; sender meaning UNKNOWN |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `((r[33] & 0x7F) << 8) + r[34]` | Discharge | — | HDS phase 2 uses q / 100 Ah. Sender meaning unknown. |
| 2 | `BA/90/9160` | `BA 90 00 00 91 60` | `(r[33] >> 7) & 1` | Discharge bit 15 | — | Raw bit. Meaning unknown. |
| 3 | `BA/90/9160` + `BA/90/9170` | `BA 90 00 00 91 60` + `BA 90 00 00 91 70` | `r1[35] + ((r2[4] & 0x03) << 8)` | Current A | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 20460 mA |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `(r[4] >> 2) + ((r[5] & 0x0F) << 6)` | Current B | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `(r[5] >> 4) + ((r[6] & 0x3F) << 4)` | Current C | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `(r[6] >> 6) + ((r[7] & 0xFF) << 2)` | Current D | — | 10-bit q; 0x3FF means No Data; HDS phase 1 (floor(q*999474/1023)-999474)/100 mA; phase 2 20*q - 15340 mA |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `(r[8] + (r[9] << 8)) & 0x3FFF` | Duration | — | HDS duration scales disagree. Dedicated display uses q h. Generic CANFI uses q / 100 h. |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `(r[9] >> 6) & 1` | Duration bit 14 | — | Raw bit; sender meaning UNKNOWN |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `(r[9] >> 7) & 1` | Duration bit 15 | — | Raw bit; sender meaning UNKNOWN |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `((r[10] & 0x7F) << 8) + r[11]` | Discharge | — | HDS phase 2 uses q / 100 Ah. Sender meaning unknown. |
| 3 | `BA/90/9170` | `BA 90 00 00 91 70` | `(r[10] >> 7) & 1` | Discharge bit 15 | — | Raw bit. Meaning unknown. |
