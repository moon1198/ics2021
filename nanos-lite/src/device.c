#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
		int i = 0;
		while (buf != NULL && i < len) {
			putch(*(char *)buf ++);
			i ++;
		}
		return i;
}

#define TEMP_BUF_SIZE 32
static char temp_buf[TEMP_BUF_SIZE];

size_t events_read(void *buf, size_t offset, size_t len) {
	memset(temp_buf, 0, TEMP_BUF_SIZE);

	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
	if (ev.keycode == AM_KEY_NONE) return 0;
	const char *name = keyname[ev.keycode];
	int ret = ev.keydown ? sprintf(temp_buf, "kd %s\n", name) : sprintf(temp_buf, "ku %s\n", name);
	if (ret > len){
		strncpy(buf, temp_buf, len - 1);
		((char *)buf)[len - 1] = '\0';
		ret = len;
	}else {
		strncpy(buf, temp_buf, ret);
		((char *)buf)[ret] = '\0';
	}
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	memset(temp_buf, 0, TEMP_BUF_SIZE);

	AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
	int width = ev.width;
	int height = ev.height;
	//printf("width : %d\nheight : %d\n", width, height);
	int ret = sprintf(temp_buf, "WIDTH: %d\nHEIGHT: %d\n", width, height);
	if (ret > len){
		strncpy(buf, temp_buf, len - 1);
		((char *)buf)[len - 1] = '\0';
		ret = len;
	}else {
		strncpy(buf, temp_buf, ret);
		((char *)buf)[ret] = '\0';
	}
  return ret;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	//printf("Here is fb_write\n");
	AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
	int width = ev.width;

	int y = offset / 4 / width;
	int x = offset / 4 - y * width;

	//printf("x = %d, y = %d, width = %d, offset = %d\n", x, y, len / 4, offset);
	io_write(AM_GPU_FBDRAW, x, y, (void *)buf, len / 4, 1, true);

	return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
