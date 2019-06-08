#include "stdio.h"

#define VM_TARGET_ARCH64 // for correct vm_size_t
#include "neovm.h"


int main(){
    VMInstance vm = VMInstanceDefault;

    // program
    /*
        rip: assembly          ; bytecode
        0  : snd 5, r16_0      ; 0x00000005 0x0005 0x80
        1  : dec r16_0, r16_1  ; 0x0000001d 0x80 0x81
    */

    vm_uint8_t bytecode[13] = {
        0x00, 0x00, 0x00, 0x05,
        0x00, 0x05, 0x80, 0x00, 
        0x00, 0x00, 0x1d, 0x80,
        0x81
    };


    VMProgram prog = vmParseProgram(bytecode, NULL);
    vmExecProgram(&prog, 0, &vm, NULL);


    if(vm.halt)
        printf("Wrong instruction! VMInstance %p halted\n", &vm);

    return 0;
}