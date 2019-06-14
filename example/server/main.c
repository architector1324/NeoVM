#include "stdio.h"

#define VM_TARGET_ARCH64 // for correct vm_size_t
#include "neovm.h"



int main(){
    VMInstance vm = vmInstance(1, 8192, (vm_uint32_t){192, 168, 1, 52}, (vm_uint16_t){0xea, 0x60});

    // program
    /*
        rip: assembly  ; bytecode
        0  : answer    ; 0x00000021
    */

    vm_uint8_t bytecode[4] = {
        0x00, 0x00, 0x00, 0x21
    };


    VMProgram prog = vmParseProgram(bytecode, 1, NULL);
    vmExecProgram(&prog, 0, &vm, NULL);

    printf("Server answered\n");


    if(vm.halt)
        printf("Wrong instruction! VMInstance %p halted\n", &vm);

    vmReleaseInstance(&vm);

    return 0;
}