#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}


static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  rtl_addi(s, dest, src1, 0);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  TODO();
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  TODO();
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  word_t mask = 0;
  switch (width){
	case 1: mask = 0xff; break;
	case 2: mask = 0xffff; break;
	case 4: mask = 0xffffffff; break;
	default: MUXDEF(CONFIG_RT_CHECK, assert(0), mask = 0);
  }
  *dest = (sword_t) (*src1) & mask; 
  // dest <- signext(src1[(width * 8 - 1) .. 0])
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  word_t mask = 0;
  switch (width){
	case 1: mask = 0xff; break;
	case 2: mask = 0xffff; break;
	case 4: mask = 0xffffffff; break;
	default: MUXDEF(CONFIG_RT_CHECK, assert(0), mask = 0);
  }
  *dest = (word_t) (*src1) & mask; 
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  TODO();
}
#endif
