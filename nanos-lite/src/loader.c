#include <proc.h>
#include <loader.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define RANGE(st, ed)       (Area) { .start = (void *)(st), .end = (void *)(ed) }
static uintptr_t loader(PCB *pcb, const char *filename) {
	int size = get_ramdisk_size();
	uint8_t buf[size];
	ramdisk_read(buf, 0, size);
	Elf32_Ehdr *elf = (Elf32_Ehdr *) buf;

	assert(*(uint32_t *)elf->e_ident == 0x464c457f);
	//ramdisk_read(elf->e_entry, 0, RAMDISK_SIZE);
	//char *buf = 0x83000000;
	//ramdisk_read(buf, 0,get_ramdisk_size());
  return elf->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

