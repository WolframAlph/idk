#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

/*
jump to 64 bit address by using following logic:
    pushq low32 bits (as 64bit value) of patched function
    movl high32 bits of patched function (fill in missing 4 bytes of previously pushed value), 0x4(%rsp)
    retq
as there is no way to do following on x86_64:
    1. jump to absolute 64bit address directly
    2. push 64bit immediate value to perform ret right after to emulate jmp
No state maintainence has to be done. No registers saving/restoring etc...
*/
static struct __attribute__((packed)) {
    uint8_t pushq;
    uint32_t low32;
    uint32_t movl;
    uint32_t high32;
    uint8_t retq;
} patch = {
    .pushq = 0x68,
    .movl = 0x042444C7,  // x86 little-endian
    .retq = 0xC3
};


void redirect(void *addr, void *replace) {
    long pagesize = sysconf(_SC_PAGE_SIZE);
    long boundary = (uintptr_t)addr & ~(pagesize-1);

    if (mprotect((void*)boundary, 1, PROT_READ | PROT_WRITE | PROT_EXEC) < 0) {
        perror("mprotect");
        exit(1);
    }

    patch.low32 = (uintptr_t)replace & 0xFFFFFFFF;
    patch.high32 = ((uintptr_t)replace >> 32) & 0xFFFFFFFF;

    memcpy(addr, &patch, sizeof(patch));

    if (mprotect((void*)boundary, 1, PROT_READ | PROT_EXEC) < 0) {
        perror("mprotect");
        exit(1);
    }
}
