#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
#ifdef CONFIG_ETRACE
	printf("<etrace> Exception id : %d\n", NO);
#endif
	cpu.csr[2]._32 = epc;
	cpu.csr[0]._32 = NO;

  return 0;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
