#define USE_RCS_IDS 0
#define USE_WIDEC_SUPPORT 0
#define NCURSES_SP_FUNCS 0
#define DECL_ERRNO 0
#define NCURSES_INTEROP_FUNCS 0

#define NCURSES_API
#define NCURSES_IMPEXP
#define NCURSES_INLINE
#define NCURSES_SP_DCLx
#define NCURSES_SP_NAME(x) x
#define NCURSES_EXPORT_VAR(x) x
#define NCURSES_EXPORT(x) x

#define SP_PARM SP
#define CURRENT_SCREEN SP
#define StdScreen(x) stdscr

#define T(x)
#define T_CALLED(x...)

#define GCC_UNUSED __attribute__((unused))

#define BLANK           (' '|A_NORMAL)
#define ZEROS           ('\0'|A_NORMAL)
#define ISBLANK	isblank

#define ChCharOf(c)	((c) & (chtype)A_CHARTEXT)
#define CharOf(c)	ChCharOf(c)

#define WINDOW_ATTRS(x) ((x)->_attrs)
#define IsValidScreen(x) 1

#define returnCode return
#define returnWin return
#define returnCPtr return
#define returnVoidPtr return
#define returnPtr return
#define returnAttr return
#define returnBool return
