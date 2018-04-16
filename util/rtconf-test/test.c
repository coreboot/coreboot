#include <stdio.h>
#include <stdlib.h>

#include "bios_knobs.h"

#define Default_source "bootorder"
char *source = Default_source;

char *cbfs_boot_map_with_leak (const char *filename, int type, size_t *o_size) {
    FILE *i = fopen (filename, "r");
    if (i == NULL)
        return NULL;

    fseek (i, 0, SEEK_END);
    *o_size = ftell(i);
    fseek (i, 0, SEEK_SET);

    char *r = malloc (*o_size);
    fread (r, *o_size, 1, i);
    fclose (i);

    return r;
}

int main (int argc, char **argv) {
    if (argc >= 2)
        source = argv[1];

#include "cases.c"

    return 0;
}

