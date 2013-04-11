About										{#about}
=====

MKernel is embedded RTOS, based on microkernel architecture.
It's designed for ARM microprocessors, however, can be easily ported to any
32bit MCU.

Features									 
========

- BSD License, royalty free, open source.
- Tickless. For system scheduling used 1 HPET timer and RTC. If RTC is
unavailable, another HPET timer can be used for RTC emulation (with full RTC
functionality)
- Hardware abstraction.RTOS is providing common interface for drivers.
- Standart microkernel syncronization: mutexes, events, semaphores, queues.
- Nested mutex priority inheritance
- Embedded dynamic memory manager, with ability of nested memory pools for 
  threads, dedicated system pool. Aligned malloc calls inside of any pools
- Safe and MPU ready. All supervisor-specific calls are wrapped around 
  swi/svc calls for context rising.
- Embedded libraries:
  * printf/sprintf. Around 1k of code.
  * time routines. POSIX-light
  * single-linked list, double linked list, ring buffer, block ring buffer
  * random number generation: hardware (if supported) or software
- Error handling:
  * kill thread on system error, print error, if configured
  * restart system on critical error, memory dump if configured
  * handle error exceptions, decode address (if supported) and exception code
- Lot of debug features:
  * memory dumps
  * object marks
  * thread profiling: name, uptime, stack allocated/current/used
  * memory profiling: red-markings, pool free/allocated size, objects fragmentation
  * stack profiling: supervisor allocated/current/used. Plus for arm7 - irq, fiq, abt
  * configurable debug console
- Supported hardware:
  * ARM7
  * cortex-m3, drivers for:STM32 F2 (gpio, uart, rcc, timer, dma, rand)

Cortex-M3 features:
------------------
- Native SVC and pendSV support

ARM7 features:
-------------
- Nested interrupts
- FIQ support
- pendSV emulation, when returning to user/system context

History
=======
0.1.5
+ sd card module (STM32F2)
+ usb core module (STM32F2)
+ usb device module (STM32F2)
+ SCSI block module
+ USB mass storage class

0.1.4
+ new module: software timers, running in independent thread
+ keyboard module new functions: keyboard_wait_for_key, keyboard_has_messages
! exception handling improved: 
	- detection of caller context (SYS/thread), instruction address, caused problem
	- if thread context, thread will be terminated, if SYS, system will be restarted
	- SVC call while interrupts are disabled error decoded and processed, instead of 
	  general hard fault
	- configurable option to halt system on fatal error, instead of rebooting

0.1.3

+ pin keyboard module with debounce logic
! sys_alloc, sys_alloc_aligned, sys_free now supported in SYS context
+ push method for debug console

0.1.2
- added examples for STM32 F2
- optimized software rand
- renamed all .s to .S

0.1.1
- added support of software random number generation
- added support for STM32 F2 HW RNG
- srand() by default in startup

Contacts
========

Author: Alexey Kramarenko, alexeyk13@yandex.ru
