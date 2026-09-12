#include "cpu/exec/helper.h"

static bool test_cc(int cc) {
	switch(cc) {
	case 0x0: return cpu.eflags.OF;
	case 0x1: return !cpu.eflags.OF;
	case 0x2: return cpu.eflags.CF;
	case 0x3: return !cpu.eflags.CF;
	case 0x4: return cpu.eflags.ZF;
	case 0x5: return !cpu.eflags.ZF;
	case 0x6: return cpu.eflags.CF || cpu.eflags.ZF;
	case 0x7: return !cpu.eflags.CF && !cpu.eflags.ZF;
	case 0x8: return cpu.eflags.SF;
	case 0x9: return !cpu.eflags.SF;
	case 0xa: return cpu.eflags.PF;
	case 0xb: return !cpu.eflags.PF;
	case 0xc: return cpu.eflags.SF != cpu.eflags.OF;
	case 0xd: return cpu.eflags.SF == cpu.eflags.OF;
	case 0xe: return cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF);
	case 0xf: return !cpu.eflags.ZF && (cpu.eflags.SF == cpu.eflags.OF);
	default:  return false;
	}
}

make_helper(setcc_b) {
	int len = decode_rm_b(eip + 1);
	write_operand_b(op_src, test_cc(ops_decoded.opcode & 0xf));
	return len + 1;
}
