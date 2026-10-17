# NinePfive
Reverse engineering resources and tools for 9.5th gen Honda Accord (2016/2017)

### [Download .rwd/.bin files](firmware/)

### [UDS command extraction](UDS/)

# Mode 0x27 41/42 (TYPE_V/Master)
The following documented K24/J35 master calculations belong only to the `41/42` four-byte seed/key plus tag format. They are separate from the `01/02` programming steps above. The listed engine/tag associations do not establish support across all 2016–2017 ECUs or prove a complete L640 flashing workflow.

```
SS = 32-bit seed, big endian
KK = calculated 32-bit key, big-endian
TT = K24 = 0x15, J35 = 0x0C
```

```
Tester → ECU: 10 03
ECU → Tester: 50 03 ...

Tester → ECU: 27 41
ECU → Tester: 67 41 SS SS SS SS TT

Tester → ECU: 27 42 KK KK KK KK TT
ECU → Tester: 67 42
```

```C
/* K24, tag 0x15 */
uint32_t key_K24(uint32_t s)
{
    uint32_t x = s + 0xAA64D267;
    return (s ^ (x >> 3 | x << 29) ^ (s >> 16) * (s & 0xFFFF)) + 0xF9849207;
}

/* J35, tag 0x0C */
uint32_t key_J35(uint32_t s)
{
    uint32_t x = s + 0x2584E18A;
    return (s ^ (x >> 2 | x << 30) ^ (s >> 16) * (s & 0xFFFF)) + 0xBE463505;
}
```


# Flashing an ECU over OBD/FCAN
Download your ECU ROM, or use the stock .rwd if available. Sequence below uses the 2016/17 Accord K24 Sport ECU.

#### Application = 0x10000:0x200000, Length = 0x001F0000
#### SUM8 checksum = `sum(application) & 0xFF == 0`

### Compensation logic
```
residual = sum(application) & 0xFF
application[field] = (application[field] - residual) & 0xFF
```

Compensation address:  `0x33C4D` (5A0-L640, KL Sport CVT ECU)

### CVN 
```
A = sum(words) & 0xFFFF
X = xor_of_all_words
cvn = BE16((A + X) & 0xFFFF) + BE16((A - X) & 0xFFFF)
```
| Field | RWD offset/value |
|---|---|
| Signature | `5A 0D 0A` |
| CVN | 4 bytes @ `0x08` |
| Transform bytes | `BB BC 96` @ `0x73` |
| Application start | BE32 `0x00010000` @ `0x76` |
| Application length | BE32 `0x001F0000` @ `0x7A` |
| Encoded application | Starts @ `0x7E` |
| SUM32 trailer | Starts @ `0x1F007E` |
| Total RWD size | `0x1F0082` bytes |

Encode every checksum-corrected application byte:
```encoded_byte = ((plain_byte - 1) & 0xFF) ^ 0x96```

Sum every container byte after the 4 byte trailer:
```trailer = LE32(sum(rwd_without_trailer) & 0xFFFFFFFF)```

Confirm application correctness, SUM8 is 0, CVN matches the header, and SUM32 matches the trailer:
```plain_byte = ((encoded_byte ^ 0x96) + 1) & 0xFF```

### Programming (Flashing over FCAN)
This should work (not proven) for all modules that can be programmed over FCAN, this includes VSA, Gauge Cluster, etc. from 2007 and up. Exact years not confirmed, but 2007 and up seems to be likely.

#### Seed constants K1, K2, K3:
`Key = ((S × K2) mod K3) XOR (S + K1)`

#### Seed constants K1, K2:
`Key = (S × K2) XOR (S + K1)`

## Command sequence
`10 03` = Extended diagnostic session

`27 01` = Obtain normal programming seed (see above)

`27 02` <key_BE16> = Submit lower 2 bytes of the calculated key (Expect `67 02`)

`10 02` = Programming entry (Wait through `7F 10 78` for automatic `50 02`)

`31 01 FF 00` = Default application erase (Wait for final `71 01 FF 00`)

`2E F1 01 BB BC 96` = Set RWD decoding parameters (Expect `6E F1 01`)

