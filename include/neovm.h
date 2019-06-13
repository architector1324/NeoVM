#pragma once

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "neovm_types.h"


/////////////////////////////////////////
//              REGISTERS
/////////////////////////////////////////

typedef vm_uint8_t vm_r8;

typedef struct vm_r16{
    vm_r8 sub_r0;
    vm_r8 sub_r1;
} vm_r16;

typedef struct vm_r32{
    vm_r16 sub_r0;
    vm_r16 sub_r1;
} vm_r32;

typedef struct vm_r64{
    vm_r32 sub_r0;
    vm_r32 sub_r1;
} vm_r64;

typedef struct vm_r128{
    vm_r64 sub_r0;
    vm_r64 sub_r1;
} vm_r128;

typedef struct vm_r256{
    vm_r128 sub_r0;
    vm_r128 sub_r1;
} vm_r256;


/////////////////////////////////////////
//               VM BASE
/////////////////////////////////////////

typedef struct VMThread{
    vm_uint256_t pc; // program counter
    vm_bool lock, wait;

    int sock; // client / server socket

    vm_uint8_t nbuf8; // 8-bit net buffer
    vm_uint16_t nbuf16;
    vm_uint32_t nbuf32;
    vm_uint64_t nbuf64;
    vm_uint128_t nbuf128;
    vm_uint256_t nbuf256;
} VMThread;


typedef struct VMInstance{
    // registers
    vm_r256 r0, r1, r2, r3;
    vm_uint256_t se256, se128, se64, se32, se16, se8;
    vm_bool halt;

    VMThread* thread;
    vm_size_t threads_count;

    vm_uint32_t ip;
    vm_uint16_t port;

    // stacks
    vm_uint256_t* stack256;
    vm_uint128_t* stack128;
    vm_uint64_t* stack64;
    vm_uint32_t* stack32;
    vm_uint16_t* stack16;
    vm_uint8_t* stack8;

    vm_size_t stack_size;
} VMInstance;

