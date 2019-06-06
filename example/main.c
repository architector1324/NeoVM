#include "stdio.h"

#define VM_TARGET_ARCH64 // for correct vm_size_t
#include "neovm.h"



int main(){
    // VMInstance vm = VMInstanceDefault;

    // // program
    // /*
    //     rip: assembly          ; bytecode
    //     0  : snd 5, r16_0      ; 0x00000005 0x0005 0x80
    //     1  : dec r16_0, r16_1  ; 0x0000001d 0x80 0x81
    // */

    // vm_uint8_t bytecode[13] = {
    //     0x00, 0x00, 0x00, 0x05,
    //     0x00, 0x05, 0x80, 0x00, 
    //     0x00, 0x00, 0x1d, 0x80,
    //     0x81
    // };


    // VMProgram prog = vmParseProgram(bytecode, NULL);
    // vmExecProgram(&prog, &vm, NULL);


    // if(vm.halt)
    //     printf("Wrong instruction! VMInstance %p halted\n", &vm);

    vm_uint32_t a = {0x00, 0x01, 0x02, 0xff};
    vm_uint32_t b = {0xfe, 0xff, 0xff, 0x01};
    vm_add_ui32_result c = vm_add_ui32(a, b);

    printf("%#x\n", 0x000102ff + 0xfeffff01);

    return 0;
}