#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t u8;

#define CBFS_TYPE_RAW 0
#define printk(LEVEL, ...) printf (__VA_ARGS__);
#define BIOS_EMERG 0
#define BIOS_ALERT 0

char *cbfs_boot_map_with_leak (const char *filename, int type, size_t *o_size);
