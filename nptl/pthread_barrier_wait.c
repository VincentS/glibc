/* Copyright (C) 2003-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Martin Schwidefsky <schwidefsky@de.ibm.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <sysdep.h>
#include <lowlevellock.h>
#include <futex-internal.h>
#include <pthreadP.h>


/* Wait on barrier.  */
int
__pthread_barrier_wait (barrier)
     pthread_barrier_t *barrier;
{
  struct pthread_barrier *ibarrier = (struct pthread_barrier *) barrier;
  int result = 0;
  int lll_private = ibarrier->private ^ FUTEX_PRIVATE_FLAG;
  int futex_private = (lll_private == LLL_PRIVATE
		       ? FUTEX_PRIVATE : FUTEX_SHARED);

  /* Make sure we are alone.  */
  lll_lock (ibarrier->lock, lll_private);

  /* One more arrival.  */
  --ibarrier->left;

  /* Are these all?  */
  if (ibarrier->left == 0)
    {
      /* Yes. Increment the event counter to avoid invalid wake-ups and
	 tell the current waiters that it is their turn.  */
      ++ibarrier->curr_event;

      /* Wake up everybody.  */
      futex_wake (&ibarrier->curr_event, INT_MAX, futex_private);

      /* This is the thread which finished the serialization.  */
      result = PTHREAD_BARRIER_SERIAL_THREAD;
    }
  else
    {
      /* The number of the event we are waiting for.  The barrier's event
	 number must be bumped before we continue.  */
      unsigned int event = ibarrier->curr_event;

      /* Before suspending, make the barrier available to others.  */
      lll_unlock (ibarrier->lock, lll_private);

      /* Wait for the event counter of the barrier to change.  */
      do
	futex_wait_simple (&ibarrier->curr_event, event, futex_private);
      while (event == ibarrier->curr_event);
    }

  /* Make sure the init_count is stored locally or in a register.  */
  unsigned int init_count = ibarrier->init_count;

  /* If this was the last woken thread, unlock.  */
  if (atomic_increment_val (&ibarrier->left) == init_count)
    /* We are done.  */
    lll_unlock (ibarrier->lock, lll_private);

  return result;
}
weak_alias (__pthread_barrier_wait, pthread_barrier_wait)
