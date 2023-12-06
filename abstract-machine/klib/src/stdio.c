#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define BUF_SIZE 1024
static char buf[BUF_SIZE];

void my_itoa (char *buf, int num, int hex){
	char num_arr[] = "0123456789abcdef";
	char temp[128];
	int len = 0;
	int i = 0;
	char sign = '+';
	int rem = 0;
	//反向写入temp
	temp[i++] = '\0';
	if (num < 0){
		sign = '-';
		buf[len++] = sign;
		num = -num;
	}
	do{
		rem = num % hex;
		temp[i++] = num_arr[rem];
		num = num / hex;
	}while(num != 0);
	i--;
	while (i >= 0){
		buf[len++] = temp[i--];	
	}
}
void my_utoa (char *buf, uint32_t num, int hex){
	char num_arr[] = "0123456789abcdef";
	char temp[128];
	int len = 0;
	int i = 0;
	int rem = 0;
	//反向写入temp
	temp[i++] = '\0';
	do{
		rem = num % hex;
		temp[i++] = num_arr[rem];
		num = num / hex;
	}while(num != 0);
	i--;
	while (i >= 0){
		buf[len++] = temp[i--];	
	}
}
int my_atoi(const char *str){
	int num = 0;
	if (str == NULL) return -1;
	while (*str == ' ') {str ++;}
	while (*str >= '0' && *str <= '9'){
		num = num * 10 + *str - '0';
		str ++;
	}
	return num;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	
  char buf[128];
  int len;
	int space_count = 0;
  char *v_out = out;
  *v_out = '\0';
  while (*fmt != '\0'){
		if (*fmt == '%'){
			if (*(fmt + 1) == '0'){
				int num = my_atoi(fmt + 2);
				while (*(fmt + 1) >= '0' && *(fmt + 1)  <= '9'){
					fmt ++;
				}
				switch (*(fmt + 1)){
					case 'd':
						my_itoa(buf, va_arg(ap, int), 10);
						space_count = num - strlen(buf);
						if (space_count > 0){
							memset(v_out, '0', space_count);
							v_out += space_count;
							*v_out = '\0';
						} 
						strcat(v_out, buf);
						break;
					case 'x':
						my_utoa(buf, va_arg(ap, int), 16);
						space_count = num - strlen(buf);
						if (space_count > 0){
							memset(v_out, '0', space_count);
							v_out += space_count;
							*v_out = '\0';
						} 
						strcat(v_out, buf);
						break;
					case 'b':
						my_itoa(buf, va_arg(ap, int), 2);
						space_count = num - strlen(buf);
						if (space_count > 0){
							memset(v_out, '0', space_count);
							v_out += space_count;
							*v_out = '\0';
						} 
						strcat(v_out, buf);
						break;
					default:
						panic("Not implemented");
						break;
				}
			}else if(*(fmt + 1) > '0' && *(fmt + 1) <= '9'){
				int num = my_atoi(fmt + 1);
				while (*(fmt + 1) >= '0' && *(fmt + 1)  <= '9'){
					fmt ++;
				}
				switch (*(fmt + 1)){
					case 'd':
						my_itoa(buf, va_arg(ap, int), 10);
						space_count = num - strlen(buf);
						if (space_count > 0){
							memset(v_out, '0', space_count);
							v_out += space_count;
							*v_out = '\0';
						} 
						strcat(v_out, buf);
						break;
					case 'x':
						my_utoa(buf, va_arg(ap, int), 16);
						space_count = num - strlen(buf);
						if (space_count > 0){
							memset(v_out, '0', space_count);
							v_out += space_count;
							*v_out = '\0';
						} 
						strcat(v_out, buf);
						break;
					case 'b':
						my_itoa(buf, va_arg(ap, int), 2);
						space_count = num - strlen(buf);
						if (space_count > 0){
							memset(v_out, '0', space_count);
							v_out += space_count;
							*v_out = '\0';
						} 
						strcat(v_out, buf);
						break;
					default:
						for (int i = 0; i < 10; i ++){
							putch(*(fmt + 1));
						}
						putch('\n');
						panic("Not implemented");
						break;
				}
			}else{
				switch (*(fmt + 1)){
					case 'd':
						my_itoa(buf, va_arg(ap, int), 10);
						strcat(v_out, buf);
						break;
					case 's':
						strcat(v_out, va_arg(ap, char*));
						break;
					case 'c':
						v_out[0] = va_arg(ap, int);
						v_out[1] = '\0';
						break;
					case 'x':
					case 'p':
						my_utoa(buf, va_arg(ap, int), 16);
						strcat(v_out, "0x");
						strcat(v_out, buf);
						break;
					default:
						putch('a');
						for (int i = 0; i < 10; i ++){
							putch(*(fmt + i));
						}
						putch('\n');
						panic("Not implemented");
						break;
				}
			}
			fmt += 2;
			while(*v_out != '\0'){v_out ++;}
		}
		else{
			*v_out++ = *fmt++;
			*v_out = '\0';
		}
  }
  *v_out = '\0';
  len = v_out - out;
  return len;
}

int printf(const char *fmt, ...) {
  va_list args;
  int len = 0;

  va_start(args, fmt);
  len = vsprintf(buf, fmt, args);
  va_end(args);
	putstr(buf);

  return len;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int len = 0;
  va_start(args, fmt);
  len = vsprintf(out, fmt, args);
  va_end(args);
  return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
