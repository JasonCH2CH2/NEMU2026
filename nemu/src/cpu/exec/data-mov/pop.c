#include "cpu/exec/helper.h"

make_helper(pop_rm_l) {
        int len = decode_rm_l(eip + 1);
        uint32_t data = swaddr_read(cpu.esp, 4);
        cpu.esp += 4;
        write_operand_l(op_src, data);
        return len + 1;
}

make_helper(pop_r_l) {
        uint8_t reg = instr_fetch(eip, 1) & 7;
        uint32_t data = swaddr_read(cpu.esp, 4);
        cpu.esp += 4;
        reg_l(reg) = data;
        return 1;
}