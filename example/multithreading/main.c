#include "stdio.h"

#define VM_TARGET_ARCH64 // for correct vm_size_t
#include "neovm.h"



int main(){
    VMInstance vm = vmInstance(2, 8192, (vm_uint32_t){192, 168, 1, 52}, (vm_uint16_t){0xea, 0x60});

    // code0
    /*
        rip: assembly          ; bytecode
        0  : snd 5, r8_0       ; 0x00000004 0x05 0x00
        1  : lock              ; 0x0000001e
        2  : inc r8_0, r8_1    ; 0x0000001c 0x00 0x01
        3  : inc r8_1, r8_1    ; 0x0000001c 0x01 0x01
        4  : unlock            ; 0x0000001f
    */

   // code1
   /*
        rip: assembly        ; bytecode
        0  : snd 2, r8_1     ; 0x00000004 0x00 0x01
        1  : inc r8_1, r8_0  ; 0x0000001c 0x01 0x00

   */


    vm_uint8_t code0[26] = {
        0x00, 0x00, 0x00, 0x04,
        0x05, 0x00, 0x00, 0x00,
        0x00, 0x1e, 0x00, 0x00,
        0x00, 0x1c, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x1c,
        0x01, 0x01, 0x00, 0x00,
        0x00, 0x1f
    };

    vm_uint8_t code1[12] = {
        0x00, 0x00, 0x00, 0x04,
        0x02, 0x01, 0x00, 0x00,
        0x00, 0x1c, 0x01, 0x00
    };


    VMProgram prog0 = vmParseProgram(code0, 5, NULL);
    VMProgram prog1 = vmParseProgram(code1, 2, NULL);

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

    vmExecProgramMT(prog, 2, &vm, NULL);

    if(vm.halt)
        printf("Wrong instruction! VMInstance %p halted\n", &vm);

    printf("r8_0 = %d\nr8_1 = %d\n", VM_R8(0, vm), VM_R8(1, vm));

    vmReleaseInstance(&vm);

    return 0;
}