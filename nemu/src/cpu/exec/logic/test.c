#include "cpu/exec/helper.h"

make_helper(test_r2rm_l) {
    int len = decode_rm_l(eip + 1);

    uint32_t result = reg_l(op_src->reg) & op_dest->val;
    update_eflags_pf_zf_sf(result);

    return len + 1;
}

make_helper(test_i2rm_l) {
    int len = decode_rm_l(eip + 1);

    uint32_t imm = instr_fetch(eip + 1 + len, 4);
    uint32_t result = imm & op_dest->val;
    update_eflags_pf_zf_sf(result);

    return len + 5;
}