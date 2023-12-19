#include <proc.h>
#include <loader.h>
#include <elf.h>


#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
# define EXPECT_TYPE EM_RISCV
#else
# error Unsupported ISA
#endif


#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define RANGE(st, ed)       (Area) { .start = (void *)(st), .end = (void *)(ed) }
static uintptr_t loader(PCB *pcb, const char *filename) {
	int elf_size = sizeof (Elf_Ehdr);
	uint8_t elf_buf[elf_size];

	int file_base = fs_get_base(filename);
	int file_size = fs_get_size(filename);
	printf("Load the %s.\n", filename);
	Log("start = %d, size = %d", file_base, file_size);

	ramdisk_read(elf_buf, file_base, elf_size);
	Elf_Ehdr *elf = (Elf_Ehdr *) elf_buf;
	//check the magic number
	assert(*(uint32_t *)elf->e_ident == 0x464c457f);
	//check the isa between machine and elf
	assert(elf->e_machine == EXPECT_TYPE);


	int phdr_size = sizeof (Elf_Phdr);
	int phdr_num = elf->e_phnum;
	uint8_t phdr_buf[phdr_size * phdr_num];
	ramdisk_read(phdr_buf, elf->e_phoff + file_base, phdr_size * phdr_num);
	Elf_Phdr *phdr = (Elf_Phdr *) phdr_buf;
	//printf("phdr_num = %d\n", phdr_num);
	for (int i = 0; i < phdr_num; ++ i) {
		//printf("phdr->p_type = %d\n", phdr->p_type);
		if (phdr->p_type == 1) {
			ramdisk_read((uint8_t *)phdr->p_vaddr, phdr->p_offset + file_base, phdr->p_filesz);
			//printf("write to %p from %p with %x Byte\n", (uint8_t *)phdr->p_vaddr, phdr->p_offset, phdr->p_filesz);
			memset((void *)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
			//printf("Set %d zeros begin %p.\n", phdr->p_memsz - phdr->p_filesz, phdr->p_vaddr + phdr->p_filesz);
		}
		phdr = phdr + 1;
	}
  return elf->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

