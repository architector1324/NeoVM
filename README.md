# NeoVM

256 bit Virtual Machine with extendable instruction set.

## Overview
- Lightweight
- Crossplatform
- Hardware-independed
- Easy to extend instruction set (add your own instructions)
- Realtime and programmed instruction execution modes
- Fake multithreading ([TDM](https://en.wikipedia.org/wiki/Time-division_multiplexing))
- Networking ([UDP](https://en.wikipedia.org/wiki/User_Datagram_Protocol))


**Features:**
- Abstract **VM** without global state (using ***Instances*** instead)
- Each Instance has ***execution threads***
- Instances can communicate by ***network***
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
2. Instruction pointers (for threads):
```
pc0
pc1
...
pc{k - 1}  ; k is a number of threads
```
3. Stacks ending pointers:
```
se8
se16
se32
se64
se128
se256
```

**Address**:
1. Code segment:
```
code_adr = num256
```
2. Stacks segment:
```
stack8_adr = se8 - 8 * (num256 + 1)
stack16_adr = se16 - 16 * (num256 + 1)
stack32_adr = se32 - 32 * (num256 + 1)
stack64_adr = se64 - 64 * (num256 + 1)
stack128_adr = se128 - 128 * (num256 + 1)
stack256_adr = se256 - 256 * (num256 + 1)
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

Taking value by address (number):
```
[adr] = num
```

Taking address:
```
instr code_adr num256      ; code address
instr reg{k}_adr num8      ; same as `instr r{k}_{m + num8}`
instr stack{k}_adr num256  ; stack{k} address
```

```
instr code_adr r256      ; code address stored in r256
instr reg{k}_adr r8      ; register address stored in r8
instr stack{k}_adr r256  ; stack{k} address stored in r256
```
```
instr code_adr [stack256_adr num256]      ; code address stored in stack256
instr reg{k}_adr [stack8_adr num256]      ; register address stored in stack8
instr stack{k}_adr [stack256_adr num256]  ; stack{k} address stored in stack256
```

*Note*: For short form we will use `code_adr`, `reg_adr` and `stack_adr` next.

**Multithreading**:
```
; mutex
lock    ; lock all threads except current
; some code
unlock  ; unlock all threads except current
```

**Networking**:
Comming soon...
```
real_port = port + thread
```
```
instr {ip / port / thread}
```

*Note*: Each Instance is working as server and client at the same time. You can use instructions to send and receive some data by network without unnecessary trouble!

**Base instruction set**:

1. Go:
```
go code_adr
```

2. Sending data:
```
snd from, to
```
```
snd num, reg
snd num, reg_adr
snd num, stack_adr

snd reg, reg
snd reg, reg_adr
snd reg, stack_adr

snd stack_adr, reg
snd stack_adr, reg_adr
snd stack_adr, stack_adr

snd reg_adr, reg
snd reg_adr, reg_adr
snd reg_adr, stack_adr

snd [stack_adr], reg
snd [stack_adr], reg_adr
snd [stack_adr], stack_adr

snd [reg_adr], reg
snd [reg_adr], reg_adr
snd [reg_adr], stack_adr
```