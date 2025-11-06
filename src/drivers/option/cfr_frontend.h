/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_OPTION_CFR_H
#define DRIVERS_OPTION_CFR_H

#include <commonlib/coreboot_tables.h>
#include <commonlib/cfr.h>
#include <string.h>
#include <types.h>

/* Front-end */
struct sm_enum_value {
	const char *ui_name;
	uint32_t value;
};

#define SM_ENUM_VALUE_END	((struct sm_enum_value) {0})

struct sm_obj_enum {
	uint32_t flags;		/* enum cfr_option_flags */
	const char *opt_name;
	const char *ui_name;
	const char *ui_helptext;
	uint32_t default_value;
	const struct sm_enum_value *values;
};

struct sm_obj_number {
	uint32_t flags;		/* enum cfr_option_flags */
	const char *opt_name;
	const char *ui_name;
	const char *ui_helptext;
	uint32_t default_value;
	uint32_t min;
	uint32_t max;
	uint32_t step;
	uint32_t display_flags;	/* enum cfr_numeric_option_display_flags */
};

struct sm_obj_bool {
	uint32_t flags;		/* enum cfr_option_flags */
	const char *opt_name;
	const char *ui_name;
	const char *ui_helptext;
	bool default_value;
};

struct sm_obj_varchar {
	uint32_t flags;		/* enum cfr_option_flags */
	const char *opt_name;
	const char *ui_name;
	const char *ui_helptext;
	const char *default_value;
};

struct sm_obj_comment {
	uint32_t flags;		/* enum cfr_option_flags */
	const char *ui_name;
	const char *ui_helptext;
};

struct sm_object;

struct sm_obj_form {
	uint32_t flags;		/* enum cfr_option_flags */
	const char *ui_name;
	const struct sm_object **obj_list;	/* NULL terminated */
};

enum sm_object_kind {
	SM_OBJ_NONE = 0,
	SM_OBJ_ENUM,
	SM_OBJ_NUMBER,
	SM_OBJ_BOOL,
	SM_OBJ_VARCHAR,
	SM_OBJ_COMMENT,
	SM_OBJ_FORM,
};

struct sm_object {
	enum sm_object_kind kind;
	const struct sm_object *dep;
	const uint32_t *dep_values;
	const uint32_t num_dep_values;
	void (*ctor)(const struct sm_object *obj, struct sm_object *new);	/* Called on object creation */
	union {
		struct sm_obj_enum sm_enum;
		struct sm_obj_number sm_number;
		struct sm_obj_bool sm_bool;
		struct sm_obj_varchar sm_varchar;
		struct sm_obj_comment sm_comment;
		struct sm_obj_form sm_form;
	};
};

/* sm_object helpers with type checking */
#define SM_DECLARE_ENUM(...)	{ .kind = SM_OBJ_ENUM,    .dep = NULL,     \
				  .dep_values = NULL, .num_dep_values = 0, \
				  .ctor = NULL, .sm_enum    = __VA_ARGS__ }
#define SM_DECLARE_NUMBER(...)	{ .kind = SM_OBJ_NUMBER,  .dep = NULL,     \
				  .dep_values = NULL, .num_dep_values = 0, \
				  .ctor = NULL, .sm_number  = __VA_ARGS__ }
#define SM_DECLARE_BOOL(...)	{ .kind = SM_OBJ_BOOL,    .dep = NULL,     \
				  .dep_values = NULL, .num_dep_values = 0, \
				  .ctor = NULL, .sm_bool    = __VA_ARGS__ }
#define SM_DECLARE_VARCHAR(...)	{ .kind = SM_OBJ_VARCHAR, .dep = NULL,     \
				  .dep_values = NULL, .num_dep_values = 0, \
				  .ctor = NULL, .sm_varchar = __VA_ARGS__ }
#define SM_DECLARE_COMMENT(...)	{ .kind = SM_OBJ_COMMENT, .dep = NULL,     \
				  .dep_values = NULL, .num_dep_values = 0, \
				  .ctor = NULL, .sm_comment = __VA_ARGS__ }
#define SM_DECLARE_FORM(...)	{ .kind = SM_OBJ_FORM,    .dep = NULL,     \
				  .dep_values = NULL, .num_dep_values = 0, \
				  .ctor = NULL, .sm_form    = __VA_ARGS__ }

#define WITH_CALLBACK(c) .ctor = (c)
#define WITH_DEP(d) .dep = (d)
#define WITH_DEP_VALUES(d, ...)							\
	.dep = (d),								\
	.dep_values = ((const uint32_t[]) { __VA_ARGS__ }),			\
	.num_dep_values = sizeof((uint32_t[]) { __VA_ARGS__ }) / sizeof(uint32_t)

/*
 * CFR Default Value Override System
 *
 * Mainboards can override default values of CFR objects defined in SoC/common
 * code by declaring an override table. This avoids duplicating object metadata.
 *
 * Usage:
 *   1. Declare overrides in mainboard cfr.c:
 *
 *      const struct cfr_default_override mb_cfr_overrides[] = {
 *          CFR_OVERRIDE_BOOL("s0ix_enable", false),
 *          CFR_OVERRIDE_ENUM("pciexp_aspm", ASPM_DISABLE),
 *          CFR_OVERRIDE_END
 *      };
 *
 *   2. Register the table in mb_cfr_setup_menu():
 *
 *      void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
 *      {
 *          cfr_register_overrides(mb_cfr_overrides);
 *          cfr_write_setup_menu(cfr_root, sm_root);
 *      }
 */

/* Override table entry */
struct cfr_default_override {
	const char *opt_name;
	enum sm_object_kind kind;
	union {
		bool bool_value;
		uint32_t uint_value;
		const char *str_value;
	};
};

/* Helper macros for override table entries */
#define CFR_OVERRIDE_BOOL(name, val)	{ .opt_name = (name), .kind = SM_OBJ_BOOL, .bool_value = (val) }
#define CFR_OVERRIDE_ENUM(name, val)	{ .opt_name = (name), .kind = SM_OBJ_ENUM, .uint_value = (val) }
#define CFR_OVERRIDE_NUMBER(name, val)	{ .opt_name = (name), .kind = SM_OBJ_NUMBER, .uint_value = (val) }
#define CFR_OVERRIDE_VARCHAR(name, val)	{ .opt_name = (name), .kind = SM_OBJ_VARCHAR, .str_value = (val) }
#define CFR_OVERRIDE_END			{ .opt_name = NULL }

/* Register mainboard override table (call before cfr_write_setup_menu) */
void cfr_register_overrides(const struct cfr_default_override *overrides);

void cfr_write_setup_menu(struct lb_cfr *cfr_root, struct sm_obj_form *sm_root[]);

#if ENV_RAMSTAGE
#define __cfr_form __attribute__((used, __section__(".rodata.cfr_forms")))
#else
#define __cfr_form __maybe_unused
#endif

/** start of compile time generated cfr form array */
extern struct sm_obj_form _cfr_forms[];
/** end of compile time generated cfr form array */
extern struct sm_obj_form _ecfr_forms[];

#endif	/* DRIVERS_OPTION_CFR_H */
