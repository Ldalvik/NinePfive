| mode/header | Request payload (hex) | Full name | shortname | Layer |
| --- | --- | --- | --- | --- |
| B9/00 | `B9 00` | Negative-only selector | — | PCM |
| B9/01 | `B9 01 02` | Selector without a positive-response path | — | PCM |
| B9/02 | `B9 02 02` | Read transmission snapshot structure | OB_SNAPSHOT_STRUCTURE | PCM |
| B9/03 | `B9 03 02 <bank>` | Read transmission snapshot bank header | OB_SNAPSHOT_BANK | PCM |
| B9/04 | `B9 04 02 <bank> 00` | Read transmission snapshot block information | OB_SNAPSHOT_BLOCK | PCM |
| B9/05 | `B9 05 02 <bank> 00 00 <record>` | Read transmission snapshot history record | OB_SNAPSHOT_RECORD | PCM |
| B9/06 | `B9 06` | Read enhanced snapshot bank/type directory | OBS_BANK_NUM_TRIG_MODE | PCM |
| B9/07 | `B9 07 00 <mode>` | Set primary on-board snapshot trigger mode | OBS_TRIGGER_MODE_ENH | PCM |
| B9/08 | `B9 08 <bank>` | Read saved enhanced snapshot bank information | OBS_BANK_ENH | PCM |
| B9/09 | `B9 09 <bank> <block>` | Read enhanced snapshot block information | OBS_BLOCK_ENH | PCM |
| B9/0A | `B9 0A <bank> <block> <record_hi> <record_lo>` | Read enhanced snapshot record | OBS_RECORD_ENH | PCM |

| Command | Request payload (hex) | Response payload (hex) | Bytes, excluding ISO-TP | Parameters |
| --- | --- | --- | --- | --- |
| B9/00 | `B9 00` | `7F B9 12` | 3 | Negative response. Two-byte minimum. |
| B9/01 | `B9 01 02` | `7F B9 12` | 3 | Negative response. Three-byte minimum. Bare `B9 01` returns `7F B9 13`. |
| B9/02 | `B9 02 02` | `F9 02 02 01 01 03 01` | 7 | 3 banks, 1 block per bank. |
| B9/03 | `B9 03 02 <bank>` | `F9 03 02 <status> <packet_DCT_ID_BE16> <data16>` | 22 | Bank 00-02. Returned header packet/DCT ID selects the data layout (0F41 or 0F42). Returns stored data even when the bank is empty. |
| B9/04 | `B9 04 02 <bank> 00` | `F9 04 02 <record_count_BE16> <packet_DCT_ID_BE16>` | 7 | Bank 00-02; block 00. Use the returned record count and packet/DCT ID: 0600/0603 (CANEAT), 0650/0651 (CANCVT). |
| B9/05 | `B9 05 02 <bank> 00 00 <record>` | `F9 05 02 <spacing_BE16> <data15>` | 20 | Bank 00-02; record below the B9/04 count, newest first. Spacing = stored token × 10. Nominal milliseconds for the documented uninterrupted 0651 recorder cadence. |
| B9/06 | `B9 06` | `F9 06 00 <current_type> 01 04 02 03` | 8 | Bank/type pairs: 00/current type, 01/04, 02/03. |
| B9/07 | `B9 07 00 <mode>` | `F9 07 00 <mode>` | 4 | Mode 00-02. |
| B9/08 | `B9 08 <bank>` | `F9 08 <saved_type> <bank> <block_count>` | 5 | Bank 00-02. Type FF makes HDS omit the bank; counts 24/5/7. |
| B9/09 | `B9 09 <bank> <block>` | `F9 09 <bank> <block> <record_count_BE16> <packet_DCT_ID_BE16>` | 8 | Bank/block ranges below. Updates the record count used by B9/0A. |
| B9/0A | `B9 0A <bank> <block> <record_hi> <record_lo>` | `F9 0A <spacing_BE16> <availability6> <data/padding48>` | 58 | BE16 record below the B9/09 count. Fixed record ignores the admitted index; ring record reads backward from the saved cursor. Ring spacing is 0 for record=count-1, otherwise the block token; fixed spacing is 0. |

| Command | Request payload (hex) | Extraction (r = full reply, without ISO-TP) | Field | Value / unit |
| --- | --- | --- | --- | --- |
| `B9/02` | `B9 02 02` | `r[3..4]` | Fixed structure bytes | 01 01 |
| `B9/02` | `B9 02 02` | `r[5]` | Bank count (HDS) | 3 |
| `B9/02` | `B9 02 02` | `r[6]` | Blocks per bank (HDS) | 1 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | `r[3]` | Saved status | See status values below. |
| `B9/03/<bank>` | `B9 03 02 <bank>` | `U16BE(r[4..5])` | Header packet/DCT ID | Known IDs are 0F41 and 0F42. |
| `B9/03/<bank>` | `B9 03 02 <bank>` | `r[6..21]` | Header record | Choose the layout by the returned packet ID. |
| `B9/04/<bank>` | `B9 04 02 <bank> 00` | `U16BE(r[3..4])` | Record count | Use the returned count. A documented count is 15. |
| `B9/04/<bank>` | `B9 04 02 <bank> 00` | `U16BE(r[5..6])` | Sample packet/DCT ID | Known IDs are 0600/0603 (CANEAT) and 0650/0651 (CANCVT). |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | `U16BE(r[3..4])` | Spacing | Stored token × 10. For the documented uninterrupted 0651 recorder cadence, q / 1000 s. |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | `r[5..19]` | Transmission record | Use the packet ID from B9/04 for this bank. |
| `B9/06` | `B9 06` | `r[2], r[4], r[6]` | Bank numbers | 00, 01, 02 |
| `B9/06` | `B9 06` | `r[3], r[5], r[7]` | Current types | Bank 00 type varies. Bank 01 type is 04 and bank 02 type is 03. |
| `B9/07/<mode>` | `B9 07 00 <mode>` | `r[2]` | Bank | 00 |
| `B9/07/<mode>` | `B9 07 00 <mode>` | `r[3]` | Accepted trigger mode | Echo of requested mode. |
| `B9/08/<bank>` | `B9 08 <bank>` | `r[2]` | Saved snapshot type | FF for an omitted bank. |
| `B9/08/<bank>` | `B9 08 <bank>` | `r[3]` | Bank number | Echo of requested bank. |
| `B9/08/<bank>` | `B9 08 <bank>` | `r[4]` | Block count | 24, 5 or 7 by bank. |
| `B9/09/<bank>/<block>` | `B9 09 <bank> <block>` | `r[2], r[3]` | Bank and block | Echo of request selectors. |
| `B9/09/<bank>/<block>` | `B9 09 <bank> <block>` | `U16BE(r[4..5])` | Record count | B9/0A record index must be below this count. |
| `B9/09/<bank>/<block>` | `B9 09 <bank> <block>` | `U16BE(r[6..7])` | Enhanced packet/DCT ID | Selects the B9/0A field layout. |
| `B9/0A/<bank>/<block>/<record>` | `B9 0A <bank> <block> <record_hi> <record_lo>` | `U16BE(r[2..3])` | Spacing | Milliseconds in HDS. Divide by 1000 for seconds. Fixed records and the oldest ring record return 0. |
| `B9/0A/<bank>/<block>/<record>` | `B9 0A <bank> <block> <record_hi> <record_lo>` | `r[4..9]` | Sample-byte availability bitmap | For sample byte `r[10+i]` (`i = 0..47`), its bit is `(r[4 + i//8] >> (7 - i%8)) & 1`. HDS uses record 0 to select fields for the block. A set bit at the field's starting byte makes it eligible. |
| `B9/0A/<bank>/<block>/<record>` | `B9 0A <bank> <block> <record_hi> <record_lo>` | `r[j-1] & mask` | HDS bit-field availability | For a documented HDS bit field starting at reply byte `j`, HDS also requires this result to be nonzero. Use the same bit mask as the field. |
| `B9/0A/<bank>/<block>/0000` | `B9 0A <bank> <block> 00 00` | `r[4..9] = 00 00 00 00 00 00` | Inactive packet | HDS treats an all-zero bitmap in record 0 as inactive. |
| `B9/0A/<bank>/<block>/<record>` | `B9 0A <bank> <block> <record_hi> <record_lo>` | `r[10..57]` | Sample and zero padding | Match the bank, block, and returned B9/09 packet ID to a documented layout. |

