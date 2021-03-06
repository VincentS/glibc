/* Special .init and .fini section support for tile.
   Copyright (C) 2012-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file with other
   programs, and to distribute those programs without any restriction
   coming from the use of this file. (The GNU Lesser General Public
   License restrictions do apply in other respects; for example, they
   cover modification of the file, and distribution when not linked
   into another program.)

   Note that people who make modified versions of this file are not
   obligated to grant this special exception for their modified
   versions; it is their choice whether to do so. The GNU Lesser
   General Public License gives permission to release a modified
   version without this exception; this exception also makes it
   possible to release a modified version which carries forward this
   exception.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* crti.S puts a function prologue at the beginning of the .init and
   .fini sections and defines global symbols for those addresses, so
   they can be called as functions.  The symbols _init and _fini are
   magic and cause the linker to emit DT_INIT and DT_FINI.  */

#include <libc-symbols.h>
#include <sysdep.h>

#ifndef PREINIT_FUNCTION
# define PREINIT_FUNCTION __gmon_start__
#endif

#ifndef PREINIT_FUNCTION_WEAK
# define PREINIT_FUNCTION_WEAK 1
#endif

#if PREINIT_FUNCTION_WEAK
	weak_extern (PREINIT_FUNCTION)
#else
	.hidden PREINIT_FUNCTION
#endif

	.section .init,"ax",@progbits
	.align 8
	.globl _init
	.type _init, @function
_init:
	{
	 move r29, sp
	 ADDI_PTR r28, sp, -REGSIZE
	 ST sp, lr
	}
	ADDI_PTR sp, sp, -(2 * REGSIZE)
	ST r28, r29
#if PREINIT_FUNCTION_WEAK
	lnk r2
0:
# ifdef __tilegx__
	moveli r1, hw2_last(_GLOBAL_OFFSET_TABLE_ - 0b)
	{
	 shl16insli r1, r1, hw1(_GLOBAL_OFFSET_TABLE_ - 0b)
	 moveli r0, hw1_last_got(PREINIT_FUNCTION)
	}
	{
	 shl16insli r1, r1, hw0(_GLOBAL_OFFSET_TABLE_ - 0b)
	 shl16insli r0, r0, hw0_got(PREINIT_FUNCTION)
	}
# else
	{
	 moveli r1, lo16(_GLOBAL_OFFSET_TABLE_ - 0b)
	 moveli r0, got_lo16(PREINIT_FUNCTION)
	}
	{
	 auli r1, r1, ha16(_GLOBAL_OFFSET_TABLE_ - 0b)
	 auli r0, r0, got_ha16(PREINIT_FUNCTION)
	}
# endif
	ADD_PTR r0, r0, r1
	ADD_PTR r0, r0, r2
	LD_PTR r0, r0
	BEQZ r0, .Lno_weak_fn
	jalr r0
#elif defined(__tilegx__) && !defined(NO_PLT_PCREL)
	/* Since we are calling from the start of the object to the PLT,
	   call by loading the full address into a register.  */
	lnk r2
0:
	moveli r0, hw2_last_plt(PREINIT_FUNCTION - 0b)
	shl16insli r0, r0, hw1_plt(PREINIT_FUNCTION - 0b)
	shl16insli r0, r0, hw0_plt(PREINIT_FUNCTION - 0b)
	add r0, r0, r2
	jalr r0
#else
	jal plt(PREINIT_FUNCTION)
#endif
.Lno_weak_fn:

	.section .fini,"ax",@progbits
	.align 8
	.globl _fini
	.type _fini, @function
_fini:
	{
	 move r29, sp
	 ADDI_PTR r28, sp, -REGSIZE
	 ST sp, lr
	}
	ADDI_PTR sp, sp, -(2 * REGSIZE)
	ST r28, r29
