/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/
#include <bdk.h>
#include <stdio.h>
#include <malloc.h>

#define STACK_CANARY 0x0BADBADBADBADBADull

typedef struct bdk_thread
{
    struct bdk_thread *next;
    uint64_t    coremask;
    uint64_t    gpr[32];   /* Reg 31 is SP */
    struct _reent lib_state;
    uint64_t    stack_canary;
    uint64_t    stack[0];
} bdk_thread_t;

typedef struct
{
    bdk_thread_t*    head;
    bdk_thread_t*    tail;
    bdk_spinlock_t   lock;
    int64_t         __padding1[16-3]; /* Stats in different cache line for speed */
    int64_t         stat_num_threads;
    int64_t         stat_no_schedulable_threads;
    int64_t         stat_next_calls;
    int64_t         stat_next_walks;
    int64_t         __padding2[16-4];
} bdk_thread_node_t;

static bdk_thread_node_t bdk_thread_node[BDK_NUMA_MAX_NODES];

extern void __bdk_thread_switch(bdk_thread_t* next_context, int delete_old);

/**
 * Main thread body for all threads
 *
 * @param func   User function to call
 * @param arg0   First argument to the user function
 * @param arg1   Second argument to the user function
 */
static void __bdk_thread_body(bdk_thread_func_t func, int arg0, void *arg1)
{
    func(arg0, arg1);
    bdk_thread_destroy();
}


/**
 * Initialize the BDK thread library
 *
 * @return Zero on success, negative on failure
 */
int bdk_thread_initialize(void)
{
    bdk_zero_memory(bdk_thread_node, sizeof(bdk_thread_node));
    _REENT_INIT_PTR(&__bdk_thread_global_reent);
    return 0;
}

static bdk_thread_t *__bdk_thread_next(void)
{
    bdk_thread_node_t *t_node = &bdk_thread_node[bdk_numa_local()];
    uint64_t coremask = bdk_core_to_mask();

    bdk_atomic_add64_nosync(&t_node->stat_next_calls, 1);
    bdk_thread_t *prev = NULL;
    bdk_thread_t *next = t_node->head;
    int walks = 0;
    while (next && !(next->coremask & coremask))
    {
        prev = next;
        next = next->next;
        walks++;
    }
    if (walks)
        bdk_atomic_add64_nosync(&t_node->stat_next_walks, walks);

    if (next)
    {
        if (t_node->tail == next)
            t_node->tail = prev;
        if (prev)
            prev->next = next->next;
        else
            t_node->head = next->next;
        next->next = NULL;
    }
    else
        bdk_atomic_add64_nosync(&t_node->stat_no_schedulable_threads, 1);

    return next;
}

/**
 * Yield the current thread and run a new one
 */
void bdk_thread_yield(void)
{
    if (BDK_DBG_MAGIC_ENABLE && (bdk_numa_local() == bdk_numa_master()))
        bdk_dbg_check_magic();
    bdk_thread_node_t *t_node = &bdk_thread_node[bdk_numa_local()];
    bdk_thread_t *current;
    BDK_MRS_NV(TPIDR_EL3, current);

    /* Yield can be called without a thread context during core init. The
       cores call bdk_wait_usec(), which yields. In this case yielding
       does nothing */
    if (bdk_unlikely(!current))
        return;

    if (bdk_unlikely(current->stack_canary != STACK_CANARY))
        bdk_fatal("bdk_thread_yield() detected a stack overflow\n");

    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        bdk_sso_process_work();

    if (t_node->head == NULL)
        return;

    bdk_spinlock_lock(&t_node->lock);

    /* Find the first thread that can run on this core */
    bdk_thread_t *next = __bdk_thread_next();

    /* If next is NULL then there are no other threads ready to run and we
        will continue without doing anything */
    if (next)
    {
        __bdk_thread_switch(next, 0);
        /* Unlock performed in __bdk_thread_switch_complete */
        return;
    }
    bdk_spinlock_unlock(&t_node->lock);
}


