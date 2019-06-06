# NeoVM

256 bit Virtual Machine with extendable instruction set.

## Overview
- Lightweight
- Platform-independed
- Hardware-independed
- Easy to extend instruction set (add your own instructions)
- Realtime and programmed instruction execution modes


**Features:**
- Abstract **VM** without global state (using ***Instances*** instead)
- Segmented memory model (for ***code***, ***registers*** and ***stacks***)
- Totally virtual 256-bit address space (different for each segment)
- 253 general purpose compound registers
- 6 separate stacks (for each ***bitdepth***)

## FAQ
**Registers**:
1. General:
```
; 256-bit
r256_0
r256_1
r256_2
r256_3
```
```
; 128-bit
r128_0, r128_1 ; (r256_0)
r128_2, r128_3 ; (r256_1)
r128_4, r128_5 ; (r256_2)
r128_6, r128_7 ; (r256_3)
```
```
; 64-bit
r64_0, r64_1   ; (r128_0)
...
r64_14, r64_15 ; (r128_7)
```
```
; 32-bit
r32_0, r32_1   ; (r64_0)
...
r32_14, r32_31 ; (r64_15)
```
```
; 16-bit
r16_0, r16_1   ; (r32_0)
...
r16_14, r16_63 ; (r32_31)
```
```
; 8-bit
r8_0, r8_1    ; (r16_0)
...
r8_14, r8_127 ; (r16_63)
```
2. Instruction pointer:
```
rip
```
3. Stacks ending pointers:
```
rsp8
rsp16
rsp32
rsp64
rsp128
rsp256
```

**Address**:
1. Code segment:
```
code_adr = num256
```
2. Stacks segment:
```
stack8_adr = rsp8 + 8 * num256
stack16_adr = rsp16 + 16 * num256
stack32_adr = rsp32 + 32 * num256
stack64_adr = rsp64 + 64 * num256
stack128_adr = rsp128 + 128 * num256
stack256_adr = rsp256 + 256 * num256
```
3. Registers segment:
```
reg8_adr = num8          ; 0 <= num8 < 128
reg16_adr = 128 + num8   ; 0 <= num8 < 64
reg32_adr = 192 + num8   ; 0 <= num8 < 32
reg64_adr = 224 + num8   ; 0 <= num8 < 16
reg128_adr = 240 + num8  ; 0 <= num8 < 8
reg256_adr = 248 + num8  ; 0 <= num8 < 4
```

Taking address:
```
instr num256               ; code_adr
instr num8                 ; reg_adr
instr rsp{k} + k * num256  ; stack{k}_adr
```