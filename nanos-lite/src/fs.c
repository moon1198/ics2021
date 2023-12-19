#include <fs.h>
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, DEV_EVENTS, PROC_DISPINFO, FD_FB};

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);


size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [DEV_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
  [PROC_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};

typedef struct {
	int fd;
  size_t open_offset;
	bool flag;
} OFinfo;


static OFinfo open_file_table[LENGTH(file_table)];

int get_open_index(int fd) {
	int nums = LENGTH(open_file_table);
	for (int i = 0; i < nums; i++) {
		if (open_file_table[i].fd == fd) {
			return i;
		}
	}
	return -1;
}

int fs_open(const char *pathname, int flags, int mode) {
	int f_nums = LENGTH(file_table);
	int of_nums = LENGTH(open_file_table);
	
	
	//printf("%s\n", pathname);
	for (int i = 0; i < f_nums; ++ i) {
		if (strcmp(pathname, file_table[i].name) == 0){
			if (i <= 2) {
				Log("ignore open %s", pathname);
				return i;
			}
			for (int j = 0; j < of_nums; ++ j) {
				if (!open_file_table[j].flag){
					open_file_table[j].flag = true;
					open_file_table[j].fd = i;
					open_file_table[j].open_offset = 0;
					return i;
				}
			}
		}
	}
	assert(0);
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
	//if (fd <= 2) {
	//	Log("ignore read %s", file_table[fd].name);
	//	return 0;
	//}
	int open_index = get_open_index(fd);
	if (file_table[fd].read != NULL) {
		return file_table[fd].read(buf, open_file_table[open_index].open_offset, len);
	}
	//Log("read %s", file_table[fd].name);
	size_t open_offset = open_file_table[open_index].open_offset;
	size_t offset = open_offset + file_table[fd].disk_offset;

	size_t read_size = offset + len < file_table[fd].disk_offset + file_table[fd].size ? len : file_table[fd].disk_offset + file_table[fd].size - offset;
	ramdisk_read(buf, offset, read_size);
	open_file_table[open_index].open_offset += read_size;
	return read_size;
}

size_t fs_write(int fd, const void *buf, size_t len) {
	//if (fd <= 2) {
	//	int i = 0;
	//	while (buf != NULL && i < len) {
	//		putch(*(char *)buf ++);
	//		i ++;
	//	}
	//	return i;
	//}
	int open_index = get_open_index(fd);
	if (file_table[fd].write != NULL && fd < FD_FB) {
		return file_table[fd].write(buf, open_file_table[open_index].open_offset, len);
	}
	size_t open_offset = open_file_table[open_index].open_offset;
	size_t offset = open_offset + file_table[fd].disk_offset;

	size_t write_size = offset + len < file_table[fd].disk_offset + file_table[fd].size ? len : file_table[fd].disk_offset + file_table[fd].size - offset;
	if(fd == FD_FB){
		file_table[fd].write(buf, offset, write_size);
	}else {
		ramdisk_write(buf, offset, write_size);
	}
	open_file_table[open_index].open_offset += write_size;
	return write_size;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
	if (fd <= 2) {
		Log("ignore lseek %s", file_table[fd].name);
		return 0;
	}
	int open_index = get_open_index(fd);
	size_t open_offset = open_file_table[open_index].open_offset;
	if (open_index == -1) {
		Log("file %s not open before lseek", file_table[fd].name);
		return -1;
	}
	size_t new_offset = -1;
	size_t size = file_table[fd].size;
	switch (whence) {
		case SEEK_SET:
			new_offset = offset;
			break;
		case SEEK_CUR:
			new_offset = offset + open_offset;
			break;
		case SEEK_END:
			new_offset = size - offset;
			break;
		default:
			Log("Unknown whence %d", whence);
	}
	if (new_offset > size || new_offset < 0) {
		Log("offset %ld has overed the range of %s.", new_offset, file_table[fd].name);
		return -1;
	}
	open_file_table[open_index].open_offset = new_offset;
	return new_offset;
}

int fs_close(int fd) {
	if (fd <= 2) {
		Log("ignore close %s", file_table[fd].name);
		return 0;
	}
	int of_nums = LENGTH(open_file_table);
	for (int j = 0; j < of_nums; ++ j) {
		if (open_file_table[j].fd == fd){
			open_file_table[j].flag = false;
			return 0;
		}
	}
	return -1;
}


int fs_get_base(const char *filename) {
	//printf("%s\n", filename);
	int f_nums = LENGTH(file_table);
	
	for (int i = 0; i < f_nums; ++ i) {
		//printf("%s\n", file_table[i].name);
		if (strcmp(filename, file_table[i].name) == 0){
			return file_table[i].disk_offset;
		}
	}

	Log("Can't find the %s.\n", filename);
	assert(0);
	return -1;
}

int fs_get_size(const char *filename) {
	int f_nums = LENGTH(file_table);
	
	for (int i = 0; i < f_nums; ++ i) {
		if (strcmp(filename, file_table[i].name) == 0){
			return file_table[i].size;
		}
	}
	assert(0);
	return -1;
}

#define KB 1024
#define MB 1024 * KB
void init_fs() {
  // TODO: initialize the size of /dev/fb
	AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
	int width = ev.width;
	int height = ev.height;
	file_table[FD_FB].size = width * height * 4;
}
