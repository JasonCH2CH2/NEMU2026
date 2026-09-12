#include "cpu/exec/helper.h"

make_helper(cmp_r2rm_l) {
    int len = decode_r2rm_l(eip + 1);

    uint32_t result = op_dest->val - op_src->val;
    update_eflags_pf_zf_sf(result);

    cpu.eflags.CF = op_dest->val < op_src->val;
    cpu.eflags.OF = ((int32_t)op_dest->val < 0) != ((int32_t)op_src->val < 0)
                  && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

    return len + 1;
}

make_helper(cmp_i2rm_l) {
    int len = decode_i2rm_l(eip + 1);

    uint32_t imm = op_src->val;
    uint32_t result = op_dest->val - imm;
    update_eflags_pf_zf_sf(result);

    cpu.eflags.CF = op_dest->val < imm;
    cpu.eflags.OF = ((int32_t)op_dest->val < 0) != ((int32_t)imm < 0)
                  && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

	return len + 1;
}

make_helper(cmp_i2rm_b) {
	int len = decode_i2rm_b(eip + 1);

	uint32_t imm = op_src->val;
	uint32_t result = op_dest->val - imm;
	update_eflags_pf_zf_sf(result);

	cpu.eflags.CF = op_dest->val < imm;
	cpu.eflags.OF = ((int32_t)op_dest->val < 0) != ((int32_t)imm < 0)
	              && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

	return len + 1;
}

make_helper(cmp_si2rm_l) {
	int len = decode_si2rm_l(eip + 1);

	int32_t imm = (int32_t)op_src->val;
	uint32_t result = op_dest->val - (uint32_t)imm;
	update_eflags_pf_zf_sf(result);

	cpu.eflags.CF = op_dest->val < (uint32_t)imm;
	cpu.eflags.OF = ((int32_t)op_dest->val < 0) != (imm < 0)
	              && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

	return len + 1;
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

make_helper(cmp_rm2r_l) {
	int len = decode_rm2r_l(eip + 1);

	uint32_t result = op_dest->val - op_src->val;
	update_eflags_pf_zf_sf(result);

	cpu.eflags.CF = op_dest->val < op_src->val;
	cpu.eflags.OF = ((int32_t)op_dest->val < 0) != ((int32_t)op_src->val < 0)
	              && ((int32_t)result < 0) != ((int32_t)op_dest->val < 0);

	return len + 1;
}

make_helper(cmp_i2a_b) {
	uint32_t imm = instr_fetch(eip + 1, 1);
	uint32_t result = reg_b(R_AL) - imm;
	update_eflags_pf_zf_sf(result);

	cpu.eflags.CF = reg_b(R_AL) < imm;
	cpu.eflags.OF = ((int8_t)reg_b(R_AL) < 0) != ((int8_t)imm < 0)
	              && ((int8_t)result < 0) != ((int8_t)reg_b(R_AL) < 0);

	return 2;
}
