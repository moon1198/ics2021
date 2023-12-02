#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  //kbd->keydown = i8042_data_port_base[0] & KEYDOWN_MASK;
  //kbd->keycode = i8042_data_port_base[0] & 0xff;
  kbd->keydown =  (uint32_t) inl(KBD_ADDR) & KEYDOWN_MASK;
  kbd->keycode = (uint8_t) inb(KBD_ADDR);
}
