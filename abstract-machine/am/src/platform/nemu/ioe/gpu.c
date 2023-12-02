#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
	int i;
	int h = inw(VGACTL_ADDR);
	int w = inw(VGACTL_ADDR + 2);
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	//AM_GPU_FBDRAW();

	for (i = 0; i < w * h; i ++) fb[i] = 0xffffffff;
	outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	int h = inw(VGACTL_ADDR);
	int w = inw(VGACTL_ADDR + 2);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w * h * sizeof(uint32_t)
  };
}

#include "klib.h"
///int index = 0;
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	//static uint32_t count = 0;
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	
	int H = inw(VGACTL_ADDR);
	int W = inw(VGACTL_ADDR + 2);
	int x = ctl->x;
	int y = ctl->y;
	int w = ctl->w;
	int h = ctl->h;
	uint32_t *pixel_base = (uint32_t *)ctl->pixels;
	//int cp_bytes = sizeof(uint32_t) * (w < W - x ? w : W - x);
	w = (w < W - x ? w : W - x);
	h = (h < H - y ? h : H - y);
	//int row = (w < W - x ? w : W - x);


	int j = 0;
	for (j = 0; j < h; j ++){
		//memcpy(&fb[(y + j) * W + x], base, cp_bytes);
		int index_base1 = (y + j) * W + x;
		for (int k = 0; k < w; k ++){
			fb[index_base1 + k] = *pixel_base ++;
		}	
	}

	//int j = 0;
	//for (j = 0; j < h - 1; j += 2){
	//	//memcpy(&fb[(y + j) * W + x], base, cp_bytes);
	//	int index_base1 = (y + j) * W + x;
	//	uint32_t *pixel_base1 = pixel_base;
	//	for (int k = 0; k < w; k ++){
	//		fb[index_base1 + k] = *pixel_base1 ++;
	//	}	
	//	
	//	int index_base2 = (y + j + 1) * W + x;
	//	uint32_t *pixel_base2 = pixel_base + w;
	//	for (int k = 0; k < w; k ++){
	//		fb[index_base2 + k] = *pixel_base2 ++;
	//	}	
	//	pixel_base += 2 * w;
	//}  
	//if (h % 2){
	//	int index_base3 = (y + h - 1) * W + x;
	//	uint32_t *pixel_base3 = (uint32_t *)ctl->pixels + (h - 1) * w;
	//	for (int k = 0; k < w; k ++){
	//		fb[index_base3 + k] = *pixel_base3 ++;
	//	}	
	//}
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = (bool) inl(SYNC_ADDR);
  //status->ready = true;
}
