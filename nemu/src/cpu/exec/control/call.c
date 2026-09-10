#include "cpu/exec/helper.h"

make_helper(call_si_l) {
        cpu.esp -= 4;
        swaddr_write(cpu.esp, 4, eip + 5);
        cpu.eip += (int32_t)instr_fetch(eip + 1, 4);
        return 5;
}