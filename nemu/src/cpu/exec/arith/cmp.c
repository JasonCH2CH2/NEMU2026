#include "cpu/exec/helper.h"

make_helper(cmp_r2rm_l) {
    int len = decode_rm_l(eip + 1);

    uint32_t result = op_dest->val - reg_l(op_src->reg);
    update_eflags_pf_zf_sf(result);

    cpu.eflags.CF = op_dest->val < reg_l(op_src->reg);
    cpu.eflags.OF = ((int32_t)op_dest->val < 0) != ((int32_t)reg_l(op_src->reg) < 0)
                  && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

    return len + 1;
}

make_helper(cmp_i2rm_l) {
    int len = decode_rm_l(eip + 1);

    uint32_t imm = instr_fetch(eip + 1 + len, 4);
    uint32_t result = op_dest->val - imm;
    update_eflags_pf_zf_sf(result);

    cpu.eflags.CF = op_dest->val < imm;
    cpu.eflags.OF = ((int32_t)op_dest->val < 0) != ((int32_t)imm < 0)
                  && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

	return len + 5;
}

make_helper(cmp_i2rm_b) {
	int len = decode_rm_b(eip + 1);

	uint32_t imm = instr_fetch(eip + 1 + len, 1);
	uint32_t result = op_dest->val - imm;
	update_eflags_pf_zf_sf(result);

	cpu.eflags.CF = op_dest->val < imm;
	cpu.eflags.OF = ((int32_t)op_dest->val < 0) != ((int32_t)imm < 0)
	              && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

	return len + 2;
}

make_helper(cmp_si2rm_l) {
	int len = decode_rm_l(eip + 1);

	int32_t imm = (int8_t)instr_fetch(eip + 1 + len, 1);
	uint32_t result = op_dest->val - (uint32_t)imm;
	update_eflags_pf_zf_sf(result);

	cpu.eflags.CF = op_dest->val < (uint32_t)imm;
	cpu.eflags.OF = ((int32_t)op_dest->val < 0) != (imm < 0)
	              && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

	return len + 2;
}

make_helper(cmp_i2a_l) {
	uint32_t imm = instr_fetch(eip + 1, 4);
	uint32_t result = cpu.eax - imm;
	update_eflags_pf_zf_sf(result);

	cpu.eflags.CF = cpu.eax < imm;
	cpu.eflags.OF = ((int32_t)cpu.eax < 0) != ((int32_t)imm < 0)
	              && ((int32_t)result < 0) != ((int32_t)cpu.eax < 0);

	return 5;
}
