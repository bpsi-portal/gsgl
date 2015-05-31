#include "config.h"
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef HAVE_STRING_H
#  include <string.h>
#endif
#ifndef HAVE_STDLIB_H
   typedef long time_t;
   typedef u_long clock_t;
   extern char *getenv();
#else
#  include <stdlib.h>
#endif

#include <stdio.h>

#ifdef HAVE_SYS_TIMES_H
# include <sys/times.h>
#endif
#ifdef HAVE_TIME_H
# include <time.h>
#endif

#if HAVE_SYSCONF && defined _SC_CLK_TCK
#  define TICKS_PER_SECOND sysconf (_SC_CLK_TCK) /* POSIX 1003.1-1996 */
#else
#  ifdef CLK_TCK
#    define TICKS_PER_SECOND CLK_TCK /* POSIX 1003.1-1988; obsolescent */
#  else
#    define TICKS_PER_SECOND HZ /* traditional UNIX */
#  endif
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#if SIZEOF_INT == 4
#define		int4	int
#elif SIZEOF_INT == 2
#define		int4	long
#else
#define		int4	int
#endif

/* get date and time */
void gsaf_gudate (int *ndy, int *ndm, int *ndd, int *nth, int *ntm, int *nts)
{
        time_t nseconds;
        struct tm *ptr;

        nseconds = time(NULL);
        ptr = localtime(&nseconds);
        *ndy = (int)(ptr->tm_year);
        *ndm = (int)(ptr->tm_mon)+1;
        *ndd = (int)(ptr->tm_mday);
        *nth = (int)(ptr->tm_hour);
        *ntm = (int)(ptr->tm_min);
        *nts = (int)(ptr->tm_sec);
}

/* get CPU time */

void gsaf_gutime (float *time)
{
	struct tms buffer;
	clock_t j;

 	times (&buffer);
	j = buffer.tms_utime;
	*time = ((double) j) / ((double) TICKS_PER_SECOND);
}

/* sleep in sec */
void gsaf_gu_sleep (float t)
{
	long i;

	i = t * 1000000 + 0.5;
	usleep(i);
}

float gsaf_gurand (float x)
{
	int4 i, k;
#ifndef SONYCISC
	/*unsigned j;*/
	i =  rand();
	k = RAND_MAX;
#else
	/*long j;*/
	i =  random();
	k = 2147483647;
#endif
	return x * ((float) i) / ((float) k);
}

void gsaf_gu_srand (int4 i)
{
#ifndef SONYCISC
	unsigned int k;
	k = i;
 	srand(k);
#endif
}

void gsaf_guflsh ()
{
	fflush (stdout);
}

/*#define DVRENV_F77 F77_FUNC(dvrenv, DVRENV)
  void DVRENV_F77 (iasc,nchar)
  int4 *iasc,*nchar;
  {
  char *str;
  int i,len;
  int4 k;
  
  for(i = 0; i < *nchar; i++)
  iasc[i] = ' ';
  str = getenv("GSAF_PARM_TEMP");
  if(str){
  len = strlen(str);
  if(len > *nchar)
  len = *nchar;
  for(i = 0; i < len; ++i)
  {    
  k = (int4)str[i];
  if(k < 32)
  k = 32;
  iasc[i] = k;
  }
  }
  }*/
