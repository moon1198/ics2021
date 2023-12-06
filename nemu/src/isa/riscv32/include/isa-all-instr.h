#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) f(add) \
	f(sll) f(slt) f(sltu) f(xor) f(srl) f(or) f(and) f(mul) f(mulh) f(mulhu) \
	f(div) f(divu) f(rem) f(remu) f(sub) f(sra) f(lui) f(auipc) f(addi) f(slli) \
	f(slti) f(sltiu) f(xori) f(ori) f(srli) f(srai) f(andi) f(jalr) f(lb) f(lh) \
	f(lw) f(lbu) f(lhu) f(sw) f(sb) f(sh) f(jal) f(inv) f(nemu_trap) f(ecall) \
	f(csrrw) f(csrrs) f(mret)

def_all_EXEC_ID();