VMThread _vmThread(VMInstance* vm, vm_size_t thread){
    VMThread result;

    result.pc = (vm_uint256_t){
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    result.lock = false;
    result.wait = false;


    // network
    result.sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in adr;
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = *((uint32_t*)&vm->ip);
    adr.sin_port = htons(ntohs(*((uint16_t*)&vm->port)) + thread);

    if(bind(result.sock, (const struct sockaddr*)&adr, sizeof(adr)) < 0) vm->thread[thread].lock = true;

    return result;
}

void _vmReleaseThread(VMThread* thread){
    thread->pc = (vm_uint256_t){
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    thread->lock = false;
    thread->wait = false;

    close(thread->sock);
    thread->sock = 0;
}

VMInstance vmInstance(vm_size_t threads_count, vm_size_t stack_size, vm_uint32_t ip, vm_uint16_t port){
    VMInstance result = {
        .halt = false,
        .ip = ip, 
        .port = port
    };

    // setup stack
    if(stack_size != 0){
        result.stack8 = malloc(stack_size);
        result.stack16 = malloc(stack_size / 2);
        result.stack32 = malloc(stack_size / 4);
        result.stack64 = malloc(stack_size / 8);
        result.stack128 = malloc(stack_size / 16);
        result.stack256 = malloc(stack_size / 32);
    }else{
        result.stack8 = NULL;
        result.stack16 = NULL;
        result.stack32 = NULL;
        result.stack64 = NULL;
        result.stack128 = NULL;
        result.stack256 = NULL;
    }

    // setup threads
    result.threads_count = threads_count;
    result.thread = malloc(threads_count * sizeof(VMThread));

    for(vm_size_t i = 0; i < threads_count; i++) result.thread[i] = _vmThread(&result, i);

    return result;
}

void vmReleaseInstance(VMInstance* vm){
    for(vm_size_t i = 0; i < vm->threads_count; i++) _vmReleaseThread(vm->thread + i);

    vm->threads_count = 0;
    vm->stack_size = 0;
    vm->ip = (vm_uint32_t){0, 0, 0, 0};
    vm->port = (vm_uint16_t){0, 0};

    free(vm->thread);
    free(vm->stack8);
    free(vm->stack16);
    free(vm->stack32);
    free(vm->stack64);
    free(vm->stack128);
    free(vm->stack256);
}


// register macros
#define VM_R8_COUNT 128
#define VM_R16_COUNT 64
#define VM_R32_COUNT 32
#define VM_R64_COUNT 16
#define VM_R128_COUNT 8
#define VM_R256_COUNT 4

#define VM_R8_START 0
#define VM_R16_START VM_R8_COUNT
#define VM_R32_START VM_R8_COUNT + VM_R16_COUNT
#define VM_R64_START VM_R8_COUNT + VM_R16_COUNT + VM_R32_COUNT
#define VM_R128_START VM_R8_COUNT + VM_R16_COUNT + VM_R32_COUNT + VM_R64_COUNT
#define VM_R256_START VM_R8_COUNT + VM_R16_COUNT + VM_R32_COUNT + VM_R64_COUNT + VM_R128_COUNT

#define VM_R8_END VM_R8_START + VM_R8_COUNT
#define VM_R16_END VM_R16_START + VM_R16_COUNT
#define VM_R32_END VM_R32_START + VM_R32_COUNT
#define VM_R64_END VM_R64_START + VM_R64_COUNT
#define VM_R128_END VM_R128_START + VM_R128_COUNT
#define VM_R256_END VM_R256_START + VM_R256_COUNT

#define VM_R8_INDEX_INBOUNDS(index) ((index >= VM_R8_START) && (index < VM_R8_END))
#define VM_R16_INDEX_INBOUNDS(index) ((index >= VM_R16_START) && (index < VM_R16_END))
#define VM_R32_INDEX_INBOUNDS(index) ((index >= VM_R32_START) && (index < VM_R32_END))
#define VM_R64_INDEX_INBOUNDS(index) ((index >= VM_R64_START) && (index < VM_R64_END))
#define VM_R128_INDEX_INBOUNDS(index) ((index >= VM_R128_START) && (index < VM_R128_END))
#define VM_R256_INDEX_INBOUNDS(index) ((index >= VM_R256_START) && (index < VM_R256_END))


#define VM_R8(reg, vm) ((vm_r8*)&(vm).r0)[(reg) % VM_R8_COUNT]
#define VM_R16(reg, vm) ((vm_r16*)&(vm).r0)[(reg) % VM_R16_COUNT]
#define VM_R32(reg, vm) ((vm_r32*)&(vm).r0)[(reg) % VM_R32_COUNT]
#define VM_R64(reg, vm) ((vm_r64*)&(vm).r0)[(reg) % VM_R64_COUNT]
#define VM_R128(reg, vm) ((vm_r128*)&(vm).r0)[(reg) % VM_R128_COUNT]
#define VM_R256(reg, vm) ((vm_r256*)&(vm).r0)[(reg) % VM_R256_COUNT]


// Instruction
typedef enum _VM_INSTRUCTION_TYPE {FREE, SINGLE, DOUBLE, TRIPLE} VM_INSTRUCTION_TYPE;
typedef enum _VM_OPERAND_TYPE {REGISTER, NUMBER, CODE_ADDRESS, REGISTER_ADDRESS, STACK_ADDRESS, NETWORK_ADDRESS} VM_OPERAND_TYPE;
typedef enum _VM_OPERAND_SIZE{
    UINT8_T = 1, UINT16_T = 2,
    UINT32_T = 4, UINT64_T = 8,
    UINT128_T = 16, UINT256_T = 32
} VM_OPERAND_SIZE;


#define VM_FREE_IMPL(impl) (((void(*)(vm_size_t, VMInstance*))(impl)))
#define VM_SINGLE_IMPL(impl) (((void(*)(const void*, vm_size_t, VMInstance*))(impl)))
#define VM_DOUBLE_IMPL(impl) (((void(*)(const void*, const void*, vm_size_t, VMInstance*))(impl)))
#define VM_TRIPLE_IMPL(impl) (((void(*)(const void*, const void*, const void*, vm_size_t, VMInstance*))(impl)))


typedef struct VMInstructionDescriptor{
    const VM_INSTRUCTION_TYPE itype;
    const VM_OPERAND_TYPE op0_type, op1_type, op2_type;
    const VM_OPERAND_SIZE op0_size, op1_size, op2_size;
    const vm_uint32_t icode;
    const char* alias;
    const void* impl;
} VMInstructionDescriptor;

typedef struct VMInstructionDescriptorsTable{
    const VMInstructionDescriptor* idt;
    vm_size_t size;
} VMInstructionDescriptorsTable;

typedef struct VMInstructionDescriptorsExt{
    VMInstructionDescriptorsTable** ext;
    vm_size_t size;
} VMInstructionDescriptorsExt;


typedef struct VMInstruction{
    const vm_uint32_t* icode;
    const void* op0;
    const void* op1;
    const void* op2;
} VMInstruction;

typedef struct VMProgram{
    VMInstruction program[1024];
    vm_size_t size;
} VMProgram;

typedef struct VMParser{
    VMInstruction instr;
    const vm_uint8_t* next;
} VMParser;



/////////////////////////////////////////////////////
//       BASE INSTRUCTION SET IMPLEMENTATIONS
/////////////////////////////////////////////////////

// more info on https://github.com/architector1324/NeoVM

void _vm_go_adr(const vm_uint256_t* adr, vm_size_t thread, VMInstance* vm){
    // go adr
    vm->thread[thread].pc = *adr;
}
void _vm_go_r(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // go r256
    vm_uint8_t _reg = *reg;

    if(VM_R256_INDEX_INBOUNDS(_reg))
        vm->thread[thread].pc = VM_UINT256_T(VM_R256(_reg - 248, *vm));
    else vm->halt = true;
}

void _vm_ask(const vm_uint64_t* nadr, vm_size_t thread, VMInstance* vm){
    vm_bool hang = true;

    struct sockaddr_in adr;

    vm_uint16_t port = {nadr->bytes[4], nadr->bytes[5]};
    vm_uint32_t ip = {nadr->bytes[0], nadr->bytes[1], nadr->bytes[2], nadr->bytes[3]};

    adr.sin_family = AF_INET; 
    adr.sin_port = *((const uint16_t*)&port);
    adr.sin_addr.s_addr = *((const uint32_t*)&ip);

    if(vm->thread[thread].wait == false){
        sendto(vm->thread[thread].sock, &hang, 1, MSG_CONFIRM, (const struct sockaddr*)&adr, sizeof(adr));
        vm->thread[thread].wait = true;
    }

    int len;
    if(recvfrom(vm->thread[thread].sock, &hang, 1, MSG_DONTWAIT, (struct sockaddr*)&adr, &len) > 0) vm->thread[thread].wait = false;

    int dump = 0;
}
void _vm_answer(vm_size_t thread, VMInstance* vm){
    vm_bool hang = true;
    vm->thread[thread].wait = true;

    int len;

    struct sockaddr_in adr;

    if(recvfrom(vm->thread[thread].sock, &hang, 1, MSG_DONTWAIT, (struct sockaddr*)&adr, &len) > 0){
        sendto(vm->thread[thread].sock, &hang, 1, MSG_CONFIRM, (const struct sockaddr*)&adr, sizeof(adr));
        vm->thread[thread].wait = false;
    }
}

void _vm_snd_r_r(const vm_uint8_t* reg0, const vm_uint8_t* reg1, vm_size_t thread, VMInstance* vm){
    // snd from_r, to_r
    vm_uint8_t _reg0 = *reg0;
    vm_uint8_t _reg1 = *reg1;

    if(VM_R8_INDEX_INBOUNDS(_reg0) && VM_R8_INDEX_INBOUNDS(_reg1))
        VM_R8(_reg1 - VM_R8_END, *vm) = VM_R8(_reg0 - VM_R8_END, *vm);
    else if(VM_R16_INDEX_INBOUNDS(_reg0) && VM_R16_INDEX_INBOUNDS(_reg1))
        VM_R16(_reg1 - VM_R16_END, *vm) = VM_R16(_reg0 - VM_R16_END, *vm);
    else if(VM_R32_INDEX_INBOUNDS(_reg0) && VM_R32_INDEX_INBOUNDS(_reg1))
        VM_R32(_reg1 - VM_R32_END, *vm) = VM_R32(_reg0 - VM_R32_END, *vm);
    else if(VM_R64_INDEX_INBOUNDS(_reg0) && VM_R64_INDEX_INBOUNDS(_reg1))
        VM_R64(_reg1 - VM_R64_END, *vm) = VM_R64(_reg0 - VM_R64_END, *vm);
    else if(VM_R128_INDEX_INBOUNDS(_reg0) && VM_R128_INDEX_INBOUNDS(_reg1))
        VM_R128(_reg1 - VM_R128_END, *vm) = VM_R128(_reg0 - VM_R128_END, *vm);
    else if(VM_R256_INDEX_INBOUNDS(_reg0) && VM_R256_INDEX_INBOUNDS(_reg1))
        VM_R256(_reg1 - VM_R256_END, *vm) = VM_R256(_reg0 - VM_R256_END, *vm);
    else vm->halt = true;
}
void _vm_snd_num_r8(const vm_uint8_t* num, const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // snd num, r8
    vm_uint8_t _reg = *reg;

    if(VM_R8_INDEX_INBOUNDS(_reg))
        VM_UINT8_T(VM_R8(*reg - VM_R8_END, *vm)) = *num;
    else vm->halt = true;
}
void _vm_snd_num_r16(const vm_uint16_t* num, const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // snd num, r16
    vm_uint8_t _reg = *reg;

    if(VM_R16_INDEX_INBOUNDS(_reg))
        VM_UINT16_T(VM_R16(*reg - VM_R16_END, *vm)) = *num;
    else vm->halt = true;
}
void _vm_snd_num_r32(const vm_uint32_t* num, const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // snd num, r32
    vm_uint8_t _reg = *reg;

    if(VM_R32_INDEX_INBOUNDS(_reg))
        VM_UINT32_T(VM_R32(*reg - VM_R32_END, *vm)) = *num;
    else vm->halt = true;
}
void _vm_snd_num_r64(const vm_uint64_t* num, const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // snd num, r64
    vm_uint8_t _reg = *reg;

    if(VM_R64_INDEX_INBOUNDS(_reg))
        VM_UINT64_T(VM_R64(*reg - VM_R64_END, *vm)) = *num;
    else vm->halt = true;
}
void _vm_snd_num_r128(const vm_uint128_t* num, const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // snd num, r128
    vm_uint8_t _reg = *reg;

    if(VM_R128_INDEX_INBOUNDS(_reg))
        VM_UINT128_T(VM_R128(*reg - VM_R128_END, *vm)) = *num;
    else vm->halt = true;
}
void _vm_snd_num_r256(const vm_uint256_t* num, const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // snd num, r256
    vm_uint8_t _reg = *reg;

    if(VM_R256_INDEX_INBOUNDS(_reg))
        VM_UINT256_T(VM_R256(*reg - VM_R256_END, *vm)) = *num;
    else vm->halt = true;
}

void _vm_push8_num(const vm_uint8_t* num, vm_size_t thread, VMInstance* vm){
    // push8 num
    vm->stack8[vm_ui256_to_size_t(vm->se8)] = *num;
    vm->se8 = vm_inc_ui256(vm->se8);
}
void _vm_push16_num(const vm_uint16_t* num, vm_size_t thread, VMInstance* vm){
    // push16 num
    vm->stack16[vm_ui256_to_size_t(vm->se16)] = *num;
    vm->se16 = vm_inc_ui256(vm->se16);
}
void _vm_push32_num(const vm_uint32_t* num, vm_size_t thread, VMInstance* vm){
    // push32 num
    vm->stack32[vm_ui256_to_size_t(vm->se32)] = *num;
    vm->se32 = vm_inc_ui256(vm->se32);
}
void _vm_push64_num(const vm_uint64_t* num, vm_size_t thread, VMInstance* vm){
    // push64 num
    vm->stack64[vm_ui256_to_size_t(vm->se64)] = *num;
    vm->se64 = vm_inc_ui256(vm->se64);
}
void _vm_push128_num(const vm_uint128_t* num, vm_size_t thread, VMInstance* vm){
    // push128 num
    vm->stack128[vm_ui256_to_size_t(vm->se128)] = *num;
    vm->se128 = vm_inc_ui256(vm->se128);
}
void _vm_push256_num(const vm_uint256_t* num, vm_size_t thread, VMInstance* vm){
    // push256 num
    vm->stack256[vm_ui256_to_size_t(vm->se256)] = *num;
    vm->se256 = vm_inc_ui256(vm->se256);
}

void _vm_push8_r(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // push8 r8
    vm_uint8_t _reg = *reg;

    if(VM_R8_INDEX_INBOUNDS(_reg)){
        vm->stack8[vm_ui256_to_size_t(vm->se8)] = VM_UINT8_T(VM_R8(*reg - VM_R8_END, *vm));
        vm->se8 = vm_inc_ui256(vm->se8);
    }else vm->halt = true;
}
void _vm_push16_r(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // push16 r16
    vm_uint8_t _reg = *reg;

    if(VM_R16_INDEX_INBOUNDS(_reg)){
        vm->stack16[vm_ui256_to_size_t(vm->se16)] = VM_UINT16_T(VM_R16(*reg - VM_R16_END, *vm));
        vm->se16 = vm_inc_ui256(vm->se16);
    }else vm->halt = true;
}
void _vm_push32_r(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // push32 r32
    vm_uint8_t _reg = *reg;

    if(VM_R32_INDEX_INBOUNDS(_reg)){
        vm->stack32[vm_ui256_to_size_t(vm->se32)] = VM_UINT32_T(VM_R32(*reg - VM_R32_END, *vm));
        vm->se32 = vm_inc_ui256(vm->se32);
    }else vm->halt = true;
}
void _vm_push64_r(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // push64 r64
    vm_uint8_t _reg = *reg;

    if(VM_R64_INDEX_INBOUNDS(_reg)){
        vm->stack64[vm_ui256_to_size_t(vm->se64)] = VM_UINT64_T(VM_R64(*reg - VM_R64_END, *vm));
        vm->se64 = vm_inc_ui256(vm->se64);
    }else vm->halt = true;
}
void _vm_push128_r(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // push128 r128
    vm_uint8_t _reg = *reg;

    if(VM_R128_INDEX_INBOUNDS(_reg)){
        vm->stack128[vm_ui256_to_size_t(vm->se128)] = VM_UINT128_T(VM_R128(*reg - VM_R128_END, *vm));
        vm->se128 = vm_inc_ui256(vm->se128);
    }else vm->halt = true;
}
void _vm_push256_r(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // push256 r256
    vm_uint8_t _reg = *reg;

    if(VM_R256_INDEX_INBOUNDS(_reg)){
        vm->stack256[vm_ui256_to_size_t(vm->se256)] = VM_UINT256_T(VM_R256(*reg - VM_R256_END, *vm));
        vm->se256 = vm_inc_ui256(vm->se256);
    }else vm->halt = true;
}

void _vm_pop8(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // pop8 r8
    vm_uint8_t _reg = *reg;

    if(VM_R8_INDEX_INBOUNDS(_reg)){
        vm->se8 = vm_dec_ui256(vm->se8);
        VM_UINT8_T(VM_R8(*reg - VM_R8_END, *vm)) = vm->stack8[vm_ui256_to_size_t(vm->se8)];
    }else vm->halt = true;
}
void _vm_pop16(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // pop16 r16
    vm_uint8_t _reg = *reg;

    if(VM_R16_INDEX_INBOUNDS(_reg)){
        vm->se16 = vm_dec_ui256(vm->se16);
        VM_UINT16_T(VM_R16(*reg - VM_R16_END, *vm)) = vm->stack16[vm_ui256_to_size_t(vm->se16)];
    }else vm->halt = true;
}
void _vm_pop32(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // pop32 r32
    vm_uint8_t _reg = *reg;

    if(VM_R32_INDEX_INBOUNDS(_reg)){
        vm->se32 = vm_dec_ui256(vm->se32);
        VM_UINT32_T(VM_R32(*reg - VM_R32_END, *vm)) = vm->stack32[vm_ui256_to_size_t(vm->se32)];
    }else vm->halt = true;
}
void _vm_pop64(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // pop64 r64
    vm_uint8_t _reg = *reg;

    if(VM_R64_INDEX_INBOUNDS(_reg)){
        vm->se64 = vm_dec_ui256(vm->se64);
        VM_UINT64_T(VM_R64(*reg - VM_R64_END, *vm)) = vm->stack64[vm_ui256_to_size_t(vm->se64)];
    }else vm->halt = true;
}
void _vm_pop128(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // pop128 r128
    vm_uint8_t _reg = *reg;

    if(VM_R128_INDEX_INBOUNDS(_reg)){
        vm->se128 = vm_dec_ui256(vm->se128);
        VM_UINT128_T(VM_R128(*reg - VM_R128_END, *vm)) = vm->stack128[vm_ui256_to_size_t(vm->se128)];
    }else vm->halt = true;
}
void _vm_pop256(const vm_uint8_t* reg, vm_size_t thread, VMInstance* vm){
    // pop256 r256
    vm_uint8_t _reg = *reg;

    if(VM_R256_INDEX_INBOUNDS(_reg)){
        vm->se256 = vm_dec_ui256(vm->se256);
        VM_UINT256_T(VM_R256(*reg - VM_R256_END, *vm)) = vm->stack256[vm_ui256_to_size_t(vm->se256)];
    }else vm->halt = true;
}

void _vm_inc_r_r(const vm_uint8_t* reg0, const vm_uint8_t* reg1, vm_size_t thread, VMInstance* vm){
    // inc from_r, to_r ; snd from_r + 1, to_r
    vm_uint8_t _reg0 = *reg0;
    vm_uint8_t _reg1 = *reg1;

    if(VM_R8_INDEX_INBOUNDS(_reg0) && VM_R8_INDEX_INBOUNDS(_reg1))
        VM_UINT8_T(VM_R8(_reg1 - VM_R8_END, *vm)) = VM_UINT8_T(VM_R8(_reg0 - VM_R8_END, *vm)) + 1;
    else if(VM_R16_INDEX_INBOUNDS(_reg0) && VM_R16_INDEX_INBOUNDS(_reg1))
        VM_UINT16_T(VM_R16(_reg1 - VM_R16_END, *vm)) = vm_inc_ui16(VM_UINT16_T(VM_R16(_reg0 - VM_R16_END, *vm)));
    else if(VM_R32_INDEX_INBOUNDS(_reg0) && VM_R32_INDEX_INBOUNDS(_reg1))
        VM_UINT32_T(VM_R32(_reg1 - VM_R32_END, *vm)) = vm_inc_ui32(VM_UINT32_T(VM_R32(_reg0 - VM_R32_END, *vm)));
    else if(VM_R64_INDEX_INBOUNDS(_reg0) && VM_R64_INDEX_INBOUNDS(_reg1))
        VM_UINT64_T(VM_R64(_reg1 - VM_R64_END, *vm)) = vm_inc_ui64(VM_UINT64_T(VM_R64(_reg0 - VM_R64_END, *vm)));
    else if(VM_R128_INDEX_INBOUNDS(_reg0) && VM_R128_INDEX_INBOUNDS(_reg1))
        VM_UINT128_T(VM_R128(_reg1 - VM_R128_END, *vm)) = vm_inc_ui128(VM_UINT128_T(VM_R128(_reg0 - VM_R128_END, *vm)));
    else if(VM_R256_INDEX_INBOUNDS(_reg0) && VM_R256_INDEX_INBOUNDS(_reg1))
        VM_UINT256_T(VM_R256(_reg1 - VM_R256_END, *vm)) = vm_inc_ui256(VM_UINT256_T(VM_R256(_reg0 - VM_R256_END, *vm)));
    else vm->halt = true;
}
void _vm_dec_r_r(const vm_uint8_t* reg0, const vm_uint8_t* reg1, vm_size_t thread, VMInstance* vm){
    // dec from_r, to_r ; snd from_r - 1, to_r
    vm_uint8_t _reg0 = *reg0;
    vm_uint8_t _reg1 = *reg1;

    if(VM_R8_INDEX_INBOUNDS(_reg0) && VM_R8_INDEX_INBOUNDS(_reg1))
        VM_UINT8_T(VM_R8(_reg1 - VM_R8_END, *vm)) = VM_UINT8_T(VM_R8(_reg0 - VM_R8_END, *vm)) - 1;
    else if(VM_R16_INDEX_INBOUNDS(_reg0) && VM_R16_INDEX_INBOUNDS(_reg1))
        VM_UINT16_T(VM_R16(_reg1 - VM_R16_END, *vm)) = vm_dec_ui16(VM_UINT16_T(VM_R16(_reg0 - VM_R16_END, *vm)));
    else if(VM_R32_INDEX_INBOUNDS(_reg0) && VM_R32_INDEX_INBOUNDS(_reg1))
        VM_UINT32_T(VM_R32(_reg1 - VM_R32_END, *vm)) = vm_dec_ui32(VM_UINT32_T(VM_R32(_reg0 - VM_R32_END, *vm)));
    else if(VM_R64_INDEX_INBOUNDS(_reg0) && VM_R64_INDEX_INBOUNDS(_reg1))
        VM_UINT64_T(VM_R64(_reg1 - VM_R64_END, *vm)) = vm_dec_ui64(VM_UINT64_T(VM_R64(_reg0 - VM_R64_END, *vm)));
    else if(VM_R128_INDEX_INBOUNDS(_reg0) && VM_R128_INDEX_INBOUNDS(_reg1))
        VM_UINT128_T(VM_R128(_reg1 - VM_R128_END, *vm)) = vm_dec_ui128(VM_UINT128_T(VM_R128(_reg0 - VM_R128_END, *vm)));
    else if(VM_R256_INDEX_INBOUNDS(_reg0) && VM_R256_INDEX_INBOUNDS(_reg1))
        VM_UINT256_T(VM_R256(_reg1 - VM_R256_END, *vm)) = vm_dec_ui256(VM_UINT256_T(VM_R256(_reg0 - VM_R256_END, *vm)));
    else vm->halt = true;
}


/////////////////////////////////////////////////////
//       GLOBAL INSTRUCTION DESCRIPTORS TABLE
/////////////////////////////////////////////////////
VMInstructionDescriptor _GIDT[31] = {
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = CODE_ADDRESS,
        .op0_size = UINT32_T,
        .icode = {0x00, 0x00, 0x00, 0x01},
        .alias = "go",
        .impl = _vm_go_adr
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x02},
        .alias = "go",
        .impl = _vm_go_r
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = REGISTER, .op1_type = REGISTER,
        .op0_size = UINT8_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x03},
        .alias = "snd",
        .impl = _vm_snd_r_r
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = NUMBER, .op1_type = REGISTER,
        .op0_size = UINT8_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x04},
        .alias = "snd",
        .impl = _vm_snd_num_r8
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = NUMBER, .op1_type = REGISTER,
        .op0_size = UINT16_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x05},
        .alias = "snd",
        .impl = _vm_snd_num_r16
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = NUMBER, .op1_type = REGISTER,
        .op0_size = UINT32_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x06},
        .alias = "snd",
        .impl = _vm_snd_num_r32
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = NUMBER, .op1_type = REGISTER,
        .op0_size = UINT64_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x07},
        .alias = "snd",
        .impl = _vm_snd_num_r64
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = NUMBER, .op1_type = REGISTER,
        .op0_size = UINT128_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x08},
        .alias = "snd",
        .impl = _vm_snd_num_r128
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = NUMBER, .op1_type = REGISTER,
        .op0_size = UINT256_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x09},
        .alias = "snd",
        .impl = _vm_snd_num_r256
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = NUMBER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x0a},
        .alias = "push8",
        .impl = _vm_push8_num
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = NUMBER,
        .op0_size = UINT16_T,
        .icode = {0x00, 0x00, 0x00, 0x0b},
        .alias = "push16",
        .impl = _vm_push16_num
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = NUMBER,
        .op0_size = UINT32_T,
        .icode = {0x00, 0x00, 0x00, 0x0c},
        .alias = "push32",
        .impl = _vm_push32_num
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = NUMBER,
        .op0_size = UINT64_T,
        .icode = {0x00, 0x00, 0x00, 0x0d},
        .alias = "push64",
        .impl = _vm_push64_num
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = NUMBER,
        .op0_size = UINT128_T,
        .icode = {0x00, 0x00, 0x00, 0x0e},
        .alias = "push128",
        .impl = _vm_push128_num
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = NUMBER,
        .op0_size = UINT256_T,
        .icode = {0x00, 0x00, 0x00, 0x0f},
        .alias = "push256",
        .impl = _vm_push256_num
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x10},
        .alias = "push8",
        .impl = _vm_push8_r
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x11},
        .alias = "push16",
        .impl = _vm_push16_r
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x12},
        .alias = "push32",
        .impl = _vm_push32_r
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x13},
        .alias = "push64",
        .impl = _vm_push64_r
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x14},
        .alias = "push128",
        .impl = _vm_push128_r
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x15},
        .alias = "push256",
        .impl = _vm_push256_r
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x16},
        .alias = "pop8",
        .impl = _vm_pop8
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x17},
        .alias = "pop16",
        .impl = _vm_pop16
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x18},
        .alias = "pop32",
        .impl = _vm_pop32
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x19},
        .alias = "pop64",
        .impl = _vm_pop64
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x1a},
        .alias = "pop128",
        .impl = _vm_pop128
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = REGISTER,
        .op0_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x1b},
        .alias = "pop256",
        .impl = _vm_pop256
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = REGISTER, .op1_type = REGISTER,
        .op0_size = UINT8_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x1c},
        .alias = "inc",
        .impl = _vm_inc_r_r
    },
    (VMInstructionDescriptor){
        .itype = DOUBLE,
        .op0_type = REGISTER, .op1_type = REGISTER,
        .op0_size = UINT8_T, .op1_size = UINT8_T,
        .icode = {0x00, 0x00, 0x00, 0x1d},
        .alias = "dec",
        .impl = _vm_dec_r_r
    },
    (VMInstructionDescriptor){
        .itype = SINGLE,
        .op0_type = NETWORK_ADDRESS,
        .op0_size = UINT64_T,
        .icode = {0x00, 0x00, 0x00, 0x20},
        .alias = "ask",
        .impl = _vm_ask
    },
    (VMInstructionDescriptor){
        .itype = FREE,
        .icode = {0x00, 0x00, 0x00, 0x21},
        .alias = "answer",
        .impl = _vm_answer
    }
};

