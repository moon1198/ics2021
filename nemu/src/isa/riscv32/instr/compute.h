def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(slli) {
  rtl_slli(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(slti) {
	//printf("rs1 = %d\n", *dsrc1); 
	//printf("simm = %d\n", id_src2->simm); 
  if (*(sword_t *)dsrc1 < id_src2->simm){
		*ddest = 1;
  }else{
		*ddest = 0;
  }
}

def_EHelper(sltiu) {
  if (*dsrc1 < id_src2->imm){
		*ddest = 1;
  }else{
		*ddest = 0;
  }
}

def_EHelper(xori) {
  word_t temp;
  rtl_sext(s, &temp, &id_src2->imm, 4);
  rtl_xori(s, ddest, dsrc1, temp);
}

def_EHelper(srai) {
  rtl_srai(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli) {
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(ori) {
  rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi) {
  word_t temp;
  rtl_sext(s, &temp, &id_src2->imm, 4);
  //sword_t temp = id_src2->simm | ((id_src2->simm & 0x800) ? 0xfffff000 : 0x0);
  rtl_andi(s, ddest, dsrc1, temp);
}

def_EHelper(auipc) {
  rtl_addi(s, ddest, &s->pc, id_src1->simm);
}

def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll) {
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  *ddest = (sword_t)(*dsrc1) < (sword_t)(*dsrc2) ? 1 : 0;
}

def_EHelper(sltu) {
  //printf("src1 = %u, src2 = %u\n", *dsrc1, *dsrc2);
  *ddest = *dsrc1 < *dsrc2 ? 1 : 0;
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl) {
  rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul) {
  rtl_muls_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) {
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhu) {
  rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div) {
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu) {
  rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem) {
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu) {
  rtl_divu_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

