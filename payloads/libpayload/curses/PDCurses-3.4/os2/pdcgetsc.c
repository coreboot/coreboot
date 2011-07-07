/* Public Domain Curses */

#include "pdcos2.h"

RCSID("$Id: pdcgetsc.c,v 1.39 2008/07/14 04:24:51 wmcbrine Exp $")

/* return width of screen/viewport */

int PDC_get_columns(void)
{
#ifdef EMXVIDEO
    int rows = 0;
#else
    VIOMODEINFO modeInfo = {0};
#endif
    int cols = 0;
    const char *env_cols;

    PDC_LOG(("PDC_get_columns() - called\n"));

#ifdef EMXVIDEO
    v_dimen(&cols, &rows);
#else
    modeInfo.cb = sizeof(modeInfo);
    VioGetMode(&modeInfo, 0);
    cols = modeInfo.col;
#endif
    env_cols = getenv("COLS");

    if (env_cols)
        cols = min(atoi(env_cols), cols);

    PDC_LOG(("PDC_get_columns() - returned: cols %d\n", cols));

    return cols;
}

/* get the cursor size/shape */

int PDC_get_cursor_mode(void)
{
#ifdef EMXVIDEO
    int curstart = 0, curend = 0;
#else
    VIOCURSORINFO cursorInfo;
#endif
    PDC_LOG(("PDC_get_cursor_mode() - called\n"));

#ifdef EMXVIDEO
    v_getctype(&curstart, &curend);
    return (curstart << 8) | curend;
#else
    VioGetCurType (&cursorInfo, 0);

    return (cursorInfo.yStart << 8) | cursorInfo.cEnd;
#endif
}

/* return number of screen rows */

int PDC_get_rows(void)
{
#ifdef EMXVIDEO
    int cols = 0;
#else
    VIOMODEINFO modeInfo = {0};
#endif
    int rows = 0;
    const char *env_rows;

    PDC_LOG(("PDC_get_rows() - called\n"));

    /* use the value from LINES environment variable, if set. MH 10-Jun-92 */
    /* and use the minimum of LINES and *ROWS.                MH 18-Jun-92 */

#ifdef EMXVIDEO
    v_dimen(&cols, &rows);
#else
    modeInfo.cb = sizeof(modeInfo);
    VioGetMode(&modeInfo, 0);
    rows = modeInfo.row;
#endif
    env_rows = getenv("LINES");

    if (env_rows)
        rows = min(atoi(env_rows), rows);

    PDC_LOG(("PDC_get_rows() - returned: rows %d\n", rows));

    return rows;
}
