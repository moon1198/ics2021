#include <isa.h>
#include <memory/paddr.h>

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img [] = {

	 0x00000297,  // auipc t0,0
   0x00028823,  // sb  zero,16(t0)
   0x0102c503,  // lbu a0,16(t0)
   0x00100073,  // ebreak (used as nemu_trap)
   0xdeadbeef,  // some data


  //0x00c000ef,

  //0x800002b7,  // lui t0,0x80000
  //0x0002a023,  // sw  zero,0(t0)
  //0x0002a503,  // lw  a0,0(t0)

  //0x0000006b,  // nemu_trap
};

static void restart() {
  /* Set the initial program counter. */
  cpu.pc = RESET_VECTOR;

  /* The zero register is always 0. */
  cpu.gpr[0]._32 = 0;
}

void init_isa() {
  /* Load built-in image. */
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));

  /* Initialize this virtual computer system. */
  restart();
}