/**
 * Create a new thread and return it. The thread will not be scheduled
 * as it isn't put in the thread list.
 *
 * @param coremask   Mask of cores the thread can run on. Each set bit is an allowed
 *                   core. Zero and -1 are both shortcuts for all cores.
 * @param func       Function to run as a thread
 * @param arg0       First argument to the function
 * @param arg1       Second argument to the function
 * @param stack_size Stack size for the new thread. Set to zero for the system default.
 *
 * @return Thread or NULL on failure
 */
static void *__bdk_thread_create(uint64_t coremask, bdk_thread_func_t func, int arg0, void *arg1, int stack_size)
{
    bdk_thread_t *thread;
    if (!stack_size)
        stack_size = BDK_THREAD_DEFAULT_STACK_SIZE;

    thread = memalign(16, sizeof(bdk_thread_t) + stack_size);
    if (thread == NULL)
    {
        bdk_error("Unable to allocate memory for new thread\n");
        return NULL;
    }
    memset(thread, 0, sizeof(bdk_thread_t) + stack_size);
    if (coremask == 0)
        coremask = -1;
    thread->coremask = coremask;
    thread->gpr[0] = (uint64_t)func;    /* x0 = Argument 0 to __bdk_thread_body */
    thread->gpr[1] = arg0;              /* x1 = Argument 1 to __bdk_thread_body */
    thread->gpr[2] = (uint64_t)arg1;    /* x2 = Argument 2 to __bdk_thread_body */
    thread->gpr[29] = 0;                /* x29 = Frame pointer */
    thread->gpr[30] = (uint64_t)__bdk_thread_body; /* x30 = Link register */
    thread->gpr[31] = (uint64_t)thread->stack + stack_size; /* x31 = Stack pointer */
    if (thread->gpr[31] & 0xf)
        bdk_fatal("Stack not aligned 0x%lx\n", thread->gpr[31]);
    _REENT_INIT_PTR(&thread->lib_state);
    extern void __sinit(struct _reent *);
    __sinit(&thread->lib_state);
    thread->stack_canary = STACK_CANARY;
    thread->next = NULL;
    return thread;
}


/**
 * Create a new thread. The thread may be scheduled to any of the
 * cores supplied in the coremask. Note that a single thread is
 * created and may only run on one core at a time. The thread may
 * not start executing until the next yield call if all cores in
 * the coremask are currently busy.
 *
 * @param node       Node to use in a Numa setup. Can be an exact ID or a
 *                   special value.
 * @param coremask   Mask of cores the thread can run on. Each set bit is an allowed
 *                   core. Zero and -1 are both shortcuts for all cores.
 * @param func       Function to run as a thread
 * @param arg0       First argument to the function
 * @param arg1       Second argument to the function
 * @param stack_size Stack size for the new thread. Set to zero for the system default.
 *
 * @return Zero on success, negative on failure
 */
int bdk_thread_create(bdk_node_t node, uint64_t coremask, bdk_thread_func_t func, int arg0, void *arg1, int stack_size)
{
    bdk_thread_node_t *t_node = &bdk_thread_node[node];
    bdk_thread_t *thread = __bdk_thread_create(coremask, func, arg0, arg1, stack_size);
    if (thread == NULL)
        return -1;

    bdk_atomic_add64_nosync(&t_node->stat_num_threads, 1);
    bdk_spinlock_lock(&t_node->lock);
    if (t_node->tail)
        t_node->tail->next = thread;
    else
        t_node->head = thread;
    t_node->tail = thread;
    bdk_spinlock_unlock(&t_node->lock);
    BDK_SEV;
    return 0;
}


/**
 * Destroy the currently running thread. This never returns.
 */
