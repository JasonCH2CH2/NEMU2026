#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/reg.h"

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
// 打印所有监视点
void info_wp() {
    if (head == NULL) {
        printf("No watchpoints.\n");
        return;
    }
    printf("Num\tType\t\tWhat\n");
    WP *p; for (p = head; p != NULL; p = p->next) {
        printf("%d\thw watchpoint\t%s\n", p->NO, p->expr);
    }
}

// 根据编号删除监视点
void delete_wp(int no) {
    WP *p; for (p = head; p != NULL; p = p->next) {
        if (p->NO == no) {
            free_wp(p);
            printf("Watchpoint %d deleted.\n", no);
            return;
        }
    }
    printf("Watchpoint %d not found.\n", no);
}

// 核心：检查是否有监视点发生变化
bool check_watchpoint() {
    WP *p = head;
    bool changed = false;
    while (p != NULL) {
        bool success;
        uint32_t new_val = expr(p->expr, &success);
        if (new_val != p->old_val) {
            // 这里必须按 PDF 要求打印
            printf("Hint watchpoint %d at address 0x%08x\n", p->NO, cpu.eip);
            printf("old value = %d (0x%08x)\n", p->old_val, p->old_val);
            printf("new value = %d (0x%08x)\n", new_val, new_val);
            p->old_val = new_val;
            changed = true;
        }
        p = p->next;
    }
    return changed;
}
