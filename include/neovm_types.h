#pragma once

/////////////////////////////////////////////////////
//                     TYPES
/////////////////////////////////////////////////////

#ifndef NULL
#define NULL (void*)0
#endif

#ifdef VM_TARGET_ARCH16
typedef unsigned short int vm_size_t;
#else
    #ifdef VM_TARGET_ARCH32
    typedef unsigned int vm_size_t;
    #else
        #ifdef VM_TARGET_ARCH64
        typedef unsigned long long vm_size_t;
        #endif
    #endif
#endif

#ifndef __cat
#define __cat(X, Y) X##Y
#endif

#ifndef _cat
#define _cat(X, Y) __cat(X, Y)
#endif

#define _vm_ui(bitdepth) _cat(vm_uint, _cat(bitdepth, _t))
#define _vm_ui_size(bitdepth) (bitdepth / 8)
#define _vm_i(bitdepth) _cat(vm_int, _cat(bitdepth, _t))
#define _vm_i_size(bitdepth) (bitdepth / 8)




typedef enum _VM_BOOL {false = 0, true} vm_bool;

// unsigned
typedef unsigned char vm_uint8_t;

#define _vm_decl_ui(bitdepth)\
typedef struct _vm_ui(bitdepth){\
    vm_uint8_t bytes[_vm_ui_size(bitdepth)];\
} _vm_ui(bitdepth);

_vm_decl_ui(16)
_vm_decl_ui(32)
_vm_decl_ui(64)
_vm_decl_ui(128)
_vm_decl_ui(256)

// signed
typedef char vm_int8_t;

#define _vm_decl_i(bitdepth)\
typedef struct _vm_i(bitdepth){\
    vm_int8_t bytes[_vm_i_size(bitdepth)];\
} _vm_i(bitdepth);

_vm_decl_i(16)
_vm_decl_i(32)
_vm_decl_i(64)
_vm_decl_i(128)
_vm_decl_i(256)


/////////////////////////////////////////////////////
//                   CONVERSIONS
/////////////////////////////////////////////////////

#define VM_UINT8_T_PTR(some8) ((vm_uint8_t*)&(some8))
#define VM_UINT16_T_PTR(some16) ((vm_uint16_t*)&(some16))
#define VM_UINT32_T_PTR(some32) ((vm_uint32_t*)&(some32))
#define VM_UINT64_T_PTR(some64) ((vm_uint64_t*)&(some64))
#define VM_UINT128_T_PTR(some128) ((vm_uint128_t*)&(some128))
#define VM_UINT256_T_PTR(some256) ((vm_uint256_t*)&(some256))

#define VM_INT8_T_PTR(some8) ((vm_int8_t*)&(some8))
#define VM_INT16_T_PTR(some16) ((vm_int16_t*)&(some16))
#define VM_INT32_T_PTR(some32) ((vm_int32_t*)&(some32))
#define VM_INT64_T_PTR(some64) ((vm_int64_t*)&(some64))
#define VM_INT128_T_PTR(some128) ((vm_int128_t*)&(some128))
#define VM_INT256_T_PTR(some256) ((vm_int256_t*)&(some256))

#define VM_UINT8_T(some8) VM_UINT8_T_PTR(some8)[0]
#define VM_UINT16_T(some16) VM_UINT16_T_PTR(some16)[0]
#define VM_UINT32_T(some32) VM_UINT32_T_PTR(some32)[0]
#define VM_UINT64_T(some64) VM_UINT64_T_PTR(some64)[0]
#define VM_UINT128_T(some128) VM_UINT128_T_PTR(some128)[0]
#define VM_UINT256_T(some256) VM_UINT256_T_PTR(some256)[0]

