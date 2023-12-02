#include <memory/host.h>
#include <memory/paddr.h>
#include <device/mmio.h>
#include <isa.h>

#if   defined(CONFIG_TARGET_AM)
static uint8_t *pmem = NULL;
#else
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
#endif

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

void init_mem() {
#if   defined(CONFIG_TARGET_AM)
  pmem = malloc(CONFIG_MSIZE);
  assert(pmem);
#endif
#ifdef CONFIG_MEM_RANDOM
  uint32_t *p = (uint32_t *)pmem;
  int i;
  for (i = 0; i < (int) (CONFIG_MSIZE / sizeof(p[0])); i ++) {
    p[i] = rand();
  }
#endif
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]",
      (paddr_t)CONFIG_MBASE, (paddr_t)CONFIG_MBASE + CONFIG_MSIZE);
}

typedef struct {
  int len;
  char buf[16][128];
  word_t index;
}ringbuf;
ringbuf mringbuf = {.len = 16, .index = 0};
char mt_buf[128];
char *p = mt_buf;

void display_mtrace(ringbuf *mtrace_buf){
  for(int i = 0; i < mtrace_buf->len; i ++){
		if (i == (mtrace_buf->index - 1) % 16){
		  printf("-->%s\n", mtrace_buf->buf[i]);
		}else{
		  printf("   %s\n", mtrace_buf->buf[i]);
		}
  }
}

word_t paddr_read(paddr_t addr, int len) {
	
	#ifdef CONFIG_MTRACE
  snprintf(mt_buf, 128, "GET val from addr 0x%08x",  addr);
  strcpy(mringbuf.buf[mringbuf.index ++ % 16], mt_buf);
	#endif
  //if (likely(in_pmem(addr))) return pmem_read(addr, len);
  //modify
  if (likely(in_pmem(addr))) {
	  return pmem_read(addr, len);
  }else{
  	MUXDEF(CONFIG_DEVICE, return mmio_read(addr, len),
			  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR ") at pc = " FMT_WORD,
  	    addr, CONFIG_MBASE, CONFIG_MBASE + CONFIG_MSIZE, cpu.pc));
  }
  //
  //MUXDEF(CONFIG_DEVICE, return mmio_read(addr, len),
	//	  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR ") at pc = " FMT_WORD,
  //    addr, CONFIG_MBASE, CONFIG_MBASE + CONFIG_MSIZE, cpu.pc));
	//	  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
	#ifdef CONFIG_MTRACE
  snprintf(mt_buf, 128, "WRITE 0x%08x to addr 0x%08x", data, addr);
  strcpy(mringbuf.buf[mringbuf.index ++ % 16], mt_buf);
	#endif
  //if (likely(in_pmem(addr))) { pmem_write(addr, len, data); return; }
  //modify
  if (likely(in_pmem(addr))) { 
		pmem_write(addr, len, data); return; 
  }else{
  	MUXDEF(CONFIG_DEVICE, mmio_write(addr, len, data),
  	  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR ") at pc = " FMT_WORD,
  	    addr, CONFIG_MBASE, CONFIG_MBASE + CONFIG_MSIZE, cpu.pc));
		return;
  }
  //
  //MUXDEF(CONFIG_DEVICE, mmio_write(addr, len, data),
  //  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR ") at pc = " FMT_WORD,
  //    addr, CONFIG_MBASE, CONFIG_MBASE + CONFIG_MSIZE, cpu.pc));
}
