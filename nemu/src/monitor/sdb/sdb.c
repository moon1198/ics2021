#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_test(char *args){
	printf("Hello, world\n");
	return 0;
}


static int cmd_si(char *args){
	uint64_t num;
	if (args == NULL){
		num = 1;
	}else{
		num = atoi(args);
	}
//	cpu_exec_steps(num);
	cpu_exec(num);
	return 0;
}

void display_wp();
void wp_display (){
	display_wp();	
}
static struct {
	const char *name;
	const char *description;
	void (*handler) ();
} info_table [] = {
	{"r", "Display the info of all registers", isa_reg_display},
	{"w", "Display the info of all watchpoint", wp_display}
};
#define NR_INFO ARRLEN(info_table)
static void info_descr_display(){
	for (int i=0; i < NR_INFO; ++i){
		printf("%s - %s\n", info_table[i].name, info_table[i].description);
	}
}
static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	int i;
	if (args == NULL){
		printf("List of info subcommands\n");
		info_descr_display();
		return 0;
	}else{
		for (i = 0; i < NR_INFO; ++i){
			if(strcmp(arg, info_table[i].name) == 0){
				info_table[i].handler();
				break;
			}
		}
	}
	if (i == NR_INFO) {
		printf ("%s isn't a good input\n", args); 
		info_descr_display();
	}
	return 0;
}

static int cmd_pc(char *args){
	printf("pc = 0x%08x\n", cpu.pc);
	return 0;
}


static int cmd_x(char *args){
	char *args_temp[2];
	args_temp[0] = malloc(sizeof(char) * 32);
	args_temp[1] = malloc(sizeof(char) * 32);
	char *str_temp;
	int index = 0;
	str_temp = strtok(args, " ");
	while (str_temp != NULL){
		if (index > 1){
			printf("too many arguments, try again\n");
			free(args_temp[0]);
			free(args_temp[1]);
			return 0;
		}
		strcpy(args_temp[index],str_temp);
		++index;
		str_temp = strtok(NULL, " ");
	};
	uint32_t Byte_num;
	vaddr_t addr;
	switch (index){
		case 0:{
				   printf("Too few arguments\n");
					free(args_temp[0]);
					free(args_temp[1]);
				   return -1;
			   }
		case 1:{
				   addr = (uint32_t)strtoul(args_temp[0], NULL, 16);
				   Byte_num = 1;
				   break;
			   }
		case 2:{
				   addr = (uint32_t)strtoul(args_temp[1], NULL, 16);
				   Byte_num = atoi(args_temp[0]);
				   break;
			   }
		default:{
					printf("Error appears\n");
					free(args_temp[0]);
					free(args_temp[1]);
					return -1;
				}
	}
	uint32_t ret;
	for (int i = 0; i < Byte_num; ++i){
//	vaddr_read中的４，表示一个指令四个字节
		ret = vaddr_read(addr, 4);
		printf("0x%08x  0x%08x\n", addr, ret);
		addr += 4;
	}
	
	free(args_temp[0]);
	free(args_temp[1]);
	
	return 0;
}

static int cmd_comp(char *args){
	
	bool *success;
	success = malloc(sizeof(bool));
	*success = true;
	printf("please input a str.\n");
	static char e[128];
	while((fgets(e, sizeof (e), stdin) != NULL) && (e[0] != 'q')){
		e[strlen(e)-1] = '\0';
		//词法分析
		expr(e, success);
		printf("please input a str.\n");

	}
	free(success);
	return 0;
}
void add_wp(char*, char*);
static int cmd_wp (char *expr){
	add_wp(expr, "watchpoint");
	return 0;
}
static int cmd_b(char *expr){
	add_wp(expr, "break");
	return 0;
}
void free_wp (int);
static int cmd_d (char * NO){
	int num = strtol(NO, NULL, 10);
	free_wp(num);
	return 0;
}


static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "test", "this is a test", cmd_test},
  { "si", "Execute n instructions", cmd_si },
  { "info", "The state of regs", cmd_info},
  { "pc", "The state of pc", cmd_pc},
  { "x", "Scanf the memory", cmd_x},
  { "comp", "computer express", cmd_comp},
  { "w", "add watchpoint", cmd_wp},
  { "d", "delete watchpoint", cmd_d},
  { "b", "add break", cmd_b}


  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
