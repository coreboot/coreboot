/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boot/coreboot_tables.h>
#include <commonlib/coreboot_tables.h>
#include <console/console.h>
#include <crc_byte.h>
#include <drivers/option/cfr_frontend.h>
#include <inttypes.h>
#include <string.h>
#include <types.h>

static uint32_t cfr_record_size(const char *startp, const char *endp)
{
	const uintptr_t start = (uintptr_t)startp;
	const uintptr_t end = (uintptr_t)endp;

	if (start > end || end - start > UINT32_MAX) {
		/*
		 * Should never be reached unless something went really
		 * wrong. Record size can never be negative, and things
		 * would break long before record length exceeds 4 GiB.
		 */
		die("%s: bad record size (start = %" PRIxPTR ", end = %" PRIxPTR ")",
			__func__, start, end);
	}
	return (uint32_t)(end - start);
}

static uint32_t write_cfr_varchar(char *current, const char *string, uint32_t tag)
{
	ASSERT(string);
	if (!string)
		return 0;

	struct lb_cfr_varbinary *cfr_str = (struct lb_cfr_varbinary *)current;
	cfr_str->tag = tag;
	cfr_str->data_length = strlen(string) + 1;
	char *data = current + sizeof(*cfr_str);
	memcpy(data, string, cfr_str->data_length);

	/* Make sure that every TAG/SIZE field is always aligned to LB_ENTRY_ALIGN */
	cfr_str->size = ALIGN_UP(sizeof(*cfr_str) + cfr_str->data_length, LB_ENTRY_ALIGN);

	return cfr_str->size;
}

static uint32_t sm_write_string_default_value(char *current, const char *string)
{
	return write_cfr_varchar(current, string ? string : "", CFR_TAG_VARCHAR_DEF_VALUE);
}

static uint32_t sm_write_opt_name(char *current, const char *string)
{
	return write_cfr_varchar(current, string, CFR_TAG_VARCHAR_OPT_NAME);
}

static uint32_t sm_write_ui_name(char *current, const char *string)
{
	return write_cfr_varchar(current, string, CFR_TAG_VARCHAR_UI_NAME);
}

static uint32_t sm_write_ui_helptext(char *current, const char *string)
{
	/* UI Helptext is optional, return if nothing to display */
	if (!string || !strlen(string))
		return 0;

	return write_cfr_varchar(current, string, CFR_TAG_VARCHAR_UI_HELPTEXT);
}

static uint32_t sm_write_dep_values(char *current,
				    const uint32_t *dep_values, const uint32_t num_dep_values)
{
	/* Dependency values are optional */
	if (!dep_values || !num_dep_values)
		return 0;

	struct lb_cfr_varbinary *cfr_values = (struct lb_cfr_varbinary *)current;
	cfr_values->tag = CFR_TAG_DEP_VALUES;
	cfr_values->data_length = sizeof(*dep_values) * num_dep_values;
	char *data = current + sizeof(*cfr_values);
	memcpy(data, dep_values, cfr_values->data_length);

	/* Make sure that every TAG/SIZE field is always aligned to LB_ENTRY_ALIGN */
	cfr_values->size = ALIGN_UP(sizeof(*cfr_values) + cfr_values->data_length, LB_ENTRY_ALIGN);

	return cfr_values->size;
}

static uint32_t sm_write_enum_value(char *current, const struct sm_enum_value *e)
{
	struct lb_cfr_enum_value *enum_val = (struct lb_cfr_enum_value *)current;
	enum_val->tag = CFR_TAG_ENUM_VALUE;
	enum_val->value = e->value;
	enum_val->size = sizeof(*enum_val);

	current += enum_val->size;
	current += sm_write_ui_name(current, e->ui_name);

	enum_val->size = cfr_record_size((char *)enum_val, current);
	return enum_val->size;
}

static uint32_t write_numeric_option(char *current, uint32_t tag, const uint64_t object_id,
		const char *opt_name, const char *ui_name, const char *ui_helptext,
		uint32_t flags, uint32_t default_value, const struct sm_enum_value *values,
		const uint64_t dep_id, const uint32_t *dep_values, const uint32_t num_dep_values)
{
	struct lb_cfr_numeric_option *option = (struct lb_cfr_numeric_option *)current;
	size_t len;

	option->tag = tag;
	option->object_id = object_id;
	option->dependency_id = dep_id;
	option->flags = flags;
	if (option->flags & (CFR_OPTFLAG_INACTIVE | CFR_OPTFLAG_VOLATILE))
		option->flags |= CFR_OPTFLAG_READONLY;
	option->default_value = default_value;
	option->size = sizeof(*option);

	current += option->size;
	len = sm_write_opt_name(current, opt_name);
	if (!len)
		return 0;
	current += len;
	len = sm_write_ui_name(current, ui_name);
	if (!len)
		return 0;
	current += len;
	current += sm_write_ui_helptext(current, ui_helptext);
	current += sm_write_dep_values(current, dep_values, num_dep_values);

	if (option->tag == CFR_TAG_OPTION_ENUM && values) {
		for (const struct sm_enum_value *e = values; e->ui_name; e++) {
			current += sm_write_enum_value(current, e);
		}
	}

	option->size = cfr_record_size((char *)option, current);
	return option->size;
}