#define VM_INT8_T(some8) VM_INT8_T_PTR(some8)[0]
#define VM_INT16_T(some16) VM_INT16_T_PTR(some16)[0]
#define VM_INT32_T(some32) VM_INT32_T_PTR(some32)[0]
#define VM_INT64_T(some64) VM_INT64_T_PTR(some64)[0]
#define VM_INT128_T(some128) VM_INT128_T_PTR(some128)[0]
#define VM_INT256_T(some256) VM_INT256_T_PTR(some256)[0]


vm_bool vm_is_big_endian(){
    unsigned short magic = 0xffaa;
    return VM_INT8_T(magic) == 0xff;
}

#define _vm_ui_to_size_t(bitdepth)\
vm_size_t _cat(vm_ui, _cat(bitdepth, _to_size_t))(_vm_ui(bitdepth) a){\
    vm_size_t result;\
    if(vm_is_big_endian()){\
        vm_size_t size = sizeof(result);\
        for(vm_size_t i = 0; i < size; i++)\
            VM_UINT8_T_PTR(result)[size - i - 1] = i < _vm_ui_size(bitdepth) ? a.bytes[_vm_ui_size(bitdepth) - i - 1] : 0x00;\
    }else{\
        vm_size_t size = sizeof(result);\
        for(vm_size_t i = 0; i < size; i++)\
            VM_UINT8_T_PTR(result)[i] = i < _vm_ui_size(bitdepth) ? a.bytes[_vm_ui_size(bitdepth) - i - 1] : 0x00;\
    }\
    return result;\
}

_vm_ui_to_size_t(16)
_vm_ui_to_size_t(32)
_vm_ui_to_size_t(64)
_vm_ui_to_size_t(128)
_vm_ui_to_size_t(256)


/////////////////////////////////////////////////////
//                   OPERATIONS
/////////////////////////////////////////////////////

// logical
#define _vm_equal_ui(bitdepth)\
vm_bool _cat(vm_equal_ui, bitdepth)(_vm_ui(bitdepth) a, _vm_ui(bitdepth) b){\
    vm_bool result = true;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++) result = result && (a.bytes[i] == b.bytes[i]);\
    return result;\
}
#define _vm_not_equal_ui(bitdepth)\
vm_bool _cat(vm_not_equal_ui, bitdepth)(_vm_ui(bitdepth) a, _vm_ui(bitdepth) b){\
    vm_bool result = false;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++) result = result || (a.bytes[i] != b.bytes[i]);\
    return result;\
}

// bitwise
#define _vm_and_ui(bitdepth)\
_vm_ui(bitdepth) _cat(vm_and_ui, bitdepth)(_vm_ui(bitdepth) a, _vm_ui(bitdepth) b){\
    _vm_ui(bitdepth) result;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++) result.bytes[i] = (a.bytes[i] & b.bytes[i]);\
    return result;\
}
#define _vm_or_ui(bitdepth)\
_vm_ui(bitdepth) _cat(vm_or_ui, bitdepth)(_vm_ui(bitdepth) a, _vm_ui(bitdepth) b){\
    _vm_ui(bitdepth) result;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++) result.bytes[i] = (a.bytes[i] | b.bytes[i]);\
    return result;\
}
#define _vm_xor_ui(bitdepth)\
_vm_ui(bitdepth) _cat(vm_xor_ui, bitdepth)(_vm_ui(bitdepth) a, _vm_ui(bitdepth) b){\
    _vm_ui(bitdepth) result;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++) result.bytes[i] = (a.bytes[i] ^ b.bytes[i]);\
    return result;\
}
#define _vm_inverse_ui(bitdepth)\
_vm_ui(bitdepth) _cat(vm_inverse_ui, bitdepth)(_vm_ui(bitdepth) a){\
    _vm_ui(bitdepth) result;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++) result.bytes[i] = ~a.bytes[i];\
    return result;\
}

