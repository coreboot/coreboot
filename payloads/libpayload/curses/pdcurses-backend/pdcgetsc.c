/* Public Domain Curses */
/* This file is BSD licensed, Copyright 2011 secunet AG */

#include "lppdc.h"

#include <stdlib.h>

/* return width of screen/viewport */

int PDC_get_columns(void)
{
    int cols;

    PDC_LOG(("PDC_get_columns() - called\n"));

    /* hardcode to 80 */
    cols = 80;

    PDC_LOG(("PDC_get_columns() - returned: cols %d\n", cols));

    return cols;
}

/* get the cursor size/shape */

int PDC_get_cursor_mode(void)
{
    PDC_LOG(("PDC_get_cursor_mode() - called\n"));

    /* only have one cursor type */
    return SP->visibility;
}

/* return number of screen rows */

int PDC_get_rows(void)
{
    int rows;

    PDC_LOG(("PDC_get_rows() - called\n"));

    /* hardcode to 25 */
    rows = 25;

    PDC_LOG(("PDC_get_rows() - returned: rows %d\n", rows));

    return rows;
}
