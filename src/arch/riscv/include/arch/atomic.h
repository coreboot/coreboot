// See LICENSE for license details.

#ifndef _RISCV_ATOMIC_H
#define _RISCV_ATOMIC_H

#include <arch/encoding.h>

typedef struct { volatile long val; } atomic_t;
typedef struct { atomic_t lock; } spinlock_t;
#define SPINLOCK_INIT {{0}}

#define mb() __sync_synchronize()

static inline void atomic_set(atomic_t* a, long val)
{
  a->val = val;
}

static inline long atomic_read(atomic_t* a)
{
  return a->val;
}

static inline long atomic_add(atomic_t* a, long inc)
{
  long ret = atomic_read(a);
  atomic_set(a, ret + inc);
  return ret;
}

static inline long atomic_swap(atomic_t* a, long val)
{
  long ret = atomic_read(a);
  atomic_set(a, val);
  return ret;
}

static inline long atomic_cas(atomic_t* a, long compare, long swap)
{
  long ret = atomic_read(a);
  if (ret == compare)
    atomic_set(a, swap);
  return ret;
}

static inline void spinlock_lock(spinlock_t* lock)
{
  do
  {
    while (atomic_read(&lock->lock))
      ;
  } while (atomic_swap(&lock->lock, -1));
  mb();
}

static inline void spinlock_unlock(spinlock_t* lock)
{
  mb();
  atomic_set(&lock->lock,0);
}

static inline long spinlock_lock_irqsave(spinlock_t* lock)
{
  long flags = clear_csr(status, SR_EI);
  spinlock_lock(lock);
  return flags;
}

static inline void spinlock_unlock_irqrestore(spinlock_t* lock, long flags)
{
  spinlock_unlock(lock);
  set_csr(status, flags & SR_EI);
}

#endif
