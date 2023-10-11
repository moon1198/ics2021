#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char type[32];
  char e[128];
  int val;
  int times;

  /* TODO: Add more members if necessary */

} WP;

static int NO = 1;
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
	WP *temp;
	if(free_ != NULL){
		temp = free_;
		free_ = free_->next;
	}else{
		printf("All WP has been used!\n");
		return NULL;
	}
	temp->next = head;
	head = temp;
	return head;
}
word_t expr(char *e, bool *success);
bool *success = false;
void add_wp(char *e, char *type){
	if (new_wp() != NULL){
		head->NO = NO++;
		strcpy(head->e, e);
		head->val = expr(e, success);
		strcpy(head->type, type);
	}else{
		return;
	}

}
void free_wp(int num){
	static WP *temp;
	static WP *pre;
	temp = head;
	while (temp != NULL) {
		if (temp->NO == num){
			break;
		}else{
			pre = temp;
			temp = temp->next;
		}
	}
	if (temp == NULL) {
		printf("not found the %dth WP!\n", num);
	}else{
		if(pre != NULL){
			pre->next = temp->next;
			temp->next = free_;
			free_ = temp;	
		}else{
			head = temp->next;
			temp->next = free_;
			free_ = temp;
		}

	}
}
void display_wp (){
	WP *temp;
	temp = head;
	printf("Num   Type            What\n");
	while(temp != NULL){
		printf("%-6d%-16s%-16s\n", temp->NO, temp->type, temp->e);
		if (temp->times != 0){
			printf("      breakpoint already hit %d time\n", temp->times);
		}
		temp = temp->next;
	}
}
bool diff_wp () {
	WP *temp;
	int temp_val;
	bool flag = false;
	temp = head;
	while(temp != NULL){
		if (strcmp(temp->type, "watchpoint") == 0){
			temp_val = expr(temp->e, success);
			if (temp->val != temp_val){
				flag = true;
				printf("Watchpoint %d: %s\n\n", temp->NO, temp->e);
				printf("Old value = %d\n", temp->val);
				printf("New value = %d\n", temp_val);
				printf("\n");
				temp->val = temp_val;
			}
		}else{
			if(cpu.pc == temp->val){
				printf("Breakpoint %d, 0x%08x\n", temp->NO, temp->val);
				flag = true;
			}
		}
		temp = temp->next;
	}
	return flag;
}
