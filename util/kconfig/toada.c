/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static void print_bool(const char *const name, const bool val)
{
	printf("   %-46s : constant boolean := %s;\n",
	       name, val ? "true" : "false");
}

static void print_hex(const char *const name, const char *val)
{
	unsigned int hexlen;

	printf("   %-46s : constant         := 16#", name);
	for (hexlen = strlen(val); hexlen > 0;) {
		const unsigned int len = hexlen % 4 ? : 4;
		char quad[] = "0000";
		unsigned int i;

		for (i = 0; i < len; ++i)
			quad[4 - len + i] = val[i];
		printf("%s", quad);

		val += len;
		hexlen -= len;
		if (hexlen > 0)
			printf("_");
	}
	printf ("#;\n");
}

static void print_dec(const char *const name, const char *const val)
{
	printf("   %-46s : constant         := %s;\n", name, val);
}

static void print_string(const char *const name, const char *const val)
{
	printf("   %-46s : constant string  := \"%s\";\n", name, val);
}

int main(int argc, char *argv[])
{
	char unset_fmt[256], string_fmt[256], set_fmt[256], line[256];
	char *prefix = "CONFIG", *package = "KConfig";

	if (argc > 3) {
		fprintf(stderr,
			"Usage: %s [<package name> [<config prefix>]]\n\n",
			argv[0]);
		return 1;
	}
	if (argc > 2)
		prefix = argv[2];
	if (argc > 1)
		package = argv[1];

	snprintf(set_fmt, sizeof(set_fmt), "%s_%%255[^=]=%%255s", prefix);
	snprintf(string_fmt, sizeof(string_fmt),
		 "%s_%%255[^=]=\"%%255[^\"]\"", prefix);
	snprintf(unset_fmt, sizeof(unset_fmt),
		 "# %s_%%255s is not set", prefix);

	printf("package %s is\n\n", package);

	while (fgets(line, sizeof(line), stdin)) {
		char name[256], val[256];

		if (line[strlen(line) - 1] != '\n') {
			fprintf(stderr,
				"Line longer than %zu chars, skipping...\n",
				sizeof(line) - 1);
			while (fgets(line, sizeof(line), stdin)) {
				if (line[strlen(line) - 1] == '\n')
					break;
			}
			continue;
		}

		if (sscanf(line, unset_fmt, name) == 1) {
			print_bool(name, false);
			continue;
		}

		if (sscanf(line, string_fmt, name, val) == 2) {
			print_string(name, val);
			continue;
		}

		switch (sscanf(line, set_fmt, name, val)) {
		case 1:
			/* ignore for now, our Kconfig is full of these atm */
			/* fprintf(stderr, "unset non-bool: %s=\n", name); */
			continue;
		case 2:
			if (strcmp(val, "\"\"") == 0) {
				print_string(name, "");
			} else if (strcmp(val, "y") == 0) {
				print_bool(name, true);
			} else if (strncmp(val, "0x", 2) == 0) {
				print_hex(name, val + 2);
			} else if (isdigit(val[0])) {
				print_dec(name, val);
			} else {
				fprintf(stderr,
					"couldn't parse value '%s' for '%s'\n",
					val, name);
			}
			continue;
		default:
			break;
		}

		unsigned int i = 0;
		while (isspace(line[i]))
			++i;
		if (line[i] == '#') {
			printf("   --%s", line + i + 1);
			continue;
		} else if (i == strlen(line)) {
			continue;
		}

		fprintf(stderr, "spurious line:\n%s", line);
	}

	printf("\nend %s;\n", package);
	return 0;
}
