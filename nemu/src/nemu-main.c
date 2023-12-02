#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

#ifdef CONFIG_EXPR_TEST
  word_t expr(char *e, bool *success);
  bool *success = false;
  FILE *fp;
  char *file_path = getenv("NEMU_HOME");
  strcat(file_path, "/input");
  fp = fopen(file_path, "r");
  assert(fp != NULL);
  char e[1024] = {};
  uint32_t result = 0;
  while(fscanf(fp,"%d %s", &result, e) == 2){
	  if (result != expr(e, success)){
			printf("%s %d %d\n", e, result, expr(e, success));
	  }else{
	  }
  }
  fclose(fp);
  printf("all has tested\n");
#endif
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
