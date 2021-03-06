/* Copyright (C) 2011-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Chris Metcalf <cmetcalf@tilera.com>, 2011.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <errno.h>

int __syscall_error (int dummy, int err);
hidden_proto (__syscall_error)

/* This routine is jumped to by all the syscall handlers, to stash
   an error number into errno.  */
int
__syscall_error (int dummy, int err)
{
  __set_errno (err);
  return -1;
}
hidden_def (__syscall_error)