| Command | Request payload (hex) | Packet/DCT ID | Value byte | Per-bit availability byte | HDS field bits |
| --- | --- | --- | --- | --- | --- |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[46]` | `r[45]` | 0..7 |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[48]` | `r[47]` | 0, 1, 2, 7 |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[50]` | `r[49]` | 0, 1, 2, 4, 5, 6, 7 |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[52]` | `r[51]` | 0, 1, 2, 3, 4, 5, 6 |
| `B9/0A/00/16/<record>` | `B9 0A 00 16 <record_hi> <record_lo>` | 0344 | `r[16]` | `r[15]` | 0 |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `r[14]` | `r[13]` | 0..7 |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[52]` | `r[51]` | 0, 1, 2, 3, 4 |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `r[23]` | `r[22]` | 0, 1 |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | `r[17]` | `r[16]` | 0, 1 |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `r[13]` | `r[12]` | 0, 1, 2, 3 |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `r[20]` | `r[19]` | 0..7 |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `r[22]` | `r[21]` | 5, 6, 7 |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `r[24]` | `r[23]` | 3, 4, 5, 6, 7 |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `r[26]` | `r[25]` | 0, 1 |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `r[28]` | `r[27]` | 0 |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `r[11]` | `r[10]` | 2, 3, 4, 5, 6, 7 |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `r[13]` | `r[12]` | 3, 4, 5, 6, 7 |

| B9/07 mode (hex) | Request payload (hex) | Meaning |
| --- | --- | --- |
| `00` | `B9 07 00 00` | Default; same DTC-associated path as 01. |
| `01` | `B9 07 00 01` | DTC trigger. |
| `02` | `B9 07 00 02` | Engine Stall trigger. |

| B9/06 or B9/08 type (hex) | Meaning |
| --- | --- |
| `01` | DTC Trigger |
| `02` | Engine Stall |
| `03` | Engine stall 2 |
| `04` | Engine hard to start |
| `FF` | Bank omitted by HDS (B9/08). |

| B9/03 status (hex) | Meaning |
| --- | --- |
| `FF` | Bank presence flag clear. |
| `FE` | Bank present; secondary flag clear. |
| `01` | Bank present; secondary flag set. |

| Enhanced bank (hex) | Snapshot | Block range (hex) | B9/08 block count |
| --- | --- | --- | --- |
| `00` | DTC Trigger / Engine Stall | `00-17` | 24 |
| `01` | Engine hard to start | `00-04` | 5 |
| `02` | Engine stall 2 | `00-06` | 7 |

| Block info command | Block info request (hex) | Record command | Record request (hex) | Packet/DCT ID (hex) | Record type | Documented sample bytes | Nominal spacing | Sample in reply | Zero padding in reply |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `B9/09/00/00` | `B9 09 00 00` | `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | Fixed | 46 | 0 | `r[10..55]` | `r[56..57]` |
| `B9/09/00/01` | `B9 09 00 01` | `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | Fixed | 42 | 0 | `r[10..51]` | `r[52..57]` |
| `B9/09/00/02` | `B9 09 00 02` | `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | Fixed | 42 | 0 | `r[10..51]` | `r[52..57]` |
| `B9/09/00/03` | `B9 09 00 03` | `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | Fixed | 42 | 0 | `r[10..51]` | `r[52..57]` |
| `B9/09/00/04` | `B9 09 00 04` | `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | Fixed | 21 | 0 | `r[10..30]` | `r[31..57]` |
| `B9/09/00/05` | `B9 09 00 05` | `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | Fixed | 36 | 0 | `r[10..45]` | `r[46..57]` |
| `B9/09/00/06` | `B9 09 00 06` | `B9/0A/00/06/<record>` | `B9 0A 00 06 <record_hi> <record_lo>` | 0320 | Ring | 4 | 50 ms | `r[10..13]` | `r[14..57]` |
| `B9/09/00/07` | `B9 09 00 07` | `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | Ring | 43 | 200 ms | `r[10..52]` | `r[53..57]` |
| `B9/09/00/08` | `B9 09 00 08` | `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | Ring | 45 | 1000 ms | `r[10..54]` | `r[55..57]` |
| `B9/09/00/09` | `B9 09 00 09` | `B9/0A/00/09/<record>` | `B9 0A 00 09 <record_hi> <record_lo>` | 0351 | Ring | 5 | 5000 ms | `r[10..14]` | `r[15..57]` |
| `B9/09/00/0A` | `B9 09 00 0A` | `B9/0A/00/0A/<record>` | `B9 0A 00 0A <record_hi> <record_lo>` | 0350 | Ring | 2 | 5000 ms | `r[10..11]` | `r[12..57]` |
| `B9/09/00/0B` | `B9 09 00 0B` | `B9/0A/00/0B/<record>` | `B9 0A 00 0B <record_hi> <record_lo>` | 0361 | Ring | 6 | 5000 ms | `r[10..15]` | `r[16..57]` |
| `B9/09/00/0C` | `B9 09 00 0C` | `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | Ring | 5 | 5000 ms | `r[10..14]` | `r[15..57]` |
| `B9/09/00/0D` | `B9 09 00 0D` | `B9/0A/00/0D/<record>` | `B9 0A 00 0D <record_hi> <record_lo>` | 0390 | Ring | 1 | 100 ms | `r[10]` | `r[11..57]` |
| `B9/09/00/0E` | `B9 09 00 0E` | `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | Ring | 14 | 200 ms | `r[10..23]` | `r[24..57]` |
| `B9/09/00/0F` | `B9 09 00 0F` | `B9/0A/00/0F/<record>` | `B9 0A 00 0F <record_hi> <record_lo>` | 03B0 | Ring | 2 | 1000 ms | `r[10..11]` | `r[12..57]` |
| `B9/09/00/10` | `B9 09 00 10` | `B9/0A/00/10/0000` | `B9 0A 00 10 00 00` | 0389 | Fixed | 2 | 0 | `r[10..11]` | `r[12..57]` |
| `B9/09/00/11` | `B9 09 00 11` | — | — | 0000 | No response builder | 0 | 0 | — | — |
| `B9/09/00/12` | `B9 09 00 12` | `B9/0A/00/12/0000` | `B9 0A 00 12 00 00` | 038A | Fixed | 6 | 0 | `r[10..15]` | `r[16..57]` |
| `B9/09/00/13` | `B9 09 00 13` | `B9/0A/00/13/<record>` | `B9 0A 00 13 <record_hi> <record_lo>` | 0335 | Ring | 4 | 200 ms | `r[10..13]` | `r[14..57]` |
| `B9/09/00/14` | `B9 09 00 14` | `B9/0A/00/14/<record>` | `B9 0A 00 14 <record_hi> <record_lo>` | 03C3 | Ring | 4 | 200 ms | `r[10..13]` | `r[14..57]` |
| `B9/09/00/15` | `B9 09 00 15` | `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | Ring | 15 | 200 ms | `r[10..24]` | `r[25..57]` |
| `B9/09/00/16` | `B9 09 00 16` | `B9/0A/00/16/<record>` | `B9 0A 00 16 <record_hi> <record_lo>` | 0344 | Ring | 1 | 1000 ms | `r[10]` | `r[11..57]` |
| `B9/09/00/17` | `B9 09 00 17` | `B9/0A/00/17/<record>` | `B9 0A 00 17 <record_hi> <record_lo>` | 0346 | Ring | 1 | 1000 ms | `r[10]` | `r[11..57]` |
| `B9/09/01/00` | `B9 09 01 00` | `B9/0A/01/00/0000` | `B9 0A 01 00 00 00` | 04B9 | Fixed | 8 | 0 | `r[10..17]` | `r[18..57]` |
| `B9/09/01/01` | `B9 09 01 01` | `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | Ring | 29 | 100 ms | `r[10..38]` | `r[39..57]` |
| `B9/09/01/02` | `B9 09 01 02` | — | — | 0000 | No response builder | 0 | 0 | — | — |
| `B9/09/01/03` | `B9 09 01 03` | — | — | 0000 | No response builder | 0 | 0 | — | — |
| `B9/09/01/04` | `B9 09 01 04` | `B9/0A/01/04/<record>` | `B9 0A 01 04 <record_hi> <record_lo>` | 04A8 | Ring | 1 | 1000 ms | `r[10]` | `r[11..57]` |
| `B9/09/02/00` | `B9 09 02 00` | `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | Fixed | 17 | 0 | `r[10..26]` | `r[27..57]` |
| `B9/09/02/01` | `B9 09 02 01` | `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | Ring | 27 | 100 ms | `r[10..36]` | `r[37..57]` |
| `B9/09/02/01` | `B9 09 02 01` | `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | Ring | 35 | 100 ms | `r[10..44]` | `r[45..57]` |
| `B9/09/02/02` | `B9 09 02 02` | `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | Ring | 8 | 200 ms | `r[10..17]` | `r[18..57]` |
| `B9/09/02/03` | `B9 09 02 03` | `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | Ring | 5 | 500 ms | `r[10..14]` | `r[15..57]` |
| `B9/09/02/04` | `B9 09 02 04` | `B9/0A/02/04/<record>` | `B9 0A 02 04 <record_hi> <record_lo>` | 04A0 | Ring | 1 | 1000 ms | `r[10]` | `r[11..57]` |
| `B9/09/02/05` | `B9 09 02 05` | `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | Ring | 23 | 100 ms | `r[10..32]` | `r[33..57]` |
| `B9/09/02/06` | `B9 09 02 06` | `B9/0A/02/06/<record>` | `B9 0A 02 06 <record_hi> <record_lo>` | 06B0 | Ring | 1 | 1000 ms | `r[10]` | `r[11..57]` |

| Command | Request payload (hex) | Packet/DCT ID | Extraction (q from reply r) | Field | shortname | HDS conversion / unit |
| --- | --- | --- | --- | --- | --- | --- |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `U16BE(r[6..7])` | Captured event/DTC word | — | Raw; FFFF has special handling. |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[8]` | Captured class byte | — | 02 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `U16BE(r[9..10])` | Distance Traveled (fixed data at DTC occurence) | DRODO_0F42 | 5*q Km; saturates at 0xFFFF |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[11] & 0x3F` | Battery voltage(fixed data at DTC occurence) | DRVB_0F42 | 0.3*q V |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `(r[11] >> 7) & 1` | "SCS" Switch(fixed data at DTC occurence) | DRSCS_0F42 | 0 = OPEN; 1 = SHORT |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[12]` | Capacity of Battery(fixed data at DTC occurence) | DRQBAT_0F42 | 0.5*q %; stored raw value 0 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[13]` | Engine Coolant Temperature(fixed data at DTC occurence) | DRTW_0F42 | q - 40 degC |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[14]` | ATF Temperature(fixed data at DTC occurence) | DRTATF_0F42 | q - 40 degC |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[15]` | Atmospheric Pressure(fixed data at DTC occurence) | DRPA_0F42 | q kPa |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[16]` | ODD Clutch estimated temperature(fixed data at DTC occurence) | DRTC_CL1_0F42 | 3*q - 39 degC; stored raw value 0 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[17]` | EVEN Clutch estimated temperature(fixed data at DTC occurence) | DRTC_CL2_0F42 | 3*q - 39 degC; stored raw value 0 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[18]` | Lapsed time from IG-ON(fixed data at DTC occurence) | DRLT_IGON_0F42 | 2*q Minute; stored raw value 0 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[19]` | Shift Map Number (fixed data at DTC occurence) | DRMAPS_0F42 | q; enum UNKNOWN |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[20]` | Invalid information(fixed data at DTC occurence) | DRINVINF_0F42 | q; stored raw value 0 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F42 | `r[21]` | Trailing byte | — | 00 |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F41 | `U16BE(r[13..14])` | Distance Traveled Since Battery Connected | DRODO | q km |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F41 | `r[17]` | Shift Map Number | DRMAPS | Raw map number. Enum unknown. |
| `B9/03/<bank>` | `B9 03 02 <bank>` | 0F41 | `r[18]` | ECT Sensor (V) | DRTWAD | 0.019531*q V |

