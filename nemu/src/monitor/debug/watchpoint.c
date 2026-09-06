#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *expr_str) {
    if (free_ == NULL) {
        printf("No free watchpoint.\n");
        return NULL;
    }

    WP *wp = free_;
    free_ = free_->next;

    wp->next = head;
    head = wp;

    strncpy(wp->expr, expr_str, sizeof(wp->expr) - 1);
    wp->expr[sizeof(wp->expr) - 1] = '\0';

    bool success;
    wp->old_value = expr(wp->expr, &success);

    if (!success) {
        printf("Bad expression.\n");

        head = wp->next;
        wp->next = free_;
        free_ = wp;

        return NULL;
    }

    printf("Watchpoint %d: %s = 0x%08x\n",
           wp->NO, wp->expr, wp->old_value);

    return wp;
}


