#include "stdio.h"

#define VM_TARGET_ARCH64 // for correct vm_size_t
#include "neovm.h"



int main(){
    VMInstance vm = vmInstance(2, 8192, (vm_uint32_t){127, 0, 0, 1}, (vm_uint16_t){0xea, 0x60});

    // code0
   /*
        pc: assembly  ; bytecode
        0 : answer    ; 0x00000021
   */

    // code1
    /*
        pc: assembly                     ; bytecode
        0 : ask {127.0.0.1 / 60000 / 0}  ; 0x00000020 0x7f000001ea60
    */
 
    vm_uint8_t code0[4] = {
        0x00, 0x00, 0x00, 0x21
    };

    vm_uint8_t code1[10] = {
        0x00, 0x00, 0x00, 0x20,
        0x7f, 0x00, 0x00, 0x01,
        0xea, 0x60
    };


    VMProgram prog0 = vmParseProgram(code0, 1, NULL);
    VMProgram prog1 = vmParseProgram(code1, 1, NULL);

    VMExec prog[2] = {
        (VMExec){
            .thread = 0,
            .prog = &prog0
        },
        (VMExec){
            .thread = 1,
            .prog = &prog1
        }
    };

    vmExecProgram(prog, 2, &vm, NULL);

    if(vm.halt)
        printf("Wrong instruction! VMInstance %p halted\n", &vm);


    vmReleaseProgram(&prog0);
    vmReleaseProgram(&prog1);
    vmReleaseInstance(&vm);

    return 0;
}