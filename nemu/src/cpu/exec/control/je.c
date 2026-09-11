#include "cpu/exec/helper.h"

make_helper(je_si_l) {
    int32_t offset = (int32_t)instr_fetch(eip + 1, 4);

    if (cpu.eflags.ZF) {
        cpu.eip += offset;
    }

    return 5;
}