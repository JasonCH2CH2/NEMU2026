#include "cpu/exec/helper.h"

make_helper(push_r_l) {
	uint8_t reg = instr_fetch(eip, 1) & 0x7;
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, reg_l(reg));
	return 1;
}

make_helper(push_i_l) {
	uint32_t imm = instr_fetch(eip + 1, 4);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, imm);
	return 5;
}

make_helper(push_i_b) {
	int32_t imm = (int8_t)instr_fetch(eip + 1, 1);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, (uint32_t)imm);
	return 2;
}

make_helper(push_rm_l) {
	int len = decode_rm_l(eip + 1);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, op_src->val);
	return len + 1;
}