`34 00 44 00 01 00 00 00 1F 00 00` = Download start `0x10000`, length `0x1F0000`

`36` <counter> <encoded application chunk> = Await `76 <matching counter>` for each block

`37` = Transfer exit; expect 77

`31 01 FF 01` = Application checksum and programming-completion validation (Wait for final 71 01 FF 01)

Some cars have a 5v+ programming wire on pin12 of the OBD. For vehicles without this, it can be ignored. Otherwise it is reuqired throughout the whole flashing procedure. Cycle ignition within 60 seconds of the flash completing, and do the PCM idle learn + CKP pattern clear/learn if your vehicle requires it.

# Honda HKWP initialization and MICU communication (BCAN/KLINE)

### J2534 setup + StartCommunication
```
PassThruConnect:
   ProtocolID = 0x04       // ISO14230
   Flags      = 0x200      // Application supplies checksums
   Baud       = 10400

SET_CONFIG, IoctlID = 0x02:
   Parameter : Value
   0x0E      : 300         // W1_MAX, ms
   0x0F      : 20          // W2_MAX, ms
   0x10      : 20          // W3_MAX, ms
   0x12      : 300         // W5_MIN, ms

PassThruStartMsgFilter:
   FilterType = 1          // PASS_FILTER
   Mask       = 00
   Pattern    = 00
   ProtocolID = 0x04
   DataSize   = 1

FAST_INIT, IoctlID = 0x05:
   ProtocolID     = 0x04
   TxFlags        = 0x200
   DataSize       = 5
   ExtraDataIndex = 4
   Data           = 81 46 F0 81 38
```
Successful StartCommunication = valid checksum and payload starting with 0xC1.

Once initialization is complete, you will need to find which MICU ID you want to target. 

### Finding the MICU diagnostic ID
| Profile | Request + checksum |
|---|---|
| MICU(DR), `0x13` | `80 13 F0 03 21 14 01 BC` |
| BCM, `0x18` | `80 18 F0 03 21 14 01 C1` |
| MICU(17), `0x17` | `80 17 F0 03 21 14 01 C0` |
| MICU(10), `0x10` | `80 10 F0 03 21 14 01 B9` |
| MICU(AS), `0x14` | `80 14 F0 03 21 14 01 BD` |

```
PASSTHRU_MSG:
   ProtocolID     = 0x04
   RxStatus       = 0
   TxFlags        = 0x200
   Timestamp      = 0
   DataSize       = 8
   ExtraDataIndex = 8
   Data           = 80 XX F0 03 21 14 01 CS
```
``XX`` is the diagnostic ID of the MICU you are pinging. CS is the checksum. 

Positive payload begins with `61 14`. Negative payloads begin with `7F 21`.
Ping each MICU for a response, if no positive response is received, switch to SLOW_INIT.

#### SLOW_INIT
When no responses are received in FAST_INIT, you can ping MICU(10) and MICU(AS) in SLOW_INIT mode.
Set 5 baud with a W5_MIN of 700ms instead of 300ms. 

```
SET_CONFIG, IoctlID = 0x02:
   Parameter = 0x12 // W5_MIN
   Value     = 700  // ms

FIVE_BAUD_INIT, IoctlID = 0x04:
   Input: SBYTE_ARRAY
      NumOfBytes = 1
      BytePtr    = [46]

   Output: SBYTE_ARRAY
      NumOfBytes = 2 // buffer
      BytePtr    = [00 00]
```

### Command example: Lock All Doors
Lock All Doors command: `80 XX F0 08 | 30 04 | 01 00 00 00 00 00 | CS`

| byte | value | name
|---|---|---|
| byte 0 | `80` | Addressed KWP header |
| byte 1 | `XX` | MICU diagnostic address |
| byte 2 | `F0` | tester |
| byte 3 | `08` | Payload length |
| byte 4 | `30` | Input/output control by local identifier |
| byte 5 | `04` | Door lock ID |
| byte 6 | `01` | Control byte (Lock All Doors) |
| bytes 7–11 | `00 00 00 00 00` | empty? |
| byte 12 | `CS` | SUM8 checksum |
