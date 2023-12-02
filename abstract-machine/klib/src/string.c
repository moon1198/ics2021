#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t count = 0;
	if (s == NULL){
		return 0;
	}else{
		char ch = s[0];
		while (ch != '\0'){
			count ++;
			ch = s[count];
		}
	}
	return count;
}

char *strcpy(char *dst, const char *src) {
  char *dy_ptr = dst;
  if (dst == NULL) {
		return NULL;
  }else if (src == NULL){
		//*dy_ptr = '\0';	
		return dst;
  }
  while (*src != '\0'){
		*dy_ptr ++ = *src ++;
  }
  *dy_ptr = *src;
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *dy_ptr = dst;
	size_t flag = n;
  if (dst == NULL) {
		return NULL;
  }else if(src == NULL){
	//	//*dy_ptr = '\0';	
		return dst;
  }
  while (*src != '\0' && flag != 0){
		*dy_ptr ++ = *src ++;
		flag --;
  }
	if (flag != 0) {
		*dy_ptr ++ = '\0';
		flag --;
	}
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *v_dst = dst;
  while(*v_dst != '\0') {v_dst ++;};
  const char *v_src = src;
  while(*v_src != '\0'){
	*v_dst++ = *v_src++;
  }
  *v_dst = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  int flag = 0;
  if ((s1 == NULL && s2 != NULL)||(s2 == NULL&&s1 != NULL)){
		return 1;
  }else if (s1 == NULL && s2 == NULL){
		return 0;	
  }else{
		while(*s1 != '\0' && *s2 != '\0'){
		  flag = (int) *s1 - (int) *s2;
		  if (flag != 0){
		    return flag;
		  }
		  s1 ++; s2 ++;
		}
		if(*s1 == '\0' && *s2 == '\0') {flag = 0;} else {flag = 1;}
  }
  return flag;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  int flag = 0;
	size_t tmp = n;
  if ((s1 == NULL && s2 != NULL)||(s2 == NULL&&s1 != NULL)){
		return 1;
  }else if (s1 == NULL && s2 == NULL){
		return 0;	
  }else{
		while(*s1 != '\0' && *s2 != '\0'){
		  flag = (int) *s1 - (int) *s2;
		  if (flag != 0){
		    return flag;
		  }
		  s1 ++; s2 ++; tmp --;
			if(tmp == 0) {return flag;}
		}
		if(*s1 == '\0' && *s2 == '\0') {flag = 0;} else {flag = 1;}
  }
  return flag;
}

void *memset(void *s, int c, size_t n) {
  char *dy_ptr = s;
	if (s == NULL){
		return NULL;
  }else{
		while(n-- > 0){
		  *dy_ptr ++ = c;	
		}
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  if (dst == NULL || src == NULL || n <= 0){
		return NULL;	  
  }
  char *v_out = (char *)dst;
  char *v_in = (char *)src;
	if (v_out > v_in + n || v_out + n < v_in){
		while(n --){
		  *v_out ++ = *v_in ++;
  	}
	}else {
		//char *buf = malloc(len);
		//memcpy(buf, v_in, len);
		while(n --){
		  *v_out ++ = *v_in ++;
  	}
	}
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  if (out == NULL || in == NULL || n <= 0){
		return NULL;	  
  }
  char *v_out = (char *)out;
  char *v_in = (char *)in;
	//内存边界判断错误(bug)
	if (v_out >= v_in + n || v_out + n  <= v_in){
		while(n --){
		  *v_out ++ = *v_in ++;
  	}
	}else {
		return NULL; 
	}
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  int flag = 0;
  const char *st1 = s1; 
  const char *st2 = s2; 
  if ((s1 == NULL && s2 != NULL)||(s2 == NULL && s1 != NULL)){
		return 1;
  }else if (s1 == NULL && s2 == NULL){
		return 0;	
  }else{
		while(n--){
		  flag = (int) *st1 - (int) *st2;
		  if (flag != 0){
		    return flag;
		  }
		  st1 ++; st2 ++;
		}
  }
  return flag;
}

#endif
