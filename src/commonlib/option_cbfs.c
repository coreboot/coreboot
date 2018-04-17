#include <cbfs.h>
#include <string.h>
#include <stdint.h>
#include <program_loading.h>
#include <types.h>

#include <commonlib/cbfs.h>
#include <commonlib/region.h>
#include <commonlib/cbfs_serialized.h>
#include <console/console.h>

#define BOOTORDER_FILE "bootorder"

static char *after_str(const char *s, const char *pattern)
{
    size_t pattern_l = strlen (pattern);
    while ((s = strchr (s, pattern[0])) != NULL) {
        if (strncmp (s, pattern, pattern_l) == 0)
            return (char*)s+pattern_l;
        s++;
    }

	return NULL;
}

#define map_cbfs_file(...) cbfs_boot_map_with_leak (__VA_ARGS__);
#define fail(...) {printk (__VA_ARGS__); return CB_CMOS_OPTION_NOT_FOUND;}

enum cb_err get_option(void *dest, const char *name)
{
	const char *boot_file = NULL;
	size_t boot_file_len = 0;
	char * token = NULL;

	boot_file = map_cbfs_file (BOOTORDER_FILE, CBFS_TYPE_RAW, &boot_file_len);
	if (boot_file == NULL)
		fail(BIOS_ALERT, "file [%s] not found in CBFS\n", BOOTORDER_FILE);
	if (boot_file_len < 4096)
		fail(BIOS_ALERT, "Missing bootorder data.\n");

	if ((token = after_str (boot_file, name)) != NULL) {
        *((u8*)dest) = *token;
        return CB_SUCCESS;
    }

	return CB_CMOS_OPTION_NOT_FOUND;
}

