#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

#define TEMP_BUF_SIZE 64
static char tmp_buf[TEMP_BUF_SIZE];
int SDL_PollEvent(SDL_Event *ev) {
	if (NDL_PollEvent(tmp_buf, TEMP_BUF_SIZE) == 1) {
		if (strncmp(tmp_buf, "kd", 2) == 0) {
			ev->key.type = SDL_KEYDOWN;
		}else if (strncmp(tmp_buf, "ku", 2) == 0) {
			ev->key.type = SDL_KEYUP;
		}
		
		for (unsigned i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i) {
			if (strlen(keyname[i]) == strlen(tmp_buf) - 4 && strncmp(tmp_buf + 3, keyname[i], strlen(tmp_buf) - 4) == 0) {
				ev->key.keysym.sym = i;
				//printf("temp_buf = %s\n", tmp_buf + 3);
				//printf("keyname = %s, i = %d\n", keyname[i], i);
				break;
			}
		}
		return 1;
		//ev->key.type = SDL_USEREVENT;
		//ev->key.keysym.sym = 0;
	}
	return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
	while (SDL_PollEvent(event) == 0){
		event->key.type = SDL_USEREVENT;
		event->key.keysym.sym = 0;
	}
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
