#include <isa.h>
//内含different_check_reg函数，可直接调用
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 32; ++i) {
	if (ref_r->gpr[i]._32 != cpu.gpr[i]._32) {
	  Log("At %s -> REF: " FMT_WORD ", DUT: " FMT_WORD, reg_name(i, 0), ref_r->gpr[i]._32, cpu.gpr[i]._32);
	  return false;
	 }
  }
  if (ref_r->pc != cpu.pc) {
	Log("At pc -> REF: " FMT_WORD ", DUT: " FMT_WORD, ref_r->pc, cpu.pc);
	return false;
  }
  return true;
}

void isa_difftest_attach() {
}