static uint32_t sm_write_opt_enum(char *current, const struct sm_obj_enum *sm_enum,
				  const uint64_t object_id, const uint64_t dep_id,
				  const uint32_t *dep_values, const uint32_t num_dep_values)

{
	return write_numeric_option(current, CFR_TAG_OPTION_ENUM, object_id,
			sm_enum->opt_name, sm_enum->ui_name, sm_enum->ui_helptext,
			sm_enum->flags, sm_enum->default_value, sm_enum->values,
			dep_id, dep_values, num_dep_values);
}

static uint32_t sm_write_opt_number(char *current, const struct sm_obj_number *sm_number,
				    const uint64_t object_id, const uint64_t dep_id,
				    const uint32_t *dep_values, const uint32_t num_dep_values)

{
	return write_numeric_option(current, CFR_TAG_OPTION_NUMBER, object_id,
			sm_number->opt_name, sm_number->ui_name, sm_number->ui_helptext,
			sm_number->flags, sm_number->default_value, NULL, dep_id,
			dep_values, num_dep_values);
}

static uint32_t sm_write_opt_bool(char *current, const struct sm_obj_bool *sm_bool,
				  const uint64_t object_id, const uint64_t dep_id,
				  const uint32_t *dep_values, const uint32_t num_dep_values)

{
	return write_numeric_option(current, CFR_TAG_OPTION_BOOL, object_id,
			sm_bool->opt_name, sm_bool->ui_name, sm_bool->ui_helptext,
			sm_bool->flags, sm_bool->default_value, NULL, dep_id,
			dep_values, num_dep_values);
}

static uint32_t sm_write_opt_varchar(char *current, const struct sm_obj_varchar *sm_varchar,
				     const uint64_t object_id, const uint64_t dep_id,
				     const uint32_t *dep_values, const uint32_t num_dep_values)

{
	struct lb_cfr_varchar_option *option = (struct lb_cfr_varchar_option *)current;
	size_t len;

	option->tag = CFR_TAG_OPTION_VARCHAR;
	option->object_id = object_id;
	option->dependency_id = dep_id;
	option->flags = sm_varchar->flags;
	if (option->flags & (CFR_OPTFLAG_INACTIVE | CFR_OPTFLAG_VOLATILE))
		option->flags |= CFR_OPTFLAG_READONLY;
	option->size = sizeof(*option);

	current += option->size;
	current += sm_write_string_default_value(current, sm_varchar->default_value);
	len = sm_write_opt_name(current, sm_varchar->opt_name);
	if (!len)
		return 0;
	current += len;
	len = sm_write_ui_name(current, sm_varchar->ui_name);
	if (!len)
		return 0;
	current += len;
	current += sm_write_ui_helptext(current, sm_varchar->ui_helptext);
	current += sm_write_dep_values(current, dep_values, num_dep_values);

	option->size = cfr_record_size((char *)option, current);
	return option->size;
}

static uint32_t sm_write_opt_comment(char *current, const struct sm_obj_comment *sm_comment,
				     const uint32_t object_id, const uint32_t dep_id,
				     const uint32_t *dep_values, const uint32_t num_dep_values)
{
	struct lb_cfr_option_comment *comment = (struct lb_cfr_option_comment *)current;
	size_t len;

	comment->tag = CFR_TAG_OPTION_COMMENT;
	comment->object_id = object_id;
	comment->dependency_id = dep_id;
	comment->flags = sm_comment->flags;
	if (comment->flags & (CFR_OPTFLAG_INACTIVE | CFR_OPTFLAG_VOLATILE))
		comment->flags |= CFR_OPTFLAG_READONLY;
	comment->size = sizeof(*comment);

	current += comment->size;
	len = sm_write_ui_name(current, sm_comment->ui_name);
	if (!len)
		return 0;
	current += len;
	current += sm_write_ui_helptext(current, sm_comment->ui_helptext);
	current += sm_write_dep_values(current, dep_values, num_dep_values);

	comment->size = cfr_record_size((char *)comment, current);
	return comment->size;
}

static uint64_t sm_gen_obj_id(void *ptr)
{
	uintptr_t id = (uintptr_t)ptr;
	/* Convert pointer to unique ID */
	return id ^ 0xffffcafecafecafe;
}

static uint32_t sm_write_object(char *current, const struct sm_object *sm_obj);

