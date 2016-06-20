#include<stdio.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "common.h"
void print_test(void)
{
	printf("hello i'm common\n");
}

int sysUsecTime(void)
{
	struct timeval    tv;
	struct timezone tz;
	struct tm         *p;
	gettimeofday(&tv, &tz);
	printf("tv_sec:%ld\n",tv.tv_sec);
	return tv.tv_sec;

}