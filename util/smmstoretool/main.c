/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <commonlib/bsd/helpers.h>

#include "data.h"
#include "guids.h"
#include "storage.h"
#include "udk2017.h"
#include "vs.h"

struct subcommand_t {
	const char *name;
	const char *description;
	void (*print_help)(FILE *f, const struct subcommand_t *info);
	int (*process)(int argc, char *argv[], const char store_file[]);
};

static void help_get(FILE *f, const struct subcommand_t *info);
static void help_guids(FILE *f, const struct subcommand_t *info);
static void help_help(FILE *f, const struct subcommand_t *info);
static void help_list(FILE *f, const struct subcommand_t *info);
static void help_remove(FILE *f, const struct subcommand_t *info);
static void help_set(FILE *f, const struct subcommand_t *info);
static int process_get(int argc, char *argv[], const char store_file[]);
static int process_guids(int argc, char *argv[], const char store_file[]);
static int process_help(int argc, char *argv[], const char store_file[]);
static int process_list(int argc, char *argv[], const char store_file[]);
static int process_remove(int argc, char *argv[], const char store_file[]);
static int process_set(int argc, char *argv[], const char store_file[]);

static const struct subcommand_t sub_commands[] = {
	{
		.name = "get",
		.description = "display current value of a variable",
		.print_help = &help_get,
		.process = &process_get,
	},
	{
		.name = "guids",
		.description = "show GUID to alias mapping",
		.print_help = &help_guids,
		.process = &process_guids,
	},
	{
		.name = "help",
		.description = "provide built-in help",
		.print_help = &help_help,
		.process = &process_help,
	},
	{
		.name = "list",
		.description = "list variables present in the store",
		.print_help = &help_list,
		.process = &process_list,
	},
	{
		.name = "remove",
		.description = "remove a variable from the store",
		.print_help = &help_remove,
		.process = &process_remove,
	},
	{
		.name = "set",
		.description = "add or updates a variable in the store",
		.print_help = &help_set,
		.process = &process_set,
	},
};

static const int sub_command_count = ARRAY_SIZE(sub_commands);

static const char *USAGE_FMT = "Usage: %s smm-store-file|rom sub-command\n"
			       "       %s -h|--help\n";

static const char *program_name;

static void print_program_usage(void)
{
	fprintf(stderr, USAGE_FMT, program_name, program_name);
	exit(EXIT_FAILURE);
}

static void print_sub_command_usage(const char sub_command[])
{
	fprintf(stderr, "\n");
	fprintf(stderr, USAGE_FMT, program_name, program_name);
	fprintf(stderr, "\n");

	for (int i = 0; i < sub_command_count; ++i) {
		const struct subcommand_t *cmd = &sub_commands[i];
		if (!str_eq(cmd->name, sub_command))
			continue;

		cmd->print_help(stderr, cmd);
		break;
	}

	exit(EXIT_FAILURE);
}

static void print_help(void)
{
	printf(USAGE_FMT, program_name, program_name);

	printf("\n");
	printf("Sub-commands:\n");
	for (int i = 0; i < sub_command_count; ++i) {
		const struct subcommand_t *cmd = &sub_commands[i];
		printf(" * %-6s - %s\n", cmd->name, cmd->description);
	}
}

static void print_types(FILE *f)
{
	fprintf(f, "Types and their values:\n");
	fprintf(f, " * bool (true, false)\n");
	fprintf(f, " * uint8 (0-255)\n");
	fprintf(f, " * uint16 (0-65535)\n");
	fprintf(f, " * uint32 (0-4294967295)\n");
	fprintf(f, " * ascii (NUL-terminated)\n");
	fprintf(f, " * unicode (widened and NUL-terminated)\n");
	fprintf(f, " * raw (output only; raw bytes on output)\n");
}

static void help_set(FILE *f, const struct subcommand_t *info)
{
	fprintf(f, "Create or update a variable:\n");
	fprintf(f, "  %s smm-store-file|rom %s \\\n", program_name, info->name);
	fprintf(f, "      -g vendor-guid \\\n");
	fprintf(f, "      -n variable-name \\\n");
	fprintf(f, "      -t variable-type \\\n");
	fprintf(f, "      -v value\n");
	fprintf(f, "\n");
	print_types(f);
}