static uint32_t sm_write_form(char *current, struct sm_obj_form *sm_form,
			      const uint64_t object_id, const uint64_t dep_id,
			      const uint32_t *dep_values, const uint32_t num_dep_values)
{
	struct lb_cfr_option_form *form = (struct lb_cfr_option_form *)current;
	size_t len;
	size_t i = 0;

	form->tag = CFR_TAG_OPTION_FORM;
	form->object_id = object_id;
	form->dependency_id = dep_id;
	form->flags = sm_form->flags;
	if (form->flags & (CFR_OPTFLAG_INACTIVE | CFR_OPTFLAG_VOLATILE))
		form->flags |= CFR_OPTFLAG_READONLY;
	form->size = sizeof(*form);

	current += form->size;
	len = sm_write_ui_name(current, sm_form->ui_name);
	if (!len)
		return 0;
	current += len;
	current += sm_write_dep_values(current, dep_values, num_dep_values);

	while (sm_form->obj_list[i])
		current += sm_write_object(current, sm_form->obj_list[i++]);

	form->size = cfr_record_size((char *)form, current);
	return form->size;
}

static uint32_t sm_write_object(char *current, const struct sm_object *sm_obj)
{
	uint64_t dep_id, obj_id;
	const uint32_t *dep_values;
	uint32_t num_dep_values;
	struct sm_object sm_obj_copy;
	assert(sm_obj);

	/* Assign uniqueue ID */
	obj_id = sm_gen_obj_id((void *)sm_obj);

	/* Set dependency ID */
	dep_id = 0;
	dep_values = NULL;
	num_dep_values = 0;
	if (sm_obj->dep) {
		if (sm_obj->dep->kind == SM_OBJ_BOOL || sm_obj->dep->kind == SM_OBJ_ENUM) {
			dep_id = sm_gen_obj_id((void *)sm_obj->dep);
			dep_values = sm_obj->dep_values;
			num_dep_values = sm_obj->num_dep_values;
		}
	}

	/* Invoke callback to update fields */
	if (sm_obj->ctor) {
		memcpy(&sm_obj_copy, sm_obj, sizeof(*sm_obj));
		sm_obj->ctor(sm_obj, &sm_obj_copy);

		assert(sm_obj->kind == sm_obj_copy.kind);
		sm_obj = (const struct sm_object *)&sm_obj_copy;
	}

	switch (sm_obj->kind) {
	case SM_OBJ_NONE:
		return 0;
	case SM_OBJ_ENUM:
		return sm_write_opt_enum(current, &sm_obj->sm_enum, obj_id,
					 dep_id, dep_values, num_dep_values);
	case SM_OBJ_NUMBER:
		return sm_write_opt_number(current, &sm_obj->sm_number, obj_id,
					   dep_id, dep_values, num_dep_values);
	case SM_OBJ_BOOL:
		return sm_write_opt_bool(current, &sm_obj->sm_bool, obj_id,
					 dep_id, dep_values, num_dep_values);
	case SM_OBJ_VARCHAR:
		return sm_write_opt_varchar(current, &sm_obj->sm_varchar, obj_id,
					    dep_id, dep_values, num_dep_values);
	case SM_OBJ_COMMENT:
		return sm_write_opt_comment(current, &sm_obj->sm_comment, obj_id,
					    dep_id, dep_values, num_dep_values);
	case SM_OBJ_FORM:
		return sm_write_form(current, (struct sm_obj_form *)&sm_obj->sm_form, obj_id,
				     dep_id, dep_values, num_dep_values);
	default:
		BUG();
		printk(BIOS_ERR, "Unknown setup menu object kind %u, ignoring\n", sm_obj->kind);
		return 0;
	}
}

void cfr_write_setup_menu(struct lb_cfr *cfr_root, struct sm_obj_form *sm_root[])
{
	void *current = cfr_root;
	struct sm_obj_form *obj;
	size_t i = 0;

	ASSERT(cfr_root);
	if (!cfr_root)
		return;

	cfr_root->tag = LB_TAG_CFR_ROOT;
	cfr_root->size = sizeof(*cfr_root);
	cfr_root->version = CFR_VERSION;

	current += cfr_root->size;
	while (sm_root && sm_root[i])
		current += sm_write_form(current, sm_root[i++], 0, 0, NULL, 0);

	/*
	 * Add generic forms.
	 */
	for (obj = &_cfr_forms[0]; obj != &_ecfr_forms[0]; obj++)
		current += sm_write_form(current, obj, 0, 0, NULL, 0);

	cfr_root->size = cfr_record_size((char *)cfr_root, current);

	cfr_root->checksum = CRC(cfr_root + 1, cfr_root->size - sizeof(*cfr_root), crc32_byte);

	printk(BIOS_DEBUG, "CFR: Written %u bytes of CFR structures at %p, with CRC32 0x%08x\n",
		cfr_root->size, cfr_root, cfr_root->checksum);
}