// arithmetic
#define _vm_inc_ui(bitdepth)\
_vm_ui(bitdepth) _cat(vm_inc_ui, bitdepth)(_vm_ui(bitdepth) a){\
    _vm_ui(bitdepth) result;\
    vm_uint8_t of = 1;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++){\
        if(i > 0 && of != 0) of = a.bytes[_vm_ui_size(bitdepth) - i] == 0xff ? 1 : 0;\
        result.bytes[_vm_ui_size(bitdepth) - i - 1] = a.bytes[_vm_ui_size(bitdepth) - i - 1] + of;\
    }\
    return result;\
}
#define _vm_dec_ui(bitdepth)\
_vm_ui(bitdepth) _cat(vm_dec_ui, bitdepth)(_vm_ui(bitdepth) a){\
    _vm_ui(bitdepth) result;\
    vm_uint8_t of = 1;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++){\
        if(i > 0 && of != 0) of = a.bytes[_vm_ui_size(bitdepth) - i] == 0 ? 1 : 0;\
        result.bytes[_vm_ui_size(bitdepth) - i - 1] = a.bytes[_vm_ui_size(bitdepth) - i - 1] - of;\
    }\
    return result;\
}

#define _vm_add_ui_of(a, b, c) ((a) >= 0xff - (c)) || ((a) >= 0xff - (b)) || ((b) >= 0xff - (c)) ||((a) >= 0xff - (b) - (c) ? 1 : 0)

#define _vm_add_ui(bitdepth)\
typedef struct _cat(vm_add_ui, _cat(bitdepth, _result)){\
    _vm_ui(bitdepth) result;\
    vm_uint8_t rem;\
} _cat(vm_add_ui, _cat(bitdepth, _result));\
_cat(vm_add_ui, _cat(bitdepth, _result)) _cat(vm_add_ui, bitdepth)(_vm_ui(bitdepth) a, _vm_ui(bitdepth) b){\
    _vm_ui(bitdepth) result;\
    vm_uint8_t of_curr = 0;\
    vm_uint8_t of_prev = 0;\
    for(vm_size_t i = 0; i < _vm_ui_size(bitdepth); i++){\
        of_curr = _vm_add_ui_of(a.bytes[_vm_ui_size(bitdepth) - i - 1], b.bytes[_vm_ui_size(bitdepth) - i - 1], of_prev);\
        result.bytes[_vm_ui_size(bitdepth) - i - 1] = a.bytes[_vm_ui_size(bitdepth) - i - 1] + b.bytes[_vm_ui_size(bitdepth) - i - 1] + of_prev;\
        of_prev = of_curr;\
    }\
    return (_cat(vm_add_ui, _cat(bitdepth, _result))){result, of_curr};\
}



_vm_equal_ui(16)
_vm_equal_ui(32)
_vm_equal_ui(64)
_vm_equal_ui(128)
_vm_equal_ui(256)

_vm_not_equal_ui(16)
_vm_not_equal_ui(32)
_vm_not_equal_ui(64)
_vm_not_equal_ui(128)
_vm_not_equal_ui(256)

_vm_and_ui(16)
_vm_and_ui(32)
_vm_and_ui(64)
_vm_and_ui(128)
_vm_and_ui(256)

_vm_or_ui(16)
_vm_or_ui(32)
_vm_or_ui(64)
_vm_or_ui(128)
_vm_or_ui(256)

_vm_xor_ui(16)
_vm_xor_ui(32)
_vm_xor_ui(64)
_vm_xor_ui(128)
_vm_xor_ui(256)

_vm_inverse_ui(16)
_vm_inverse_ui(32)
_vm_inverse_ui(64)
_vm_inverse_ui(128)
_vm_inverse_ui(256)

_vm_inc_ui(16)
_vm_inc_ui(32)
_vm_inc_ui(64)
_vm_inc_ui(128)
_vm_inc_ui(256)

_vm_dec_ui(16)
_vm_dec_ui(32)
_vm_dec_ui(64)
_vm_dec_ui(128)
_vm_dec_ui(256)

_vm_add_ui(16)
_vm_add_ui(32)
_vm_add_ui(64)
_vm_add_ui(128)
_vm_add_ui(256)