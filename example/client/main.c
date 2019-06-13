#include "stdio.h"

#define VM_TARGET_ARCH64 // for correct vm_size_t
#include "neovm.h"



int main(){
    VMInstance vm = vmInstance(1, 8192, (vm_uint32_t){127, 0, 0, 1}, (vm_uint16_t){0xc3, 0x50});

    // program
    /*
        rip: assembly ; bytecode
        0  : ask {127.0.0.1 / 60000 / 0} ; 0x00000020 0x7f000001ea600000
    */


    vm_uint8_t bytecode[12] = {
        0x00, 0x00, 0x00, 0x20,
        0x7f, 0x00, 0x00, 0x01,
        0xea, 0x60, 0x00, 0x00
    };


    VMProgram prog = vmParseProgram(bytecode, 1, NULL);
    vmExecProgram(&prog, 0, &vm, NULL);

    printf("Client asked\n");


    if(vm.halt)
        printf("Wrong instruction! VMInstance %p halted\n", &vm);

    vmReleaseInstance(&vm);

    return 0;
}