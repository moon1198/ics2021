#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>


#include <cpu/decode.h>
#include <isa-def.h>

void cpu_exec_steps(uint64_t n);
void isa_reg_display();
void vaddr_write(vaddr_t addr, int len, word_t data);
word_t vaddr_read(vaddr_t addr, int len);


word_t expr(char *e, bool *success);

#endif
