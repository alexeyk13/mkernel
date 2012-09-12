/*
	M-Kernel - embedded RTOS
	Copyright (c) 2011-2012, Alexey Kramarenko
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
		list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "time.h"

//time_t = 0
#define EPOCH_YEAR						1970

//is year is leap?
#define IS_LEAP_YEAR(year)				(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)					(IS_LEAP_YEAR(year) ? 366 : 365)

//seconds in day
#define SECS_IN_DAY						(24l * 60l * 60l)

const unsigned short MDAY[2][12] =	{{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
												 { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};

const unsigned short YDAY[2][12] =	{{  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
												 {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}};

#define USEC_1S							1000000ul
#define USEC_1MS							1000ul
#define MSEC_1S							1000ul

#define MAX_US_DELTA						2146
#define MAX_MS_DELTA						2147482

/** \addtogroup lib_time time
	time routines
	\{
 */

/**
	\brief POSIX analogue. Convert struct tm to time_t
	\param ts: time in struct \ref tm
	\retval time in \ref time_t
*/
time_t mktime(struct tm* ts)
{
	register time_t days_from_epoch;
	days_from_epoch = (ts->tm_year - EPOCH_YEAR) * 365;
	days_from_epoch += (ts->tm_year - EPOCH_YEAR) / 4 + ((ts->tm_year % 4) && ts->tm_year % 4 < EPOCH_YEAR % 4);
	days_from_epoch -= (ts->tm_year - EPOCH_YEAR) / 100 + ((ts->tm_year % 100) && ts->tm_year % 100 < EPOCH_YEAR % 100);
	days_from_epoch += (ts->tm_year - EPOCH_YEAR) / 400 + ((ts->tm_year % 400) && ts->tm_year % 400 < EPOCH_YEAR % 400);

	days_from_epoch += YDAY[IS_LEAP_YEAR(ts->tm_year)][ts->tm_mon] + ts->tm_mday - 1;
	return days_from_epoch * SECS_IN_DAY + (ts->tm_hour * 60 + ts->tm_min) * 60 + ts->tm_sec;
}

/**
	\brief POSIX analogue. Convert time_t to struct tm
	\param time: time in \ref time_t
	\param ts: result time in struct \ref tm
	\retval same as ts
*/
struct tm* gmtime(time_t time, struct tm* ts)
{
	register time_t val = time;
	//first - decode time
	ts->tm_sec = val % 60;
	val /= 60;
	ts->tm_min = val % 60;
	val /= 60;
	ts->tm_hour = val % 24;
	val /= 24;

	//year between start date
	ts->tm_year = EPOCH_YEAR;
	while (val >= YEARSIZE(ts->tm_year))
	{
		val -= YEARSIZE(ts->tm_year);
		ts->tm_year++;
	}

	//decode month
	ts->tm_mon = 0;
	while (val >= MDAY[IS_LEAP_YEAR(ts->tm_year)][ts->tm_mon])
	{
		val -= MDAY[IS_LEAP_YEAR(ts->tm_year)][ts->tm_mon];
		ts->tm_mon++;
	}
	ts->tm_mday = val + 1;
	return ts;
}

/**
	\brief compare time.
	\param from: time from
	\param to: time to
	\retval if "to" > "from", return 1, \n
	if "to" < "from", return -1, \n
	if "to" == "from", return 0
*/
int time_compare(TIME* from, TIME* to)
{
	int res = -1;
	if (to->sec > from->sec)
		res = 1;
	else if (to->sec == from->sec)
	{
		if (to->usec > from->usec)
			res = 1;
		else if (to->usec == from->usec)
			res = 0;
		//else res = -1
	}//else res = -1
	return res;
}

/**
	\brief res = from + to
	\param from: time from
	\param to: time to
	\param res: result time. Safe to be same as "from" or "to"
	\retval none
*/
void time_add(TIME* from, TIME* to, TIME* res)
{
	res->sec = to->sec + from->sec;
	res->usec = to->usec + from->usec;
	//loan
	if (res->usec >= USEC_1S)
	{
		++res->sec;
		res->usec -= USEC_1S;
	}
}

/**
	\brief res = to - from
	\param from: time from
	\param to: time to
	\param res: result time. Safe to be same as "from" or "to"
	\retval none
*/
void time_sub(TIME* from, TIME* to, TIME* res)
{
	if (time_compare(from, to) > 0)
	{
		res->sec = to->sec - from->sec;
		//borrow
		if (to->usec >= from->usec)
			res->usec = to->usec - from->usec;
		else
		{
			res->usec = USEC_1S - (from->usec - to->usec);
			--res->sec;
		}
	}
	else
		res->sec = res->usec = 0;
}

/**
	\brief convert time in microseconds to \ref TIME structure
	\param us: microseconds
	\param time: pointer to allocated result \ref TIME structure
	\retval none
*/
void us_to_time(int us, TIME* time)
{
	time->sec = us / USEC_1S;
	time->usec = us % USEC_1S;
}

/**
	\brief convert time in milliseconds to \ref TIME structure
	\param ms: milliseconds
	\param time: pointer to allocated result \ref TIME structure
	\retval none
*/
void ms_to_time(int ms, TIME* time)
{
	time->sec = ms / MSEC_1S;
	time->usec = (ms % MSEC_1S) * USEC_1MS;
}

/**
	\brief convert time from \ref TIME structure to microseconds
	\param time: pointer to \ref TIME structure. Maximal value: 0hr, 35 min, 46 seconds
	\retval time in microseconds
*/
int time_to_us(TIME* time)
{
	return time->sec <= MAX_US_DELTA ? (int)(time->sec * USEC_1S + time->usec) : (int)(MAX_US_DELTA * USEC_1S);
}

/**
	\brief convert time from \ref TIME structure to milliseconds
	\param time: pointer to \ref TIME structure. Maximal value: 24days, 20hr, 31 min, 22 seconds
	\retval time in milliseconds
*/
int time_to_ms(TIME* time)
{
	return time->sec <= MAX_MS_DELTA ? (int)(time->sec * MSEC_1S + time->usec / USEC_1MS) : (int)(MAX_MS_DELTA * MSEC_1S);
}

/** \} */ // end of lib_time group
