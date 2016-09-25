/*
 *  CPUTimer.cpp
 *
 *  Created by Humberto Longo on 02/04/13.
 *  Instituto de Informatica - UFG
 *
 */

#include "CPUTimer.h"

//-------------------------------------------------------------------------

CPUTimer::CPUTimer()
{
	started = false;

	CPUCurrSecs = 0;
	CPUTotalSecs = 0;
	CronoCurrSecs = 0;
	CronoTotalSecs = 0;
}

//-------------------------------------------------------------------------

double CPUTimer::getCPUCurrSecs()
{
	return CPUCurrSecs;
}

//-------------------------------------------------------------------------

double CPUTimer::getCPUTotalSecs()
{
	return CPUTotalSecs;
}

//-------------------------------------------------------------------------

double CPUTimer::getCronoCurrSecs()
{
	return CronoCurrSecs;
}

//-------------------------------------------------------------------------

double CPUTimer::getCronoTotalSecs()
{
	return CronoTotalSecs;
}

//-------------------------------------------------------------------------

bool CPUTimer::start()
{
	bool status = true;

	CPUCurrSecs = 0;
	CronoCurrSecs = 0;

  CPUTStart = getCPUTime();
  CronoTStart = getRealTime();

	gottime = false;
	started = status;

	return ( status );
}

//-------------------------------------------------------------------------

bool CPUTimer::stop()
{
	bool status = true;

	if (started)
	{
    CPUTStop = getCPUTime();
    CronoTStop = getRealTime();

    CPUTotalSecs += CPUTStop - CPUTStart;
    CronoTotalSecs += CronoTStop - CronoTStart;
	}
	else
	{
		std::cout << "CPUTimer::stop(): called without calling CPUTimer::start() first!\n";
		status = false;
	}

	started = false;

	return status;
}

//-------------------------------------------------------------------------

void CPUTimer::reset()
{
	started = false;

	CPUCurrSecs = 0;
	CPUTotalSecs = 0;
	CronoCurrSecs = 0;
	CronoTotalSecs = 0;
}

//-------------------------------------------------------------------------

void CPUTimer::operator += ( CPUTimer t )
{
	CPUCurrSecs += t.getCPUCurrSecs();
	CPUTotalSecs += t.getCPUTotalSecs();
	CronoCurrSecs += t.getCronoCurrSecs();
	CronoTotalSecs += t.getCronoTotalSecs();
}

//-------------------------------------------------------------------------

/**
 * Returns the amount of CPU time used by the current process,
 * in seconds, or -1.0 if an error occurred.
 */
double CPUTimer::getCPUTime( )
{
  #if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  /* Prefer high-res POSIX timers, when available. */
  clockid_t id;
  struct timespec ts;

  #if _POSIX_CPUTIME > 0
  /* Clock ids vary by OS.  Query the id, if possible. */
  if ( clock_getcpuclockid( 0, &id ) == -1 )
  #endif
  #if defined(CLOCK_PROCESS_CPUTIME_ID)
    /* Use known clock id for AIX, Linux, or Solaris. */
    id = CLOCK_PROCESS_CPUTIME_ID;
  #elif defined(CLOCK_VIRTUAL)
    /* Use known clock id for BSD or HP-UX. */
    id = CLOCK_VIRTUAL;
  #else
    id = (clockid_t) - 1;
  #endif

  if ((id != (clockid_t) - 1) && (clock_gettime( id, &ts ) != -1))
    return ((double) ts.tv_sec + (double) ts.tv_nsec / 1000000000.0);
  #endif

  #if defined(RUSAGE_SELF)
  struct rusage rusage;

  if (getrusage( RUSAGE_SELF, &rusage ) != -1)
    return ((double) rusage.ru_utime.tv_sec + (double) rusage.ru_utime.tv_usec / 1000000.0);
  #endif

	/* Failed. */
	return -1.0;
}

//-------------------------------------------------------------------------

/**
 * Returns the amount of real time used by the current process,
 * in seconds, or -1.0 if an error occurred.
 */
double CPUTimer::getRealTime()
{
  #if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
	struct timespec ts;
  #if defined(CLOCK_MONOTONIC_PRECISE)
	/* BSD. --------------------------------------------- */
	const clockid_t id = CLOCK_MONOTONIC_PRECISE;
  #elif defined(CLOCK_MONOTONIC_RAW)
	/* Linux. ------------------------------------------- */
	const clockid_t id = CLOCK_MONOTONIC_RAW;
  #elif defined(CLOCK_HIGHRES)
	/* Solaris. ----------------------------------------- */
	const clockid_t id = CLOCK_HIGHRES;
  #elif defined(CLOCK_MONOTONIC)
	/* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
	const clockid_t id = CLOCK_MONOTONIC;
  #elif defined(CLOCK_REALTIME)
	/* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
	const clockid_t id = CLOCK_REALTIME;
  #else
  /* Unknown. */
	const clockid_t id = (clockid_t) - 1;
  #endif

	if ((id != (clockid_t) - 1) && (clock_gettime( id, &ts ) != -1))
		return ((double) ts.tv_sec + (double) ts.tv_nsec / 1000000000.0);

  #elif defined(__MACH__) && defined(__APPLE__)
	/* OSX. --------------------------------------------- */
	static double timeConvert = 0.0;

	if (timeConvert == 0.0)
	{
		mach_timebase_info_data_t timeBase;

		(void) mach_timebase_info( &timeBase );
		timeConvert = (double) timeBase.numer / (double) timeBase.denom / 1000000000.0;
	}

	return (double) mach_absolute_time( ) * timeConvert;
  #else

  /* Failed. */
	return -1.0;
  #endif

  /* To avoid compilation warnings only!!! */
	return -1.0;
}

//-------------------------------------------------------------------------
