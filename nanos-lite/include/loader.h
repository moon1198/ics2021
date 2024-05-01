#ifndef __LOADER_H_
#define __LOADER_H_


int fs_get_size(const char *filename);
int fs_get_base(const char *filename);
size_t ramdisk_read(void *buf, size_t offset, size_t len);


#endif

