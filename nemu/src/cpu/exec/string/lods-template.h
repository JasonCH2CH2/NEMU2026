#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_, SUFFIX)) {
#if DATA_BYTE == 1
	reg_b(R_AL) = MEM_R(cpu.esi);
#else
	REG(R_EAX) = MEM_R(cpu.esi);
#endif
	cpu.esi += (cpu.eflags.DF ? -DATA_BYTE : DATA_BYTE);

	print_asm("lods" str(SUFFIX) " %%ds:(%%esi),%%eax");
	return 1;
}

#include "cpu/exec/template-end.h"