static int process_set(int argc, char *argv[], const char store_file[])
{
	const char *name = NULL;
	const char *value = NULL;
	const char *type_str = NULL;
	const char *guid_str = NULL;
	int opt;
	while ((opt = getopt(argc, argv, "n:t:v:g:")) != -1) {
		switch (opt) {
		case 'n':
			name = optarg;
			break;
		case 't':
			type_str = optarg;
			break;
		case 'v':
			value = optarg;
			break;
		case 'g':
			guid_str = optarg;
			break;

		case '?': /* parsing error */
			print_sub_command_usage(argv[0]);
		}
	}

	if (argv[optind] != NULL) {
		fprintf(stderr, "First unexpected positional argument: %s\n",
			argv[optind]);
		print_sub_command_usage(argv[0]);
	}

	if (name == NULL || value == NULL || type_str == NULL ||
	    guid_str == NULL) {
		fprintf(stderr, "All options are required\n");
		print_sub_command_usage(argv[0]);
	}

	if (name[0] == '\0') {
		fprintf(stderr, "Variable name can't be empty\n");
		print_sub_command_usage(argv[0]);
	}

	EFI_GUID guid;
	if (!parse_guid(guid_str, &guid)) {
		fprintf(stderr, "Failed to parse GUID: %s\n", guid_str);
		return EXIT_FAILURE;
	}

	enum data_type type;
	if (!parse_data_type(type_str, &type)) {
		fprintf(stderr, "Failed to parse type: %s\n", type_str);
		return EXIT_FAILURE;
	}

	size_t data_size;
	void *data = make_data(value, &data_size, type);
	if (data == NULL) {
		fprintf(stderr, "Failed to parse value \"%s\" as %s\n",
			value, type_str);
		return EXIT_FAILURE;
	}

	struct storage_t storage;
	if (!storage_open(store_file, &storage, /*rw=*/true)) {
		free(data);
		return EXIT_FAILURE;
	}

	struct var_t *var = vs_find(&storage.vs, name, &guid);
	if (var == NULL) {
		var = vs_new_var(&storage.vs);
		var->name = to_uchars(name, &var->name_size);
		var->data = data;
		var->data_size = data_size;
		var->guid = guid;
	} else {
		free(var->data);
		var->data = data;
		var->data_size = data_size;
	}

	return storage_write_back(&storage) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void help_list(FILE *f, const struct subcommand_t *info)
{
	fprintf(f, "List variables in the store:\n");
	fprintf(f, "  %s smm-store-file|rom %s\n", program_name, info->name);
}

static int process_list(int argc, char *argv[], const char store_file[])
{
	if (argc != 1) {
		fprintf(stderr, "Invalid invocation\n");
		print_sub_command_usage(argv[0]);
	}

	struct storage_t storage;
	if (!storage_open(store_file, &storage, /*rw=*/false))
		return EXIT_FAILURE;

	for (struct var_t *v = storage.vs.vars; v != NULL; v = v->next) {
		char *name = to_chars(v->name, v->name_size);
		char *guid = format_guid(&v->guid, /*use_alias=*/true);

		printf("%-*s:%s (%zu %s)\n", GUID_LEN, guid, name, v->data_size,
		       v->data_size == 1 ? "byte" : "bytes");

		free(name);
		free(guid);
	}

	storage_drop(&storage);
	return EXIT_SUCCESS;
}

static void help_get(FILE *f, const struct subcommand_t *info)
{
	fprintf(f, "Read variable's value:\n");
	fprintf(f, "  %s smm-store-file|rom %s \\\n", program_name, info->name);
	fprintf(f, "      -g vendor-guid \\\n");
	fprintf(f, "      -n variable-name \\\n");
	fprintf(f, "      -t variable-type\n");
	fprintf(f, "\n");
	print_types(f);
}

static int process_get(int argc, char *argv[], const char store_file[])
{
	const char *name = NULL;
	const char *type_str = NULL;
	const char *guid_str = NULL;
	int opt;
	while ((opt = getopt(argc, argv, "n:g:t:")) != -1) {
		switch (opt) {
		case 'n':
			name = optarg;
			break;
		case 'g':
			guid_str = optarg;
			break;
		case 't':
			type_str = optarg;
			break;

		case '?': /* parsing error */
			print_sub_command_usage(argv[0]);
		}
	}

	if (name == NULL || type_str == NULL || guid_str == NULL) {
		fprintf(stderr, "All options are required\n");
		print_sub_command_usage(argv[0]);
	}

	EFI_GUID guid;
	if (!parse_guid(guid_str, &guid)) {
		fprintf(stderr, "Failed to parse GUID: %s\n", guid_str);
		return EXIT_FAILURE;
	}

	enum data_type type;
	if (!parse_data_type(type_str, &type)) {
		fprintf(stderr, "Failed to parse type: %s\n", type_str);
		return EXIT_FAILURE;
	}

	struct storage_t storage;
	if (!storage_open(store_file, &storage, /*rw=*/false))
		return EXIT_FAILURE;

	int result = EXIT_SUCCESS;

	struct var_t *var = vs_find(&storage.vs, name, &guid);
	if (var == NULL) {
		result = EXIT_FAILURE;
		fprintf(stderr, "Couldn't find variable \"%s:%s\"\n",
			guid_str, name);
	} else if (var->data_size == 0) {
		fprintf(stderr, "There is no data to show.\n");
		result = EXIT_FAILURE;
	} else {
		print_data(var->data, var->data_size, type);
	}

	storage_drop(&storage);
	return result;
}

static void help_help(FILE *f, const struct subcommand_t *info)
{
	fprintf(f, "Display generic help:\n");
	fprintf(f, "  %s smm-store-file|rom %s\n", program_name, info->name);
	fprintf(f, "\n");
	fprintf(f, "Display sub-command help:\n");
	fprintf(f, "  %s smm-store-file|rom %s sub-command\n",
		program_name, info->name);
}

static int process_help(int argc, char *argv[], const char store_file[])
{
	(void)store_file;

	if (argc == 1) {
		print_help();
		return EXIT_SUCCESS;
	}

	if (argc != 2) {
		fprintf(stderr, "Invalid invocation\n");
		print_sub_command_usage(argv[0]);
		return EXIT_FAILURE;
	}

	const char *sub_command = argv[1];

	for (int i = 0; i < sub_command_count; ++i) {
		const struct subcommand_t *cmd = &sub_commands[i];
		if (!str_eq(cmd->name, sub_command))
			continue;

		cmd->print_help(stdout, cmd);
		return EXIT_SUCCESS;
	}

	fprintf(stderr, "Unknown sub-command: %s\n", sub_command);
	print_help();
	return EXIT_FAILURE;
}

static void help_remove(FILE *f, const struct subcommand_t *info)
{
	fprintf(f, "Remove a variable:\n");
	fprintf(f, "  %s smm-store-file|rom %s \\\n", program_name, info->name);
	fprintf(f, "      -g vendor-guid \\\n");
	fprintf(f, "      -n variable-name\n");
}

static int process_remove(int argc, char *argv[], const char store_file[])
{
	const char *name = NULL;
	const char *guid_str = NULL;
	int opt;
	while ((opt = getopt(argc, argv, "n:g:")) != -1) {
		switch (opt) {
		case 'n':
			name = optarg;
			break;
		case 'g':
			guid_str = optarg;
			break;

		case '?': /* parsing error */
			print_sub_command_usage(argv[0]);
		}
	}

	if (name == NULL || guid_str == NULL) {
		fprintf(stderr, "All options are required\n");
		print_sub_command_usage(argv[0]);
	}

	EFI_GUID guid;
	if (!parse_guid(guid_str, &guid)) {
		fprintf(stderr, "Failed to parse GUID: %s\n", guid_str);
		return EXIT_FAILURE;
	}

	struct storage_t storage;
	if (!storage_open(store_file, &storage, /*rw=*/true))
		return EXIT_FAILURE;

	int result = EXIT_SUCCESS;

	struct var_t *var = vs_find(&storage.vs, name, &guid);
	if (var == NULL) {
		result = EXIT_FAILURE;
		fprintf(stderr, "Couldn't find variable \"%s:%s\"\n",
			guid_str, name);
	} else {
		vs_delete(&storage.vs, var);
	}

	storage_write_back(&storage);
	return result;
}

static void help_guids(FILE *f, const struct subcommand_t *info)
{
	fprintf(f, "List recognized GUIDS:\n");
	fprintf(f, "  %s smm-store-file|rom %s\n", program_name, info->name);
}

static int process_guids(int argc, char *argv[], const char store_file[])
{
	(void)store_file;

	if (argc != 1) {
		fprintf(stderr, "Invalid invocation\n");
		print_sub_command_usage(argv[0]);
	}

	for (int i = 0; i < known_guid_count; ++i) {
		char *guid = format_guid(&known_guids[i].guid,
					 /*use_alias=*/false);
		printf("%-10s -> %s\n", known_guids[i].alias, guid);
		free(guid);
	}
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	program_name = argv[0];

	if (argc > 1 && (str_eq(argv[1], "-h") || str_eq(argv[1], "--help"))) {
		print_help();
		exit(EXIT_SUCCESS);
	}

	if (argc < 3)
		print_program_usage();

	const char *store_file = argv[1];
	const char *sub_command = argv[2];

	int sub_command_argc = argc - 2;
	char **sub_command_argv = argv + 2;

	for (int i = 0; i < sub_command_count; ++i) {
		const struct subcommand_t *cmd = &sub_commands[i];
		if (!str_eq(cmd->name, sub_command))
			continue;

		return cmd->process(sub_command_argc,
				    sub_command_argv,
				    store_file);
	}

	fprintf(stderr, "Unknown sub-command: %s\n", sub_command);
	print_help();
	return EXIT_FAILURE;
}