VMInstructionDescriptorsTable GIDT = {
    .idt = _GIDT,
    .size = 32
};


/////////////////////////////////////////
//               METHODS
/////////////////////////////////////////

const VMInstructionDescriptor* _vmFindInstructionIDT(const vm_uint32_t* icode, const VMInstructionDescriptorsTable* idt){
    vm_size_t size = idt->size;

    for(vm_size_t i = 0; i < size; i++){
        if(vm_equal_ui32(*icode, idt->idt[i].icode)) return idt->idt + i;
    }    
    return NULL;
}
const VMInstructionDescriptor* vmFindInstruction(const vm_uint32_t* icode, const VMInstructionDescriptorsExt* ext){
    const VMInstructionDescriptor* desc = _vmFindInstructionIDT(icode, &GIDT);
    if(desc != NULL) return desc;

    if(ext != NULL){
        for(vm_size_t i = 0; i < ext->size; i++){
            desc = _vmFindInstructionIDT(icode, ext->ext[i]);
            if(desc != NULL) return desc;
        }
    }

    return NULL;
}

void vmExecInstruction(const VMInstruction* instr, vm_size_t thread, VMInstance* vm, const VMInstructionDescriptorsExt* ext){
    if(vm->halt == false){
        const VMInstructionDescriptor* desc = vmFindInstruction(instr->icode, ext);

        if(desc != NULL){
            switch (desc->itype){
            case FREE:
                VM_FREE_IMPL(desc->impl)(thread, vm);
                break;
            case SINGLE:
                VM_SINGLE_IMPL(desc->impl)(instr->op0, thread, vm);
                break;
            case DOUBLE:
                VM_DOUBLE_IMPL(desc->impl)(instr->op0, instr->op1, thread, vm);
                break;
            case TRIPLE:
                VM_TRIPLE_IMPL(desc->impl)(instr->op0, instr->op1, instr->op2, thread, vm);
                break;
            default:
                break;
            }
        }
    }else ; // do some exception here
}

