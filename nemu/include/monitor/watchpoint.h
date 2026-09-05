#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
    	char expr[64];      // 存表达式
    	uint32_t old_val;   // 存旧值

} WP;

#endif
