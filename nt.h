#ifndef NANOTHREADS_H
#define NANOTHREADS_H


#include <stdint.h>


#define NT_MAX_SLEEP	1000
typedef uint32_t nt_time_t;
typedef uint16_t nt_line_t;


typedef struct nt_s
{
	nt_time_t (*fn)(nt_line_t*);
	nt_time_t sleep;
	nt_line_t cont;
} nt_t;


#define ntDef(name)		nt_time_t name(nt_line_t *__cont)
#define ntBegin()		switch (*__cont) { case 0:
#define ntEnd()			default: ; } return 0;

#define ntSub(fn)		do { static nt_line_t __sub_cont; __sub_cont=0; \
							(*__cont)=__LINE__; case __LINE__: { nt_time_t __sub_sleep=fn(&__sub_cont); if (__sub_sleep != 0) return __sub_sleep; } \
						} while(0)
#define ntReturn()		do { return 0; } while (0)

#define ntSleep(t)		do { \
							(*__cont)=__LINE__; return (t); case __LINE__: ; \
						} while(0)
#define ntYield()		ntSleep(1)


static inline nt_time_t ntSchedule(nt_t *list)
{
	nt_time_t sleep=NT_MAX_SLEEP;
	
	for (nt_t *l=list; l->fn; l++)
	{
		if (l->sleep == 0)
			l->sleep = l->fn(&l->cont);
		
		if (l->sleep < sleep) sleep=l->sleep;
	}
	
	if (sleep > 0)
	{
		for (nt_t *l=list; l->fn; l++)
			l->sleep-=sleep;
	}
	
	return sleep;
}


// TESTS
#if 0
	
	#include <stdio.h>
	
	ntDef(sub)
	{
		ntBegin();
			printf("Sub enter\n");
			printf("Sub sleep 10\n");
			ntSleep(10);
			printf("Sub sleep 5\n");
			ntSleep(5);
			printf("Sub exit\n");
		ntEnd();
	}
	
	ntDef(task1)
	{
		ntBegin();
			printf("Task1 enter\n");
			while (1)
			{
				printf("Task1 run sub\n");
				ntSub(sub);
				printf("Task1 sub return\n");
			}
		ntEnd();
	}
	
	ntDef(task2)
	{
		ntBegin();
			printf("Task2 enter\n");
			while (1)
			{
				printf("Task2 sleep 3\n");
				ntSleep(3);
			}
		ntEnd();
	}
	
		
	int main(void)
	{
		nt_t tasks[]={
			{ fn: task1 },
			{ fn: task2 },
			{ fn: 0 },
		};
		
		for (int i=0; i<10; i++)
		{
			printf("Sleep %d\n", ntSchedule(tasks));
		}
		
		return 0;
	}

#endif



#endif