| Command | Request payload (hex) | Packet/DCT ID | Extraction (q from reply r) | Field | shortname | HDS conversion / unit |
| --- | --- | --- | --- | --- | --- | --- |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[5]` | Vehicle Speed | DRVSP_0651 | q km/h |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[6] & 0x0F` | APP Sensor | DRAP_0651 | 7*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[6] >> 4) & 0x0F` | TP Sensor | DRTH_0651 | 7*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[7] & 0x7F` | Calculated Engine torque | DRTQENG_0651 | 5*q - 100 N_m |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[7] >> 7) & 1` | A/T P Switch | DRPOSI_P_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[8] & 0x7F` | Engine Speed | DRNEB_0651 | 100*q RPM |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[8] >> 7) & 1` | A/T R Switch | DRPOSI_R_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[9] & 0x7F` | Drive Pulley rpm | DRNDR_0651 | 100*q RPM |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[9] >> 7) & 1` | A/T N Switch | DRPOSI_N_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[10] & 0x7F` | Torque Converter Turbine Speed | DRNT_0651 | 100*q RPM |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[10] >> 7) & 1` | A/T D Switch | DRPOSI_D_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[11] & 0x7F` | Secondary Shaft Speed (rpm) | DRNVEL_0651 | 100*q RPM |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[11] >> 7) & 1` | A/T S Switch | DRPOSI_S_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[12] & 0x3F` | CVTF Temperature | DRTATF_0651 | 3*q - 39 degC |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[12] >> 6) & 1` | D Indicator | DRDIND_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[12] >> 7) & 1` | A/T L Switch | DRPOSI_L_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[13] & 0x3F` | Slip ratio of torque converter | DRETR1_0651 | 2.5*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[13] >> 6) & 0x03` | Electric-powered oil pump operated condition | DRDOPST_0651 | 30*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[14]` | RPM ratio of Drive pulley / Driven pulley | DRRATIO_0651 | 0.02*q (ratio). |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[15] & 0x07` | Battery voltage | DRVB_0651 | 2*q + 5 V |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[15] >> 3) & 0x1F` | Driven Pulley Fluid Pressure | DRPDN_0651 | q kgf/cm2 |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[16] & 0x7F` | Actual Electric Current of Linear Solenoid A | DRLSDR_0651 | 10*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[16] >> 7) & 1` | Brake Switch | DRBRKSW_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[17] & 0x7F` | Actual Electric Current of Linear Solenoid B | DRLSDN_0651 | 10*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[17] >> 7) & 1` | ACCUMU solenoid commanded state | DRSOLACM_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[18] & 0x7F` | Actual Electric Current of Linear Solenoid C | DRLSCPC_0651 | 10*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[18] >> 7) & 1` | SH-C(SOL-C) solenoid commanded state | DRSOLC_0651 | 0 = OFF; 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `r[19] & 0x7F` | Actual Electric Current of Linear Solenoid D | DRLSLCC_0651 | 10*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0651 | `(r[19] >> 7) & 1` | SH-B(SOL-B) solenoid commanded state | DRSOLB_0651 | 0 = OFF; 1 = ON |

| Command | Request payload (hex) | Packet/DCT ID | Extraction (q from reply r) | Field | shortname | HDS conversion / unit |
| --- | --- | --- | --- | --- | --- | --- |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[5]` | Output Shaft (Countershaft) Speed | DRVNC | q km/h |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[6]` | Input Shaft (Mainshaft) Speed | DRVNM | q km/h |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[7]` | TP Sensor | DRTH | 0.41667*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[8]` | Engine Speed | DRNEB | 50*q rpm |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[9] & 1` | A/T 1 Switch | DRATP1 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[9] >> 1) & 1` | A/T L Switch or 2 Switch | DRATPL | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[9] >> 2) & 1` | A/T S Switch or D3 Switch or B Switch | DRATPS | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[9] >> 4) & 1` | A/T D Switch | DRATPD | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[9] >> 5) & 1` | A/T N Switch | DRATPN | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[9] >> 6) & 1` | A/T R Switch | DRATPR | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[9] >> 7) & 1` | A/T P Switch | DRATPP | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[10] & 1` | Forward Switch(ATPFWD or ATPFWD2) | DRATPFWD | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[10] >> 1) & 1` | SS Mode Switch | DRTMODE | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[10] >> 2) & 1` | Downshift Switch | DRSDN | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[10] >> 3) & 1` | Upshift Switch | DRSUP | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[10] >> 4) & 1` | Reverse Switch(ATPRVS or ATPRVS2) | DRATPRVS | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[10] >> 5) & 1` | Snow Mode Switch | DRSNOWSW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[10] >> 6) & 1` | Low Hold Switch | DRLHSW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[11]` | Battery Voltage | DRVBSL | 0.1*q V |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `U16BE(r[12..13])` | Gear Ratio | DRGR | 0.000122*q ratio |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[14]` | ETR | DRETR | q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[15] & 1` | A/T T.C.C. Sol Vlv. | DRLC | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[15] >> 1) & 1` | A/T Shift Sol Vlv. A | DRSHA | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[15] >> 2) & 1` | A/T Shift Sol Vlv. B | DRSHB | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[15] >> 3) & 1` | A/T Shift Sol Vlv. C | DRSHC | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[15] >> 4) & 1` | A/T Shift Sol Vlv. D | DRSHD | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[15] >> 5) & 1` | A/T Shift Sol Vlv. E | DRSHE | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[15] >> 6) & 1` | Line Pressure Sol VLv. A | DRPLA | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[15] >> 7) & 1` | Shift Lock Sol | DRSLC | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[16] & 1` | 2nd Pressure Switch or Transmission Fluid Pressure Switch A | DROP2SW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[16] >> 1) & 1` | 3rd Pressure Switch or Transmission Fluid Pressure Switch B | DROP3SW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[16] >> 2) & 1` | 4th Pressure Switch | DROP4SW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[16] >> 3) & 1` | Brake Switch | DRBRKSW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[16] >> 4) & 1` | D3/D4 Switch | DRD3SW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[16] >> 5) & 1` | Line Pressure Pressure Switch | DROPPL | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[16] >> 6) & 1` | A/T Temp Indicator | DRATFIND | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[16] >> 7) & 1` | D4/D5/D Indicator | DRDIND | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[17]` | ATF Temp Sensor (V) | DRTATFAD | 0.0196*q V |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[18]` | App Sensor | DRAP | 0.41667*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `r[19] & 1` | 5Th Pressure Switch | DR2OP5SW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0600 | `(r[19] >> 1) & 1` | 6Th Pressure Switch | DR2OP6SW | 0 = OFF, 1 = ON |

| Command | Request payload (hex) | Packet/DCT ID | Extraction (q from reply r) | Field | shortname | HDS conversion / unit |
| --- | --- | --- | --- | --- | --- | --- |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[5] & 0x3f` | Output Shaft (Countershaft) Speed | DRVNC1_0603 | 3*q km/h |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[5] >> 6) & 0x3` | Shift Position Indicator | DRPOSI_LAMP_0603 | 0 = OTHER, 1 = D, 2 = N/P, 3 = R |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[6] & 0x3f` | Input Shaft (Mainshaft) Speed | DRVNM1_0603 | 3*q km/h |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[6] >> 6) & 0x3` | Paddle Shift Switch | DRAT_IN1_0603 | 0 = OTHER, 1 = Shift UP, 2 = Shift DOWN, 3 = PARK |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[7] & 0x3f` | Engine Speed | DRNE_0603 | 100*q rpm |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[7] >> 6) & 1` | A/T Temp Indicator | DRATFIND_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[7] >> 7) & 1` | Brake Switch | DRBRKSW_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[8] & 0x1f` | APP Sensor (%) | DRAPSAT_0603 | 3*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[8] >> 5) & 0x7` | Battery Voltage | DRVB_0603 | q + 5 V |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[9] & 0x1f` | TP Sensor | DRTH1_0603 | 3*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[9] >> 5) & 1` | 4th Pressure Switch | DROPSW4_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[9] >> 6) & 1` | 3rd Pressure Switch | DROPSW3/B_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[9] >> 7) & 1` | 2nd Pressure Switch | DROPSW2/A_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[10] & 0x3f` | ATF Temperature | DRTATF_0603 | 3*q - 39 °C |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[10] >> 6) & 1` | 6th Pressure Switch | DROPSW6_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[10] >> 7) & 1` | 5th Pressure Switch | DROPSW5_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[11] >> 1) & 1` | Line Pressure Sol VLv. A | DRPL-A_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[11] >> 2) & 1` | A/T Shift Sol Vlv. E | DRSHE_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[11] >> 3) & 1` | A/T Shift Sol Vlv. D | DRSHD_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[11] >> 4) & 1` | A/T Shift Sol Vlv. C | DRSHC_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[11] >> 5) & 1` | A/T Shift Sol Vlv. B | DRSHB_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[11] >> 6) & 1` | SH-A/LU solenoid commanded state | DRSHA/LU_SOL_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[11] >> 7) & 1` | Line Pressure Pressure Switch | DROPPLSW_0603 | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[12] & 0xf` | A/T Clutch Pressure Control Solenoid Valve B Actual | DRLS-B_0603 | 100*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[12] >> 4) & 0xf` | A/T Clutch Pressure Control Solenoid Valve A Actual | DRLS-A_0603 | 100*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[13] & 0xf` | A/T Clutch Pressure Control Solenoid Valve D Actual | DRLS-D_0603 | 100*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[13] >> 4) & 0xf` | A/T Clutch Pressure Control Solenoid Valve C Actual | DRLS-C_0603 | 100*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[14] >> 4) & 0xf` | A/T Clutch Pressure Control Solenoid Valve PLA Actual | DRLSPLA_0603 | 100*q mA |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[15] & 0xf` | Shift Control | DRSO_0603 | 0 = N/P, 1–9 = gear, 10–13 = undefined, 14 = Park, 15 = R |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `(r[15] >> 4) & 0xf` | Target gear Shifting | DRSHTRGET_0603 | 0 = N/P, 1–9 = gear, 10–13 = undefined, 14 = Park, 15 = R |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[16] & 0x3f` | ETR | DRETR1_0603 | 2.5*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0603 | `r[17]` | RPM ratio of Main shaft / Counter shaft. | DRGRATIO_0603 | Raw ratio code. Scale unknown. |

| Command | Request payload (hex) | Packet/DCT ID | Extraction (q from reply r) | Field | shortname | HDS conversion / unit |
| --- | --- | --- | --- | --- | --- | --- |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[5]` | Vehicle Speed | DRVSP | q km/h |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[6]` | APP Sensor | DRAP | 0.416667*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[7]` | TP Sensor | DRTH | 0.416667*q % |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[8]` | Engine Speed | DRNEB | 50*q rpm |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[9]` | Drive Pulley rpm | DRNDR | 50*q rpm |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[10]` | Driven Pulley rpm | DRNDN | 50*q rpm |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[11]` | Secondary Shaft Speed (rpm) | DRNVEL | 50*q rpm |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[12]` | Torque Converter Turbine Speed | DRNT | 50*q rpm |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[13] >> 2) & 1` | A/T L Switch | DRATPL | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[13] >> 3) & 1` | A/T S Switch | DRATPS | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[13] >> 4) & 1` | A/T D Switch | DRATPD | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[13] >> 5) & 1` | A/T N Switch | DRATPN | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[13] >> 6) & 1` | A/T R Switch | DRATPR | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[13] >> 7) & 1` | A/T P Switch | DRATPP | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[14] >> 1) & 1` | Shift Sol A | DRSHA | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[14] >> 2) & 1` | Shift Sol B | DRSHB | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[14] >> 3) & 1` | Shift Sol O/P | DRSHO/P | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[14] >> 6) & 1` | Inhibitor Sol | DRINHSOL | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[14] >> 7) & 1` | Shift Lock Sol | DRSLC | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[15] >> 6) & 1` | Brake Switch | DRBRKSW | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `(r[15] >> 7) & 1` | D Indicator | DRDIND | 0 = OFF, 1 = ON |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[16]` | Battery Voltage | DRVBSL | 0.1*q V |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[17]` | Driven Pulley Pressure(V) | DRPDNAD | 0.0195313*q V |
| `B9/05/<bank>/<record>` | `B9 05 02 <bank> 00 00 <record>` | 0650 | `r[18]` | Start Clutch Control Sol Command | DRICMDSC | 0.005*q A (5*q mA) |

| Command | Request payload (hex) | Enhanced packet/DCT ID | Reply extraction (q) | Field | shortname | Conversion / unit |
| --- | --- | --- | --- | --- | --- | --- |
| `B9/0A/00/06/<record>` | `B9 0A 00 06 <record_hi> <record_lo>` | 0320 | `r[10]` | Cyl Crank Speed #1 | MFTRQCYL1_OBS | 0.5*q - 8 rad/s |
| `B9/0A/00/06/<record>` | `B9 0A 00 06 <record_hi> <record_lo>` | 0320 | `r[11]` | Cyl Crank Speed #2 | MFTRQCYL2_OBS | 0.5*q - 8 rad/s |
| `B9/0A/00/06/<record>` | `B9 0A 00 06 <record_hi> <record_lo>` | 0320 | `r[12]` | Cyl Crank Speed #3 | MFTRQCYL3_OBS | 0.5*q - 8 rad/s |
| `B9/0A/00/06/<record>` | `B9 0A 00 06 <record_hi> <record_lo>` | 0320 | `r[13]` | Cyl Crank Speed #4 | MFTRQCYL4_OBS | 0.5*q - 8 rad/s |
| `B9/0A/00/06/<record>` | `B9 0A 00 06 <record_hi> <record_lo>` | 0320 | Unavailable. `r[14]` is zero padding. | Cyl Crank Speed #5 | MFTRQCYL5_OBS | HDS nominal 0.5*q - 8 rad/s |
| `B9/0A/00/06/<record>` | `B9 0A 00 06 <record_hi> <record_lo>` | 0320 | Unavailable. `r[15]` is zero padding. | Cyl Crank Speed #6 | MFTRQCYL6_OBS | HDS nominal 0.5*q - 8 rad/s |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `S16BE(r[10..11])` | FTP Sensor (Fine) | EVAPVP_OBS | 0.25*q Pa |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[12..13])` | Engine Speed | NE_OBS | 0.25*q RPM |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[14]` | MAP Sensor | MAP_OBS | q kPa |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[15..16])` | MAF Sensor | GAIR_OBS | 0.01*q g/s |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[17..18])` | GAIRAVE | GAIRAVE_OBS | 0.01*q g/s |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[19..20])` | MAPAVE | MAPAVE_OBS | 0.01*q g/s |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[21..22])` | MAF Sensor (Hi Res) | GAIRCYL_OBS | 0.1*q mg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[23]` | Spark Advance | IG_OBS | 0.5*q - 64 deg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[24]` | Knock Retard | KNRTD_OBS | 0.5*q deg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[25..26])` | F Injector | TOUT_OBS | 0.004*q ms |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[27]` | TP Sensor | ABSTH_OBS | 0.3921*q % / 0.0196*q V |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[28]` | REL TP Sensor | RELTH_OBS | 0.3921*q % / 0.3529*q deg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[29..30])` | Target TH VLV | THO_OBS | 0.005*q deg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[31..32])` | Idle Target TH | THICMD_OBS | 0.005*q deg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[33]` | EGR L COMMAND | LCMD_OBS | 0.05*q mm |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[34]` | EGR Valve Lift | LACT_OBS | 0.05*q mm |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[35]` | CMP Ctrl Cmd | VTCCMD_OBS | 0.5*q - 20 deg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[36]` | CMP Ctrl | VTCANGL_OBS | 0.5*q - 20 deg |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[37]` | EVAP PC Duty | DPCS_OBS | 0.3906*q % |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[38]` | AF FB (ST Fuel Trim) | KLAF_OBS | q / 128 (ratio). HDS coefficient is 0.0078. |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[39..40])` | AF LAMBDA CMD | CMDEQRAT_OBS | q / 32768 (ratio). HDS coefficient is 0.0000305. |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[41..42])` | AF Lambda | LAMBDA_OBS | q / 32768 (ratio). HDS coefficient is 0.0000305. |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[43..44])` | AF Sensor | XILAF_OBS | 0.0039063*q - 128 mA |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[45]` | AF Sensor Impedance | AFRAC_OBS | 2*q Ohm |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[46]` | HO2S S2 | SVO2_OBS | 0.0196*q V |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[47]` | Slip Ratio of Torq or Startclutch | ETSENAN_OBS | q % |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[48..49])` | Misfire | CMFIRE_OBS | q counts, saturating at 65535. |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `r[50]` | Battery | VB_OBS | 0.1*q V |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | `U16BE(r[51..52])` | Commanded Electric Current of Linear Solenoid E | ICMDENAN_OBS | 0.1*q L/min (HDS). |
| `B9/0A/00/07/<record>` | `B9 0A 00 07 <record_hi> <record_lo>` | 0330 | Unavailable. `r[53..54]` is zero padding. | Target Torque (Idle Speed) | TQTRGSLW_OBS | HDS nominal 0.02*(q < 0x8000 ? q : q - 0x10000) N_m |
| `B9/0A/00/13/<record>` | `B9 0A 00 13 <record_hi> <record_lo>` | 0335 | `r[10]` | RT28SRC | RT28SRC_OBS | Raw value. |
| `B9/0A/00/13/<record>` | `B9 0A 00 13 <record_hi> <record_lo>` | 0335 | `r[11]` | SUM28FOCL | SUM28FOCL_OBS | Raw value. |
| `B9/0A/00/13/<record>` | `B9 0A 00 13 <record_hi> <record_lo>` | 0335 | `r[12]` | CRT28FSTG | CRT28FSTG_OBS | Raw value. |
| `B9/0A/00/13/<record>` | `B9 0A 00 13 <record_hi> <record_lo>` | 0335 | `S8(r[13])` | Auxiliary sample | — | Signed raw counts. Unit unknown. |
| `B9/0A/00/13/<record>` | `B9 0A 00 13 <record_hi> <record_lo>` | 0335 | Unavailable. `r[14]` is zero padding. | EX VTC Advance Angle | EXVTCABSAEA_OBS | HDS nominal 0.2*q deg |
| `B9/0A/00/13/<record>` | `B9 0A 00 13 <record_hi> <record_lo>` | 0335 | Unavailable. `r[15]` is zero padding. | EX VTC Target Advance Angle | EXVTCCMDEA_OBS | HDS nominal 0.2*q deg |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[10]` | ECT Sensor 2 | ECT2A_OBS | q - 40 degC |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[11]` | IAT Sensor (1) | IAT_OBS | q - 40 degC |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[12]` | Fuel Level | FLI_OBS | 0.3921*q % |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[13]` | Vehicle Speed | VSP_OBS | q km/h |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[14]` | ECT Sensor 1 | ECT_OBS | q - 40 degC |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `U16BE(r[15..16])` | EST ECT1 | CTW1_OBS | 0.1*q degC |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `U16BE(r[17..18])` | EST ECT2 | CTW2_OBS | 0.1*q degC |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[19]` | Baro Sensor | BARO_OBS | q kPa |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[20]` | Knock Ctrl | KIGKREF_OBS | q / 128 (ratio). |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[21]` | Knock Ctrl EGR | KIGKREFE_OBS | q / 128 (ratio). |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[22]` | Gear Position | NGP_OBS | 0 = N; 1 = 1st; 2 = 2nd; 3 = 3rd; 4 = 4th; 5 = 5th; 6 = 6th; 7-255 = N.A. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `U16BE(r[23..24])` | Idle Air Block Lean | IAIRBL_OBS | 0.1*q |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[25]` | (Rocker Arm) Oil Pressure Sensor (B) | POIL_B_OBS | 0.0196*q V |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `U16BE(r[26..27])` | (Rocker Arm) Oil Pressure Sensor (B) | EOP_OBS | q kPa |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[28]` | DACGF | DACGF_OBS | 0.5*q % |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[29]` | ELD (Electric Load Value) | ELOAD_OBS | 0.5*q A |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[30]` | EPS Consumption Current | IEPSLD_OBS | 0.5*q A |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[31]` | AF FB AVE (LT Fuel Trim) | KLAFAVE_OBS | q / 128 (ratio). HDS coefficient is 0.0078. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[32]` | KAFREF | KAFREF_OBS | 0.0078*q |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[33]` | TOTVAP | TOTVAP_OBS | Raw purge-vapor correction. Unit unknown. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[34]` | FSS | FSS_OBS | 1 = OL COND, 2 = CLOSED, 4 = OL DRV, 8 = OL DTC, 16 = CL O2SX. Other values = NONE. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[35]` | Cyl1 Total Misfire | NMCYLOBS1_OBS | q counts, saturating at 255. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[36]` | Cyl2 Total Misfire | NMCYLOBS2_OBS | q counts, saturating at 255. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[37]` | Cyl3 Total Misfire | NMCYLOBS3_OBS | q counts, saturating at 255. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[38]` | Cyl4 Total Misfire | NMCYLOBS4_OBS | q counts, saturating at 255. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `U16BE(r[39..40])` | OBSV61AFNE | OBSV61AFNE_OBS | Raw value. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `U16BE(r[41..42])` | OBSV61AF1 | OBSV61AF1_OBS | Raw value. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `U16BE(r[43..44])` | OBSV61AF2 | OBSV61AF2_OBS | Raw value. |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[45] >> 1) & 1` | EVAP integrity monitor configuration | — | 0 = disabled, 1 = enabled |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[45] >> 2) & 1` | Transmission-family selector present | — | 0 = zero selector, 1 = nonzero selector |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 0) & 1` | IG1 Level | IG1LVL_OBS | 0 = LOW; 1 = HIGH |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 1) & 1` | EVAP CVS Valve | VSV_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 2) & 1` | PNP Switch | ATNP_OBS | 0 = GEAR; 1 = P-N |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 3) & 1` | Brake Switch | BKSW_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 4) & 1` | Idling | IDLING_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 5) & 1` | Fuel Cut | FC_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 6) & 1` | A/C Switch | ACS_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[46] >> 7) & 1` | A/C Clutch | ACC_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[48] >> 0) & 1` | Clutch Pedal Position Switch A | CLSW_OBS | 0 = OPEN; 1 = CLOSE |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[48] >> 1) & 1` | PSP Switch | PSW_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[48] >> 2) & 1` | IMT (IMRC) Valve Cmd | SVSCMD_OBS | 0 = CLOSE; 1 = OPEN |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[48] >> 7) & 1` | Engine Idle Stop Mode Control | ISSTAT_OBS | 0 = Uncontrolled; 1 = Controlled |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[49]` | Zero-filled sample byte | — | 00 |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[50] >> 0) & 1` | Rocker Arm Oil Control Sol.  1 | VTS1_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[50] >> 1) & 1` | ROCKER ARM OIL CONTROL SOL.  2 | VTS2_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[50] >> 2) & 1` | ROCKER ARM OIL PRESSURE SWITCH | VTMACD_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[50] >> 4) & 1` | Rocker Arm Oil Control Solenoid A | VTSI_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[50] >> 5) & 1` | Rocker Arm Oil Control Solenoid B | VTSE_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[50] >> 6) & 1` | Rocker Arm Oil Pressure Switch A | VTMI_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[50] >> 7) & 1` | Rocker Arm Oil Pressure Switch B | VTME_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `r[51]` | Zero-filled sample byte | — | 00 |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[52] >> 0) & 1` | Main Relay | FSR_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[52] >> 1) & 1` | Low Fuel Indi. | FWARN_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[52] >> 2) & 1` | Fuel Rail Pressure SW | P2SW_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[52] >> 3) & 1` | Engine Cold Start Condition | FFVCSC_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[52] >> 4) & 1` | VTEC Solenoid Valve | VTS_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[52] >> 5) & 1` | VTEC PRESS SW | VTM_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `(r[52] >> 6) & 1` | Relief Valve | RVLVOPN_OBS | 0 = Close; 1 = Open |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | `S16BE(r[53..54])` | A/C Pressure Sensor | ACRP_OBS | q kPa |
| `B9/0A/00/08/<record>` | `B9 0A 00 08 <record_hi> <record_lo>` | 0340 | Unavailable. `r[55]` is zero padding. | Power consumption of  HCU | PWFBHCU_OBS | HDS nominal 2*q W |
| `B9/0A/00/16/<record>` | `B9 0A 00 16 <record_hi> <record_lo>` | 0344 | Unavailable. `r[10]` is the only sample byte. `r[11]` is padding. | Rocker Arm Oil Pressure Sensor | EOPSPHY_OBS | HDS expects `U16BE(r[10..11])` and q kPa. |
| `B9/0A/00/16/<record>` | `B9 0A 00 16 <record_hi> <record_lo>` | 0344 | Unavailable. `r[12]` is zero padding. | Fuel Pump Control | DFPC_OBS | HDS nominal 0 = OFF(0%); 1-32 = Fail; 33 = LOW(33%); 34-65 = Fail; 66 = MID(66%); 67-99 = Fail; 100 = HIGH(100%); 101-255 = Fail / q |
| `B9/0A/00/16/<record>` | `B9 0A 00 16 <record_hi> <record_lo>` | 0344 | Unavailable. `r[13..14]` is zero padding. | A/C Pressure Sensor | ACRP_OBS | HDS nominal (q < 0x8000 ? q : q - 0x10000) kPa |
| `B9/0A/00/16/<record>` | `B9 0A 00 16 <record_hi> <record_lo>` | 0344 | Unavailable. Bit 0 of `r[16]` is zero padding. | A/T R Switch | ATPR_OBS | HDS nominal 0 = OFF; 1 = ON |
| `B9/0A/00/17/<record>` | `B9 0A 00 17 <record_hi> <record_lo>` | 0346 | Unavailable. `r[10]` is the only sample byte. `r[11]` is padding. | Target Idle Torque Correction Learning | KTQFRCN_OBS | HDS expects `S16BE(r[10..11])` and 0.02*q N·m. |
| `B9/0A/00/17/<record>` | `B9 0A 00 17 <record_hi> <record_lo>` | 0346 | Unavailable. `r[12..13]` is zero padding. | TC Boost Pressure Upper Limit | P3TCOBJ_OBS | HDS nominal 0.01333*q kPa |
| `B9/0A/00/0A/<record>` | `B9 0A 00 0A <record_hi> <record_lo>` | 0350 | `S16BE(r[10..11])` | FTP Sensor (Fine) | EVAPVP_OBS | 0.25*q Pa |
| `B9/0A/00/09/<record>` | `B9 0A 00 09 <record_hi> <record_lo>` | 0351 | `U16BE(r[10..11])` | Engine Speed | NE_OBS | 0.25*q RPM |
| `B9/0A/00/09/<record>` | `B9 0A 00 09 <record_hi> <record_lo>` | 0351 | `r[12]` | MAP Sensor | MAP_OBS | q kPa |
| `B9/0A/00/09/<record>` | `B9 0A 00 09 <record_hi> <record_lo>` | 0351 | `U16BE(r[13..14])` | MAF Sensor | GAIR_OBS | 0.01*q g/s |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `r[10]` | ECT Sensor 2 | ECT2A_OBS | q - 40 degC |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `r[11]` | IAT Sensor (1) | IAT_OBS | q - 40 degC |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `r[12]` | Fuel Level | FLI_OBS | 0.3921*q % |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[13] >> 1) & 1` | EVAP integrity monitor configuration | — | 0 = disabled, 1 = enabled |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[13] >> 2) & 1` | Transmission-family selector present | — | 0 = zero selector, 1 = nonzero selector |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 0) & 1` | IG1 Level | IG1LVL_OBS | 0 = LOW; 1 = HIGH |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 1) & 1` | EVAP CVS Valve | VSV_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 2) & 1` | PNP Switch | ATNP_OBS | 0 = GEAR; 1 = P-N |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 3) & 1` | Brake Switch | BKSW_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 4) & 1` | Idling | IDLING_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 5) & 1` | Fuel Cut | FC_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 6) & 1` | A/C Switch | ACS_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/0C/<record>` | `B9 0A 00 0C <record_hi> <record_lo>` | 0360 | `(r[14] >> 7) & 1` | A/C Clutch | ACC_OBS | 0 = OFF; 1 = ON |
| `B9/0A/00/0B/<record>` | `B9 0A 00 0B <record_hi> <record_lo>` | 0361 | `r[10]` | Vehicle Speed | VSP_OBS | q km/h |
| `B9/0A/00/0B/<record>` | `B9 0A 00 0B <record_hi> <record_lo>` | 0361 | `r[11]` | ECT Sensor 1 | ECT_OBS | q - 40 degC |
| `B9/0A/00/0B/<record>` | `B9 0A 00 0B <record_hi> <record_lo>` | 0361 | `U16BE(r[12..13])` | EST ECT1 | CTW1_OBS | 0.1*q degC |
| `B9/0A/00/0B/<record>` | `B9 0A 00 0B <record_hi> <record_lo>` | 0361 | `U16BE(r[14..15])` | EST ECT2 | CTW2_OBS | 0.1*q degC |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[10..11])` | Captured event/DTC word | DRSAEDTCM_OBS / DRSAEDTCS_OBS | Raw word. Stored as 0 for other trigger types. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[12]` | OBRTRGMD | OBRTRGMD_OBS | 01 = DTC Trigger, 02 = Engine Stall. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U24BE(r[13..15])` | TOTAL MILEAGE | DSTODO_OBS | q Km |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[16..17])` | Drive Dist | DSTTRV_OBS | 10*q m |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[18..19])` | EGR Flow Open Ratio | EGRST_OBS | 0.1*q % |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[20]` | VTCZP | VTCZP_OBS | 0.5*q - 20 deg |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[21]` | DBW Stuck Ratio | KTHC_B_OBS | 0.3922*q % |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[22]` | KAFREFX | KAFREFX_OBS | q / 128 (ratio). HDS coefficient is 0.0078. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[23]` | Zero-filled sample byte | — | 00 |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[24..25])` | Eng Run Time | ERUNTM_OBS | q s |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[26..27])` | OBSERUNTMP | OBSERUNTMP_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[28..29])` | Soak Time | IGOFFTMR_OBS | 0.1666*q Minute |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[30..31])` | OBSTIGOF86 | OBSTIGOF86_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[32..33])` | OBSRT67BF0 | OBSRT67BF0_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[34..35])` | OBSRT67BF1 | OBSRT67BF1_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[36..37])` | OBSRT67BF2 | OBSRT67BF2_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[38..39])` | OBSRT67BF3 | OBSRT67BF3_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[40..41])` | OBSRT67BF4 | OBSRT67BF4_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[42]` | OBSINJRNGC1 | OBSINJRNGC1_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[43]` | OBSINJRNGC2 | OBSINJRNGC2_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[44]` | OBSINJRNGC3 | OBSINJRNGC3_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[45]` | OBSINJRNGC4 | OBSINJRNGC4_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[46]` | OBSIGRNGC1 | OBSIGRNGC1_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[47]` | OBSIGRNGC2 | OBSIGRNGC2_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[48]` | OBSIGRNGC3 | OBSIGRNGC3_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[49]` | OBSIGRNGC4 | OBSIGRNGC4_OBS | Raw value. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[50]` | LAF Sensor Lean Side Deviation Learned | KAFFC_OBS | q / 128 (ratio). Stored as 0 in the documented layout. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[51] & 0x03` | Capture configuration flags | — | Raw mask. Bits 2–7 are zero. |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `(r[52] >> 0) & 1` | CKP Pulser F/B Learn | KCREND_OBS | 0 = Not Completed; 1 = COMPLETED |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `(r[52] >> 1) & 1` | IAT Sensor 1 Block Heater Info | BH10C_OBS | 0 = NOT EXIST; 1 = Exist |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `(r[52] >> 2) & 1` | ACLRDY | ACLRDY_OBS | 0 = NOT COMPLETED; 1 = COMPLETED |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `(r[52] >> 3) & 1` | Fuel Alcohol Content Learned Condition | KREFBSON_OBS | 0 = Completed; 1 = Not completed |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `(r[52] >> 4) & 1` | Engine Oil Dilution Influence | OCTM_OBS | 0 = Not Exist; 1 = Exist |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `U16BE(r[53..54])` | DSTTRVLDC | DSTTRVLDC_OBS | 10*q m |
| `B9/0A/00/00/0000` | `B9 0A 00 00 00 00` | 0380 | `r[55]` | Lean-side correction factor | — | q / 32 ratio |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[10..11])` | OBSCMFZN0 | OBSCMFZN0_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[12..13])` | OBSCMFZN1 | OBSCMFZN1_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[14..15])` | OBSCMFZN2 | OBSCMFZN2_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[16..17])` | OBSCMFZN3 | OBSCMFZN3_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[18..19])` | OBSCMFZN4 | OBSCMFZN4_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[20..21])` | OBSCMFZN5 | OBSCMFZN5_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[22..23])` | OBSCMFZN6 | OBSCMFZN6_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[24..25])` | OBSCMFZN7 | OBSCMFZN7_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[26..27])` | OBSCMFZN8 | OBSCMFZN8_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[28..29])` | OBSCMFZN9 | OBSCMFZN9_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[30..31])` | OBSCMFZN10 | OBSCMFZN10_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[32..33])` | OBSCMFZN11 | OBSCMFZN11_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[34..35])` | OBSCMFZN12 | OBSCMFZN12_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[36..37])` | OBSCMFZN13 | OBSCMFZN13_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[38..39])` | OBSCMFZN14 | OBSCMFZN14_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[40..41])` | OBSCMFZN15 | OBSCMFZN15_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[42..43])` | OBSCMFZN16 | OBSCMFZN16_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[44..45])` | OBSCMFZN17 | OBSCMFZN17_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[46..47])` | OBSCMFZN18 | OBSCMFZN18_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[48..49])` | OBSCMFZN19 | OBSCMFZN19_OBS | Raw value. |
| `B9/0A/00/01/0000` | `B9 0A 00 01 00 00` | 0381 | `U16BE(r[50..51])` | OBSCMFZN20 | OBSCMFZN20_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[10..11])` | OBSCMFZNH0 | OBSCMFZNH0_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[12..13])` | OBSCMFZNH1 | OBSCMFZNH1_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[14..15])` | OBSCMFZNH2 | OBSCMFZNH2_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[16..17])` | OBSCMFZNH3 | OBSCMFZNH3_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[18..19])` | OBSCMFZNH4 | OBSCMFZNH4_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[20..21])` | OBSCMFZNH5 | OBSCMFZNH5_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[22..23])` | OBSCMFZNH6 | OBSCMFZNH6_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[24..25])` | OBSCMFZNH7 | OBSCMFZNH7_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[26..27])` | OBSCMFZNH8 | OBSCMFZNH8_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[28..29])` | OBSCMFZNH9 | OBSCMFZNH9_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[30..31])` | OBSCMFZNH10 | OBSCMFZNH10_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[32..33])` | OBSCMFZNH11 | OBSCMFZNH11_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[34..35])` | OBSCMFZNH12 | OBSCMFZNH12_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[36..37])` | OBSCMFZNH13 | OBSCMFZNH13_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[38..39])` | OBSCMFZNH14 | OBSCMFZNH14_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[40..41])` | OBSCMFZNH15 | OBSCMFZNH15_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[42..43])` | OBSCMFZNH16 | OBSCMFZNH16_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[44..45])` | OBSCMFZNH17 | OBSCMFZNH17_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[46..47])` | OBSCMFZNH18 | OBSCMFZNH18_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[48..49])` | OBSCMFZNH19 | OBSCMFZNH19_OBS | Raw value. |
| `B9/0A/00/02/0000` | `B9 0A 00 02 00 00` | 0382 | `U16BE(r[50..51])` | OBSCMFZNH20 | OBSCMFZNH20_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[10..11])` | OBSCMFZNL0 | OBSCMFZNL0_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[12..13])` | OBSCMFZNL1 | OBSCMFZNL1_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[14..15])` | OBSCMFZNL2 | OBSCMFZNL2_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[16..17])` | OBSCMFZNL3 | OBSCMFZNL3_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[18..19])` | OBSCMFZNL4 | OBSCMFZNL4_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[20..21])` | OBSCMFZNL5 | OBSCMFZNL5_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[22..23])` | OBSCMFZNL6 | OBSCMFZNL6_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[24..25])` | OBSCMFZNL7 | OBSCMFZNL7_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[26..27])` | OBSCMFZNL8 | OBSCMFZNL8_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[28..29])` | OBSCMFZNL9 | OBSCMFZNL9_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[30..31])` | OBSCMFZNL10 | OBSCMFZNL10_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[32..33])` | OBSCMFZNL11 | OBSCMFZNL11_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[34..35])` | OBSCMFZNL12 | OBSCMFZNL12_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[36..37])` | OBSCMFZNL13 | OBSCMFZNL13_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[38..39])` | OBSCMFZNL14 | OBSCMFZNL14_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[40..41])` | OBSCMFZNL15 | OBSCMFZNL15_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[42..43])` | OBSCMFZNL16 | OBSCMFZNL16_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[44..45])` | OBSCMFZNL17 | OBSCMFZNL17_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[46..47])` | OBSCMFZNL18 | OBSCMFZNL18_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[48..49])` | OBSCMFZNL19 | OBSCMFZNL19_OBS | Raw value. |
| `B9/0A/00/03/0000` | `B9 0A 00 03 00 00` | 0383 | `U16BE(r[50..51])` | OBSCMFZNL20 | OBSCMFZNL20_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[10]` | OBSKAV45BF0 | OBSKAV45BF0_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[11]` | OBSKAV45BF1 | OBSKAV45BF1_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[12]` | OBSKAV45BF2 | OBSKAV45BF2_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[13]` | OBSKAV45BF3 | OBSKAV45BF3_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[14]` | OBSKAV45BF4 | OBSKAV45BF4_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[15]` | OBSKAV45BF5 | OBSKAV45BF5_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[16]` | OBSKAV45BF6 | OBSKAV45BF6_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[17]` | OBSKAV45BF7 | OBSKAV45BF7_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[18]` | OBSKAV45BF8 | OBSKAV45BF8_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[19]` | OBSKAV45BF9 | OBSKAV45BF9_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[20]` | OBSKAV45BF10 | OBSKAV45BF10_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[21]` | OBSKAV45BF11 | OBSKAV45BF11_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[22]` | OBSKAV45BF12 | OBSKAV45BF12_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[23]` | OBSKAV45BF13 | OBSKAV45BF13_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[24]` | OBSKAV45BF14 | OBSKAV45BF14_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[25]` | OBSKAV45BF15 | OBSKAV45BF15_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[26]` | OBSKAV45BF16 | OBSKAV45BF16_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[27]` | OBSKAV45BF17 | OBSKAV45BF17_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[28]` | OBSKAV45BF18 | OBSKAV45BF18_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[29]` | OBSKAV45BF19 | OBSKAV45BF19_OBS | Raw value. |
| `B9/0A/00/04/0000` | `B9 0A 00 04 00 00` | 0384 | `r[30]` | OBSKAV45BF20 | OBSKAV45BF20_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[10]` | OBSTWRMX | OBSTWRMX_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[11]` | OBSTWRMN | OBSTWRMN_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[12]` | OBSTW2RMX | OBSTW2RMX_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[13]` | OBSTW2RMN | OBSTW2RMN_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[14]` | OBSTARMX | OBSTARMX_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[15]` | OBSTARMN | OBSTARMN_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[16]` | OBSTW86RMX | OBSTW86RMX_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[17]` | OBSTW86RMN | OBSTW86RMN_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[18]` | Start ECT | TW86INI_OBS | q - 40 degC |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[19]` | TW2INI | TW2INI_OBS | q - 40 °C (wire). HDS displays q °C. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[20]` | Start IAT | TA86INI_OBS | q - 40 degC |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[21]` | OBSTW86INI | OBSTW86INI_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[22]` | OBSTW286INI | OBSTW286INI_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[23]` | OBSTA86INI | OBSTA86INI_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `U16BE(r[24..25])` | OBSTFC87 | OBSTFC87_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[26]` | OBSTCFUEL | OBSTCFUEL_OBS | q - 40 °C (wire). HDS displays q. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `U16BE(r[27..28])` | ELMUGAS | ELMUGAS_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `U16BE(r[29..30])` | OBSCFUGAS | OBSCFUGAS_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[31]` | OBSKFCAPJD | OBSKFCAPJD_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[32]` | OBSMODEVP | OBSMODEVP_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `U16BE(r[33..34])` | OBSCVPEOTM | OBSCVPEOTM_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[35]` | OBSCH90H | OBSCH90H_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[36]` | CDC90GDACT | CDC90GDACT_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[37]` | CDC90GOK | CDC90GOK_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[38]` | CDC90HDACT | CDC90HDACT_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[39]` | CDC90HOK | CDC90HOK_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `U24BE(r[40..42])` | A/F Imbalance Measurement Value | AFIINTGRT_OBS | q / 32768 (no HDS unit). |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `U16BE(r[43..44])` | OBSRT61AF | OBSRT61AF_OBS | Raw value. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | `r[45]` | Misfire Driving Cycle | CDCMFA_OBS | Raw state code. HDS labels it a driving-cycle count. |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | Unavailable. `r[46..47]` is zero padding. | OBSUADPLB1 | OBSUADPLB1_OBS | HDS nominal Raw value |
| `B9/0A/00/05/0000` | `B9 0A 00 05 00 00` | 0385 | Unavailable. `r[48..49]` is zero padding. | OBSUADPB1 | OBSUADPB1_OBS | HDS nominal Raw value |
| `B9/0A/00/10/0000` | `B9 0A 00 10 00 00` | 0389 | `U16BE(r[10..11])` | SMG43FB1 | SMG43FB1_OBS | Raw value. |
| `B9/0A/00/10/0000` | `B9 0A 00 10 00 00` | 0389 | Unavailable. `r[12]` is zero padding. | EX VTC Cam Shaft No Advanced | EXVTCZP_OBS | HDS nominal 0.5*q deg |
| `B9/0A/00/12/0000` | `B9 0A 00 12 00 00` | 038A | `U16BE(r[10..11])` | Average Pressure of the Fuel Tank(Hi) | DPTOCLAVEH_OBS | 0.002*q mmHg (HDS). Stored as 0 in the documented layout. |
| `B9/0A/00/12/0000` | `B9 0A 00 12 00 00` | 038A | `U16BE(r[12..13])` | DPTOCLLTHD | DPTOCLLTHD_OBS | 0.002*q mmHg (HDS). Stored as 0 in the documented layout. |
| `B9/0A/00/12/0000` | `B9 0A 00 12 00 00` | 038A | `U16BE(r[14..15])` | OBSKCMDS2B1 | OBSKCMDS2B1_OBS | Raw value. |
| `B9/0A/00/0D/<record>` | `B9 0A 00 0D <record_hi> <record_lo>` | 0390 | `r[10]` | SVO2 for CAT | SVO2_CAT_OBS | 0.0196*q V |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `U16BE(r[10..11])` | NE for CAT | NE_CAT_OBS | 0.25*q RPM |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `U16BE(r[12..13])` | GAIR for CAT | GAIR_CAT_OBS | 0.01*q g/s |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `r[14]` | KLAF for CAT | KLAF_CAT_OBS | q / 128 (ratio). HDS coefficient is 0.0078. |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `U16BE(r[15..16])` | XILAF for CAT | XILAF_CAT_OBS | 0.0039063*q - 128 mA |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `U16BE(r[17..18])` | TCAT for CAT | TCAT_CAT_OBS | 0.1*q - 40 degC |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `U16BE(r[19..20])` | CATRT for CAT | CATRT_CAT_OBS | 0.001*q V |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `r[21]` | OBSCRT67 for CAT | OBSCRT67_CAT_OBS | Raw value. |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `r[22] & 0x03` | Catalyst monitor configuration | — | Raw flags. Bits 2–7 are zero. |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `(r[23] >> 0) & 1` | DEXEC67 for CAT | DEXEC67_CAT_OBS | 0 = NG; 1 = OK |
| `B9/0A/00/0E/<record>` | `B9 0A 00 0E <record_hi> <record_lo>` | 03A0 | `(r[23] >> 1) & 1` | DEXEC68 for CAT | DEXEC68_CAT_OBS | 0 = NG; 1 = OK |
| `B9/0A/00/0F/<record>` | `B9 0A 00 0F <record_hi> <record_lo>` | 03B0 | `r[10]` | ECT for CAT | ECT_CAT_OBS | q - 40 degC |
| `B9/0A/00/0F/<record>` | `B9 0A 00 0F <record_hi> <record_lo>` | 03B0 | `r[11]` | IAT for CAT | IAT_CAT_OBS | q - 40 degC |
| `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | `U16BE(r[10..11])` | Fuel Pressure Direct Injection System | PFDIO_OBS | 10*q kPa |
| `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | `U16BE(r[12..13])` | FUEL PRESSURE CONVERTED FROM PF SENSOR | PFDIREL_OBS | 10*q kPa |
| `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | `r[14..17]` | Zero-filled sample bytes | — | 00 00 00 00 |
| `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | `r[18..24]` | RSPI-B received status bytes 7–13 | — | Raw bytes |
| `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | Unavailable. `r[25]` is zero padding. | EWG Voltage From Lift Sensor | WGLIFT_OBS | HDS nominal 0.0195313*q V |
| `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | Unavailable. `r[26..27]` is zero padding. | EWG Target Lift | WGLCMD_OBS | HDS nominal (q < 0x8000 ? q : q - 0x10000)/1000 mm |
| `B9/0A/00/15/<record>` | `B9 0A 00 15 <record_hi> <record_lo>` | 03C1 | Unavailable. `r[28..29]` is zero padding. | TC Boost Pressure | TCBP_OBS | HDS nominal 0.01333*q kPa |
| `B9/0A/00/14/<record>` | `B9 0A 00 14 <record_hi> <record_lo>` | 03C3 | `U16BE(r[10..11])` | APP Sensor | APP_OBS | 0.005*q % |
| `B9/0A/00/14/<record>` | `B9 0A 00 14 <record_hi> <record_lo>` | 03C3 | `U16BE(r[12..13])` | Throttle VLV | RELTHW_OBS | 0.005*q deg |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | `r[10]` | Vehicle Speed | VSP_OBS | q km/h |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | `U16BE(r[11..12])` | EVAP PC Duty (Hi Res) | DPCSENAN_OBS | 0.1*q % |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | `r[13]` | Slip Ratio of Torq or Startclutch | ETSENAN_OBS | q % |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | `S16BE(r[14..15])` | A/C Pressure Sensor | ACRP_OBS | q kPa |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | `r[16] & 1` | Idle Stop configuration | — | 0 = disabled, 1 = enabled |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | `(r[17] >> 0) & 1` | Engine Idle Stop Mode Control | ISSTAT_OBS | 0 = Uncontrolled; 1 = Controlled |
| `B9/0A/02/02/<record>` | `B9 0A 02 02 <record_hi> <record_lo>` | 0481 | Unavailable. Bit 1 of `r[17]` is stored as 0. | Target Idle Torque Correction Learning | CNDSCISTP_OBS | HDS nominal 0.02*q N·m |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `U16BE(r[10..11])` | Battery (Hi Res) | VBENAN_OBS | 0.0025*q V |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `(r[12] >> 1) & 1` | Transmission family configuration group | — | 0 = other, 1 = family 0x1000, 0x2000, or 0x3000 |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `(r[13] >> 0) & 1` | Brake Switch | BKSW_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `(r[13] >> 1) & 1` | PNP Switch | ATNP_OBS | 0 = GEAR; 1 = P-N |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `(r[13] >> 2) & 1` | PSP Switch | PSW_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `(r[13] >> 3) & 1` | A/C Clutch | ACC_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/03/<record>` | `B9 0A 02 03 <record_hi> <record_lo>` | 0490 | `r[14]` | EGR Valve Position Sensor (EGR Vls) | LIFT_OBS | 0.0196*q V |
| `B9/0A/02/04/<record>` | `B9 0A 02 04 <record_hi> <record_lo>` | 04A0 | `r[10]` | ECT Sensor 1 | ECT_OBS | q - 40 degC |
| `B9/0A/01/04/<record>` | `B9 0A 01 04 <record_hi> <record_lo>` | 04A8 | `r[10]` | ECT Sensor 1 | ECT_OBS | q - 40 °C |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `U24BE(r[10..12])` | TOTAL MILEAGE | DSTODO_OBS | q Km |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `U16BE(r[13..14])` | Eng Run Time | ERUNTM_OBS | q s |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `U16BE(r[15..16])` | Drive Dist | DSTTRV_OBS | 10*q m |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `U16BE(r[17..18])` | D/C Count | NESTART_OBS | q cranking events, saturating at 65535. |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 0) & 1` | Idle Stop Prohibition Information (TRANSMISSION) | ISINFTM_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 1) & 1` | Idle Stop Inhibit (PCU) | ISINFPCU_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 2) & 1` | Idle Stop Inhibit (DC-DC Converter) | ISINFDCDC_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 3) & 1` | Idle Stop Inhibit (Brake System) | ISINFVBM_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 4) & 1` | Idle Stop Inhibit (Battery Charging) | ISINFBAT_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 5) & 1` | Idle Stop Inhibit (HVAC) | ISINFAC_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 6) & 1` | Idle Stop Inhibit (ABS/VSA Unit) | ISINFVSA_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[20] >> 7) & 1` | Idle Stop Inhibit (EPS Unit) | ISINFEPS_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `r[21]` | Idle Stop configuration byte | — | Raw hex. 00 or E0. |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[22] >> 5) & 1` | Idle Stop Inhibit (Seat Belt Unfastened) | ISINFSBSW_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[22] >> 6) & 1` | Idle Stop Inhibit (Hood Open) | ISINFHSW_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[22] >> 7) & 1` | Idle Stop Inhibit (Idle Stop Cancel Switch) | ISINFCSW_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `r[23]` | Idle Stop configuration byte | — | Raw hex. 00, E0, or F8. |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[24] >> 3) & 1` | Idle Stop Inhibit(AT/CVT: Transition Condition) | ISINFTMTRG_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[24] >> 4) & 1` | Idle Stop Inhibit (Inclined State) | ISINFSLOPE_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[24] >> 5) & 1` | Idle Stop Inhibit (Operating Condition) | ISINFTRG_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[24] >> 6) & 1` | Idle Stop Inhibit (Driving History) | ISINFRUN_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[24] >> 7) & 1` | Idle Stop Inhibit (Precondition) | ISINFSTB_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `r[25]` | Idle Stop configuration byte | — | Raw hex. 00 or 03. |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[26] >> 0) & 1` | Idle Stop Inhibit (Low Battery Voltage) | ISINHDODR_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/02/00/0000` | `B9 0A 02 00 00 00` | 04B1 | `(r[26] >> 1) & 1` | Survey Infomation Battery Management(7) | BATTPOR_OBS | 0 = Normal; 1 = INHIBITED |
| `B9/0A/01/00/0000` | `B9 0A 01 00 00 00` | 04B9 | `U24BE(r[10..12])` | TOTAL MILEAGE | DSTODO_OBS | q km. Documented maximum is 999999. |
| `B9/0A/01/00/0000` | `B9 0A 01 00 00 00` | 04B9 | `U16BE(r[13..14])` | Eng Run Time | ERUNTM_OBS | q s |
| `B9/0A/01/00/0000` | `B9 0A 01 00 00 00` | 04B9 | `U16BE(r[15..16])` | Drive Dist | DSTTRV_OBS | 10*q m |
| `B9/0A/01/00/0000` | `B9 0A 01 00 00 00` | 04B9 | `r[17]` | HTS TRIGGER | PTOBSMHTS_OBS | 0x11 = A, 0x12 = B, 0x21 = C. Other values undefined. |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[10..11])` | Engine Speed | NE_OBS | 0.25*q RPM |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[12..13])` | MAP Sensor (Hi Res) | PBAENAN_OBS | 0.1*q mmHg |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[14..15])` | APP Sensor | APP_OBS | 0.005*q % |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[16..17])` | Throttle VLV | RELTHW_OBS | 0.005*q deg |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[18..19])` | F Injector | TOUT_OBS | 0.004*q ms |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `r[20]` | AF FB (ST Fuel Trim) | KLAF_OBS | q / 128 (ratio). HDS coefficient is 0.0078. |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[21..22])` | PURGE CORRECTION FACTOR | KEVACT_OBS | q / 32768 (ratio). HDS coefficient is 0.0000305. |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[23..24])` | MAF Sensor (Hi Res) | GAIRCYL_OBS | 0.1*q mg |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `r[25]` | EGR Valve Lift | LACT_OBS | 0.05*q mm |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `r[26]` | EGR L COMMAND | LCMD_OBS | 0.05*q mm |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[27..28])` | CKP Noise | NACCRK_OBS | q counts. |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[29..30])` | CKP No Pulse | DCCRK_OBS | q counts. |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[31..32])` | Misfire | CMFIRE_OBS | q counts, saturating at 65535. |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[33..34])` | Fuel Pressure Direct Injection System | PFDIO_OBS | 10*q kPa |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 04C2 | `U16BE(r[35..36])` | FUEL PRESSURE CONVERTED FROM PF SENSOR | PFDIREL_OBS | 10*q kPa |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[10..11])` | Engine Speed | NE_OBS | 0.25*q rpm |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[12..13])` | MAP Sensor (Hi Res) | PBAENAN_OBS | 0.1*q mmHg |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[14..15])` | APP Sensor | APP_OBS | 0.005*q % |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[16..17])` | Throttle VLV | RELTHW_OBS | 0.005*q ° |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[18..19])` | F Injector | TOUT_OBS | 0.004*q ms |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[20..21])` | Injector #4 on Duration | TOUT4_OBS | 0.004*q ms |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `r[22]` | ST Fuel Trim B1 | KLAF-B1_OBS | 0.0078*q ratio |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `r[23]` | ST Fuel Trim B2 | KLAF-B2_OBS | 0.0078*q ratio |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[24..25])` | PURGE CORRECTION FACTOR | KEVACT_OBS | 0.0000305*q ratio |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[26..27])` | PURGE CORRECTION FACTOR (FOR BANK2) | KEVACTB2_OBS | 0.0000305*q ratio |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `S16BE(r[28..29])` | Spark Advance (Hi Res) | IGENAN_OBS | 0.1*q ° |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[30..31])` | Target TH VLV | THO_OBS | 0.005*q ° |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[32..33])` | Commanded Electric Current of Linear Solenoid E | ICMDENAN_OBS | 0.1*q L/min |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[34..35])` | MAF Sensor (Hi Res) | GAIRCYL_OBS | 0.1*q mg |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `r[36]` | EGR Valve Lift | LACT_OBS | 0.05*q mm |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `r[37]` | EGR L COMMAND | LCMD_OBS | 0.05*q mm |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `r[38]` | VPS Status | NDACTCYL_OBS | Raw status code. |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[39..40])` | CKP Noise | NACCRK_OBS | q counts |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[41..42])` | CKP No Pulse | DCCRK_OBS | q counts |
| `B9/0A/02/01/<record>` | `B9 0A 02 01 <record_hi> <record_lo>` | 0470 | `U16BE(r[43..44])` | Misfire | CMFIRE_OBS | q counts |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[10..11])` | Engine Speed | NE_OBS | 0.25*q rpm |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[12..13])` | MAP Sensor (Hi Res) | PBAENAN_OBS | 0.1*q mmHg |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[14..15])` | APP Sensor | APP_OBS | 0.005*q % (HDS) |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[16..17])` | Throttle VLV | RELTHW_OBS | 0.005*q ° |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[18..19])` | F Injector | TOUT_OBS | 0.004*q ms |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `r[20]` | EGR Valve Lift | LACT_OBS | 0.05*q mm |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[21..22])` | Battery (Hi Res) | VBENAN_OBS | 0.0025*q V |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[23..24])` | CKP Noise | NACCRK_OBS | q counts |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[25..26])` | CKP No Pulse | DCCRK_OBS | q counts |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `r[27] & 1` | Immobilizer configuration flag | — | HDS requires bit 0 here and bit 5 of `r[6]` to display `IMBLIZ_OBS`. |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `(r[28] >> 0) & 1` | Immobilizer | IMBLIZ_OBS | 0 = RUN; 1 = BAN |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[29..30])` | CMP B Noise | NACTDC_OBS | q counts |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[31..32])` | CMP B No Pulse | DCTDC_OBS | q counts |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[33..34])` | MAF Sensor (Hi Res) | GAIRCYL_OBS | 0.1*q mg |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[35..36])` | Fuel Pressure Direct Injection System | PFDIO_OBS | 10*q kPa |
| `B9/0A/01/01/<record>` | `B9 0A 01 01 <record_hi> <record_lo>` | 04D2 | `U16BE(r[37..38])` | FUEL PRESSURE CONVERTED FROM PF SENSOR | PFDIREL_OBS | 10*q kPa |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[11] >> 2) & 1` | L INDICATOR | ATINDL_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[11] >> 3) & 1` | S INDICATOR | ATINDS_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[11] >> 4) & 1` | D INDICATOR | ATINDD_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[11] >> 5) & 1` | N INDICATOR | ATINDN_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[11] >> 6) & 1` | R INDICATOR | ATINDR_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[11] >> 7) & 1` | P INDICATOR | ATINDP_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `r[12]` | Zero-filled sample byte | — | 00 |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[13] >> 3) & 1` | 1 INDICATOR | ATIND1_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[13] >> 4) & 1` | 2 INDICATOR | ATIND2_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[13] >> 5) & 1` | 3 INDICATOR | ATINDD3_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[13] >> 6) & 1` | 4 INDICATOR | ATINDD4_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[13] >> 7) & 1` | 5 INDICATOR | ATINDD5_OBS | 0 = OFF; 1 = ON |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `U16BE(r[14..15])` (stored zero) | AT LC SOL VLV COMMAND | ICMDLC_OBS | 0.1*q mA |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `U16BE(r[16..17])` | AT LC SOL VLV ACTUAL | IACTLC_OBS | 0.1*q mA |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `U16BE(r[18..19])` | INPUT SHAFT(DRIVE PULLEY) SPEED RPM | NDRW_OBS | 0.25*q RPM |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `U16BE(r[20..21])` | OUTPUT SHAFT(DRIVE PULLEY) SPEED RPM | NDNW_OBS | 0.25*q RPM |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `U16BE(r[22..23])` | SECONDARY SHAFT SPEED1 (RPM) | VEL1W_OBS | 0.25*q RPM |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `r[24]` | PULLY RATIO | RATIO_OBS | q / 64 (ratio). HDS displays q. |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `U16BE(r[25..26])` | START CLUTCH CONTROL SOL ACTUAL | IACTSCW_OBS | 0.1*q mA |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `r[27]` | SCMODE0 | SCMODE0_OBS | Raw value. |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `U16BE(r[28..29])` | Torque Converter Turbine Speed | — | 0.25*q rpm |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `r[30]` | Pressure Control Solenoid Actual Current | — | 5*q mA |
| `B9/0A/02/05/<record>` | `B9 0A 02 05 <record_hi> <record_lo>` | 06A0 | `(r[32] >> 3) & 1` | Shift Solenoid B Command | — | 0 = OFF, 1 = ON |
| `B9/0A/02/06/<record>` | `B9 0A 02 06 <record_hi> <record_lo>` | 06B0 | `r[10]` | ATF/CVTF Temperature | TATF_OBS | q - 40 degC |
