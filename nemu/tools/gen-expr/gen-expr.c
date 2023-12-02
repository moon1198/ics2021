#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>


// this should be enough
static char buf[65536] = {};
static char *loc = buf;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
int count = 0;

static inline uint32_t choose(uint32_t num){
	return rand() % num;
}
static inline void gen_space(){
	int loop = choose(5);
	for (;loop > 0; -- loop){
		sprintf(loc, " ");
		loc += strlen(loc);
	}
}
static inline void gen_num(){
	switch (choose(1)){
		case 0: sprintf(loc, "%u", rand() % 100);
				loc += strlen(loc);
				break;
		case 1: sprintf(loc, "0x%x", rand() % 100);
				loc += strlen(loc);
				break;
	}
}
static inline void gen(char ch){
	sprintf(loc, "%c", ch);
	loc += strlen(loc);
}
static inline void gen_rand_op(){
	const char *op_array[] = {"+", "-", "*", "/" , "==", "!=", "&&"}; 
	int len = sizeof(op_array) / sizeof(op_array[0]);
	sprintf(loc, "%s", op_array[choose(len)]);
	loc += strlen(loc);
}
#define RE_COUNT 50
static void gen_rand_expr() {
	if (count < RE_COUNT) {
		count ++;
		switch (choose(3)){
			case 0: gen_num(); ; break;
		//	case 1: gen('('); gen_rand_expr(); gen(')'); break;
			default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
		}
	}
}
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  //printf("loop = %d\n", loop);
  for (i = 0; i < loop; i ++) {
	count = RE_COUNT;
	while (count >= RE_COUNT){
		count = 0;
		loc = buf;
    	gen_rand_expr(count);
		*loc = '\0';
	}

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;

    if (fscanf(fp, "%d", &result)) {
			pclose(fp);

    	printf("%u %s\n", result, buf);
		}
  }
  return 0;
}
