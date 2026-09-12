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

make_helper(jcc_b) {
	int32_t offset = (int8_t)instr_fetch(eip + 1, 1);
	int cc = ops_decoded.opcode & 0xf;
	fprintf(stderr, "JCC_B eip=%08x cc=%x zf=%d take=%d\n",
			eip, cc, cpu.eflags.ZF, test_cc(cc));
	if(test_cc(cc)) {
		cpu.eip += offset;
	}
	return 2;
}

make_helper(jcc_l) {
	int32_t offset = (int32_t)instr_fetch(eip + 1, 4);
	if(test_cc(ops_decoded.opcode & 0xf)) {
		cpu.eip += offset;
	}
	return 5;
}
