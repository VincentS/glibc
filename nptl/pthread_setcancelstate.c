/* Copyright (C) 2002-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include "pthreadP.h"
#include <atomic.h>


int
__pthread_setcancelstate (state, oldstate)
     int state;
     int *oldstate;
{
  volatile struct pthread *self;

  if (state < PTHREAD_CANCEL_ENABLE || state > PTHREAD_CANCEL_DISABLE)
    return EINVAL;

  self = THREAD_SELF;

  /* Store the old value.  */
  if (oldstate != NULL)
    *oldstate = THREAD_GETMEM (self, cancelstate);
  self->cancelstate = state;

  if (CANCEL_ENABLED_AND_CANCELED_AND_ASYNCHRONOUS (self))
    __do_cancel ();

  return 0;
}
strong_alias (__pthread_setcancelstate, pthread_setcancelstate)
hidden_def (__pthread_setcancelstate)
