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

#ifndef MAGIC_H
#define MAGIC_H

/*
		magic.h - magics for kernel marks
*/

#define MAGIC_MEM_POOL_ENTRY						0xbde4e2f1
#define MAGIC_MEM_POOL_ALIGN_SPACE				0x851ab238
#define MAGIC_MEM_POOL_UNUSED						0xed8bb5a9
#define MAGIC_RANGE_TOP								0x27f0d1c2
#define MAGIC_RANGE_BOTTOM							0xb45f8dc3

#define MAGIC_TIMER									0xbecafcf5
#define MAGIC_THREAD									0x7de32076
#define MAGIC_MUTEX									0xd0cc6e26
#define MAGIC_EVENT									0x57e198c7
#define MAGIC_SEMAPHORE								0xabfd92d9
#define MAGIC_QUEUE									0x6b54bbeb

#define MAGIC_UNINITIALIZED						0xcdcdcdcd
#define MAGIC_UNINITIALIZED_BYTE					0xcd

#endif // MAGIC_H