void bdk_thread_destroy(void)
{
    bdk_thread_node_t *t_node = &bdk_thread_node[bdk_numa_local()];
    bdk_thread_t *current;
    BDK_MRS_NV(TPIDR_EL3, current);
    if (bdk_unlikely(!current))
        bdk_fatal("bdk_thread_destroy() called without thread context\n");
    if (bdk_unlikely(current->stack_canary != STACK_CANARY))
        bdk_fatal("bdk_thread_destroy() detected a stack overflow\n");

    fflush(NULL);
    bdk_atomic_add64_nosync(&t_node->stat_num_threads, -1);

    while (1)
    {
        if (BDK_DBG_MAGIC_ENABLE && (bdk_numa_local() == bdk_numa_master()))
            bdk_dbg_check_magic();
        if (t_node->head)
        {
            bdk_spinlock_lock(&t_node->lock);
            /* Find the first thread that can run on this core */
            bdk_thread_t *next = __bdk_thread_next();

            /* If next is NULL then there are no other threads ready to run and we
                will continue without doing anything */
            if (next)
            {
                __bdk_thread_switch(next, 1);
                bdk_fatal("bdk_thread_destroy() should never get here\n");
            }
            bdk_spinlock_unlock(&t_node->lock);
        }
        if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
            bdk_sso_process_work();
        BDK_WFE;
    }
}

struct _reent __bdk_thread_global_reent;
struct _reent *__bdk_thread_getreent(void)
{
    bdk_thread_t *current;
    BDK_MRS_NV(TPIDR_EL3, current);
    if (current)
        return &current->lib_state;
    else
        return &__bdk_thread_global_reent;
}

void __bdk_thread_switch_complete(bdk_thread_t* old_context, int delete_old)
{
    bdk_thread_node_t *t_node = &bdk_thread_node[bdk_numa_local()];
    if (bdk_unlikely(delete_old))
    {
        bdk_spinlock_unlock(&t_node->lock);
        free(old_context);
    }
    else
    {
        if (bdk_likely(old_context))
        {
            if (t_node->tail)
                t_node->tail->next = old_context;
            else
                t_node->head = old_context;
            t_node->tail = old_context;
        }
        bdk_spinlock_unlock(&t_node->lock);
        if (bdk_likely(old_context))
            BDK_SEV;
    }
}


/**
 * Called to create the initial thread for a CPU. Must be called
 * once for each CPU.
 *
 * @param func       Function to run as new thread. It is guaranteed that this will
 *                   be the next thread run by the core.
 * @param arg0       First thread argument
 * @param arg1       Second thread argument
 * @param stack_size Initial stack size, or zero for the default
 */
void bdk_thread_first(bdk_thread_func_t func, int arg0, void *arg1, int stack_size)
{
    bdk_thread_node_t *t_node = &bdk_thread_node[bdk_numa_local()];
    void *thread = __bdk_thread_create(bdk_core_to_mask(), func, arg0, arg1, stack_size);
    if (thread)
    {
        bdk_atomic_add64_nosync(&t_node->stat_num_threads, 1);
        bdk_spinlock_lock(&t_node->lock);
        __bdk_thread_switch(thread, 0);
    }
    bdk_fatal("Create of __bdk_init_main thread failed\n");
}

/**
 * Display statistics about the number of threads and scheduling
 */
void bdk_thread_show_stats()
{
    for (bdk_node_t node = BDK_NODE_0; node < BDK_NUMA_MAX_NODES; node++)
    {
        if (!bdk_numa_exists(node))
            continue;
        bdk_thread_node_t *t_node = &bdk_thread_node[node];
        printf("Node %d\n", node);
        printf("   Active threads:      %ld\n", t_node->stat_num_threads);
        printf("   Schedule checks:     %ld\n", t_node->stat_next_calls);
        int64_t div = t_node->stat_next_calls;
        if (!div)
            div = 1;
        printf("   Average walk depth:  %ld\n",
            t_node->stat_next_walks / div);
        printf("   Not switching: %ld (%ld%%)\n",
            t_node->stat_no_schedulable_threads,
            t_node->stat_no_schedulable_threads * 100 / div);
        bdk_atomic_set64(&t_node->stat_next_calls, 0);
        bdk_atomic_set64(&t_node->stat_next_walks, 0);
        bdk_atomic_set64(&t_node->stat_no_schedulable_threads, 0);
    }
}
