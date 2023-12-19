#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <timer.h>
#include <proc.h>


void naive_uload(PCB *pcb, const char *filename);

//#define CONFIG_STRACE


static int sys_yield(){
	yield();
	return 0;
}

static int sys_open(const char *pathname, int flags, int mode){
#ifdef CONFIG_STRACE
	printf("Open the file %s\n", pathname);
#endif
	return fs_open(pathname, flags, mode);
}

static int sys_read(int fd, void *buf, size_t len){
	return fs_read(fd, buf, len);
}

static int sys_write(int fd, const void *buf, size_t len){
	return fs_write(fd, buf, len);
}

static int sys_lseek(int fd, size_t offset, size_t whence){
	return fs_lseek(fd, offset, whence);
}

static int sys_close(int fd){
	return fs_close(fd);
}

static int sys_gettimeofday(timeval *tv, timezone *tz) {
	uint64_t us = io_read(AM_TIMER_UPTIME).us;
	tv->tv_sec = us / 1000000;
	tv->tv_usec = us % 1000000; 
	//printf("sec = %d, usec = %d\n", tv->tv_sec, tv->tv_usec);
	return 0;
}

static void sys_execve(const char *pathname, char *const argv[], char *const envp[]){
	naive_uload(NULL, pathname);
}

static void sys_exit(int status){
	sys_execve("/bin/menu", NULL, NULL);
	halt(status);
}

//static int sys_write(int fd, const void *buf, size_t count){
//	int num = 0;
//	const char *ch = buf;
//	if (fd == 1 || fd == 2){
//		while (ch != buf + count){
//			putch(*ch);
//			ch ++;
//			num ++;
//		}
//	}else {
//	}
//	return num;
//}

static int sys_brk(void *addr){
	return 0;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; //调用号
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
	uintptr_t ret = 0;
#ifdef CONFIG_STRACE
	printf("SYSCALL ID = %d\n", a[0]);
#endif
  switch (a[0]) {
		case SYS_exit:
			sys_exit(a[1]); break;
  	case SYS_yield:
			ret = sys_yield(); break;
  	case SYS_open:
			ret = sys_open((void *)a[1], a[2], a[3]); break;
  	case SYS_read:
			ret = sys_read(a[1], (void *)a[2], a[3]); break;
  	case SYS_write:
			ret = sys_write(a[1], (void *)a[2], a[3]); break;
  	case SYS_kill:
  	case SYS_getpid:
  	case SYS_close:
			ret = sys_close(a[1]); break;
  	case SYS_lseek:
			ret = sys_lseek(a[1], a[2], a[3]); break;
  	case SYS_brk:
			ret = sys_brk((void *)a[1]); break;
  	case SYS_fstat:
  	case SYS_time:
  	case SYS_signal:
  	case SYS_execve:
			sys_execve((char*)a[1], (char **)a[2], (char **)a[3]); break;
  	case SYS_fork:
  	case SYS_link:
  	case SYS_unlink:
  	case SYS_wait:
  	case SYS_times:
  	case SYS_gettimeofday:
			ret = sys_gettimeofday((timeval *)a[1], (timezone *)a[2]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
	c->GPRx = ret;
}
