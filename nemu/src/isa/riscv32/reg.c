#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

static int regs_long = sizeof(regs) / sizeof(regs[0]);

void isa_reg_display() {
	for (int i = 0;i < regs_long;i++){
		printf("%3s = 0x%08x ;", regs[i], (word_t) cpu.gpr[i]._32);
		if (i%4 == 3)printf("\n");
		if (i%8 == 7)printf("\n");
	}
}

word_t isa_reg_str2val(const char *s, bool *success) {
	word_t result = 0;
	for (int i = 0; i < regs_long; ++ i){
		if (strcmp(s, regs[i]) == 0){
			result = (word_t) cpu.gpr[i]._32;
			//printf("%3s = 0x%08x ;\n", regs[i],result);
			return result;
		}
	}
	printf("The reg is invaild!\n");
	success = false; 
	return result;
}
