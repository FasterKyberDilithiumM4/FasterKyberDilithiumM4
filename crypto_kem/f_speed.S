/*
This file contains wrappers for assembly code that is not callable via a function from the regular sources.
*/
.syntax unified
.cpu cortex-m4
.thumb

#include "implvariant.h"

#ifdef opt

#include "matacc.i"

.global dummy_wrapper
.type dummy_wrapper, %function
.align 2
dummy_wrapper:
push {r0-r11, r14}
pop {r0-r11, pc}
.size dummy_wrapper, . - dummy_wrapper

// cache
.global doublebasemul_asm_cache_16_32_wrapper
.type doublebasemul_asm_cache_16_32_wrapper, %function
.align 2
doublebasemul_asm_cache_16_32_wrapper:
push {r0-r11, r14}
vmov s11, r0
doublebasemul_asm_cache_16_32 r0, r0, r0, r0, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0, r14
pop {r0-r11, pc}
.size doublebasemul_asm_cache_16_32_wrapper, . - doublebasemul_asm_cache_16_32_wrapper

.global doublebasemul_asm_acc_cache_32_32_wrapper
.type doublebasemul_asm_acc_cache_32_32_wrapper, %function
.align 2
doublebasemul_asm_acc_cache_32_32_wrapper:
push {r0-r11, r14}
vmov s11, r0
doublebasemul_asm_acc_cache_32_32 r0, r0, r0, r0, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0, r14
pop {r0-r11, pc}
.size doublebasemul_asm_acc_cache_32_32_wrapper, . - doublebasemul_asm_acc_cache_32_32_wrapper

.global doublebasemul_asm_acc_cache_32_16_wrapper
.type doublebasemul_asm_acc_cache_32_16_wrapper, %function
.align 2
doublebasemul_asm_acc_cache_32_16_wrapper:
push {r0-r11, r14}
vmov s11, r0
vmov s12, r0
doublebasemul_asm_acc_cache_32_16 r0, r0, r0, r0, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0, r14
pop {r0-r11, pc}
.size doublebasemul_asm_acc_cache_32_16_wrapper, . - doublebasemul_asm_acc_cache_32_16_wrapper

// opt
.global doublebasemul_asm_opt_16_32_wrapper
.type doublebasemul_asm_opt_16_32_wrapper, %function
.align 2
doublebasemul_asm_opt_16_32_wrapper:
push {r0-r11, r14}
vmov s11, r0
doublebasemul_asm_opt_16_32 r0, r0, r0, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0, r14
pop {r0-r11, pc}
.size doublebasemul_asm_opt_16_32_wrapper, . - doublebasemul_asm_opt_16_32_wrapper

.global doublebasemul_asm_acc_opt_32_32_wrapper
.type doublebasemul_asm_acc_opt_32_32_wrapper, %function
.align 2
doublebasemul_asm_acc_opt_32_32_wrapper:
push {r0-r11, r14}
vmov s11, r0
doublebasemul_asm_acc_opt_32_32 r0, r0, r0, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0, r14
pop {r0-r11, pc}
.size doublebasemul_asm_acc_opt_32_32_wrapper, . - doublebasemul_asm_acc_opt_32_32_wrapper

.global doublebasemul_asm_acc_opt_32_16_wrapper
.type doublebasemul_asm_acc_opt_32_16_wrapper, %function
.align 2
doublebasemul_asm_acc_opt_32_16_wrapper:
push {r0-r11, r14}
vmov s11, r0
doublebasemul_asm_acc_opt_32_16 r0, r0, r0, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r0, r14
pop {r0-r11, pc}
.size doublebasemul_asm_acc_opt_32_16_wrapper, . - doublebasemul_asm_acc_opt_32_16_wrapper

#elif defined(optstack)
#include "matacc.i"

.global doublebasemul_asm_wrapper
.type doublebasemul_asm_wrapper, %function
.align 2
doublebasemul_asm_wrapper:
push {r0-r11, r14}
doublebasemul_asm r0, r0, r0, r0, r4, r5, r6, r7, r8, r9, r10, r11, r12, r12, r14
pop {r0-r11, pc}
.size doublebasemul_asm_wrapper, . - doublebasemul_asm_wrapper

.global doublebasemul_asm_acc_wrapper
.type doublebasemul_asm_acc_wrapper, %function
.align 2
doublebasemul_asm_acc_wrapper:
push {r0-r11, r14}
doublebasemul_asm_acc r0, r0, r0, r0, r4, r5, r6, r7, r8, r9, r10, r11, r12, r12, r14
pop {r0-r11, pc}
.size doublebasemul_asm_acc_wrapper, . - doublebasemul_asm_acc_wrapper

#endif