#include "rand.h"
#include "delay.h"
#include "sys_timer.h"
#include "hw_config.h"
#include "memmap.h"

#ifndef HW_RAND

static unsigned int _rand __attribute__ ((section (".sys_bss"))) =			0;

void srand()
{
	_rand = 0x30d02149;
	delay_ms(35);
	int i;
	for (i = 0; i < 0x100; ++i)
		_rand ^=  *((unsigned int *)(SYSTEM_POOL_BASE + i));
	TIME uptime;
	get_uptime(&uptime);
	_rand ^= uptime.usec;

}

unsigned int rand()
{
	_rand = _rand * 0x1b8365e9 + 0x6071d;
	int i;
	for (i = 0; i < 0x100; i += _rand & 0x13)
		_rand ^=  *((unsigned int *)(SYSTEM_POOL_BASE + i));
	TIME uptime;
	get_uptime(&uptime);
	_rand ^= uptime.usec;
	return _rand;
}

#endif
