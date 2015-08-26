// See LICENSE for license details.

#ifndef _RISCV_ATOMIC_H
#define _RISCV_ATOMIC_H

#include <arch/encoding.h>

#define disable_irqsave() clear_csr(sstatus, SSTATUS_IE)
#define enable_irqrestore(flags) set_csr(sstatus, (flags) & SSTATUS_IE)

typedef struct { int lock; } spinlock_t;
#define SPINLOCK_INIT {0}

#define mb() __sync_synchronize()
#define atomic_set(ptr, val) (*(volatile typeof(*(ptr)) *)(ptr) = val)
#define atomic_read(ptr) (*(volatile typeof(*(ptr)) *)(ptr))

#ifdef PK_ENABLE_ATOMICS
# define atomic_add(ptr, inc) __sync_fetch_and_add(ptr, inc)
# define atomic_swap(ptr, swp) __sync_lock_test_and_set(ptr, swp)
# define atomic_cas(ptr, cmp, swp) __sync_val_compare_and_swap(ptr, cmp, swp)
#else
# define atomic_add(ptr, inc) ({ \
  long flags = disable_irqsave(); \
  typeof(ptr) res = *(volatile typeof(ptr))(ptr); \
  *(volatile typeof(ptr))(ptr) = res + (inc); \
  enable_irqrestore(flags); \
  res; })
# define atomic_swap(ptr, swp) ({ \
  long flags = disable_irqsave(); \
  typeof(*ptr) res = *(volatile typeof(ptr))(ptr); \
  *(volatile typeof(ptr))(ptr) = (swp); \
  enable_irqrestore(flags); \
  res; })
# define atomic_cas(ptr, cmp, swp) ({ \
  long flags = disable_irqsave(); \
  typeof(ptr) res = *(volatile typeof(ptr))(ptr); \
  if (res == (cmp)) *(volatile typeof(ptr))(ptr) = (swp); \
  enable_irqrestore(flags); \
  res; })
#endif

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
  long flags = disable_irqsave();
  spinlock_lock(lock);
  return flags;
}

static inline void spinlock_unlock_irqrestore(spinlock_t* lock, long flags)
{
  spinlock_unlock(lock);
  enable_irqrestore(flags);
}

#endif
