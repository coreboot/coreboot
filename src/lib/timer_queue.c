/* SPDX-License-Identifier: GPL-2.0-only */

#include <timer.h>

#define MAX_TIMER_QUEUE_ENTRIES 64

/* The timer queue is implemented using a min heap. Therefore the first
 * element is the one with smallest time to expiration. */
struct timer_queue {
	int num_entries;
	int max_entries;
	struct timeout_callback *queue[MAX_TIMER_QUEUE_ENTRIES];
};

static struct timer_queue global_timer_queue = {
	.num_entries = 0,
	.max_entries = MAX_TIMER_QUEUE_ENTRIES,
	.queue = { 0 },
};

static inline int timer_queue_empty(struct timer_queue *tq)
{
	return tq->num_entries == 0;
}

static inline int timer_queue_full(struct timer_queue *tq)
{
	return tq->num_entries == tq->max_entries;
}

static inline struct timeout_callback *timer_queue_head(struct timer_queue *tq)
{
	if (timer_queue_empty(tq))
		return NULL;
	return tq->queue[0];
}

static int timer_queue_insert(struct timer_queue *tq,
			      struct timeout_callback *tocb)
{
	int index;

	/* No more slots. */
	if (timer_queue_full(tq))
		return -1;

	index = tq->num_entries;
	tq->num_entries++;
	tq->queue[index] = tocb;

	while (index != 0) {
		struct timeout_callback *parent;
		int parent_index;

		parent_index = (index - 1) / 2;
		parent = tq->queue[parent_index];

		/* All other ancestors are less than or equal to the current. */
		if (mono_time_cmp(&parent->expiration, &tocb->expiration) <= 0)
			break;

		/* The parent is greater than current. Swap them. */
		tq->queue[parent_index] = tocb;
		tq->queue[index] = parent;

		index = parent_index;
	}

	return 0;
}

/* Get the index containing the entry with smallest value. */
static int timer_queue_min_child_index(struct timer_queue *tq, int index)
{
	int left_child_index;
	int right_child_index;

	left_child_index = 2 * index + 1;

	if (left_child_index >= tq->num_entries)
		return -1;

	right_child_index = left_child_index + 1;

	if (right_child_index >= tq->num_entries)
		return left_child_index;

	if (mono_time_cmp(&tq->queue[left_child_index]->expiration,
			&tq->queue[right_child_index]->expiration) < 0) {
		return left_child_index;
	}
	return right_child_index;
}

static void timer_queue_remove_head(struct timer_queue *tq)
{
	int index;
	struct timeout_callback *tocb;

	/* In order to remove the head the deepest child is replaced in the
	 * head slot and bubbled down the tree. */
	tq->num_entries--;
	tocb = tq->queue[tq->num_entries];
	tq->queue[0] = tocb;

	index = 0;
	while (1) {
		int min_child_index;
		struct timeout_callback *child;

		min_child_index = timer_queue_min_child_index(tq, index);

		/* No more entries to compare against. */
		if (min_child_index < 0)
			break;

		child = tq->queue[min_child_index];

		/* Current index is the correct place since it is smaller or
		 * equal to the smallest child. */
		if (mono_time_cmp(&tocb->expiration, &child->expiration) <= 0)
			break;

		/* Need to swap with smallest child. */
		tq->queue[min_child_index] = tocb;
		tq->queue[index] = child;

		index = min_child_index;
	}
}

static struct timeout_callback *
timer_queue_expired(struct timer_queue *tq, struct mono_time *current_time)
{
	struct timeout_callback *tocb;

	tocb = timer_queue_head(tq);

	if (tocb == NULL)
		return NULL;

	/* The timeout callback hasn't expired yet. */
	if (mono_time_before(current_time, &tocb->expiration))
		return NULL;

	timer_queue_remove_head(tq);

	return tocb;
}

int timer_sched_callback(struct timeout_callback *tocb, uint64_t us)
{
	struct mono_time current_time;

	if ((long)us < 0)
		return -1;

	timer_monotonic_get(&current_time);
	tocb->expiration = current_time;
	mono_time_add_usecs(&tocb->expiration, us);

	/* The expiration overflowed. */
	if (us != 0 && !mono_time_before(&current_time, &tocb->expiration))
		return -1;

	return timer_queue_insert(&global_timer_queue, tocb);
}

int timers_run(void)
{
	struct timeout_callback *tocb;
	struct mono_time current_time;

	timer_monotonic_get(&current_time);
	tocb = timer_queue_expired(&global_timer_queue, &current_time);

	if (tocb != NULL)
		tocb->callback(tocb);

	return !timer_queue_empty(&global_timer_queue);
}
