# METER — Mode 10 (Diagnostic Session Control)

Target: 2017 US Accord meter 78100-T2F-A140-M1. Unknown values are retained explicitly.

| Command | Name | Source | Header | Payload | Request bytes | Response layout | Response bytes | SecurityAccess mode | Field source | Field name | Field ID | Offset / bit | Mask | Parse / decode |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 10/01 | Enter default diagnostic session | METER A140 HDS/runtime research | 10 01 | — | 2 | 50 01 [P2/P2* timing bytes if supplied] | UNKNOWN | none proven | — | — | — | — | — | — |
| 10/03 | Enter extended diagnostic session | METER A140 HDS/runtime research | 10 03 | — | 2 | 50 03 [P2/P2* timing bytes if supplied] | UNKNOWN | none proven | — | — | — | — | — | — |
