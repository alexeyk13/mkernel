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

#ifndef _TIME_H_
#define _TIME_H_

/*
		time routines
 */

#include "types.h"

/** \addtogroup lib_time time
	time routines
	\{
 */

/**
	\brief POSIX analogue of struct tm
	\our struct tm is shorter, than POSIX. M-Kernel didn't use tm_wday, tm_yday, tm_isdst and negative values for perfomance reasons, tm_year - is absolute value
*/
struct tm {
	unsigned char tm_sec;                   //!< seconds after the minute [0, 59]
	unsigned char tm_min;                   //!< minutes after the hour [0, 59]
	unsigned char tm_hour;                  //!< hours since midnight [0, 23]
	unsigned char tm_mday;                  //!< day of the month [1, 31]
	unsigned char tm_mon;                   //!< months since January [0, 11]
	unsigned short tm_year;                 //!< years since 0
};

//In 2037, please change this to unsigned long long. In 32 bits mcu changing this can significally decrease perfomance
/**
	\brief time_t POSIX analogue
*/
typedef unsigned long time_t;

/**
	\brief structure for holding time units
*/
typedef struct {
	time_t sec;										//!< seconds
	unsigned long usec;							//!< microseconds
}TIME;

/** \} */ // end of lib_time group

//refer to POSIX
time_t mktime(struct tm* ts);
//posix gmtime isn't safe because of static return value.
struct tm* gmtime(time_t time, struct tm* ts);

//to > from ? 1, to < from ? - 1, to == from ? 0
int time_compare(TIME* from, TIME* to);
//res = to + from. safe to use, when res == from or res == to
void time_add(TIME* from, TIME* to, TIME* res);
//res = to - from. if to < from, res = 0. safe to use, when res == from or res == to
void time_sub(TIME* from, TIME* to, TIME* res);
void us_to_time(int us, TIME* time);
void ms_to_time(int ms, TIME* time);
int time_to_us(TIME* time);
int time_to_ms(TIME* time);

#endif /*_TIME_H_*/
