/* SPDX-License-Identifier: BSD-3-Clause or GPL-2.0-only */

#include <stdlib.h>
#include <stdarg.h>

#include "kv_pair.h"

/* Internal variable for output style. Use accessors to get/set style. */
static enum kv_pair_style _style;

void kv_pair_set_style(enum kv_pair_style style)
{
	_style = style;
}

enum kv_pair_style kv_pair_get_style(void)
{
	return _style;
}

struct kv_pair *kv_pair_new(void)
{
	struct kv_pair *kv;

	kv = calloc(1, sizeof(*kv));
	if (!kv)
		return NULL;

	return kv;
}

struct kv_pair *kv_pair_add(struct kv_pair *kv_list,
			    const char *key, const char *value)
{
	struct kv_pair *kv_new;
	struct kv_pair *kv_ptr;

	kv_new = kv_pair_new();
	if (!kv_new)
		return NULL;

	/* save key=value strings if provided */
	if (key) {
		kv_new->key = strdup(key);
		if (!kv_new->key)
			goto kv_pair_add_failed;
	}
	if (value) {
		kv_new->value = strdup(value);
		if (!kv_new->value)
			goto kv_pair_add_failed;
	}

	/* first in the list if no list provided */
	if (kv_list) {
		/* find the end of list */
		for (kv_ptr = kv_list; kv_ptr->next != NULL;
		     kv_ptr = kv_ptr->next)
			;

		/* link in the new pair at the end */
		kv_ptr->next = kv_new;
	}

	/* return pointer to the new pair */
	return kv_new;

kv_pair_add_failed:
	kv_pair_free(kv_new);
	return NULL;
}

struct kv_pair *kv_pair_add_bool(struct kv_pair *kv_list,
				 const char *key, int value)
{
	const char *str;

	if (value) {
		str = "yes";
	} else {
		str = "no";
	}
	return kv_pair_add(kv_list, key, str);
}

struct kv_pair *kv_pair_fmt(struct kv_pair *kv_list,
			    const char *kv_key, const char *format, ...)
{
	char kv_value[KV_PAIR_MAX_VALUE_LEN];
	va_list vptr;

	memset(kv_value, 0, sizeof(kv_value));

	va_start(vptr, format);
	vsnprintf(kv_value, sizeof(kv_value), format, vptr);
	va_end(vptr);

	return kv_pair_add(kv_list, kv_key, kv_value);
}

void kv_pair_free(struct kv_pair *kv_list)
{
	struct kv_pair *kv_ptr = kv_list;
	struct kv_pair *kv_next;

	while (kv_ptr != NULL) {
		/* free key/value strings */
		if (kv_ptr->key)
			free(kv_ptr->key);
		if (kv_ptr->value)
			free(kv_ptr->value);

		/* free current pair move to next */
		kv_next = kv_ptr->next;
		free(kv_ptr);
		kv_ptr = kv_next;
	}
}

void kv_pair_print_to_file(FILE* fp, struct kv_pair *kv_list,
			   enum kv_pair_style style)
{
	struct kv_pair *kv_ptr;

	switch (style) {
	case KV_STYLE_PAIR:
		for (kv_ptr = kv_list; kv_ptr != NULL; kv_ptr = kv_ptr->next) {
			if (kv_ptr->key && kv_ptr->value) {
				fprintf(fp, "%s=\"%s\" ",
					kv_ptr->key, kv_ptr->value);
			}
		}
		break;

	case KV_STYLE_VALUE:
		for (kv_ptr = kv_list; kv_ptr != NULL; kv_ptr = kv_ptr->next) {
			if (kv_ptr->value) {
				fprintf(fp, "%s", kv_ptr->value);
				if (kv_ptr->next)
					fprintf(fp, " | ");
			}
		}
		break;

	case KV_STYLE_LONG:
		for (kv_ptr = kv_list; kv_ptr != NULL; kv_ptr = kv_ptr->next) {
			if (kv_ptr->key && kv_ptr->value)
				fprintf(fp, "%-20s | %s\n",
					kv_ptr->key, kv_ptr->value);
		}
		break;
	}

	fprintf(fp, "\n");
}

void kv_pair_print(struct kv_pair *kv_list)
{
	kv_pair_print_to_file(stdout, kv_list, kv_pair_get_style());
}

const char *kv_pair_get_value(struct kv_pair *kv_list, const char *kv_key)
{
	const char *kv_value = NULL;
	struct kv_pair *kv_ptr;

	for (kv_ptr = kv_list; kv_ptr != NULL; kv_ptr = kv_ptr->next) {
		if (kv_ptr->key && strcmp(kv_ptr->key, kv_key) == 0) {
			kv_value = kv_ptr->value;
			break;
		}
	}
	return kv_value;
}

int kv_pair_size(struct kv_pair *kv_list) {
	struct kv_pair *kv_ptr;
	int count;

	count = 0;
	for (kv_ptr = kv_list; kv_ptr != NULL; kv_ptr = kv_ptr->next) {
		if (kv_ptr->key) {
			count++;
		}
	}
	return count;
}
