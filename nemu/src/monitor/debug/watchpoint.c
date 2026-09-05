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
WP* new_wp() {
    if (free_ == NULL) assert(0); // 没空闲节点了直接报错
    WP *wp = free_;
    free_ = free_->next;
    wp->next = head;
    head = wp;
    return wp;
}
void free_wp(WP *wp) {
    if (wp == NULL || head == NULL) return;
    if (head == wp) {
        head = head->next;
    } else {
        WP *curr = head;
        while (curr->next != NULL && curr->next != wp) {
            curr = curr->next;
        }
        if (curr->next == wp) curr->next = wp->next;
    }
    wp->next = free_;
    free_ = wp;
}
