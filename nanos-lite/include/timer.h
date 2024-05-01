#ifndef __TIMER_H_
#define __TIMER_H_


#define time_t uint64_t
#define suseconds_t uint64_t

typedef struct {
	time_t tv_sec;     /* seconds */
	suseconds_t tv_usec;    /* microseconds */
}timeval;

typedef struct {
	int tz_minuteswest;     /* minutes west of Greenwich */
	int tz_dsttime;         /* type of DST correction */
}timezone;

#endif