// single thread
void vmExecProgram(const VMProgram* prog, vm_size_t thread, VMInstance* vm, const VMInstructionDescriptorsExt* ext){
    if(thread < vm->threads_count){
        if(vm->thread[thread].lock == false){
            VM_UINT256_T(vm->thread[thread].pc) = (vm_uint256_t){
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00
            };

            for(vm_size_t i = 0; i < prog->size;){
                if(vm->halt) break;
                vmExecInstruction(prog->program + vm_ui256_to_size_t(vm->thread[thread].pc), thread, vm, ext);
                if(vm->thread[thread].wait == false){
                    VM_UINT256_T(vm->thread[thread].pc) = vm_inc_ui256(VM_UINT256_T(vm->thread[thread].pc));
                    i++;
                }
            }
        }
    } else vm->halt = true;
}

void vmExecProgramMT(const VMProgram* progs, vm_size_t progs_count, VMInstance* vm, const VMInstructionDescriptorsExt* ext){
    
}

// multithread



VMParser vmParseInstruction(const vm_uint8_t* bytecode, const VMInstructionDescriptorsExt* ext){
    VMParser result = {
        .instr = {.icode = NULL},
        .next = bytecode
    };

    const VMInstructionDescriptor* desc = vmFindInstruction((vm_uint32_t*)bytecode, ext);

    if(desc != NULL){
        result.instr.icode = (vm_uint32_t*)bytecode;

        switch (desc->itype){
        case FREE:
            result.next = bytecode + sizeof(vm_uint32_t);
            return result;
            break;
        case SINGLE:
            result.instr.op0 = bytecode + sizeof(vm_uint32_t);
            result.next = bytecode + sizeof(vm_uint32_t) + desc->op0_size;
            return result;
            break;
        case DOUBLE:
            result.instr.op0 = bytecode + sizeof(vm_uint32_t);
            result.instr.op1 = bytecode + sizeof(vm_uint32_t) + desc->op0_size;
            result.next = bytecode + sizeof(vm_uint32_t) + desc->op0_size + desc->op1_size;
            return result;
        case TRIPLE:
            result.instr.op0 = bytecode + sizeof(vm_uint32_t);
            result.instr.op1 = bytecode + sizeof(vm_uint32_t) + desc->op0_size;
            result.instr.op2 = bytecode + sizeof(vm_uint32_t) + desc->op0_size + desc->op1_size;
            result.next = bytecode + sizeof(vm_uint32_t) + desc->op0_size + desc->op1_size + desc->op2_size;
            return result;
        default:
            break;
        }
    }

    return result;
}

VMProgram vmParseProgram(const vm_uint8_t* bytecode, vm_size_t prog_size, const VMInstructionDescriptorsExt* ext){
    VMProgram result = {.size = 0};

    VMParser parser = vmParseInstruction(bytecode, ext);

    while(parser.instr.icode != NULL && result.size < prog_size){
        result.program[result.size++] = parser.instr;
        parser = vmParseInstruction(parser.next, ext);
    }

    return result;
}