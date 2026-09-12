#include "cpu/exec/helper.h"

make_helper(test_r2rm_l) {
    int len = decode_r2rm_l(eip + 1);

    uint32_t result = op_dest->val & op_src->val;
    update_eflags_pf_zf_sf(result);

	return len + 1;
}

make_helper(test_r2rm_w) {
    int len = decode_r2rm_w(eip + 1);

    uint32_t result = op_dest->val & op_src->val;
    update_eflags_pf_zf_sf(result);

    return len + 1;
}

make_helper_v(test_r2rm)

make_helper(test_r2rm_b) {
    int len = decode_r2rm_b(eip + 1);

    uint32_t result = op_dest->val & op_src->val;
    update_eflags_pf_zf_sf(result);

    return len + 1;
}

make_helper(test_i2rm_l) {
    int len = decode_i2rm_l(eip + 1);

    uint32_t result = op_dest->val & op_src->val;
    update_eflags_pf_zf_sf(result);

	return len + 1;
}

make_helper(test_i2rm_w) {
    int len = decode_i2rm_w(eip + 1);

    uint32_t result = op_dest->val & op_src->val;
    update_eflags_pf_zf_sf(result);

    return len + 1;
}

make_helper_v(test_i2rm)

make_helper(test_i2rm_b) {
    int len = decode_i2rm_b(eip + 1);

    uint32_t result = op_dest->val & op_src->val;
    update_eflags_pf_zf_sf(result);

    return len + 1;
}
