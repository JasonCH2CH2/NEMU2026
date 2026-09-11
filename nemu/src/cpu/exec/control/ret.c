#include "cpu/exec/helper.h"

make_helper(ret_l) {
    cpu.eip = swaddr_read(cpu.esp, 4) - 1;
    cpu.esp += 4;
    return 1;
}