#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>


static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

static void init_dispinfo() {
	int buf_size = 1024; 
	char * buf = (char *)malloc(buf_size * sizeof(char));
	int fd = open("/proc/dispinfo", 0, 0);
	int ret = read(fd, buf, buf_size);
	assert(ret < buf_size); // to be cautious...
	assert(close(fd) == 0);
	
	int i = 0;
	int width = 0, height = 0;
	
	assert(strncmp(buf + i, "WIDTH", 5) == 0);
	i += 5;
	for (; i < buf_size; ++i) {
		if (buf[i] == ':') { i++; break; }
		assert(buf[i] == ' ');
	}
	for (; i < buf_size; ++i) {
		if (buf[i] >= '0' && buf[i] <= '9') break;
		assert(buf[i] == ' ');
	}
	for (; i < buf_size; ++i) {
		if (buf[i] >= '0' && buf[i] <= '9') {
			width = width * 10 + buf[i] - '0';
		} else {
			break;
		}
	}
	assert(buf[i++] == '\n');
	
	assert(strncmp(buf + i, "HEIGHT", 6) == 0);
	i += 6;
	for (; i < buf_size; ++i) {
		if (buf[i] == ':') { i++; break; }
		assert(buf[i] == ' ');
	}
	for (; i < buf_size; ++i) {
		if (buf[i] >= '0' && buf[i] <= '9') break;
		assert(buf[i] == ' ');
	}
	for (; i < buf_size; ++i) {
		if (buf[i] >= '0' && buf[i] <= '9') {
			height = height * 10 + buf[i] - '0';
		} else {
			break;
		}
	}
	
	free(buf);
	
	screen_w = width;
	screen_h = height;
}

uint32_t NDL_GetTicks() {
	struct timeval now;
	assert(gettimeofday(&now, NULL) == 0);
	return now.tv_usec / 1000 + now.tv_sec * 1000;
}

int NDL_PollEvent(char *buf, int len) {

	int fd = open("/dev/events", 0, 0);
	//printf("open fd = %d\n", fd);
	int ret = read(fd, buf, len);
	//printf("ret = %d\n", ret);
	//printf("event_buf = %s\n", buf);
	
	close(fd);
	//assert(close(fd) == 0);
	return ret == 0 ? 0 : 1;
}

void NDL_OpenCanvas(int *w, int *h) {
//  if (getenv("NWM_APP")) {
//    int fbctl = 4;
//    fbdev = 5;
//		if (*w == 0 && *h == 0) {
//			*w = screen_w;
//			*h = screen_h;
//		}
//		printf("*w = %d, *h = %d\n", *w, *h);
//		canvas_w = *w; canvas_h = *h;
//    //screen_w = *w; screen_h = *h;
//    char buf[64];
//    int len = sprintf(buf, "%d %d", screen_w, screen_h);
//    // let NWM resize the window and create the frame buffer
//    write(fbctl, buf, len);
//    while (1) {
//      // 3 = evtdev
//      int nread = read(3, buf, sizeof(buf) - 1);
//      if (nread <= 0) continue;
//      buf[nread] = '\0';
//      if (strcmp(buf, "mmap ok") == 0) break;
//    }
//    close(fbctl);
//  }

	if (*w == 0 && *h == 0) {
		*w = screen_w;
		*h = screen_h;
	}
	//printf("*w = %d, *h = %d\n", *w, *h);
	canvas_w = *w; canvas_h = *h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	int fd = open("/dev/fb", 0, 0);
	//printf("open fd = %d\n", fd);
	//printf("x = %d, y = %d, width = %d, height = %d\n", x, y, w, h);
	int ret = 0;
	for (int i = 0; i < h; ++ i) {
		lseek(fd, ((y + i) * screen_w + x) * 4, SEEK_SET);
		//ret += write(fd, pixels + ret, w * 4);
		write(fd, pixels + w * i, w * 4);
	}
	//printf("\n");

	//close(fd);
	//assert(close(fd) == 0);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
	init_dispinfo();
  return 0;
}

void NDL_Quit() {
}
