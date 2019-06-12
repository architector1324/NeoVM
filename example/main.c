#include "stdio.h"

#define VM_TARGET_ARCH64 // for correct vm_size_t
#include "neovm.h"



int main(){
    VMInstance vm = vmInstance(1, 8192);

    // client
    /*
        rip: assembly                   ; bytecode
        0  : ask {192.168.0.1 / 60000}  ; 0x00000020 0xc0a80001ea600000
    */

    // server
    /*
        rip: assembly                      ; bytecode
        0  : answer {192.168.0.1 / 60000}  ; 0x00000021 0xc0a80001ea600000
    */

    vm_uint8_t client[12] = {
        0x00, 0x00, 0x00, 0x20,
        0xc0, 0xa8, 0x00, 0x01,
        0xea, 0x60, 0x00, 0x00
    };

    vm_uint8_t server[12] = {
        0x00, 0x00, 0x00, 0x21,
        0xc0, 0xa8, 0x00, 0x01,
        0xea, 0x60, 0x00, 0x00
    };


    VMProgram prog = vmParseProgram(client, NULL);
    vmExecProgram(&prog, 0, &vm, NULL);


    if(vm.halt)
        printf("Wrong instruction! VMInstance %p halted\n", &vm);

    vmReleaseInstance(&vm);

    return 0;
}