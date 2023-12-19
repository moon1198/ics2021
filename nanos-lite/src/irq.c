#include <common.h>
#include "syscall.h"
#include <irq.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
		case EVENT_YIELD:
			c->GPRx = 0;
			break;
		case EVENT_SYSCALL:
			do_syscall(c);
			break;
    default: panic("Unhandled event ID = %d", c->GPR1);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
