#include "cpu/exec/helper.h"

make_helper(ret_l) {
    cpu.eip = swaddr_read(cpu.esp, 4) - 1;
    cpu.esp += 4;
    return 1;
}

make_helper(ret_i_w) {
    uint16_t imm = instr_fetch(eip + 1, 2);
    cpu.eip = swaddr_read(cpu.esp, 4) - 3;
    cpu.esp += 4 + imm;
    return 3;
}
