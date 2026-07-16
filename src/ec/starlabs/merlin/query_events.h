/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_STARLABS_MERLIN_QUERY_EVENTS_H
#define EC_STARLABS_MERLIN_QUERY_EVENTS_H

#include <stdint.h>

enum starlabs_ec_query_action_type {
	STARLABS_EC_QUERY_NONE,
	STARLABS_EC_QUERY_LID,
	STARLABS_EC_QUERY_NOTIFY,
	STARLABS_EC_QUERY_HID,
	STARLABS_EC_QUERY_DEBUG,
	STARLABS_EC_QUERY_SPC,
};

struct starlabs_ec_query_action {
	enum starlabs_ec_query_action_type type;
	const char *path;
	uint64_t value;
	uint64_t value2;
};

struct starlabs_ec_query_event {
	const char *method;
	struct starlabs_ec_query_action actions[2];
};

#define EC_QUERY_LID {.type = STARLABS_EC_QUERY_LID}
#define EC_QUERY_NOTIFY(path_, value_) \
	{.type = STARLABS_EC_QUERY_NOTIFY, .path = (path_), .value = (value_)}
#define EC_QUERY_HID(value_)     {.type = STARLABS_EC_QUERY_HID, .value = (value_)}
#define EC_QUERY_DEBUG(message_) {.type = STARLABS_EC_QUERY_DEBUG, .path = (message_)}
#define EC_QUERY_SPC(value_, value2_) \
	{.type = STARLABS_EC_QUERY_SPC, .value = (value_), .value2 = (value2_)}

#define EC_QUERY_EVENT0(method_) {.method = (method_)}
#define EC_QUERY_EVENT1(method_, action_)                   \
	{                                                   \
		.method = (method_), .actions = { action_ } \
	}
#define EC_QUERY_EVENT2(method_, action_, action2_)                   \
	{                                                             \
		.method = (method_), .actions = { action_, action2_ } \
	}
#define EC_QUERY_EVENT_END {.method = 0}

#endif
