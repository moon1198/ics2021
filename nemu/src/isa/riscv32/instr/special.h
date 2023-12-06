#include "ftrace.h"
//const char *get_func_name(vaddr_t addr){
//	for (int i = 0; i < func_symbol_num; i++){
//		if (id_src1->imm >= func_symbol[i].start && id_src1->imm < func_symbol[i].end){
//			return func_symbol[i].name;
//		}
//	return NULL;
//}


def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
} def_EHelper(nemu_trap) { rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}

#ifdef CONFIG_FTRACE
static int space_num = 0;
static int call_ret_flag = 0;
#endif

//1:call, 0:ret
def_EHelper(jal) {
#ifdef CONFIG_FTRACE
#define NOT_DISPLAY_FUNC
  char *name = NULL;
  name = get_func_name(id_src1->imm);

  if (call_ret_flag){
	space_num += 1;
  }
  call_ret_flag = 1;
  if (name != NULL){
#ifdef NOT_DISPLAY_FUNC
		if (strcmp(name, "putch")){
#endif
  	printf(FMT_PADDR ": call %*s[%s@" FMT_PADDR "]\n", s->pc, space_num, "", name, id_src1->imm);
#ifdef NOT_DISPLAY_FUNC
		}
#endif
  }else{
  	printf(FMT_PADDR ": call %*s[???@" FMT_PADDR "]\n", s->pc, space_num, "", id_src1->imm);
  }
#endif
  rtl_li(s, ddest, s->snpc);
  rtl_j(s, id_src1->imm);
}

def_EHelper(jalr) {
  rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm);
#ifdef CONFIG_FTRACE
  if (s->isa.instr.i.rs1 == 1 && s->isa.instr.i.rd == 0){
    if (!call_ret_flag){
      space_num -= 1;
    }
    call_ret_flag = 0;
    char *name = NULL;
	name = get_func_name(s->pc);
	if (name != NULL){
#ifdef NOT_DISPLAY_FUNC
		if (strcmp(name, "putch")){
#endif
		printf(FMT_PADDR ": ret  %*s[%s]\n", s->pc, space_num, "", name);
#ifdef NOT_DISPLAY_FUNC
		}
#endif
	}else {
		printf(FMT_PADDR ": ret  %*s[???]\n", s->pc, space_num, "");
	}
  }else {
    if (call_ret_flag){
      space_num += 1;
    }
    call_ret_flag = 1;
    char *name = NULL;
	name = get_func_name(s->dnpc);
  	if (name != NULL){
#ifdef NOT_DISPLAY_FUNC
		if (strcmp(name, "putch")){
#endif
  		printf(FMT_PADDR ": call %*s[%s@" FMT_PADDR "]\n", s->pc, space_num, "", name, s->dnpc);
#ifdef NOT_DISPLAY_FUNC
		}
#endif
  	}else{
  		printf(FMT_PADDR ": call %*s[???@" FMT_PADDR "]\n", s->pc, space_num, "", s->dnpc);
  	}
  }
#endif
  rtl_li(s, ddest, s->snpc);
  //printf("%#08x: ret [%s@%#08x]\n");
}

def_EHelper(beq) {
  if (*(dsrc1) == *(dsrc2)){
	rtl_j(s, id_dest->imm);
  }
}

def_EHelper(bne) {
  if (*dsrc1 != *dsrc2){
	rtl_j(s, id_dest->imm);
  }
}

def_EHelper(blt) {
  if ((sword_t) (*dsrc1) < (sword_t) (*dsrc2)){
	rtl_j(s, id_dest->imm);
  }
}

def_EHelper(bge) {
  if ((sword_t) (*dsrc1) >= (sword_t) (*dsrc2)){
	rtl_j(s, id_dest->imm);
  }
}

def_EHelper(bltu) {
  if (*dsrc1 < *dsrc2){
	rtl_j(s, id_dest->imm);
  }
}

def_EHelper(bgeu) {
  if ((word_t) (*dsrc1) >= (word_t) (*dsrc2)){
	rtl_j(s, id_dest->imm);
  }
}

def_EHelper(ecall) {
	isa_raise_intr(11, s->pc);
	rtl_j(s, cpu.mtvec);
}

def_EHelper(csrrw) {
	//printf("%d", id_src2->imm);
	word_t tmp = 0;
	switch (id_src2->imm) {
		case 0b1101000010:
			if (ddest == (rtlreg_t*)&cpu.gpr[0]){
				cpu.csr[0]._32 = *dsrc1;
			}else {
				tmp = cpu.csr[0]._32;
				cpu.csr[0]._32 = *dsrc1;
				*ddest = tmp;
			}
			break;
		case 0b1100000000:
			if (ddest == (rtlreg_t*)&cpu.gpr[0]){
				cpu.csr[1]._32 = *dsrc1;
			}else {
				tmp = cpu.csr[1]._32;
				cpu.csr[1]._32 = *dsrc1;
				*ddest = tmp;
			}
			break;
		case 0b1101000001:
			if (ddest == (rtlreg_t*)&cpu.gpr[0]){
				cpu.csr[2]._32 = *dsrc1;
			}else {
				tmp = cpu.csr[2]._32;
				cpu.csr[2]._32 = *dsrc1;
				*ddest = tmp;
			}
			break;
		case 0b1100000101:
			if (ddest == (rtlreg_t*)&cpu.gpr[0]){
				cpu.mtvec = *dsrc1;
			}else {
				tmp = cpu.mtvec;
				cpu.mtvec = *dsrc1;
				*ddest = tmp;
			}
			break;
		default:
			panic("other csr was used, not implement");
	}
}

def_EHelper(csrrs) {
	word_t tmp = 0;
	switch (id_src2->imm) {
		case 0b1101000010:
			tmp = cpu.csr[0]._32;
			cpu.csr[0]._32 = *dsrc1 | tmp;
			*ddest = tmp;
			break;
		case 0b1100000000:
			tmp = cpu.csr[1]._32;
			cpu.csr[1]._32 = *dsrc1 | tmp;
			*ddest = tmp;
			break;
		case 0b1101000001:
			tmp = cpu.csr[2]._32;
			cpu.csr[2]._32 = *dsrc1 | tmp;
			*ddest = tmp;
			break;
		default:
			panic("other csr was used, not implement");
	}
}

def_EHelper(mret) {
	rtl_j(s, csr(2) + 4);
}
