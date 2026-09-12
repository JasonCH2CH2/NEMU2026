#include "cpu/exec/helper.h"

make_helper(call_si_l) {
        cpu.esp -= 4;
        swaddr_write(cpu.esp, 4, eip + 5);
        cpu.eip += (int32_t)instr_fetch(eip + 1, 4);
        return 5;
}

make_helper(call_rm_l) {
        int len = decode_rm_l(eip + 1);
        cpu.esp -= 4;
        swaddr_write(cpu.esp, 4, eip + len + 1);
        cpu.eip = op_src->val - (len + 1);
        return len + 1;
}
