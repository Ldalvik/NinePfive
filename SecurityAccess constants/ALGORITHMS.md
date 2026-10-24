# SecurityAccess algorithms
`BE16(a,b) = (a << 8) | b` = Read six-byte records as three consecutive BE16 words

| Record type | Meaning |
|---|---|
| SHARED16 | Parameter set associated with the shared 16-bit calculation |
| UNKNOWN | Six-byte record |
| MARKER | Literal record bytes |

## SHARED16
```
divisor = k2 != 0 ? k2 : 65536
key16 = ((seed16 + k0) XOR ((seed16 * k1) % divisor)) & FFFF
```

## TYPE_V
```tex
left  = ROL16((seed16 + SFC1) & FFFF, ROL)
right = ROR16((seed16 + SFC2) & FFFF, ROR)
key16 = ((left XOR right) + FINAL_ADD) & FFFF
```

## ROTATE32
```
left  = ROL32((seed32 + ADD_LEFT) & FFFFFFFF, ROL)
right = ROR32((seed32 + ADD_RIGHT) & FFFFFFFF, ROR)
key32 = ((left XOR right) + FINAL_ADD) & FFFFFFFF
```

## PRODUCT32
```text
product = (seed32 >> 16) * (seed32 & FFFF)
rotated = ROR32((seed32 + ADD_ROTATE) & FFFFFFFF, ROR)
key32 = ((seed32 XOR rotated XOR product) + FINAL_ADD) & FFFFFFFF
```

## Rotations
```text
ROL(x,n) = ((x << n) | (x >> (W-n))) & ((1 << W)-1)
ROR(x,n) = ((x >> n) | (x << (W-n))) & ((1 << W)-1)
```
