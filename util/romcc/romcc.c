#undef VERSION_MAJOR
#undef VERSION_MINOR
#undef RELEASE_DATE
#undef VERSION
#define VERSION_MAJOR "0"
#define VERSION_MINOR "80"
#define RELEASE_DATE "18 November 2015"
#define VERSION VERSION_MAJOR "." VERSION_MINOR

#include <stdarg.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <time.h>

#define MAX_CWD_SIZE 4096
#define MAX_ALLOCATION_PASSES 100

/* NOTE: Before you even start thinking to touch anything
 * in this code, set DEBUG_ROMCC_WARNINGS to 1 to get an
 * insight on the original author's thoughts. We introduced
 * this switch as romcc was about the only thing producing
 * massive warnings in our code..
 */
#define DEBUG_ROMCC_WARNINGS 0

#define DEBUG_CONSISTENCY 1
#define DEBUG_SDP_BLOCKS 0
#define DEBUG_TRIPLE_COLOR 0

#define DEBUG_DISPLAY_USES 1
#define DEBUG_DISPLAY_TYPES 1
#define DEBUG_REPLACE_CLOSURE_TYPE_HIRES 0
#define DEBUG_DECOMPOSE_PRINT_TUPLES 0
#define DEBUG_DECOMPOSE_HIRES  0
#define DEBUG_INITIALIZER 0
#define DEBUG_UPDATE_CLOSURE_TYPE 0
#define DEBUG_LOCAL_TRIPLE 0
#define DEBUG_BASIC_BLOCKS_VERBOSE 0
#define DEBUG_CPS_RENAME_VARIABLES_HIRES 0
#define DEBUG_SIMPLIFY_HIRES 0
#define DEBUG_SHRINKING 0
#define DEBUG_COALESCE_HITCHES 0
#define DEBUG_CODE_ELIMINATION 0

#define DEBUG_EXPLICIT_CLOSURES 0

#if DEBUG_ROMCC_WARNINGS
#warning "FIXME give clear error messages about unused variables"
#warning "FIXME properly handle multi dimensional arrays"
#warning "FIXME handle multiple register sizes"
#endif

/*  Control flow graph of a loop without goto.
 *
 *        AAA
 *   +---/
 *  /
 * / +--->CCC
 * | |    / \
 * | |  DDD EEE    break;
 * | |    \    \
 * | |    FFF   \
 *  \|    / \    \
 *   |\ GGG HHH   |   continue;
 *   | \  \   |   |
 *   |  \ III |  /
 *   |   \ | /  /
 *   |    vvv  /
 *   +----BBB /
 *         | /
 *         vv
 *        JJJ
 *
 *
 *             AAA
 *     +-----+  |  +----+
 *     |      \ | /     |
 *     |       BBB  +-+ |
 *     |       / \ /  | |
 *     |     CCC JJJ / /
 *     |     / \    / /
 *     |   DDD EEE / /
 *     |    |   +-/ /
 *     |   FFF     /
 *     |   / \    /
 *     | GGG HHH /
 *     |  |   +-/
 *     | III
 *     +--+
 *
 *
 * DFlocal(X) = { Y <- Succ(X) | idom(Y) != X }
 * DFup(Z)    = { Y <- DF(Z) | idom(Y) != X }
 *
 *
 * [] == DFlocal(X) U DF(X)
 * () == DFup(X)
 *
 * Dominator graph of the same nodes.
 *
 *           AAA     AAA: [ ] ()
 *          /   \
 *        BBB    JJJ BBB: [ JJJ ] ( JJJ )  JJJ: [ ] ()
 *         |
 *        CCC        CCC: [ ] ( BBB, JJJ )
 *        / \
 *     DDD   EEE     DDD: [ ] ( BBB ) EEE: [ JJJ ] ()
 *      |
 *     FFF           FFF: [ ] ( BBB )
 *     / \
 *  GGG   HHH        GGG: [ ] ( BBB ) HHH: [ BBB ] ()
 *   |
 *  III              III: [ BBB ] ()
 *
 *
 * BBB and JJJ are definitely the dominance frontier.
 * Where do I place phi functions and how do I make that decision.
 *
 */

struct filelist {
	const char *filename;
	struct filelist *next;
};

struct filelist *include_filelist = NULL;

static void __attribute__((noreturn)) die(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stdout);
	fflush(stderr);
	exit(1);
}

static void *xmalloc(size_t size, const char *name)
{
	void *buf;
	buf = malloc(size);
	if (!buf) {
		die("Cannot malloc %ld bytes to hold %s: %s\n",
			size + 0UL, name, strerror(errno));
	}
	return buf;
}

static void *xcmalloc(size_t size, const char *name)
{
	void *buf;
	buf = xmalloc(size, name);
	memset(buf, 0, size);
	return buf;
}

static void *xrealloc(void *ptr, size_t size, const char *name)
{
	void *buf;
	buf = realloc(ptr, size);
	if (!buf) {
		die("Cannot realloc %ld bytes to hold %s: %s\n",
			size + 0UL, name, strerror(errno));
	}
	return buf;
}

static void xfree(const void *ptr)
{
	free((void *)ptr);
}

static char *xstrdup(const char *str)
{
	char *new;
	int len;
	len = strlen(str);
	new = xmalloc(len + 1, "xstrdup string");
	memcpy(new, str, len);
	new[len] = '\0';
	return new;
}

static void xchdir(const char *path)
{
	if (chdir(path) != 0) {
		die("chdir to `%s' failed: %s\n",
			path, strerror(errno));
	}
}

static int exists(const char *dirname, const char *filename)
{
	char cwd[MAX_CWD_SIZE];
	int does_exist;

	if (getcwd(cwd, sizeof(cwd)) == 0) {
		die("cwd buffer to small");
	}

	does_exist = 1;
	if (chdir(dirname) != 0) {
		does_exist = 0;
	}
	if (does_exist && (access(filename, O_RDONLY) < 0)) {
		if ((errno != EACCES) && (errno != EROFS)) {
			does_exist = 0;
		}
	}
	xchdir(cwd);
	return does_exist;
}

static off_t get_file_size(FILE *f)
{
	struct stat s;
	int fd = fileno(f);
	if (fd == -1) return -1;
	if (fstat(fd, &s) == -1) return -1;
	return s.st_size;
}

static char *slurp_file(const char *dirname, const char *filename, off_t *r_size)
{
	char cwd[MAX_CWD_SIZE];
	char *buf;
	off_t size, progress;
	ssize_t result;
	FILE* file;

	if (!filename) {
		*r_size = 0;
		return 0;
	}
	if (getcwd(cwd, sizeof(cwd)) == 0) {
		die("cwd buffer to small");
	}
	xchdir(dirname);
	file = fopen(filename, "rb");
	xchdir(cwd);
	if (file == NULL) {
		die("Cannot open '%s' : %s\n",
			filename, strerror(errno));
	}
	size = get_file_size(file);
	if (size == -1) {
		die("Could not fetch size of '%s': %s\n", filename, strerror(errno));
	}
	*r_size = size +1;
	buf = xmalloc(size +2, filename);
	buf[size] = '\n'; /* Make certain the file is newline terminated */
	buf[size+1] = '\0'; /* Null terminate the file for good measure */
	progress = 0;
	while(progress < size) {
		result = fread(buf + progress, 1, size - progress, file);
		if (result < 0) {
			if ((errno == EINTR) ||	(errno == EAGAIN))
				continue;
			die("read on %s of %ld bytes failed: %s\n",
				filename, (size - progress)+ 0UL, strerror(errno));
		}
		progress += result;
	}
	fclose(file);
	return buf;
}

/* Types on the destination platform */
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME this assumes 32bit x86 is the destination"
#endif
typedef int8_t   schar_t;
typedef uint8_t  uchar_t;
typedef int8_t   char_t;
typedef int16_t  short_t;
typedef uint16_t ushort_t;
typedef int32_t  int_t;
typedef uint32_t uint_t;
typedef int32_t  long_t;
#define ulong_t uint32_t

#define SCHAR_T_MIN (-128)
#define SCHAR_T_MAX 127
#define UCHAR_T_MAX 255
#define CHAR_T_MIN  SCHAR_T_MIN
#define CHAR_T_MAX  SCHAR_T_MAX
#define SHRT_T_MIN  (-32768)
#define SHRT_T_MAX  32767
#define USHRT_T_MAX 65535
#define INT_T_MIN   (-LONG_T_MAX - 1)
#define INT_T_MAX   2147483647
#define UINT_T_MAX  4294967295U
#define LONG_T_MIN  (-LONG_T_MAX - 1)
#define LONG_T_MAX  2147483647
#define ULONG_T_MAX 4294967295U

#define SIZEOF_I8    8
#define SIZEOF_I16   16
#define SIZEOF_I32   32
#define SIZEOF_I64   64

#define SIZEOF_CHAR    8
#define SIZEOF_SHORT   16
#define SIZEOF_INT     32
#define SIZEOF_LONG    (sizeof(long_t)*SIZEOF_CHAR)


#define ALIGNOF_CHAR    8
#define ALIGNOF_SHORT   16
#define ALIGNOF_INT     32
#define ALIGNOF_LONG    (sizeof(long_t)*SIZEOF_CHAR)

#define REG_SIZEOF_REG     32
#define REG_SIZEOF_CHAR    REG_SIZEOF_REG
#define REG_SIZEOF_SHORT   REG_SIZEOF_REG
#define REG_SIZEOF_INT     REG_SIZEOF_REG
#define REG_SIZEOF_LONG    REG_SIZEOF_REG

#define REG_ALIGNOF_REG     REG_SIZEOF_REG
#define REG_ALIGNOF_CHAR    REG_SIZEOF_REG
#define REG_ALIGNOF_SHORT   REG_SIZEOF_REG
#define REG_ALIGNOF_INT     REG_SIZEOF_REG
#define REG_ALIGNOF_LONG    REG_SIZEOF_REG

/* Additional definitions for clarity.
 * I currently assume a long is the largest native
 * machine word and that a pointer fits into it.
 */
#define SIZEOF_WORD     SIZEOF_LONG
#define SIZEOF_POINTER  SIZEOF_LONG
#define ALIGNOF_WORD    ALIGNOF_LONG
#define ALIGNOF_POINTER ALIGNOF_LONG
#define REG_SIZEOF_POINTER  REG_SIZEOF_LONG
#define REG_ALIGNOF_POINTER REG_ALIGNOF_LONG

struct file_state {
	struct file_state *prev;
	const char *basename;
	char *dirname;
	const char *buf;
	off_t size;
	const char *pos;
	int line;
	const char *line_start;
	int report_line;
	const char *report_name;
	const char *report_dir;
	int macro      : 1;
	int trigraphs  : 1;
	int join_lines : 1;
};
struct hash_entry;
struct token {
	int tok;
	struct hash_entry *ident;
	const char *pos;
	int str_len;
	union {
		ulong_t integer;
		const char *str;
		int notmacro;
	} val;
};

/* I have two classes of types:
 * Operational types.
 * Logical types.  (The type the C standard says the operation is of)
 *
 * The operational types are:
 * chars
 * shorts
 * ints
 * longs
 *
 * floats
 * doubles
 * long doubles
 *
 * pointer
 */


/* Machine model.
 * No memory is useable by the compiler.
 * There is no floating point support.
 * All operations take place in general purpose registers.
 * There is one type of general purpose register.
 * Unsigned longs are stored in that general purpose register.
 */

/* Operations on general purpose registers.
 */

#define OP_SDIVT      0
#define OP_UDIVT      1
#define OP_SMUL       2
#define OP_UMUL       3
#define OP_SDIV       4
#define OP_UDIV       5
#define OP_SMOD       6
#define OP_UMOD       7
#define OP_ADD        8
#define OP_SUB        9
#define OP_SL        10
#define OP_USR       11
#define OP_SSR       12
#define OP_AND       13
#define OP_XOR       14
#define OP_OR        15
#define OP_POS       16 /* Dummy positive operator don't use it */
#define OP_NEG       17
#define OP_INVERT    18

#define OP_EQ        20
#define OP_NOTEQ     21
#define OP_SLESS     22
#define OP_ULESS     23
#define OP_SMORE     24
#define OP_UMORE     25
#define OP_SLESSEQ   26
#define OP_ULESSEQ   27
#define OP_SMOREEQ   28
#define OP_UMOREEQ   29

#define OP_LFALSE    30  /* Test if the expression is logically false */
#define OP_LTRUE     31  /* Test if the expression is logcially true */

#define OP_LOAD      32
#define OP_STORE     33
/* For OP_STORE ->type holds the type
 * RHS(0) holds the destination address
 * RHS(1) holds the value to store.
 */

#define OP_UEXTRACT  34
/* OP_UEXTRACT extracts an unsigned bitfield from a pseudo register
 * RHS(0) holds the pseudo register to extract from
 * ->type holds the size of the bitfield.
 * ->u.bitfield.size holds the size of the bitfield.
 * ->u.bitfield.offset holds the offset to extract from
 */
#define OP_SEXTRACT  35
/* OP_SEXTRACT extracts a signed bitfield from a pseudo register
 * RHS(0) holds the pseudo register to extract from
 * ->type holds the size of the bitfield.
 * ->u.bitfield.size holds the size of the bitfield.
 * ->u.bitfield.offset holds the offset to extract from
 */
#define OP_DEPOSIT   36
/* OP_DEPOSIT replaces a bitfield with a new value.
 * RHS(0) holds the value to replace a bitifield in.
 * RHS(1) holds the replacement value
 * ->u.bitfield.size holds the size of the bitfield.
 * ->u.bitfield.offset holds the deposit into
 */

#define OP_NOOP      37

#define OP_MIN_CONST 50
#define OP_MAX_CONST 58
#define IS_CONST_OP(X) (((X) >= OP_MIN_CONST) && ((X) <= OP_MAX_CONST))
#define OP_INTCONST  50
/* For OP_INTCONST ->type holds the type.
 * ->u.cval holds the constant value.
 */
#define OP_BLOBCONST 51
/* For OP_BLOBCONST ->type holds the layout and size
 * information.  u.blob holds a pointer to the raw binary
 * data for the constant initializer.
 */
#define OP_ADDRCONST 52
/* For OP_ADDRCONST ->type holds the type.
 * MISC(0) holds the reference to the static variable.
 * ->u.cval holds an offset from that value.
 */
#define OP_UNKNOWNVAL 59
/* For OP_UNKNOWNAL ->type holds the type.
 * For some reason we don't know what value this type has.
 * This allows for variables that have don't have values
 * assigned yet, or variables whose value we simply do not know.
 */

#define OP_WRITE     60
/* OP_WRITE moves one pseudo register to another.
 * MISC(0) holds the destination pseudo register, which must be an OP_DECL.
 * RHS(0) holds the pseudo to move.
 */

#define OP_READ      61
/* OP_READ reads the value of a variable and makes
 * it available for the pseudo operation.
 * Useful for things like def-use chains.
 * RHS(0) holds points to the triple to read from.
 */
#define OP_COPY      62
/* OP_COPY makes a copy of the pseudo register or constant in RHS(0).
 */
#define OP_CONVERT   63
/* OP_CONVERT makes a copy of the pseudo register or constant in RHS(0).
 * And then the type is converted appropriately.
 */
#define OP_PIECE     64
/* OP_PIECE returns one piece of a instruction that returns a structure.
 * MISC(0) is the instruction
 * u.cval is the LHS piece of the instruction to return.
 */
#define OP_ASM       65
/* OP_ASM holds a sequence of assembly instructions, the result
 * of a C asm directive.
 * RHS(x) holds input value x to the assembly sequence.
 * LHS(x) holds the output value x from the assembly sequence.
 * u.blob holds the string of assembly instructions.
 */

#define OP_DEREF     66
/* OP_DEREF generates an lvalue from a pointer.
 * RHS(0) holds the pointer value.
 * OP_DEREF serves as a place holder to indicate all necessary
 * checks have been done to indicate a value is an lvalue.
 */
#define OP_DOT       67
/* OP_DOT references a submember of a structure lvalue.
 * MISC(0) holds the lvalue.
 * ->u.field holds the name of the field we want.
 *
 * Not seen after structures are flattened.
 */
#define OP_INDEX     68
/* OP_INDEX references a submember of a tuple or array lvalue.
 * MISC(0) holds the lvalue.
 * ->u.cval holds the index into the lvalue.
 *
 * Not seen after structures are flattened.
 */
#define OP_VAL       69
/* OP_VAL returns the value of a subexpression of the current expression.
 * Useful for operators that have side effects.
 * RHS(0) holds the expression.
 * MISC(0) holds the subexpression of RHS(0) that is the
 * value of the expression.
 *
 * Not seen outside of expressions.
 */

#define OP_TUPLE     70
/* OP_TUPLE is an array of triples that are either variable
 * or values for a structure or an array.  It is used as
 * a place holder when flattening compound types.
 * The value represented by an OP_TUPLE is held in N registers.
 * LHS(0..N-1) refer to those registers.
 * ->use is a list of statements that use the value.
 *
 * Although OP_TUPLE always has register sized pieces they are not
 * used until structures are flattened/decomposed into their register
 * components.
 * ???? registers ????
 */

#define OP_BITREF    71
/* OP_BITREF describes a bitfield as an lvalue.
 * RHS(0) holds the register value.
 * ->type holds the type of the bitfield.
 * ->u.bitfield.size holds the size of the bitfield.
 * ->u.bitfield.offset holds the offset of the bitfield in the register
 */


#define OP_FCALL     72
/* OP_FCALL performs a procedure call.
 * MISC(0) holds a pointer to the OP_LIST of a function
 * RHS(x) holds argument x of a function
 *
 * Currently not seen outside of expressions.
 */
#define OP_PROG      73
/* OP_PROG is an expression that holds a list of statements, or
 * expressions.  The final expression is the value of the expression.
 * RHS(0) holds the start of the list.
 */

/* statements */
#define OP_LIST      80
/* OP_LIST Holds a list of statements that compose a function, and a result value.
 * RHS(0) holds the list of statements.
 * A list of all functions is maintained.
 */

#define OP_BRANCH    81 /* an unconditional branch */
/* For branch instructions
 * TARG(0) holds the branch target.
 * ->next holds where to branch to if the branch is not taken.
 * The branch target can only be a label
 */

#define OP_CBRANCH   82 /* a conditional branch */
/* For conditional branch instructions
 * RHS(0) holds the branch condition.
 * TARG(0) holds the branch target.
 * ->next holds where to branch to if the branch is not taken.
 * The branch target can only be a label
 */

#define OP_CALL      83 /* an uncontional branch that will return */
/* For call instructions
 * MISC(0) holds the OP_RET that returns from the branch
 * TARG(0) holds the branch target.
 * ->next holds where to branch to if the branch is not taken.
 * The branch target can only be a label
 */

#define OP_RET       84 /* an uncontinonal branch through a variable back to an OP_CALL */
/* For call instructions
 * RHS(0) holds the variable with the return address
 * The branch target can only be a label
 */

#define OP_LABEL     86
/* OP_LABEL is a triple that establishes an target for branches.
 * ->use is the list of all branches that use this label.
 */

#define OP_ADECL     87
/* OP_ADECL is a triple that establishes an lvalue for assignments.
 * A variable takes N registers to contain.
 * LHS(0..N-1) refer to an OP_PIECE triple that represents
 * the Xth register that the variable is stored in.
 * ->use is a list of statements that use the variable.
 *
 * Although OP_ADECL always has register sized pieces they are not
 * used until structures are flattened/decomposed into their register
 * components.
 */

#define OP_SDECL     88
/* OP_SDECL is a triple that establishes a variable of static
 * storage duration.
 * ->use is a list of statements that use the variable.
 * MISC(0) holds the initializer expression.
 */


#define OP_PHI       89
/* OP_PHI is a triple used in SSA form code.
 * It is used when multiple code paths merge and a variable needs
 * a single assignment from any of those code paths.
 * The operation is a cross between OP_DECL and OP_WRITE, which
 * is what OP_PHI is generated from.
 *
 * RHS(x) points to the value from code path x
 * The number of RHS entries is the number of control paths into the block
 * in which OP_PHI resides.  The elements of the array point to point
 * to the variables OP_PHI is derived from.
 *
 * MISC(0) holds a pointer to the orginal OP_DECL node.
 */

#if 0
/* continuation helpers
 */
#define OP_CPS_BRANCH    90 /* an unconditional branch */
/* OP_CPS_BRANCH calls a continuation
 * RHS(x) holds argument x of the function
 * TARG(0) holds OP_CPS_START target
 */
#define OP_CPS_CBRANCH   91  /* a conditional branch */
/* OP_CPS_CBRANCH conditionally calls one of two continuations
 * RHS(0) holds the branch condition
 * RHS(x + 1) holds argument x of the function
 * TARG(0) holds the OP_CPS_START to jump to when true
 * ->next holds the OP_CPS_START to jump to when false
 */
#define OP_CPS_CALL      92  /* an uncontional branch that will return */
/* For OP_CPS_CALL instructions
 * RHS(x) holds argument x of the function
 * MISC(0) holds the OP_CPS_RET that returns from the branch
 * TARG(0) holds the branch target.
 * ->next holds where the OP_CPS_RET will return to.
 */
#define OP_CPS_RET       93
/* OP_CPS_RET conditionally calls one of two continuations
 * RHS(0) holds the variable with the return function address
 * RHS(x + 1) holds argument x of the function
 * The branch target may be any OP_CPS_START
 */
#define OP_CPS_END       94
/* OP_CPS_END is the triple at the end of the program.
 * For most practical purposes it is a branch.
 */
#define OP_CPS_START     95
/* OP_CPS_START is a triple at the start of a continuation
 * The arguments variables takes N registers to contain.
 * LHS(0..N-1) refer to an OP_PIECE triple that represents
 * the Xth register that the arguments are stored in.
 */
#endif

/* Architecture specific instructions */
#define OP_CMP         100
#define OP_TEST        101
#define OP_SET_EQ      102
#define OP_SET_NOTEQ   103
#define OP_SET_SLESS   104
#define OP_SET_ULESS   105
#define OP_SET_SMORE   106
#define OP_SET_UMORE   107
#define OP_SET_SLESSEQ 108
#define OP_SET_ULESSEQ 109
#define OP_SET_SMOREEQ 110
#define OP_SET_UMOREEQ 111

#define OP_JMP         112
#define OP_JMP_EQ      113
#define OP_JMP_NOTEQ   114
#define OP_JMP_SLESS   115
#define OP_JMP_ULESS   116
#define OP_JMP_SMORE   117
#define OP_JMP_UMORE   118
#define OP_JMP_SLESSEQ 119
#define OP_JMP_ULESSEQ 120
#define OP_JMP_SMOREEQ 121
#define OP_JMP_UMOREEQ 122

/* Builtin operators that it is just simpler to use the compiler for */
#define OP_INB         130
#define OP_INW         131
#define OP_INL         132
#define OP_OUTB        133
#define OP_OUTW        134
#define OP_OUTL        135
#define OP_BSF         136
#define OP_BSR         137
#define OP_RDMSR       138
#define OP_WRMSR       139
#define OP_HLT         140

struct op_info {
	const char *name;
	unsigned flags;
#define PURE       0x001 /* Triple has no side effects */
#define IMPURE     0x002 /* Triple has side effects */
#define PURE_BITS(FLAGS) ((FLAGS) & 0x3)
#define DEF        0x004 /* Triple is a variable definition */
#define BLOCK      0x008 /* Triple stores the current block */
#define STRUCTURAL 0x010 /* Triple does not generate a machine instruction */
#define BRANCH_BITS(FLAGS) ((FLAGS) & 0xe0 )
#define UBRANCH    0x020 /* Triple is an unconditional branch instruction */
#define CBRANCH    0x040 /* Triple is a conditional branch instruction */
#define RETBRANCH  0x060 /* Triple is a return instruction */
#define CALLBRANCH 0x080 /* Triple is a call instruction */
#define ENDBRANCH  0x0a0 /* Triple is an end instruction */
#define PART       0x100 /* Triple is really part of another triple */
#define BITFIELD   0x200 /* Triple manipulates a bitfield */
	signed char lhs, rhs, misc, targ;
};

#define OP(LHS, RHS, MISC, TARG, FLAGS, NAME) { \
	.name = (NAME), \
	.flags = (FLAGS), \
	.lhs = (LHS), \
	.rhs = (RHS), \
	.misc = (MISC), \
	.targ = (TARG), \
	 }
static const struct op_info table_ops[] = {
[OP_SDIVT      ] = OP( 2,  2, 0, 0, PURE | BLOCK , "sdivt"),
[OP_UDIVT      ] = OP( 2,  2, 0, 0, PURE | BLOCK , "udivt"),
[OP_SMUL       ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "smul"),
[OP_UMUL       ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "umul"),
[OP_SDIV       ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "sdiv"),
[OP_UDIV       ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "udiv"),
[OP_SMOD       ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "smod"),
[OP_UMOD       ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "umod"),
[OP_ADD        ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "add"),
[OP_SUB        ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "sub"),
[OP_SL         ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "sl"),
[OP_USR        ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "usr"),
[OP_SSR        ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "ssr"),
[OP_AND        ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "and"),
[OP_XOR        ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "xor"),
[OP_OR         ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "or"),
[OP_POS        ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK , "pos"),
[OP_NEG        ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK , "neg"),
[OP_INVERT     ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK , "invert"),

[OP_EQ         ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "eq"),
[OP_NOTEQ      ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "noteq"),
[OP_SLESS      ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "sless"),
[OP_ULESS      ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "uless"),
[OP_SMORE      ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "smore"),
[OP_UMORE      ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "umore"),
[OP_SLESSEQ    ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "slesseq"),
[OP_ULESSEQ    ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "ulesseq"),
[OP_SMOREEQ    ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "smoreeq"),
[OP_UMOREEQ    ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK , "umoreeq"),
[OP_LFALSE     ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK , "lfalse"),
[OP_LTRUE      ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK , "ltrue"),

[OP_LOAD       ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "load"),
[OP_STORE      ] = OP( 0,  2, 0, 0, PURE | BLOCK , "store"),

[OP_UEXTRACT   ] = OP( 0,  1, 0, 0, PURE | DEF | BITFIELD, "uextract"),
[OP_SEXTRACT   ] = OP( 0,  1, 0, 0, PURE | DEF | BITFIELD, "sextract"),
[OP_DEPOSIT    ] = OP( 0,  2, 0, 0, PURE | DEF | BITFIELD, "deposit"),

[OP_NOOP       ] = OP( 0,  0, 0, 0, PURE | BLOCK | STRUCTURAL, "noop"),

[OP_INTCONST   ] = OP( 0,  0, 0, 0, PURE | DEF, "intconst"),
[OP_BLOBCONST  ] = OP( 0,  0, 0, 0, PURE , "blobconst"),
[OP_ADDRCONST  ] = OP( 0,  0, 1, 0, PURE | DEF, "addrconst"),
[OP_UNKNOWNVAL ] = OP( 0,  0, 0, 0, PURE | DEF, "unknown"),

#if DEBUG_ROMCC_WARNINGS
#warning "FIXME is it correct for OP_WRITE to be a def?  I currently use it as one..."
#endif
[OP_WRITE      ] = OP( 0,  1, 1, 0, PURE | DEF | BLOCK, "write"),
[OP_READ       ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "read"),
[OP_COPY       ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "copy"),
[OP_CONVERT    ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "convert"),
[OP_PIECE      ] = OP( 0,  0, 1, 0, PURE | DEF | STRUCTURAL | PART, "piece"),
[OP_ASM        ] = OP(-1, -1, 0, 0, PURE, "asm"),
[OP_DEREF      ] = OP( 0,  1, 0, 0, 0 | DEF | BLOCK, "deref"),
[OP_DOT        ] = OP( 0,  0, 1, 0, PURE | DEF | PART, "dot"),
[OP_INDEX      ] = OP( 0,  0, 1, 0, PURE | DEF | PART, "index"),

[OP_VAL        ] = OP( 0,  1, 1, 0, 0 | DEF | BLOCK, "val"),
[OP_TUPLE      ] = OP(-1,  0, 0, 0, 0 | PURE | BLOCK | STRUCTURAL, "tuple"),
[OP_BITREF     ] = OP( 0,  1, 0, 0, 0 | DEF | PURE | STRUCTURAL | BITFIELD, "bitref"),
/* Call is special most it can stand in for anything so it depends on context */
[OP_FCALL      ] = OP( 0, -1, 1, 0, 0 | BLOCK | CALLBRANCH, "fcall"),
[OP_PROG       ] = OP( 0,  1, 0, 0, 0 | IMPURE | BLOCK | STRUCTURAL, "prog"),
/* The sizes of OP_FCALL depends upon context */

[OP_LIST       ] = OP( 0,  1, 1, 0, 0 | DEF | STRUCTURAL, "list"),
[OP_BRANCH     ] = OP( 0,  0, 0, 1, PURE | BLOCK | UBRANCH, "branch"),
[OP_CBRANCH    ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "cbranch"),
[OP_CALL       ] = OP( 0,  0, 1, 1, PURE | BLOCK | CALLBRANCH, "call"),
[OP_RET        ] = OP( 0,  1, 0, 0, PURE | BLOCK | RETBRANCH, "ret"),
[OP_LABEL      ] = OP( 0,  0, 0, 0, PURE | BLOCK | STRUCTURAL, "label"),
[OP_ADECL      ] = OP( 0,  0, 0, 0, PURE | BLOCK | STRUCTURAL, "adecl"),
[OP_SDECL      ] = OP( 0,  0, 1, 0, PURE | BLOCK | STRUCTURAL, "sdecl"),
/* The number of RHS elements of OP_PHI depend upon context */
[OP_PHI        ] = OP( 0, -1, 1, 0, PURE | DEF | BLOCK, "phi"),

#if 0
[OP_CPS_BRANCH ] = OP( 0, -1, 0, 1, PURE | BLOCK | UBRANCH,     "cps_branch"),
[OP_CPS_CBRANCH] = OP( 0, -1, 0, 1, PURE | BLOCK | CBRANCH,     "cps_cbranch"),
[OP_CPS_CALL   ] = OP( 0, -1, 1, 1, PURE | BLOCK | CALLBRANCH,  "cps_call"),
[OP_CPS_RET    ] = OP( 0, -1, 0, 0, PURE | BLOCK | RETBRANCH,   "cps_ret"),
[OP_CPS_END    ] = OP( 0, -1, 0, 0, IMPURE | BLOCK | ENDBRANCH, "cps_end"),
[OP_CPS_START  ] = OP( -1, 0, 0, 0, PURE | BLOCK | STRUCTURAL,  "cps_start"),
#endif

[OP_CMP        ] = OP( 0,  2, 0, 0, PURE | DEF | BLOCK, "cmp"),
[OP_TEST       ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "test"),
[OP_SET_EQ     ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_eq"),
[OP_SET_NOTEQ  ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_noteq"),
[OP_SET_SLESS  ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_sless"),
[OP_SET_ULESS  ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_uless"),
[OP_SET_SMORE  ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_smore"),
[OP_SET_UMORE  ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_umore"),
[OP_SET_SLESSEQ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_slesseq"),
[OP_SET_ULESSEQ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_ulesseq"),
[OP_SET_SMOREEQ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_smoreq"),
[OP_SET_UMOREEQ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "set_umoreq"),
[OP_JMP        ] = OP( 0,  0, 0, 1, PURE | BLOCK | UBRANCH, "jmp"),
[OP_JMP_EQ     ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_eq"),
[OP_JMP_NOTEQ  ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_noteq"),
[OP_JMP_SLESS  ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_sless"),
[OP_JMP_ULESS  ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_uless"),
[OP_JMP_SMORE  ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_smore"),
[OP_JMP_UMORE  ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_umore"),
[OP_JMP_SLESSEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_slesseq"),
[OP_JMP_ULESSEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_ulesseq"),
[OP_JMP_SMOREEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_smoreq"),
[OP_JMP_UMOREEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | CBRANCH, "jmp_umoreq"),

[OP_INB        ] = OP( 0,  1, 0, 0, IMPURE | DEF | BLOCK, "__inb"),
[OP_INW        ] = OP( 0,  1, 0, 0, IMPURE | DEF | BLOCK, "__inw"),
[OP_INL        ] = OP( 0,  1, 0, 0, IMPURE | DEF | BLOCK, "__inl"),
[OP_OUTB       ] = OP( 0,  2, 0, 0, IMPURE| BLOCK, "__outb"),
[OP_OUTW       ] = OP( 0,  2, 0, 0, IMPURE| BLOCK, "__outw"),
[OP_OUTL       ] = OP( 0,  2, 0, 0, IMPURE| BLOCK, "__outl"),
[OP_BSF        ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "__bsf"),
[OP_BSR        ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "__bsr"),
[OP_RDMSR      ] = OP( 2,  1, 0, 0, IMPURE | BLOCK, "__rdmsr"),
[OP_WRMSR      ] = OP( 0,  3, 0, 0, IMPURE | BLOCK, "__wrmsr"),
[OP_HLT        ] = OP( 0,  0, 0, 0, IMPURE | BLOCK, "__hlt"),
};
#undef OP
#define OP_MAX      (sizeof(table_ops)/sizeof(table_ops[0]))

static const char *tops(int index)
{
	static const char unknown[] = "unknown op";
	if (index < 0) {
		return unknown;
	}
	if (index >= OP_MAX) {
		return unknown;
	}
	return table_ops[index].name;
}

struct asm_info;
struct triple;
struct block;
struct triple_set {
	struct triple_set *next;
	struct triple *member;
};

#define MAX_LHS  63
#define MAX_RHS  127
#define MAX_MISC 3
#define MAX_TARG 1

struct occurrence {
	int count;
	const char *filename;
	const char *function;
	int line;
	int col;
	struct occurrence *parent;
};
struct bitfield {
	ulong_t size : 8;
	ulong_t offset : 24;
};
struct triple {
	struct triple *next, *prev;
	struct triple_set *use;
	struct type *type;
	unsigned int op : 8;
	unsigned int template_id : 7;
	unsigned int lhs  : 6;
	unsigned int rhs  : 7;
	unsigned int misc : 2;
	unsigned int targ : 1;
#define TRIPLE_SIZE(TRIPLE) \
	((TRIPLE)->lhs + (TRIPLE)->rhs + (TRIPLE)->misc + (TRIPLE)->targ)
#define TRIPLE_LHS_OFF(PTR)  (0)
#define TRIPLE_RHS_OFF(PTR)  (TRIPLE_LHS_OFF(PTR) + (PTR)->lhs)
#define TRIPLE_MISC_OFF(PTR) (TRIPLE_RHS_OFF(PTR) + (PTR)->rhs)
#define TRIPLE_TARG_OFF(PTR) (TRIPLE_MISC_OFF(PTR) + (PTR)->misc)
#define LHS(PTR,INDEX) ((PTR)->param[TRIPLE_LHS_OFF(PTR) + (INDEX)])
#define RHS(PTR,INDEX) ((PTR)->param[TRIPLE_RHS_OFF(PTR) + (INDEX)])
#define TARG(PTR,INDEX) ((PTR)->param[TRIPLE_TARG_OFF(PTR) + (INDEX)])
#define MISC(PTR,INDEX) ((PTR)->param[TRIPLE_MISC_OFF(PTR) + (INDEX)])
	unsigned id; /* A scratch value and finally the register */
#define TRIPLE_FLAG_FLATTENED   (1 << 31)
#define TRIPLE_FLAG_PRE_SPLIT   (1 << 30)
#define TRIPLE_FLAG_POST_SPLIT  (1 << 29)
#define TRIPLE_FLAG_VOLATILE    (1 << 28)
#define TRIPLE_FLAG_INLINE      (1 << 27) /* ???? */
#define TRIPLE_FLAG_LOCAL	(1 << 26)

#define TRIPLE_FLAG_COPY TRIPLE_FLAG_VOLATILE
	struct occurrence *occurrence;
	union {
		ulong_t cval;
		struct bitfield bitfield;
		struct block  *block;
		void *blob;
		struct hash_entry *field;
		struct asm_info *ainfo;
		struct triple *func;
		struct symbol *symbol;
	} u;
	struct triple *param[2];
};

struct reg_info {
	unsigned reg;
	unsigned regcm;
};
struct ins_template {
	struct reg_info lhs[MAX_LHS + 1], rhs[MAX_RHS + 1];
};

struct asm_info {
	struct ins_template tmpl;
	char *str;
};

struct block_set {
	struct block_set *next;
	struct block *member;
};
struct block {
	struct block *work_next;
	struct triple *first, *last;
	int edge_count;
	struct block_set *edges;
	int users;
	struct block_set *use;
	struct block_set *idominates;
	struct block_set *domfrontier;
	struct block *idom;
	struct block_set *ipdominates;
	struct block_set *ipdomfrontier;
	struct block *ipdom;
	int vertex;

};

struct symbol {
	struct symbol *next;
	struct hash_entry *ident;
	struct triple *def;
	struct type *type;
	int scope_depth;
};

struct macro_arg {
	struct macro_arg *next;
	struct hash_entry *ident;
};
struct macro {
	struct hash_entry *ident;
	const char *buf;
	int buf_len;
	struct macro_arg *args;
	int argc;
};

struct hash_entry {
	struct hash_entry *next;
	const char *name;
	int name_len;
	int tok;
	struct macro *sym_define;
	struct symbol *sym_label;
	struct symbol *sym_tag;
	struct symbol *sym_ident;
};

#define HASH_TABLE_SIZE 2048

struct compiler_state {
	const char *label_prefix;
	const char *ofilename;
	unsigned long flags;
	unsigned long debug;
	unsigned long max_allocation_passes;

	size_t include_path_count;
	const char **include_paths;

	size_t define_count;
	const char **defines;

	size_t undef_count;
	const char **undefs;
};
struct arch_state {
	unsigned long features;
};
struct basic_blocks {
	struct triple *func;
	struct triple *first;
	struct block *first_block, *last_block;
	int last_vertex;
};
#define MAX_PP_IF_DEPTH 63
struct compile_state {
	struct compiler_state *compiler;
	struct arch_state *arch;
	FILE *output;
	FILE *errout;
	FILE *dbgout;
	struct file_state *file;
	struct occurrence *last_occurrence;
	const char *function;
	int    token_base;
	struct token token[6];
	struct hash_entry *hash_table[HASH_TABLE_SIZE];
	struct hash_entry *i_switch;
	struct hash_entry *i_case;
	struct hash_entry *i_continue;
	struct hash_entry *i_break;
	struct hash_entry *i_default;
	struct hash_entry *i_return;
	struct hash_entry *i_noreturn;
	struct hash_entry *i_unused;
	struct hash_entry *i_packed;
	/* Additional hash entries for predefined macros */
	struct hash_entry *i_defined;
	struct hash_entry *i___VA_ARGS__;
	struct hash_entry *i___FILE__;
	struct hash_entry *i___LINE__;
	/* Additional hash entries for predefined identifiers */
	struct hash_entry *i___func__;
	/* Additional hash entries for attributes */
	struct hash_entry *i_noinline;
	struct hash_entry *i_always_inline;
	int scope_depth;
	unsigned char if_bytes[(MAX_PP_IF_DEPTH + CHAR_BIT -1)/CHAR_BIT];
	int if_depth;
	int eat_depth, eat_targ;
	struct file_state *macro_file;
	struct triple *functions;
	struct triple *main_function;
	struct triple *first;
	struct triple *global_pool;
	struct basic_blocks bb;
	int functions_joined;
};

/* visibility global/local */
/* static/auto duration */
/* typedef, register, inline */
#define STOR_SHIFT         0
#define STOR_MASK     0x001f
/* Visibility */
#define STOR_GLOBAL   0x0001
/* Duration */
#define STOR_PERM     0x0002
/* Definition locality */
#define STOR_NONLOCAL 0x0004  /* The definition is not in this translation unit */
/* Storage specifiers */
#define STOR_AUTO     0x0000
#define STOR_STATIC   0x0002
#define STOR_LOCAL    0x0003
#define STOR_EXTERN   0x0007
#define STOR_INLINE   0x0008
#define STOR_REGISTER 0x0010
#define STOR_TYPEDEF  0x0018

#define QUAL_SHIFT         5
#define QUAL_MASK     0x00e0
#define QUAL_NONE     0x0000
#define QUAL_CONST    0x0020
#define QUAL_VOLATILE 0x0040
#define QUAL_RESTRICT 0x0080

#define TYPE_SHIFT         8
#define TYPE_MASK     0x1f00
#define TYPE_INTEGER(TYPE)    ((((TYPE) >= TYPE_CHAR) && ((TYPE) <= TYPE_ULLONG)) || ((TYPE) == TYPE_ENUM) || ((TYPE) == TYPE_BITFIELD))
#define TYPE_ARITHMETIC(TYPE) ((((TYPE) >= TYPE_CHAR) && ((TYPE) <= TYPE_LDOUBLE)) || ((TYPE) == TYPE_ENUM) || ((TYPE) == TYPE_BITFIELD))
#define TYPE_UNSIGNED(TYPE)   ((TYPE) & 0x0100)
#define TYPE_SIGNED(TYPE)     (!TYPE_UNSIGNED(TYPE))
#define TYPE_MKUNSIGNED(TYPE) (((TYPE) & ~0xF000) | 0x0100)
#define TYPE_RANK(TYPE)       ((TYPE) & ~0xF1FF)
#define TYPE_PTR(TYPE)        (((TYPE) & TYPE_MASK) == TYPE_POINTER)
#define TYPE_DEFAULT  0x0000
#define TYPE_VOID     0x0100
#define TYPE_CHAR     0x0200
#define TYPE_UCHAR    0x0300
#define TYPE_SHORT    0x0400
#define TYPE_USHORT   0x0500
#define TYPE_INT      0x0600
#define TYPE_UINT     0x0700
#define TYPE_LONG     0x0800
#define TYPE_ULONG    0x0900
#define TYPE_LLONG    0x0a00 /* long long */
#define TYPE_ULLONG   0x0b00
#define TYPE_FLOAT    0x0c00
#define TYPE_DOUBLE   0x0d00
#define TYPE_LDOUBLE  0x0e00 /* long double */

/* Note: TYPE_ENUM is chosen very carefully so TYPE_RANK works */
#define TYPE_ENUM     0x1600
#define TYPE_LIST     0x1700
/* TYPE_LIST is a basic building block when defining enumerations
 * type->field_ident holds the name of this enumeration entry.
 * type->right holds the entry in the list.
 */

#define TYPE_STRUCT   0x1000
/* For TYPE_STRUCT
 * type->left holds the link list of TYPE_PRODUCT entries that
 * make up the structure.
 * type->elements hold the length of the linked list
 */
#define TYPE_UNION    0x1100
/* For TYPE_UNION
 * type->left holds the link list of TYPE_OVERLAP entries that
 * make up the union.
 * type->elements hold the length of the linked list
 */
#define TYPE_POINTER  0x1200
/* For TYPE_POINTER:
 * type->left holds the type pointed to.
 */
#define TYPE_FUNCTION 0x1300
/* For TYPE_FUNCTION:
 * type->left holds the return type.
 * type->right holds the type of the arguments
 * type->elements holds the count of the arguments
 */
#define TYPE_PRODUCT  0x1400
/* TYPE_PRODUCT is a basic building block when defining structures
 * type->left holds the type that appears first in memory.
 * type->right holds the type that appears next in memory.
 */
#define TYPE_OVERLAP  0x1500
/* TYPE_OVERLAP is a basic building block when defining unions
 * type->left and type->right holds to types that overlap
 * each other in memory.
 */
#define TYPE_ARRAY    0x1800
/* TYPE_ARRAY is a basic building block when definitng arrays.
 * type->left holds the type we are an array of.
 * type->elements holds the number of elements.
 */
#define TYPE_TUPLE    0x1900
/* TYPE_TUPLE is a basic building block when defining
 * positionally reference type conglomerations. (i.e. closures)
 * In essence it is a wrapper for TYPE_PRODUCT, like TYPE_STRUCT
 * except it has no field names.
 * type->left holds the liked list of TYPE_PRODUCT entries that
 * make up the closure type.
 * type->elements hold the number of elements in the closure.
 */
#define TYPE_JOIN     0x1a00
/* TYPE_JOIN is a basic building block when defining
 * positionally reference type conglomerations. (i.e. closures)
 * In essence it is a wrapper for TYPE_OVERLAP, like TYPE_UNION
 * except it has no field names.
 * type->left holds the liked list of TYPE_OVERLAP entries that
 * make up the closure type.
 * type->elements hold the number of elements in the closure.
 */
#define TYPE_BITFIELD 0x1b00
/* TYPE_BITFIED is the type of a bitfield.
 * type->left holds the type basic type TYPE_BITFIELD is derived from.
 * type->elements holds the number of bits in the bitfield.
 */
#define TYPE_UNKNOWN  0x1c00
/* TYPE_UNKNOWN is the type of an unknown value.
 * Used on unknown consts and other places where I don't know the type.
 */

#define ATTRIB_SHIFT                 16
#define ATTRIB_MASK          0xffff0000
#define ATTRIB_NOINLINE      0x00010000
#define ATTRIB_ALWAYS_INLINE 0x00020000

#define ELEMENT_COUNT_UNSPECIFIED ULONG_T_MAX

struct type {
	unsigned int type;
	struct type *left, *right;
	ulong_t elements;
	struct hash_entry *field_ident;
	struct hash_entry *type_ident;
};

#define TEMPLATE_BITS      7
#define MAX_TEMPLATES      (1<<TEMPLATE_BITS)
#define MAX_REG_EQUIVS     16
#define MAX_REGC           14
#define MAX_REGISTERS      75
#define REGISTER_BITS      7
#define MAX_VIRT_REGISTERS (1<<REGISTER_BITS)
#define REG_ERROR          0
#define REG_UNSET          1
#define REG_UNNEEDED       2
#define REG_VIRT0          (MAX_REGISTERS + 0)
#define REG_VIRT1          (MAX_REGISTERS + 1)
#define REG_VIRT2          (MAX_REGISTERS + 2)
#define REG_VIRT3          (MAX_REGISTERS + 3)
#define REG_VIRT4          (MAX_REGISTERS + 4)
#define REG_VIRT5          (MAX_REGISTERS + 5)
#define REG_VIRT6          (MAX_REGISTERS + 6)
#define REG_VIRT7          (MAX_REGISTERS + 7)
#define REG_VIRT8          (MAX_REGISTERS + 8)
#define REG_VIRT9          (MAX_REGISTERS + 9)

#if (MAX_REGISTERS + 9) > MAX_VIRT_REGISTERS
#error "MAX_VIRT_REGISTERS to small"
#endif
#if (MAX_REGC + REGISTER_BITS) >= 26
#error "Too many id bits used"
#endif

/* Provision for 8 register classes */
#define REG_SHIFT  0
#define REGC_SHIFT REGISTER_BITS
#define REGC_MASK (((1 << MAX_REGC) - 1) << REGISTER_BITS)
#define REG_MASK (MAX_VIRT_REGISTERS -1)
#define ID_REG(ID)              ((ID) & REG_MASK)
#define SET_REG(ID, REG)        ((ID) = (((ID) & ~REG_MASK) | ((REG) & REG_MASK)))
#define ID_REGCM(ID)		(((ID) & REGC_MASK) >> REGC_SHIFT)
#define SET_REGCM(ID, REGCM)	((ID) = (((ID) & ~REGC_MASK) | (((REGCM) << REGC_SHIFT) & REGC_MASK)))
#define SET_INFO(ID, INFO)	((ID) = (((ID) & ~(REG_MASK | REGC_MASK)) | \
		(((INFO).reg) & REG_MASK) | ((((INFO).regcm) << REGC_SHIFT) & REGC_MASK)))

#define ARCH_INPUT_REGS 4
#define ARCH_OUTPUT_REGS 4

static const struct reg_info arch_input_regs[ARCH_INPUT_REGS];
static const struct reg_info arch_output_regs[ARCH_OUTPUT_REGS];
static unsigned arch_reg_regcm(struct compile_state *state, int reg);
static unsigned arch_regcm_normalize(struct compile_state *state, unsigned regcm);
static unsigned arch_regcm_reg_normalize(struct compile_state *state, unsigned regcm);
static void arch_reg_equivs(
	struct compile_state *state, unsigned *equiv, int reg);
static int arch_select_free_register(
	struct compile_state *state, char *used, int classes);
static unsigned arch_regc_size(struct compile_state *state, int class);
static int arch_regcm_intersect(unsigned regcm1, unsigned regcm2);
static unsigned arch_type_to_regcm(struct compile_state *state, struct type *type);
static const char *arch_reg_str(int reg);
static struct reg_info arch_reg_constraint(
	struct compile_state *state, struct type *type, const char *constraint);
static struct reg_info arch_reg_clobber(
	struct compile_state *state, const char *clobber);
static struct reg_info arch_reg_lhs(struct compile_state *state,
	struct triple *ins, int index);
static struct reg_info arch_reg_rhs(struct compile_state *state,
	struct triple *ins, int index);
static int arch_reg_size(int reg);
static struct triple *transform_to_arch_instruction(
	struct compile_state *state, struct triple *ins);
static struct triple *flatten(
	struct compile_state *state, struct triple *first, struct triple *ptr);
static void print_dominators(struct compile_state *state,
	FILE *fp, struct basic_blocks *bb);
static void print_dominance_frontiers(struct compile_state *state,
	FILE *fp, struct basic_blocks *bb);



#define DEBUG_ABORT_ON_ERROR    0x00000001
#define DEBUG_BASIC_BLOCKS      0x00000002
#define DEBUG_FDOMINATORS       0x00000004
#define DEBUG_RDOMINATORS       0x00000008
#define DEBUG_TRIPLES           0x00000010
#define DEBUG_INTERFERENCE      0x00000020
#define DEBUG_SCC_TRANSFORM     0x00000040
#define DEBUG_SCC_TRANSFORM2    0x00000080
#define DEBUG_REBUILD_SSA_FORM  0x00000100
#define DEBUG_INLINE            0x00000200
#define DEBUG_RANGE_CONFLICTS   0x00000400
#define DEBUG_RANGE_CONFLICTS2  0x00000800
#define DEBUG_COLOR_GRAPH       0x00001000
#define DEBUG_COLOR_GRAPH2      0x00002000
#define DEBUG_COALESCING        0x00004000
#define DEBUG_COALESCING2       0x00008000
#define DEBUG_VERIFICATION	0x00010000
#define DEBUG_CALLS		0x00020000
#define DEBUG_CALLS2		0x00040000
#define DEBUG_TOKENS            0x80000000

#define DEBUG_DEFAULT ( \
	DEBUG_ABORT_ON_ERROR | \
	DEBUG_BASIC_BLOCKS | \
	DEBUG_FDOMINATORS | \
	DEBUG_RDOMINATORS | \
	DEBUG_TRIPLES | \
	0 )

#define DEBUG_ALL ( \
	DEBUG_ABORT_ON_ERROR   | \
	DEBUG_BASIC_BLOCKS     | \
	DEBUG_FDOMINATORS      | \
	DEBUG_RDOMINATORS      | \
	DEBUG_TRIPLES          | \
	DEBUG_INTERFERENCE     | \
	DEBUG_SCC_TRANSFORM    | \
	DEBUG_SCC_TRANSFORM2   | \
	DEBUG_REBUILD_SSA_FORM | \
	DEBUG_INLINE           | \
	DEBUG_RANGE_CONFLICTS  | \
	DEBUG_RANGE_CONFLICTS2 | \
	DEBUG_COLOR_GRAPH      | \
	DEBUG_COLOR_GRAPH2     | \
	DEBUG_COALESCING       | \
	DEBUG_COALESCING2      | \
	DEBUG_VERIFICATION     | \
	DEBUG_CALLS	       | \
	DEBUG_CALLS2	       | \
	DEBUG_TOKENS           | \
	0 )

#define COMPILER_INLINE_MASK               0x00000007
#define COMPILER_INLINE_ALWAYS             0x00000000
#define COMPILER_INLINE_NEVER              0x00000001
#define COMPILER_INLINE_DEFAULTON          0x00000002
#define COMPILER_INLINE_DEFAULTOFF         0x00000003
#define COMPILER_INLINE_NOPENALTY          0x00000004
#define COMPILER_ELIMINATE_INEFECTUAL_CODE 0x00000008
#define COMPILER_SIMPLIFY                  0x00000010
#define COMPILER_SCC_TRANSFORM             0x00000020
#define COMPILER_SIMPLIFY_OP               0x00000040
#define COMPILER_SIMPLIFY_PHI              0x00000080
#define COMPILER_SIMPLIFY_LABEL            0x00000100
#define COMPILER_SIMPLIFY_BRANCH           0x00000200
#define COMPILER_SIMPLIFY_COPY             0x00000400
#define COMPILER_SIMPLIFY_ARITH            0x00000800
#define COMPILER_SIMPLIFY_SHIFT            0x00001000
#define COMPILER_SIMPLIFY_BITWISE          0x00002000
#define COMPILER_SIMPLIFY_LOGICAL          0x00004000
#define COMPILER_SIMPLIFY_BITFIELD         0x00008000

#define COMPILER_TRIGRAPHS                 0x40000000
#define COMPILER_PP_ONLY                   0x80000000

#define COMPILER_DEFAULT_FLAGS ( \
	COMPILER_TRIGRAPHS | \
	COMPILER_ELIMINATE_INEFECTUAL_CODE | \
	COMPILER_INLINE_DEFAULTON | \
	COMPILER_SIMPLIFY_OP | \
	COMPILER_SIMPLIFY_PHI | \
	COMPILER_SIMPLIFY_LABEL | \
	COMPILER_SIMPLIFY_BRANCH | \
	COMPILER_SIMPLIFY_COPY | \
	COMPILER_SIMPLIFY_ARITH | \
	COMPILER_SIMPLIFY_SHIFT | \
	COMPILER_SIMPLIFY_BITWISE | \
	COMPILER_SIMPLIFY_LOGICAL | \
	COMPILER_SIMPLIFY_BITFIELD | \
	0 )

#define GLOBAL_SCOPE_DEPTH   1
#define FUNCTION_SCOPE_DEPTH (GLOBAL_SCOPE_DEPTH + 1)

static void compile_file(struct compile_state *old_state, const char *filename, int local);



static void init_compiler_state(struct compiler_state *compiler)
{
	memset(compiler, 0, sizeof(*compiler));
	compiler->label_prefix = "";
	compiler->ofilename = "auto.inc";
	compiler->flags = COMPILER_DEFAULT_FLAGS;
	compiler->debug = 0;
	compiler->max_allocation_passes = MAX_ALLOCATION_PASSES;
	compiler->include_path_count = 1;
	compiler->include_paths      = xcmalloc(sizeof(char *), "include_paths");
	compiler->define_count       = 1;
	compiler->defines            = xcmalloc(sizeof(char *), "defines");
	compiler->undef_count        = 1;
	compiler->undefs             = xcmalloc(sizeof(char *), "undefs");
}

struct compiler_flag {
	const char *name;
	unsigned long flag;
};

struct compiler_arg {
	const char *name;
	unsigned long mask;
	struct compiler_flag flags[16];
};

static int set_flag(
	const struct compiler_flag *ptr, unsigned long *flags,
	int act, const char *flag)
{
	int result = -1;
	for(; ptr->name; ptr++) {
		if (strcmp(ptr->name, flag) == 0) {
			break;
		}
	}
	if (ptr->name) {
		result = 0;
		*flags &= ~(ptr->flag);
		if (act) {
			*flags |= ptr->flag;
		}
	}
	return result;
}

static int set_arg(
	const struct compiler_arg *ptr, unsigned long *flags, const char *arg)
{
	const char *val;
	int result = -1;
	int len;
	val = strchr(arg, '=');
	if (val) {
		len = val - arg;
		val++;
		for(; ptr->name; ptr++) {
			if (strncmp(ptr->name, arg, len) == 0) {
				break;
			}
		}
		if (ptr->name) {
			*flags &= ~ptr->mask;
			result = set_flag(&ptr->flags[0], flags, 1, val);
		}
	}
	return result;
}


static void flag_usage(FILE *fp, const struct compiler_flag *ptr,
	const char *prefix, const char *invert_prefix)
{
	for(;ptr->name; ptr++) {
		fprintf(fp, "%s%s\n", prefix, ptr->name);
		if (invert_prefix) {
			fprintf(fp, "%s%s\n", invert_prefix, ptr->name);
		}
	}
}

static void arg_usage(FILE *fp, const struct compiler_arg *ptr,
	const char *prefix)
{
	for(;ptr->name; ptr++) {
		const struct compiler_flag *flag;
		for(flag = &ptr->flags[0]; flag->name; flag++) {
			fprintf(fp, "%s%s=%s\n",
				prefix, ptr->name, flag->name);
		}
	}
}

static int append_string(size_t *max, const char ***vec, const char *str,
	const char *name)
{
	size_t count;
	count = ++(*max);
	*vec = xrealloc(*vec, sizeof(char *)*count, "name");
	(*vec)[count -1] = 0;
	(*vec)[count -2] = str;
	return 0;
}

static void arg_error(char *fmt, ...);
static void arg_warning(char *fmt, ...);
static const char *identifier(const char *str, const char *end);

static int append_include_path(struct compiler_state *compiler, const char *str)
{
	int result;
	if (!exists(str, ".")) {
		arg_warning("Warning: Nonexistent include path: `%s'\n",
			str);
	}
	result = append_string(&compiler->include_path_count,
		&compiler->include_paths, str, "include_paths");
	return result;
}

static int append_define(struct compiler_state *compiler, const char *str)
{
	const char *end, *rest;
	int result;

	end = strchr(str, '=');
	if (!end) {
		end = str + strlen(str);
	}
	rest = identifier(str, end);
	if (rest != end) {
		int len = end - str - 1;
		arg_error("Invalid name cannot define macro: `%*.*s'\n",
			len, len, str);
	}
	result = append_string(&compiler->define_count,
		&compiler->defines, str, "defines");
	return result;
}

static int append_undef(struct compiler_state *compiler, const char *str)
{
	const char *end, *rest;
	int result;

	end = str + strlen(str);
	rest = identifier(str, end);
	if (rest != end) {
		int len = end - str - 1;
		arg_error("Invalid name cannot undefine macro: `%*.*s'\n",
			len, len, str);
	}
	result = append_string(&compiler->undef_count,
		&compiler->undefs, str, "undefs");
	return result;
}

static const struct compiler_flag romcc_flags[] = {
	{ "trigraphs",                 COMPILER_TRIGRAPHS },
	{ "pp-only",                   COMPILER_PP_ONLY },
	{ "eliminate-inefectual-code", COMPILER_ELIMINATE_INEFECTUAL_CODE },
	{ "simplify",                  COMPILER_SIMPLIFY },
	{ "scc-transform",             COMPILER_SCC_TRANSFORM },
	{ "simplify-op",               COMPILER_SIMPLIFY_OP },
	{ "simplify-phi",              COMPILER_SIMPLIFY_PHI },
	{ "simplify-label",            COMPILER_SIMPLIFY_LABEL },
	{ "simplify-branch",           COMPILER_SIMPLIFY_BRANCH },
	{ "simplify-copy",             COMPILER_SIMPLIFY_COPY },
	{ "simplify-arith",            COMPILER_SIMPLIFY_ARITH },
	{ "simplify-shift",            COMPILER_SIMPLIFY_SHIFT },
	{ "simplify-bitwise",          COMPILER_SIMPLIFY_BITWISE },
	{ "simplify-logical",          COMPILER_SIMPLIFY_LOGICAL },
	{ "simplify-bitfield",         COMPILER_SIMPLIFY_BITFIELD },
	{ 0, 0 },
};
static const struct compiler_arg romcc_args[] = {
	{ "inline-policy",             COMPILER_INLINE_MASK,
		{
			{ "always",      COMPILER_INLINE_ALWAYS, },
			{ "never",       COMPILER_INLINE_NEVER, },
			{ "defaulton",   COMPILER_INLINE_DEFAULTON, },
			{ "defaultoff",  COMPILER_INLINE_DEFAULTOFF, },
			{ "nopenalty",   COMPILER_INLINE_NOPENALTY, },
			{ 0, 0 },
		},
	},
	{ 0, 0 },
};
static const struct compiler_flag romcc_opt_flags[] = {
	{ "-O",  COMPILER_SIMPLIFY },
	{ "-O2", COMPILER_SIMPLIFY | COMPILER_SCC_TRANSFORM },
	{ "-E",  COMPILER_PP_ONLY },
	{ 0, 0, },
};
static const struct compiler_flag romcc_debug_flags[] = {
	{ "all",                   DEBUG_ALL },
	{ "abort-on-error",        DEBUG_ABORT_ON_ERROR },
	{ "basic-blocks",          DEBUG_BASIC_BLOCKS },
	{ "fdominators",           DEBUG_FDOMINATORS },
	{ "rdominators",           DEBUG_RDOMINATORS },
	{ "triples",               DEBUG_TRIPLES },
	{ "interference",          DEBUG_INTERFERENCE },
	{ "scc-transform",         DEBUG_SCC_TRANSFORM },
	{ "scc-transform2",        DEBUG_SCC_TRANSFORM2 },
	{ "rebuild-ssa-form",      DEBUG_REBUILD_SSA_FORM },
	{ "inline",                DEBUG_INLINE },
	{ "live-range-conflicts",  DEBUG_RANGE_CONFLICTS },
	{ "live-range-conflicts2", DEBUG_RANGE_CONFLICTS2 },
	{ "color-graph",           DEBUG_COLOR_GRAPH },
	{ "color-graph2",          DEBUG_COLOR_GRAPH2 },
	{ "coalescing",            DEBUG_COALESCING },
	{ "coalescing2",           DEBUG_COALESCING2 },
	{ "verification",          DEBUG_VERIFICATION },
	{ "calls",                 DEBUG_CALLS },
	{ "calls2",                DEBUG_CALLS2 },
	{ "tokens",                DEBUG_TOKENS },
	{ 0, 0 },
};

static int compiler_encode_flag(
	struct compiler_state *compiler, const char *flag)
{
	int act;
	int result;

	act = 1;
	result = -1;
	if (strncmp(flag, "no-", 3) == 0) {
		flag += 3;
		act = 0;
	}
	if (strncmp(flag, "-O", 2) == 0) {
		result = set_flag(romcc_opt_flags, &compiler->flags, act, flag);
	}
	else if (strncmp(flag, "-E", 2) == 0) {
		result = set_flag(romcc_opt_flags, &compiler->flags, act, flag);
	}
	else if (strncmp(flag, "-I", 2) == 0) {
		result = append_include_path(compiler, flag + 2);
	}
	else if (strncmp(flag, "-D", 2) == 0) {
		result = append_define(compiler, flag + 2);
	}
	else if (strncmp(flag, "-U", 2) == 0) {
		result = append_undef(compiler, flag + 2);
	}
	else if (act && strncmp(flag, "label-prefix=", 13) == 0) {
		result = 0;
		compiler->label_prefix = flag + 13;
	}
	else if (act && strncmp(flag, "max-allocation-passes=", 22) == 0) {
		unsigned long max_passes;
		char *end;
		max_passes = strtoul(flag + 22, &end, 10);
		if (end[0] == '\0') {
			result = 0;
			compiler->max_allocation_passes = max_passes;
		}
	}
	else if (act && strcmp(flag, "debug") == 0) {
		result = 0;
		compiler->debug |= DEBUG_DEFAULT;
	}
	else if (strncmp(flag, "debug-", 6) == 0) {
		flag += 6;
		result = set_flag(romcc_debug_flags, &compiler->debug, act, flag);
	}
	else {
		result = set_flag(romcc_flags, &compiler->flags, act, flag);
		if (result < 0) {
			result = set_arg(romcc_args, &compiler->flags, flag);
		}
	}
	return result;
}

static void compiler_usage(FILE *fp)
{
	flag_usage(fp, romcc_opt_flags, "", 0);
	flag_usage(fp, romcc_flags, "-f", "-fno-");
	arg_usage(fp,  romcc_args, "-f");
	flag_usage(fp, romcc_debug_flags, "-fdebug-", "-fno-debug-");
	fprintf(fp, "-flabel-prefix=<prefix for assembly language labels>\n");
	fprintf(fp, "--label-prefix=<prefix for assembly language labels>\n");
	fprintf(fp, "-I<include path>\n");
	fprintf(fp, "-D<macro>[=defn]\n");
	fprintf(fp, "-U<macro>\n");
}

static void do_cleanup(struct compile_state *state)
{
	if (state->output) {
		fclose(state->output);
		unlink(state->compiler->ofilename);
		state->output = 0;
	}
	if (state->dbgout) {
		fflush(state->dbgout);
	}
	if (state->errout) {
		fflush(state->errout);
	}
}

static struct compile_state *exit_state;
static void exit_cleanup(void)
{
	if (exit_state) {
		do_cleanup(exit_state);
	}
}

static int get_col(struct file_state *file)
{
	int col;
	const char *ptr, *end;
	ptr = file->line_start;
	end = file->pos;
	for(col = 0; ptr < end; ptr++) {
		if (*ptr != '\t') {
			col++;
		}
		else {
			col = (col & ~7) + 8;
		}
	}
	return col;
}

static void loc(FILE *fp, struct compile_state *state, struct triple *triple)
{
	int col;
	if (triple && triple->occurrence) {
		struct occurrence *spot;
		for(spot = triple->occurrence; spot; spot = spot->parent) {
			fprintf(fp, "%s:%d.%d: ",
				spot->filename, spot->line, spot->col);
		}
		return;
	}
	if (!state->file) {
		return;
	}
	col = get_col(state->file);
	fprintf(fp, "%s:%d.%d: ",
		state->file->report_name, state->file->report_line, col);
}

static void __attribute__ ((noreturn)) internal_error(struct compile_state *state, struct triple *ptr,
	const char *fmt, ...)
{
	FILE *fp = state->errout;
	va_list args;
	va_start(args, fmt);
	loc(fp, state, ptr);
	fputc('\n', fp);
	if (ptr) {
		fprintf(fp, "%p %-10s ", ptr, tops(ptr->op));
	}
	fprintf(fp, "Internal compiler error: ");
	vfprintf(fp, fmt, args);
	fprintf(fp, "\n");
	va_end(args);
	do_cleanup(state);
	abort();
}


static void internal_warning(struct compile_state *state, struct triple *ptr,
	const char *fmt, ...)
{
	FILE *fp = state->errout;
	va_list args;
	va_start(args, fmt);
	loc(fp, state, ptr);
	if (ptr) {
		fprintf(fp, "%p %-10s ", ptr, tops(ptr->op));
	}
	fprintf(fp, "Internal compiler warning: ");
	vfprintf(fp, fmt, args);
	fprintf(fp, "\n");
	va_end(args);
}



static void __attribute__ ((noreturn)) error(struct compile_state *state, struct triple *ptr,
	const char *fmt, ...)
{
	FILE *fp = state->errout;
	va_list args;
	va_start(args, fmt);
	loc(fp, state, ptr);
	fputc('\n', fp);
	if (ptr && (state->compiler->debug & DEBUG_ABORT_ON_ERROR)) {
		fprintf(fp, "%p %-10s ", ptr, tops(ptr->op));
	}
	vfprintf(fp, fmt, args);
	va_end(args);
	fprintf(fp, "\n");
	do_cleanup(state);
	if (state->compiler->debug & DEBUG_ABORT_ON_ERROR) {
		abort();
	}
	exit(1);
}

static void warning(struct compile_state *state, struct triple *ptr,
	const char *fmt, ...)
{
	FILE *fp = state->errout;
	va_list args;
	va_start(args, fmt);
	loc(fp, state, ptr);
	fprintf(fp, "warning: ");
	if (ptr && (state->compiler->debug & DEBUG_ABORT_ON_ERROR)) {
		fprintf(fp, "%p %-10s ", ptr, tops(ptr->op));
	}
	vfprintf(fp, fmt, args);
	fprintf(fp, "\n");
	va_end(args);
}

#define FINISHME() warning(state, 0, "FINISHME @ %s.%s:%d", __FILE__, __func__, __LINE__)

static void valid_op(struct compile_state *state, int op)
{
	char *fmt = "invalid op: %d";
	if (op >= OP_MAX) {
		internal_error(state, 0, fmt, op);
	}
	if (op < 0) {
		internal_error(state, 0, fmt, op);
	}
}

static void valid_ins(struct compile_state *state, struct triple *ptr)
{
	valid_op(state, ptr->op);
}

#if DEBUG_ROMCC_WARNING
static void valid_param_count(struct compile_state *state, struct triple *ins)
{
	int lhs, rhs, misc, targ;
	valid_ins(state, ins);
	lhs  = table_ops[ins->op].lhs;
	rhs  = table_ops[ins->op].rhs;
	misc = table_ops[ins->op].misc;
	targ = table_ops[ins->op].targ;

	if ((lhs >= 0) && (ins->lhs != lhs)) {
		internal_error(state, ins, "Bad lhs count");
	}
	if ((rhs >= 0) && (ins->rhs != rhs)) {
		internal_error(state, ins, "Bad rhs count");
	}
	if ((misc >= 0) && (ins->misc != misc)) {
		internal_error(state, ins, "Bad misc count");
	}
	if ((targ >= 0) && (ins->targ != targ)) {
		internal_error(state, ins, "Bad targ count");
	}
}
#endif

static struct type void_type;
static struct type unknown_type;
static void use_triple(struct triple *used, struct triple *user)
{
	struct triple_set **ptr, *new;
	if (!used)
		return;
	if (!user)
		return;
	ptr = &used->use;
	while(*ptr) {
		if ((*ptr)->member == user) {
			return;
		}
		ptr = &(*ptr)->next;
	}
	/* Append new to the head of the list,
	 * copy_func and rename_block_variables
	 * depends on this.
	 */
	new = xcmalloc(sizeof(*new), "triple_set");
	new->member = user;
	new->next   = used->use;
	used->use   = new;
}

static void unuse_triple(struct triple *used, struct triple *unuser)
{
	struct triple_set *use, **ptr;
	if (!used) {
		return;
	}
	ptr = &used->use;
	while(*ptr) {
		use = *ptr;
		if (use->member == unuser) {
			*ptr = use->next;
			xfree(use);
		}
		else {
			ptr = &use->next;
		}
	}
}

static void put_occurrence(struct occurrence *occurrence)
{
	if (occurrence) {
		occurrence->count -= 1;
		if (occurrence->count <= 0) {
			if (occurrence->parent) {
				put_occurrence(occurrence->parent);
			}
			xfree(occurrence);
		}
	}
}

static void get_occurrence(struct occurrence *occurrence)
{
	if (occurrence) {
		occurrence->count += 1;
	}
}


static struct occurrence *new_occurrence(struct compile_state *state)
{
	struct occurrence *result, *last;
	const char *filename;
	const char *function;
	int line, col;

	function = "";
	filename = 0;
	line = 0;
	col  = 0;
	if (state->file) {
		filename = state->file->report_name;
		line     = state->file->report_line;
		col      = get_col(state->file);
	}
	if (state->function) {
		function = state->function;
	}
	last = state->last_occurrence;
	if (last &&
		(last->col == col) &&
		(last->line == line) &&
		(last->function == function) &&
		((last->filename == filename) ||
			(strcmp(last->filename, filename) == 0)))
	{
		get_occurrence(last);
		return last;
	}
	if (last) {
		state->last_occurrence = 0;
		put_occurrence(last);
	}
	result = xmalloc(sizeof(*result), "occurrence");
	result->count    = 2;
	result->filename = filename;
	result->function = function;
	result->line     = line;
	result->col      = col;
	result->parent   = 0;
	state->last_occurrence = result;
	return result;
}

static struct occurrence *inline_occurrence(struct compile_state *state,
	struct occurrence *base, struct occurrence *top)
{
	struct occurrence *result, *last;
	if (top->parent) {
		internal_error(state, 0, "inlining an already inlined function?");
	}
	/* If I have a null base treat it that way */
	if ((base->parent == 0) &&
		(base->col == 0) &&
		(base->line == 0) &&
		(base->function[0] == '\0') &&
		(base->filename[0] == '\0')) {
		base = 0;
	}
	/* See if I can reuse the last occurrence I had */
	last = state->last_occurrence;
	if (last &&
		(last->parent   == base) &&
		(last->col      == top->col) &&
		(last->line     == top->line) &&
		(last->function == top->function) &&
		(last->filename == top->filename)) {
		get_occurrence(last);
		return last;
	}
	/* I can't reuse the last occurrence so free it */
	if (last) {
		state->last_occurrence = 0;
		put_occurrence(last);
	}
	/* Generate a new occurrence structure */
	get_occurrence(base);
	result = xmalloc(sizeof(*result), "occurrence");
	result->count    = 2;
	result->filename = top->filename;
	result->function = top->function;
	result->line     = top->line;
	result->col      = top->col;
	result->parent   = base;
	state->last_occurrence = result;
	return result;
}

static struct occurrence dummy_occurrence = {
	.count    = 2,
	.filename = __FILE__,
	.function = "",
	.line     = __LINE__,
	.col      = 0,
	.parent   = 0,
};

/* The undef triple is used as a place holder when we are removing pointers
 * from a triple.  Having allows certain sanity checks to pass even
 * when the original triple that was pointed to is gone.
 */
static struct triple unknown_triple = {
	.next      = &unknown_triple,
	.prev      = &unknown_triple,
	.use       = 0,
	.op        = OP_UNKNOWNVAL,
	.lhs       = 0,
	.rhs       = 0,
	.misc      = 0,
	.targ      = 0,
	.type      = &unknown_type,
	.id        = -1, /* An invalid id */
	.u = { .cval = 0, },
	.occurrence = &dummy_occurrence,
	.param = { [0] = 0, [1] = 0, },
};


static size_t registers_of(struct compile_state *state, struct type *type);

static struct triple *alloc_triple(struct compile_state *state,
	int op, struct type *type, int lhs_wanted, int rhs_wanted,
	struct occurrence *occurrence)
{
	size_t size, extra_count, min_count;
	int lhs, rhs, misc, targ;
	struct triple *ret, dummy;
	dummy.op = op;
	dummy.occurrence = occurrence;
	valid_op(state, op);
	lhs = table_ops[op].lhs;
	rhs = table_ops[op].rhs;
	misc = table_ops[op].misc;
	targ = table_ops[op].targ;

	switch(op) {
	case OP_FCALL:
		rhs = rhs_wanted;
		break;
	case OP_PHI:
		rhs = rhs_wanted;
		break;
	case OP_ADECL:
		lhs = registers_of(state, type);
		break;
	case OP_TUPLE:
		lhs = registers_of(state, type);
		break;
	case OP_ASM:
		rhs = rhs_wanted;
		lhs = lhs_wanted;
		break;
	}
	if ((rhs < 0) || (rhs > MAX_RHS)) {
		internal_error(state, &dummy, "bad rhs count %d", rhs);
	}
	if ((lhs < 0) || (lhs > MAX_LHS)) {
		internal_error(state, &dummy, "bad lhs count %d", lhs);
	}
	if ((misc < 0) || (misc > MAX_MISC)) {
		internal_error(state, &dummy, "bad misc count %d", misc);
	}
	if ((targ < 0) || (targ > MAX_TARG)) {
		internal_error(state, &dummy, "bad targs count %d", targ);
	}

	min_count = sizeof(ret->param)/sizeof(ret->param[0]);
	extra_count = lhs + rhs + misc + targ;
	extra_count = (extra_count < min_count)? 0 : extra_count - min_count;

	size = sizeof(*ret) + sizeof(ret->param[0]) * extra_count;
	ret = xcmalloc(size, "tripple");
	ret->op        = op;
	ret->lhs       = lhs;
	ret->rhs       = rhs;
	ret->misc      = misc;
	ret->targ      = targ;
	ret->type      = type;
	ret->next      = ret;
	ret->prev      = ret;
	ret->occurrence = occurrence;
	/* A simple sanity check */
	if ((ret->op != op) ||
		(ret->lhs != lhs) ||
		(ret->rhs != rhs) ||
		(ret->misc != misc) ||
		(ret->targ != targ) ||
		(ret->type != type) ||
		(ret->next != ret) ||
		(ret->prev != ret) ||
		(ret->occurrence != occurrence)) {
		internal_error(state, ret, "huh?");
	}
	return ret;
}

struct triple *dup_triple(struct compile_state *state, struct triple *src)
{
	struct triple *dup;
	int src_lhs, src_rhs, src_size;
	src_lhs = src->lhs;
	src_rhs = src->rhs;
	src_size = TRIPLE_SIZE(src);
	get_occurrence(src->occurrence);
	dup = alloc_triple(state, src->op, src->type, src_lhs, src_rhs,
		src->occurrence);
	memcpy(dup, src, sizeof(*src));
	memcpy(dup->param, src->param, src_size * sizeof(src->param[0]));
	return dup;
}

static struct triple *copy_triple(struct compile_state *state, struct triple *src)
{
	struct triple *copy;
	copy = dup_triple(state, src);
	copy->use = 0;
	copy->next = copy->prev = copy;
	return copy;
}

static struct triple *new_triple(struct compile_state *state,
	int op, struct type *type, int lhs, int rhs)
{
	struct triple *ret;
	struct occurrence *occurrence;
	occurrence = new_occurrence(state);
	ret = alloc_triple(state, op, type, lhs, rhs, occurrence);
	return ret;
}

static struct triple *build_triple(struct compile_state *state,
	int op, struct type *type, struct triple *left, struct triple *right,
	struct occurrence *occurrence)
{
	struct triple *ret;
	size_t count;
	ret = alloc_triple(state, op, type, -1, -1, occurrence);
	count = TRIPLE_SIZE(ret);
	if (count > 0) {
		ret->param[0] = left;
	}
	if (count > 1) {
		ret->param[1] = right;
	}
	return ret;
}

static struct triple *triple(struct compile_state *state,
	int op, struct type *type, struct triple *left, struct triple *right)
{
	struct triple *ret;
	size_t count;
	ret = new_triple(state, op, type, -1, -1);
	count = TRIPLE_SIZE(ret);
	if (count >= 1) {
		ret->param[0] = left;
	}
	if (count >= 2) {
		ret->param[1] = right;
	}
	return ret;
}

static struct triple *branch(struct compile_state *state,
	struct triple *targ, struct triple *test)
{
	struct triple *ret;
	if (test) {
		ret = new_triple(state, OP_CBRANCH, &void_type, -1, 1);
		RHS(ret, 0) = test;
	} else {
		ret = new_triple(state, OP_BRANCH, &void_type, -1, 0);
	}
	TARG(ret, 0) = targ;
	/* record the branch target was used */
	if (!targ || (targ->op != OP_LABEL)) {
		internal_error(state, 0, "branch not to label");
	}
	return ret;
}

static int triple_is_label(struct compile_state *state, struct triple *ins);
static int triple_is_call(struct compile_state *state, struct triple *ins);
static int triple_is_cbranch(struct compile_state *state, struct triple *ins);
static void insert_triple(struct compile_state *state,
	struct triple *first, struct triple *ptr)
{
	if (ptr) {
		if ((ptr->id & TRIPLE_FLAG_FLATTENED) || (ptr->next != ptr)) {
			internal_error(state, ptr, "expression already used");
		}
		ptr->next       = first;
		ptr->prev       = first->prev;
		ptr->prev->next = ptr;
		ptr->next->prev = ptr;

		if (triple_is_cbranch(state, ptr->prev) ||
			triple_is_call(state, ptr->prev)) {
			unuse_triple(first, ptr->prev);
			use_triple(ptr, ptr->prev);
		}
	}
}

static int triple_stores_block(struct compile_state *state, struct triple *ins)
{
	/* This function is used to determine if u.block
	 * is utilized to store the current block number.
	 */
	int stores_block;
	valid_ins(state, ins);
	stores_block = (table_ops[ins->op].flags & BLOCK) == BLOCK;
	return stores_block;
}

static int triple_is_branch(struct compile_state *state, struct triple *ins);
static struct block *block_of_triple(struct compile_state *state,
	struct triple *ins)
{
	struct triple *first;
	if (!ins || ins == &unknown_triple) {
		return 0;
	}
	first = state->first;
	while(ins != first && !triple_is_branch(state, ins->prev) &&
		!triple_stores_block(state, ins))
	{
		if (ins == ins->prev) {
			internal_error(state, ins, "ins == ins->prev?");
		}
		ins = ins->prev;
	}
	return triple_stores_block(state, ins)? ins->u.block: 0;
}

static void generate_lhs_pieces(struct compile_state *state, struct triple *ins);
static struct triple *pre_triple(struct compile_state *state,
	struct triple *base,
	int op, struct type *type, struct triple *left, struct triple *right)
{
	struct block *block;
	struct triple *ret;
	int i;
	/* If I am an OP_PIECE jump to the real instruction */
	if (base->op == OP_PIECE) {
		base = MISC(base, 0);
	}
	block = block_of_triple(state, base);
	get_occurrence(base->occurrence);
	ret = build_triple(state, op, type, left, right, base->occurrence);
	generate_lhs_pieces(state, ret);
	if (triple_stores_block(state, ret)) {
		ret->u.block = block;
	}
	insert_triple(state, base, ret);
	for(i = 0; i < ret->lhs; i++) {
		struct triple *piece;
		piece = LHS(ret, i);
		insert_triple(state, base, piece);
		use_triple(ret, piece);
		use_triple(piece, ret);
	}
	if (block && (block->first == base)) {
		block->first = ret;
	}
	return ret;
}

static struct triple *post_triple(struct compile_state *state,
	struct triple *base,
	int op, struct type *type, struct triple *left, struct triple *right)
{
	struct block *block;
	struct triple *ret, *next;
	int zlhs, i;
	/* If I am an OP_PIECE jump to the real instruction */
	if (base->op == OP_PIECE) {
		base = MISC(base, 0);
	}
	/* If I have a left hand side skip over it */
	zlhs = base->lhs;
	if (zlhs) {
		base = LHS(base, zlhs - 1);
	}

	block = block_of_triple(state, base);
	get_occurrence(base->occurrence);
	ret = build_triple(state, op, type, left, right, base->occurrence);
	generate_lhs_pieces(state, ret);
	if (triple_stores_block(state, ret)) {
		ret->u.block = block;
	}
	next = base->next;
	insert_triple(state, next, ret);
	zlhs = ret->lhs;
	for(i = 0; i < zlhs; i++) {
		struct triple *piece;
		piece = LHS(ret, i);
		insert_triple(state, next, piece);
		use_triple(ret, piece);
		use_triple(piece, ret);
	}
	if (block && (block->last == base)) {
		block->last = ret;
		if (zlhs) {
			block->last = LHS(ret, zlhs - 1);
		}
	}
	return ret;
}

static struct type *reg_type(
	struct compile_state *state, struct type *type, int reg);

static void generate_lhs_piece(
	struct compile_state *state, struct triple *ins, int index)
{
	struct type *piece_type;
	struct triple *piece;
	get_occurrence(ins->occurrence);
	piece_type = reg_type(state, ins->type, index * REG_SIZEOF_REG);

	if ((piece_type->type & TYPE_MASK) == TYPE_BITFIELD) {
		piece_type = piece_type->left;
	}
#if 0
{
	static void name_of(FILE *fp, struct type *type);
	FILE * fp = state->errout;
	fprintf(fp, "piece_type(%d): ", index);
	name_of(fp, piece_type);
	fprintf(fp, "\n");
}
#endif
	piece = alloc_triple(state, OP_PIECE, piece_type, -1, -1, ins->occurrence);
	piece->u.cval  = index;
	LHS(ins, piece->u.cval) = piece;
	MISC(piece, 0) = ins;
}

static void generate_lhs_pieces(struct compile_state *state, struct triple *ins)
{
	int i, zlhs;
	zlhs = ins->lhs;
	for(i = 0; i < zlhs; i++) {
		generate_lhs_piece(state, ins, i);
	}
}

static struct triple *label(struct compile_state *state)
{
	/* Labels don't get a type */
	struct triple *result;
	result = triple(state, OP_LABEL, &void_type, 0, 0);
	return result;
}

static struct triple *mkprog(struct compile_state *state, ...)
{
	struct triple *prog, *head, *arg;
	va_list args;
	int i;

	head = label(state);
	prog = new_triple(state, OP_PROG, &void_type, -1, -1);
	RHS(prog, 0) = head;
	va_start(args, state);
	i = 0;
	while((arg = va_arg(args, struct triple *)) != 0) {
		if (++i >= 100) {
			internal_error(state, 0, "too many arguments to mkprog");
		}
		flatten(state, head, arg);
	}
	va_end(args);
	prog->type = head->prev->type;
	return prog;
}
static void name_of(FILE *fp, struct type *type);
static void display_triple(FILE *fp, struct triple *ins)
{
	struct occurrence *ptr;
	const char *reg;
	char pre, post, vol;
	pre = post = vol = ' ';
	if (ins) {
		if (ins->id & TRIPLE_FLAG_PRE_SPLIT) {
			pre = '^';
		}
		if (ins->id & TRIPLE_FLAG_POST_SPLIT) {
			post = ',';
		}
		if (ins->id & TRIPLE_FLAG_VOLATILE) {
			vol = 'v';
		}
		reg = arch_reg_str(ID_REG(ins->id));
	}
	if (ins == 0) {
		fprintf(fp, "(%p) <nothing> ", ins);
	}
	else if (ins->op == OP_INTCONST) {
		fprintf(fp, "(%p) %c%c%c %-7s %-2d %-10s <0x%08lx>         ",
			ins, pre, post, vol, reg, ins->template_id, tops(ins->op),
			(unsigned long)(ins->u.cval));
	}
	else if (ins->op == OP_ADDRCONST) {
		fprintf(fp, "(%p) %c%c%c %-7s %-2d %-10s %-10p <0x%08lx>",
			ins, pre, post, vol, reg, ins->template_id, tops(ins->op),
			MISC(ins, 0), (unsigned long)(ins->u.cval));
	}
	else if (ins->op == OP_INDEX) {
		fprintf(fp, "(%p) %c%c%c %-7s %-2d %-10s %-10p <0x%08lx>",
			ins, pre, post, vol, reg, ins->template_id, tops(ins->op),
			RHS(ins, 0), (unsigned long)(ins->u.cval));
	}
	else if (ins->op == OP_PIECE) {
		fprintf(fp, "(%p) %c%c%c %-7s %-2d %-10s %-10p <0x%08lx>",
			ins, pre, post, vol, reg, ins->template_id, tops(ins->op),
			MISC(ins, 0), (unsigned long)(ins->u.cval));
	}
	else {
		int i, count;
		fprintf(fp, "(%p) %c%c%c %-7s %-2d %-10s",
			ins, pre, post, vol, reg, ins->template_id, tops(ins->op));
		if (table_ops[ins->op].flags & BITFIELD) {
			fprintf(fp, " <%2d-%2d:%2d>",
				ins->u.bitfield.offset,
				ins->u.bitfield.offset + ins->u.bitfield.size,
				ins->u.bitfield.size);
		}
		count = TRIPLE_SIZE(ins);
		for(i = 0; i < count; i++) {
			fprintf(fp, " %-10p", ins->param[i]);
		}
		for(; i < 2; i++) {
			fprintf(fp, "           ");
		}
	}
	if (ins) {
		struct triple_set *user;
#if DEBUG_DISPLAY_TYPES
		fprintf(fp, " <");
		name_of(fp, ins->type);
		fprintf(fp, "> ");
#endif
#if DEBUG_DISPLAY_USES
		fprintf(fp, " [");
		for(user = ins->use; user; user = user->next) {
			fprintf(fp, " %-10p", user->member);
		}
		fprintf(fp, " ]");
#endif
		fprintf(fp, " @");
		for(ptr = ins->occurrence; ptr; ptr = ptr->parent) {
			fprintf(fp, " %s,%s:%d.%d",
				ptr->function,
				ptr->filename,
				ptr->line,
				ptr->col);
		}
		if (ins->op == OP_ASM) {
			fprintf(fp, "\n\t%s", ins->u.ainfo->str);
		}
	}
	fprintf(fp, "\n");
	fflush(fp);
}

static int equiv_types(struct type *left, struct type *right);
static void display_triple_changes(
	FILE *fp, const struct triple *new, const struct triple *orig)
{

	int new_count, orig_count;
	new_count = TRIPLE_SIZE(new);
	orig_count = TRIPLE_SIZE(orig);
	if ((new->op != orig->op) ||
		(new_count != orig_count) ||
		(memcmp(orig->param, new->param,
			orig_count * sizeof(orig->param[0])) != 0) ||
		(memcmp(&orig->u, &new->u, sizeof(orig->u)) != 0))
	{
		struct occurrence *ptr;
		int i, min_count, indent;
		fprintf(fp, "(%p %p)", new, orig);
		if (orig->op == new->op) {
			fprintf(fp, " %-11s", tops(orig->op));
		} else {
			fprintf(fp, " [%-10s %-10s]",
				tops(new->op), tops(orig->op));
		}
		min_count = new_count;
		if (min_count > orig_count) {
			min_count = orig_count;
		}
		for(indent = i = 0; i < min_count; i++) {
			if (orig->param[i] == new->param[i]) {
				fprintf(fp, " %-11p",
					orig->param[i]);
				indent += 12;
			} else {
				fprintf(fp, " [%-10p %-10p]",
					new->param[i],
					orig->param[i]);
				indent += 24;
			}
		}
		for(; i < orig_count; i++) {
			fprintf(fp, " [%-9p]", orig->param[i]);
			indent += 12;
		}
		for(; i < new_count; i++) {
			fprintf(fp, " [%-9p]", new->param[i]);
			indent += 12;
		}
		if ((new->op == OP_INTCONST)||
			(new->op == OP_ADDRCONST)) {
			fprintf(fp, " <0x%08lx>",
				(unsigned long)(new->u.cval));
			indent += 13;
		}
		for(;indent < 36; indent++) {
			putc(' ', fp);
		}

#if DEBUG_DISPLAY_TYPES
		fprintf(fp, " <");
		name_of(fp, new->type);
		if (!equiv_types(new->type, orig->type)) {
			fprintf(fp, " -- ");
			name_of(fp, orig->type);
		}
		fprintf(fp, "> ");
#endif

		fprintf(fp, " @");
		for(ptr = orig->occurrence; ptr; ptr = ptr->parent) {
			fprintf(fp, " %s,%s:%d.%d",
				ptr->function,
				ptr->filename,
				ptr->line,
				ptr->col);

		}
		fprintf(fp, "\n");
		fflush(fp);
	}
}

static int triple_is_pure(struct compile_state *state, struct triple *ins, unsigned id)
{
	/* Does the triple have no side effects.
	 * I.e. Rexecuting the triple with the same arguments
	 * gives the same value.
	 */
	unsigned pure;
	valid_ins(state, ins);
	pure = PURE_BITS(table_ops[ins->op].flags);
	if ((pure != PURE) && (pure != IMPURE)) {
		internal_error(state, 0, "Purity of %s not known",
			tops(ins->op));
	}
	return (pure == PURE) && !(id & TRIPLE_FLAG_VOLATILE);
}

static int triple_is_branch_type(struct compile_state *state,
	struct triple *ins, unsigned type)
{
	/* Is this one of the passed branch types? */
	valid_ins(state, ins);
	return (BRANCH_BITS(table_ops[ins->op].flags) == type);
}

static int triple_is_branch(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a branch instruction? */
	valid_ins(state, ins);
	return (BRANCH_BITS(table_ops[ins->op].flags) != 0);
}

static int triple_is_cbranch(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a conditional branch instruction? */
	return triple_is_branch_type(state, ins, CBRANCH);
}

static int triple_is_ubranch(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a unconditional branch instruction? */
	unsigned type;
	valid_ins(state, ins);
	type = BRANCH_BITS(table_ops[ins->op].flags);
	return (type != 0) && (type != CBRANCH);
}

static int triple_is_call(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a call instruction? */
	return triple_is_branch_type(state, ins, CALLBRANCH);
}

static int triple_is_ret(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a return instruction? */
	return triple_is_branch_type(state, ins, RETBRANCH);
}

#if DEBUG_ROMCC_WARNING
static int triple_is_simple_ubranch(struct compile_state *state, struct triple *ins)
{
	/* Is this triple an unconditional branch and not a call or a
	 * return? */
	return triple_is_branch_type(state, ins, UBRANCH);
}
#endif

static int triple_is_end(struct compile_state *state, struct triple *ins)
{
	return triple_is_branch_type(state, ins, ENDBRANCH);
}

static int triple_is_label(struct compile_state *state, struct triple *ins)
{
	valid_ins(state, ins);
	return (ins->op == OP_LABEL);
}

static struct triple *triple_to_block_start(
	struct compile_state *state, struct triple *start)
{
	while(!triple_is_branch(state, start->prev) &&
		(!triple_is_label(state, start) || !start->use)) {
		start = start->prev;
	}
	return start;
}

static int triple_is_def(struct compile_state *state, struct triple *ins)
{
	/* This function is used to determine which triples need
	 * a register.
	 */
	int is_def;
	valid_ins(state, ins);
	is_def = (table_ops[ins->op].flags & DEF) == DEF;
	if (ins->lhs >= 1) {
		is_def = 0;
	}
	return is_def;
}

static int triple_is_structural(struct compile_state *state, struct triple *ins)
{
	int is_structural;
	valid_ins(state, ins);
	is_structural = (table_ops[ins->op].flags & STRUCTURAL) == STRUCTURAL;
	return is_structural;
}

static int triple_is_part(struct compile_state *state, struct triple *ins)
{
	int is_part;
	valid_ins(state, ins);
	is_part = (table_ops[ins->op].flags & PART) == PART;
	return is_part;
}

static int triple_is_auto_var(struct compile_state *state, struct triple *ins)
{
	return (ins->op == OP_PIECE) && (MISC(ins, 0)->op == OP_ADECL);
}

static struct triple **triple_iter(struct compile_state *state,
	size_t count, struct triple **vector,
	struct triple *ins, struct triple **last)
{
	struct triple **ret;
	ret = 0;
	if (count) {
		if (!last) {
			ret = vector;
		}
		else if ((last >= vector) && (last < (vector + count - 1))) {
			ret = last + 1;
		}
	}
	return ret;

}

static struct triple **triple_lhs(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	return triple_iter(state, ins->lhs, &LHS(ins,0),
		ins, last);
}

static struct triple **triple_rhs(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	return triple_iter(state, ins->rhs, &RHS(ins,0),
		ins, last);
}

static struct triple **triple_misc(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	return triple_iter(state, ins->misc, &MISC(ins,0),
		ins, last);
}

static struct triple **do_triple_targ(struct compile_state *state,
	struct triple *ins, struct triple **last, int call_edges, int next_edges)
{
	size_t count;
	struct triple **ret, **vector;
	int next_is_targ;
	ret = 0;
	count = ins->targ;
	next_is_targ = 0;
	if (triple_is_cbranch(state, ins)) {
		next_is_targ = 1;
	}
	if (!call_edges && triple_is_call(state, ins)) {
		count = 0;
	}
	if (next_edges && triple_is_call(state, ins)) {
		next_is_targ = 1;
	}
	vector = &TARG(ins, 0);
	if (!ret && next_is_targ) {
		if (!last) {
			ret = &ins->next;
		} else if (last == &ins->next) {
			last = 0;
		}
	}
	if (!ret && count) {
		if (!last) {
			ret = vector;
		}
		else if ((last >= vector) && (last < (vector + count - 1))) {
			ret = last + 1;
		}
		else if (last == vector + count - 1) {
			last = 0;
		}
	}
	if (!ret && triple_is_ret(state, ins) && call_edges) {
		struct triple_set *use;
		for(use = ins->use; use; use = use->next) {
			if (!triple_is_call(state, use->member)) {
				continue;
			}
			if (!last) {
				ret = &use->member->next;
				break;
			}
			else if (last == &use->member->next) {
				last = 0;
			}
		}
	}
	return ret;
}

static struct triple **triple_targ(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	return do_triple_targ(state, ins, last, 1, 1);
}

static struct triple **triple_edge_targ(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	return do_triple_targ(state, ins, last,
		state->functions_joined, !state->functions_joined);
}

static struct triple *after_lhs(struct compile_state *state, struct triple *ins)
{
	struct triple *next;
	int lhs, i;
	lhs = ins->lhs;
	next = ins->next;
	for(i = 0; i < lhs; i++) {
		struct triple *piece;
		piece = LHS(ins, i);
		if (next != piece) {
			internal_error(state, ins, "malformed lhs on %s",
				tops(ins->op));
		}
		if (next->op != OP_PIECE) {
			internal_error(state, ins, "bad lhs op %s at %d on %s",
				tops(next->op), i, tops(ins->op));
		}
		if (next->u.cval != i) {
			internal_error(state, ins, "bad u.cval of %d %d expected",
				next->u.cval, i);
		}
		next = next->next;
	}
	return next;
}

/* Function piece accessor functions */
static struct triple *do_farg(struct compile_state *state,
	struct triple *func, unsigned index)
{
	struct type *ftype;
	struct triple *first, *arg;
	unsigned i;

	ftype = func->type;
	if(index >= (ftype->elements + 2)) {
		internal_error(state, func, "bad argument index: %d", index);
	}
	first = RHS(func, 0);
	arg = first->next;
	for(i = 0; i < index; i++, arg = after_lhs(state, arg)) {
		/* do nothing */
	}
	if (arg->op != OP_ADECL) {
		internal_error(state, 0, "arg not adecl?");
	}
	return arg;
}
static struct triple *fresult(struct compile_state *state, struct triple *func)
{
	return do_farg(state, func, 0);
}
static struct triple *fretaddr(struct compile_state *state, struct triple *func)
{
	return do_farg(state, func, 1);
}
static struct triple *farg(struct compile_state *state,
	struct triple *func, unsigned index)
{
	return do_farg(state, func, index + 2);
}


static void display_func(struct compile_state *state, FILE *fp, struct triple *func)
{
	struct triple *first, *ins;
	fprintf(fp, "display_func %s\n", func->type->type_ident->name);
	first = ins = RHS(func, 0);
	do {
		if (triple_is_label(state, ins) && ins->use) {
			fprintf(fp, "%p:\n", ins);
		}
		display_triple(fp, ins);

		if (triple_is_branch(state, ins)) {
			fprintf(fp, "\n");
		}
		if (ins->next->prev != ins) {
			internal_error(state, ins->next, "bad prev");
		}
		ins = ins->next;
	} while(ins != first);
}

static void verify_use(struct compile_state *state,
	struct triple *user, struct triple *used)
{
	int size, i;
	size = TRIPLE_SIZE(user);
	for(i = 0; i < size; i++) {
		if (user->param[i] == used) {
			break;
		}
	}
	if (triple_is_branch(state, user)) {
		if (user->next == used) {
			i = -1;
		}
	}
	if (i == size) {
		internal_error(state, user, "%s(%p) does not use %s(%p)",
			tops(user->op), user, tops(used->op), used);
	}
}

static int find_rhs_use(struct compile_state *state,
	struct triple *user, struct triple *used)
{
	struct triple **param;
	int size, i;
	verify_use(state, user, used);

#if DEBUG_ROMCC_WARNINGS
#warning "AUDIT ME ->rhs"
#endif
	size = user->rhs;
	param = &RHS(user, 0);
	for(i = 0; i < size; i++) {
		if (param[i] == used) {
			return i;
		}
	}
	return -1;
}

static void free_triple(struct compile_state *state, struct triple *ptr)
{
	size_t size;
	size = sizeof(*ptr) - sizeof(ptr->param) +
		(sizeof(ptr->param[0])*TRIPLE_SIZE(ptr));
	ptr->prev->next = ptr->next;
	ptr->next->prev = ptr->prev;
	if (ptr->use) {
		internal_error(state, ptr, "ptr->use != 0");
	}
	put_occurrence(ptr->occurrence);
	memset(ptr, -1, size);
	xfree(ptr);
}

static void release_triple(struct compile_state *state, struct triple *ptr)
{
	struct triple_set *set, *next;
	struct triple **expr;
	struct block *block;
	if (ptr == &unknown_triple) {
		return;
	}
	valid_ins(state, ptr);
	/* Make certain the we are not the first or last element of a block */
	block = block_of_triple(state, ptr);
	if (block) {
		if ((block->last == ptr) && (block->first == ptr)) {
			block->last = block->first = 0;
		}
		else if (block->last == ptr) {
			block->last = ptr->prev;
		}
		else if (block->first == ptr) {
			block->first = ptr->next;
		}
	}
	/* Remove ptr from use chains where it is the user */
	expr = triple_rhs(state, ptr, 0);
	for(; expr; expr = triple_rhs(state, ptr, expr)) {
		if (*expr) {
			unuse_triple(*expr, ptr);
		}
	}
	expr = triple_lhs(state, ptr, 0);
	for(; expr; expr = triple_lhs(state, ptr, expr)) {
		if (*expr) {
			unuse_triple(*expr, ptr);
		}
	}
	expr = triple_misc(state, ptr, 0);
	for(; expr; expr = triple_misc(state, ptr, expr)) {
		if (*expr) {
			unuse_triple(*expr, ptr);
		}
	}
	expr = triple_targ(state, ptr, 0);
	for(; expr; expr = triple_targ(state, ptr, expr)) {
		if (*expr){
			unuse_triple(*expr, ptr);
		}
	}
	/* Reomve ptr from use chains where it is used */
	for(set = ptr->use; set; set = next) {
		next = set->next;
		valid_ins(state, set->member);
		expr = triple_rhs(state, set->member, 0);
		for(; expr; expr = triple_rhs(state, set->member, expr)) {
			if (*expr == ptr) {
				*expr = &unknown_triple;
			}
		}
		expr = triple_lhs(state, set->member, 0);
		for(; expr; expr = triple_lhs(state, set->member, expr)) {
			if (*expr == ptr) {
				*expr = &unknown_triple;
			}
		}
		expr = triple_misc(state, set->member, 0);
		for(; expr; expr = triple_misc(state, set->member, expr)) {
			if (*expr == ptr) {
				*expr = &unknown_triple;
			}
		}
		expr = triple_targ(state, set->member, 0);
		for(; expr; expr = triple_targ(state, set->member, expr)) {
			if (*expr == ptr) {
				*expr = &unknown_triple;
			}
		}
		unuse_triple(ptr, set->member);
	}
	free_triple(state, ptr);
}

static void print_triples(struct compile_state *state);
static void print_blocks(struct compile_state *state, const char *func, FILE *fp);

#define TOK_UNKNOWN       0
#define TOK_SPACE         1
#define TOK_SEMI          2
#define TOK_LBRACE        3
#define TOK_RBRACE        4
#define TOK_COMMA         5
#define TOK_EQ            6
#define TOK_COLON         7
#define TOK_LBRACKET      8
#define TOK_RBRACKET      9
#define TOK_LPAREN        10
#define TOK_RPAREN        11
#define TOK_STAR          12
#define TOK_DOTS          13
#define TOK_MORE          14
#define TOK_LESS          15
#define TOK_TIMESEQ       16
#define TOK_DIVEQ         17
#define TOK_MODEQ         18
#define TOK_PLUSEQ        19
#define TOK_MINUSEQ       20
#define TOK_SLEQ          21
#define TOK_SREQ          22
#define TOK_ANDEQ         23
#define TOK_XOREQ         24
#define TOK_OREQ          25
#define TOK_EQEQ          26
#define TOK_NOTEQ         27
#define TOK_QUEST         28
#define TOK_LOGOR         29
#define TOK_LOGAND        30
#define TOK_OR            31
#define TOK_AND           32
#define TOK_XOR           33
#define TOK_LESSEQ        34
#define TOK_MOREEQ        35
#define TOK_SL            36
#define TOK_SR            37
#define TOK_PLUS          38
#define TOK_MINUS         39
#define TOK_DIV           40
#define TOK_MOD           41
#define TOK_PLUSPLUS      42
#define TOK_MINUSMINUS    43
#define TOK_BANG          44
#define TOK_ARROW         45
#define TOK_DOT           46
#define TOK_TILDE         47
#define TOK_LIT_STRING    48
#define TOK_LIT_CHAR      49
#define TOK_LIT_INT       50
#define TOK_LIT_FLOAT     51
#define TOK_MACRO         52
#define TOK_CONCATENATE   53

#define TOK_IDENT         54
#define TOK_STRUCT_NAME   55
#define TOK_ENUM_CONST    56
#define TOK_TYPE_NAME     57

#define TOK_AUTO          58
#define TOK_BREAK         59
#define TOK_CASE          60
#define TOK_CHAR          61
#define TOK_CONST         62
#define TOK_CONTINUE      63
#define TOK_DEFAULT       64
#define TOK_DO            65
#define TOK_DOUBLE        66
#define TOK_ELSE          67
#define TOK_ENUM          68
#define TOK_EXTERN        69
#define TOK_FLOAT         70
#define TOK_FOR           71
#define TOK_GOTO          72
#define TOK_IF            73
#define TOK_INLINE        74
#define TOK_INT           75
#define TOK_LONG          76
#define TOK_REGISTER      77
#define TOK_RESTRICT      78
#define TOK_RETURN        79
#define TOK_SHORT         80
#define TOK_SIGNED        81
#define TOK_SIZEOF        82
#define TOK_STATIC        83
#define TOK_STRUCT        84
#define TOK_SWITCH        85
#define TOK_TYPEDEF       86
#define TOK_UNION         87
#define TOK_UNSIGNED      88
#define TOK_VOID          89
#define TOK_VOLATILE      90
#define TOK_WHILE         91
#define TOK_ASM           92
#define TOK_ATTRIBUTE     93
#define TOK_ALIGNOF       94
#define TOK_FIRST_KEYWORD TOK_AUTO
#define TOK_LAST_KEYWORD  TOK_ALIGNOF

#define TOK_MDEFINE       100
#define TOK_MDEFINED      101
#define TOK_MUNDEF        102
#define TOK_MINCLUDE      103
#define TOK_MLINE         104
#define TOK_MERROR        105
#define TOK_MWARNING      106
#define TOK_MPRAGMA       107
#define TOK_MIFDEF        108
#define TOK_MIFNDEF       109
#define TOK_MELIF         110
#define TOK_MENDIF        111

#define TOK_FIRST_MACRO   TOK_MDEFINE
#define TOK_LAST_MACRO    TOK_MENDIF

#define TOK_MIF           112
#define TOK_MELSE         113
#define TOK_MIDENT        114

#define TOK_EOL		  115
#define TOK_EOF           116

static const char *tokens[] = {
[TOK_UNKNOWN     ] = ":unknown:",
[TOK_SPACE       ] = ":space:",
[TOK_SEMI        ] = ";",
[TOK_LBRACE      ] = "{",
[TOK_RBRACE      ] = "}",
[TOK_COMMA       ] = ",",
[TOK_EQ          ] = "=",
[TOK_COLON       ] = ":",
[TOK_LBRACKET    ] = "[",
[TOK_RBRACKET    ] = "]",
[TOK_LPAREN      ] = "(",
[TOK_RPAREN      ] = ")",
[TOK_STAR        ] = "*",
[TOK_DOTS        ] = "...",
[TOK_MORE        ] = ">",
[TOK_LESS        ] = "<",
[TOK_TIMESEQ     ] = "*=",
[TOK_DIVEQ       ] = "/=",
[TOK_MODEQ       ] = "%=",
[TOK_PLUSEQ      ] = "+=",
[TOK_MINUSEQ     ] = "-=",
[TOK_SLEQ        ] = "<<=",
[TOK_SREQ        ] = ">>=",
[TOK_ANDEQ       ] = "&=",
[TOK_XOREQ       ] = "^=",
[TOK_OREQ        ] = "|=",
[TOK_EQEQ        ] = "==",
[TOK_NOTEQ       ] = "!=",
[TOK_QUEST       ] = "?",
[TOK_LOGOR       ] = "||",
[TOK_LOGAND      ] = "&&",
[TOK_OR          ] = "|",
[TOK_AND         ] = "&",
[TOK_XOR         ] = "^",
[TOK_LESSEQ      ] = "<=",
[TOK_MOREEQ      ] = ">=",
[TOK_SL          ] = "<<",
[TOK_SR          ] = ">>",
[TOK_PLUS        ] = "+",
[TOK_MINUS       ] = "-",
[TOK_DIV         ] = "/",
[TOK_MOD         ] = "%",
[TOK_PLUSPLUS    ] = "++",
[TOK_MINUSMINUS  ] = "--",
[TOK_BANG        ] = "!",
[TOK_ARROW       ] = "->",
[TOK_DOT         ] = ".",
[TOK_TILDE       ] = "~",
[TOK_LIT_STRING  ] = ":string:",
[TOK_IDENT       ] = ":ident:",
[TOK_TYPE_NAME   ] = ":typename:",
[TOK_LIT_CHAR    ] = ":char:",
[TOK_LIT_INT     ] = ":integer:",
[TOK_LIT_FLOAT   ] = ":float:",
[TOK_MACRO       ] = "#",
[TOK_CONCATENATE ] = "##",

[TOK_AUTO        ] = "auto",
[TOK_BREAK       ] = "break",
[TOK_CASE        ] = "case",
[TOK_CHAR        ] = "char",
[TOK_CONST       ] = "const",
[TOK_CONTINUE    ] = "continue",
[TOK_DEFAULT     ] = "default",
[TOK_DO          ] = "do",
[TOK_DOUBLE      ] = "double",
[TOK_ELSE        ] = "else",
[TOK_ENUM        ] = "enum",
[TOK_EXTERN      ] = "extern",
[TOK_FLOAT       ] = "float",
[TOK_FOR         ] = "for",
[TOK_GOTO        ] = "goto",
[TOK_IF          ] = "if",
[TOK_INLINE      ] = "inline",
[TOK_INT         ] = "int",
[TOK_LONG        ] = "long",
[TOK_REGISTER    ] = "register",
[TOK_RESTRICT    ] = "restrict",
[TOK_RETURN      ] = "return",
[TOK_SHORT       ] = "short",
[TOK_SIGNED      ] = "signed",
[TOK_SIZEOF      ] = "sizeof",
[TOK_STATIC      ] = "static",
[TOK_STRUCT      ] = "struct",
[TOK_SWITCH      ] = "switch",
[TOK_TYPEDEF     ] = "typedef",
[TOK_UNION       ] = "union",
[TOK_UNSIGNED    ] = "unsigned",
[TOK_VOID        ] = "void",
[TOK_VOLATILE    ] = "volatile",
[TOK_WHILE       ] = "while",
[TOK_ASM         ] = "asm",
[TOK_ATTRIBUTE   ] = "__attribute__",
[TOK_ALIGNOF     ] = "__alignof__",

[TOK_MDEFINE     ] = "#define",
[TOK_MDEFINED    ] = "#defined",
[TOK_MUNDEF      ] = "#undef",
[TOK_MINCLUDE    ] = "#include",
[TOK_MLINE       ] = "#line",
[TOK_MERROR      ] = "#error",
[TOK_MWARNING    ] = "#warning",
[TOK_MPRAGMA     ] = "#pragma",
[TOK_MIFDEF      ] = "#ifdef",
[TOK_MIFNDEF     ] = "#ifndef",
[TOK_MELIF       ] = "#elif",
[TOK_MENDIF      ] = "#endif",

[TOK_MIF         ] = "#if",
[TOK_MELSE       ] = "#else",
[TOK_MIDENT      ] = "#:ident:",
[TOK_EOL         ] = "EOL",
[TOK_EOF         ] = "EOF",
};

static unsigned int hash(const char *str, int str_len)
{
	unsigned int hash;
	const char *end;
	end = str + str_len;
	hash = 0;
	for(; str < end; str++) {
		hash = (hash *263) + *str;
	}
	hash = hash & (HASH_TABLE_SIZE -1);
	return hash;
}

static struct hash_entry *lookup(
	struct compile_state *state, const char *name, int name_len)
{
	struct hash_entry *entry;
	unsigned int index;
	index = hash(name, name_len);
	entry = state->hash_table[index];
	while(entry &&
		((entry->name_len != name_len) ||
			(memcmp(entry->name, name, name_len) != 0))) {
		entry = entry->next;
	}
	if (!entry) {
		char *new_name;
		/* Get a private copy of the name */
		new_name = xmalloc(name_len + 1, "hash_name");
		memcpy(new_name, name, name_len);
		new_name[name_len] = '\0';

		/* Create a new hash entry */
		entry = xcmalloc(sizeof(*entry), "hash_entry");
		entry->next = state->hash_table[index];
		entry->name = new_name;
		entry->name_len = name_len;

		/* Place the new entry in the hash table */
		state->hash_table[index] = entry;
	}
	return entry;
}

static void ident_to_keyword(struct compile_state *state, struct token *tk)
{
	struct hash_entry *entry;
	entry = tk->ident;
	if (entry && ((entry->tok == TOK_TYPE_NAME) ||
		(entry->tok == TOK_ENUM_CONST) ||
		((entry->tok >= TOK_FIRST_KEYWORD) &&
			(entry->tok <= TOK_LAST_KEYWORD)))) {
		tk->tok = entry->tok;
	}
}

static void ident_to_macro(struct compile_state *state, struct token *tk)
{
	struct hash_entry *entry;
	entry = tk->ident;
	if (!entry)
		return;
	if ((entry->tok >= TOK_FIRST_MACRO) && (entry->tok <= TOK_LAST_MACRO)) {
		tk->tok = entry->tok;
	}
	else if (entry->tok == TOK_IF) {
		tk->tok = TOK_MIF;
	}
	else if (entry->tok == TOK_ELSE) {
		tk->tok = TOK_MELSE;
	}
	else {
		tk->tok = TOK_MIDENT;
	}
}

static void hash_keyword(
	struct compile_state *state, const char *keyword, int tok)
{
	struct hash_entry *entry;
	entry = lookup(state, keyword, strlen(keyword));
	if (entry && entry->tok != TOK_UNKNOWN) {
		die("keyword %s already hashed", keyword);
	}
	entry->tok  = tok;
}

static void romcc_symbol(
	struct compile_state *state, struct hash_entry *ident,
	struct symbol **chain, struct triple *def, struct type *type, int depth)
{
	struct symbol *sym;
	if (*chain && ((*chain)->scope_depth >= depth)) {
		error(state, 0, "%s already defined", ident->name);
	}
	sym = xcmalloc(sizeof(*sym), "symbol");
	sym->ident = ident;
	sym->def   = def;
	sym->type  = type;
	sym->scope_depth = depth;
	sym->next = *chain;
	*chain    = sym;
}

static void symbol(
	struct compile_state *state, struct hash_entry *ident,
	struct symbol **chain, struct triple *def, struct type *type)
{
	romcc_symbol(state, ident, chain, def, type, state->scope_depth);
}

static void var_symbol(struct compile_state *state,
	struct hash_entry *ident, struct triple *def)
{
	if ((def->type->type & TYPE_MASK) == TYPE_PRODUCT) {
		internal_error(state, 0, "bad var type");
	}
	symbol(state, ident, &ident->sym_ident, def, def->type);
}

static void label_symbol(struct compile_state *state,
	struct hash_entry *ident, struct triple *label, int depth)
{
	romcc_symbol(state, ident, &ident->sym_label, label, &void_type, depth);
}

static void start_scope(struct compile_state *state)
{
	state->scope_depth++;
}

static void end_scope_syms(struct compile_state *state,
	struct symbol **chain, int depth)
{
	struct symbol *sym, *next;
	sym = *chain;
	while(sym && (sym->scope_depth == depth)) {
		next = sym->next;
		xfree(sym);
		sym = next;
	}
	*chain = sym;
}

static void end_scope(struct compile_state *state)
{
	int i;
	int depth;
	/* Walk through the hash table and remove all symbols
	 * in the current scope.
	 */
	depth = state->scope_depth;
	for(i = 0; i < HASH_TABLE_SIZE; i++) {
		struct hash_entry *entry;
		entry = state->hash_table[i];
		while(entry) {
			end_scope_syms(state, &entry->sym_label, depth);
			end_scope_syms(state, &entry->sym_tag,   depth);
			end_scope_syms(state, &entry->sym_ident, depth);
			entry = entry->next;
		}
	}
	state->scope_depth = depth - 1;
}

static void register_keywords(struct compile_state *state)
{
	hash_keyword(state, "auto",          TOK_AUTO);
	hash_keyword(state, "break",         TOK_BREAK);
	hash_keyword(state, "case",          TOK_CASE);
	hash_keyword(state, "char",          TOK_CHAR);
	hash_keyword(state, "const",         TOK_CONST);
	hash_keyword(state, "continue",      TOK_CONTINUE);
	hash_keyword(state, "default",       TOK_DEFAULT);
	hash_keyword(state, "do",            TOK_DO);
	hash_keyword(state, "double",        TOK_DOUBLE);
	hash_keyword(state, "else",          TOK_ELSE);
	hash_keyword(state, "enum",          TOK_ENUM);
	hash_keyword(state, "extern",        TOK_EXTERN);
	hash_keyword(state, "float",         TOK_FLOAT);
	hash_keyword(state, "for",           TOK_FOR);
	hash_keyword(state, "goto",          TOK_GOTO);
	hash_keyword(state, "if",            TOK_IF);
	hash_keyword(state, "inline",        TOK_INLINE);
	hash_keyword(state, "int",           TOK_INT);
	hash_keyword(state, "long",          TOK_LONG);
	hash_keyword(state, "register",      TOK_REGISTER);
	hash_keyword(state, "restrict",      TOK_RESTRICT);
	hash_keyword(state, "return",        TOK_RETURN);
	hash_keyword(state, "short",         TOK_SHORT);
	hash_keyword(state, "signed",        TOK_SIGNED);
	hash_keyword(state, "sizeof",        TOK_SIZEOF);
	hash_keyword(state, "static",        TOK_STATIC);
	hash_keyword(state, "struct",        TOK_STRUCT);
	hash_keyword(state, "switch",        TOK_SWITCH);
	hash_keyword(state, "typedef",       TOK_TYPEDEF);
	hash_keyword(state, "union",         TOK_UNION);
	hash_keyword(state, "unsigned",      TOK_UNSIGNED);
	hash_keyword(state, "void",          TOK_VOID);
	hash_keyword(state, "volatile",      TOK_VOLATILE);
	hash_keyword(state, "__volatile__",  TOK_VOLATILE);
	hash_keyword(state, "while",         TOK_WHILE);
	hash_keyword(state, "asm",           TOK_ASM);
	hash_keyword(state, "__asm__",       TOK_ASM);
	hash_keyword(state, "__attribute__", TOK_ATTRIBUTE);
	hash_keyword(state, "__alignof__",   TOK_ALIGNOF);
}

static void register_macro_keywords(struct compile_state *state)
{
	hash_keyword(state, "define",        TOK_MDEFINE);
	hash_keyword(state, "defined",       TOK_MDEFINED);
	hash_keyword(state, "undef",         TOK_MUNDEF);
	hash_keyword(state, "include",       TOK_MINCLUDE);
	hash_keyword(state, "line",          TOK_MLINE);
	hash_keyword(state, "error",         TOK_MERROR);
	hash_keyword(state, "warning",       TOK_MWARNING);
	hash_keyword(state, "pragma",        TOK_MPRAGMA);
	hash_keyword(state, "ifdef",         TOK_MIFDEF);
	hash_keyword(state, "ifndef",        TOK_MIFNDEF);
	hash_keyword(state, "elif",          TOK_MELIF);
	hash_keyword(state, "endif",         TOK_MENDIF);
}


static void undef_macro(struct compile_state *state, struct hash_entry *ident)
{
	if (ident->sym_define != 0) {
		struct macro *macro;
		struct macro_arg *arg, *anext;
		macro = ident->sym_define;
		ident->sym_define = 0;

		/* Free the macro arguments... */
		anext = macro->args;
		while(anext) {
			arg = anext;
			anext = arg->next;
			xfree(arg);
		}

		/* Free the macro buffer */
		xfree(macro->buf);

		/* Now free the macro itself */
		xfree(macro);
	}
}

static void do_define_macro(struct compile_state *state,
	struct hash_entry *ident, const char *body,
	int argc, struct macro_arg *args)
{
	struct macro *macro;
	struct macro_arg *arg;
	size_t body_len;

	/* Find the length of the body */
	body_len = strlen(body);
	macro = ident->sym_define;
	if (macro != 0) {
		int identical_bodies, identical_args;
		struct macro_arg *oarg;
		/* Explicitly allow identical redfinitions of the same macro */
		identical_bodies =
			(macro->buf_len == body_len) &&
			(memcmp(macro->buf, body, body_len) == 0);
		identical_args = macro->argc == argc;
		oarg = macro->args;
		arg = args;
		while(identical_args && arg) {
			identical_args = oarg->ident == arg->ident;
			arg = arg->next;
			oarg = oarg->next;
		}
		if (identical_bodies && identical_args) {
			xfree(body);
			return;
		}
		error(state, 0, "macro %s already defined\n", ident->name);
	}
#if 0
	fprintf(state->errout, "#define %s: `%*.*s'\n",
		ident->name, body_len, body_len, body);
#endif
	macro = xmalloc(sizeof(*macro), "macro");
	macro->ident   = ident;
	macro->buf     = body;
	macro->buf_len = body_len;
	macro->args    = args;
	macro->argc    = argc;

	ident->sym_define = macro;
}

static void define_macro(
	struct compile_state *state,
	struct hash_entry *ident,
	const char *body, int body_len,
	int argc, struct macro_arg *args)
{
	char *buf;
	buf = xmalloc(body_len + 1, "macro buf");
	memcpy(buf, body, body_len);
	buf[body_len] = '\0';
	do_define_macro(state, ident, buf, argc, args);
}

static void register_builtin_macro(struct compile_state *state,
	const char *name, const char *value)
{
	struct hash_entry *ident;

	if (value[0] == '(') {
		internal_error(state, 0, "Builtin macros with arguments not supported");
	}
	ident = lookup(state, name, strlen(name));
	define_macro(state, ident, value, strlen(value), -1, 0);
}

static void register_builtin_macros(struct compile_state *state)
{
	char buf[30];
	char scratch[30];
	time_t now;
	struct tm *tm;
	now = time(NULL);
	tm = localtime(&now);

	register_builtin_macro(state, "__ROMCC__", VERSION_MAJOR);
	register_builtin_macro(state, "__ROMCC_MINOR__", VERSION_MINOR);
	register_builtin_macro(state, "__FILE__", "\"This should be the filename\"");
	register_builtin_macro(state, "__LINE__", "54321");

	strftime(scratch, sizeof(scratch), "%b %e %Y", tm);
	sprintf(buf, "\"%s\"", scratch);
	register_builtin_macro(state, "__DATE__", buf);

	strftime(scratch, sizeof(scratch), "%H:%M:%S", tm);
	sprintf(buf, "\"%s\"", scratch);
	register_builtin_macro(state, "__TIME__", buf);

	/* I can't be a conforming implementation of C :( */
	register_builtin_macro(state, "__STDC__", "0");
	/* In particular I don't conform to C99 */
	register_builtin_macro(state, "__STDC_VERSION__", "199901L");

}

static void process_cmdline_macros(struct compile_state *state)
{
	const char **macro, *name;
	struct hash_entry *ident;
	for(macro = state->compiler->defines; (name = *macro); macro++) {
		const char *body;
		size_t name_len;

		name_len = strlen(name);
		body = strchr(name, '=');
		if (!body) {
			body = "\0";
		} else {
			name_len = body - name;
			body++;
		}
		ident = lookup(state, name, name_len);
		define_macro(state, ident, body, strlen(body), -1, 0);
	}
	for(macro = state->compiler->undefs; (name = *macro); macro++) {
		ident = lookup(state, name, strlen(name));
		undef_macro(state, ident);
	}
}

static int spacep(int c)
{
	int ret = 0;
	switch(c) {
	case ' ':
	case '\t':
	case '\f':
	case '\v':
	case '\r':
		ret = 1;
		break;
	}
	return ret;
}

static int digitp(int c)
{
	int ret = 0;
	switch(c) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		ret = 1;
		break;
	}
	return ret;
}
static int digval(int c)
{
	int val = -1;
	if ((c >= '0') && (c <= '9')) {
		val = c - '0';
	}
	return val;
}

static int hexdigitp(int c)
{
	int ret = 0;
	switch(c) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		ret = 1;
		break;
	}
	return ret;
}
static int hexdigval(int c)
{
	int val = -1;
	if ((c >= '0') && (c <= '9')) {
		val = c - '0';
	}
	else if ((c >= 'A') && (c <= 'F')) {
		val = 10 + (c - 'A');
	}
	else if ((c >= 'a') && (c <= 'f')) {
		val = 10 + (c - 'a');
	}
	return val;
}

static int octdigitp(int c)
{
	int ret = 0;
	switch(c) {
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
		ret = 1;
		break;
	}
	return ret;
}
static int octdigval(int c)
{
	int val = -1;
	if ((c >= '0') && (c <= '7')) {
		val = c - '0';
	}
	return val;
}

static int letterp(int c)
{
	int ret = 0;
	switch(c) {
	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z':
	case '_':
		ret = 1;
		break;
	}
	return ret;
}

static const char *identifier(const char *str, const char *end)
{
	if (letterp(*str)) {
		for(; str < end; str++) {
			int c;
			c = *str;
			if (!letterp(c) && !digitp(c)) {
				break;
			}
		}
	}
	return str;
}

static int char_value(struct compile_state *state,
	const signed char **strp, const signed char *end)
{
	const signed char *str;
	int c;
	str = *strp;
	c = *str++;
	if ((c == '\\') && (str < end)) {
		switch(*str) {
		case 'n':  c = '\n'; str++; break;
		case 't':  c = '\t'; str++; break;
		case 'v':  c = '\v'; str++; break;
		case 'b':  c = '\b'; str++; break;
		case 'r':  c = '\r'; str++; break;
		case 'f':  c = '\f'; str++; break;
		case 'a':  c = '\a'; str++; break;
		case '\\': c = '\\'; str++; break;
		case '?':  c = '?';  str++; break;
		case '\'': c = '\''; str++; break;
		case '"':  c = '"';  str++; break;
		case 'x':
			c = 0;
			str++;
			while((str < end) && hexdigitp(*str)) {
				c <<= 4;
				c += hexdigval(*str);
				str++;
			}
			break;
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			c = 0;
			while((str < end) && octdigitp(*str)) {
				c <<= 3;
				c += octdigval(*str);
				str++;
			}
			break;
		default:
			error(state, 0, "Invalid character constant");
			break;
		}
	}
	*strp = str;
	return c;
}

static const char *next_char(struct file_state *file, const char *pos, int index)
{
	const char *end = file->buf + file->size;
	while(pos < end) {
		/* Lookup the character */
		int size = 1;
		int c = *pos;
		/* Is this a trigraph? */
		if (file->trigraphs &&
			(c == '?') && ((end - pos) >= 3) && (pos[1] == '?'))
		{
			switch(pos[2]) {
			case '=': c = '#'; break;
			case '/': c = '\\'; break;
			case '\'': c = '^'; break;
			case '(': c = '['; break;
			case ')': c = ']'; break;
			case '!': c = '!'; break;
			case '<': c = '{'; break;
			case '>': c = '}'; break;
			case '-': c = '~'; break;
			}
			if (c != '?') {
				size = 3;
			}
		}
		/* Is this an escaped newline? */
		if (file->join_lines &&
			(c == '\\') && (pos + size < end) && ((pos[1] == '\n') || ((pos[1] == '\r') && (pos[2] == '\n'))))
		{
			int cr_offset = ((pos[1] == '\r') && (pos[2] == '\n'))?1:0;
			/* At the start of a line just eat it */
			if (pos == file->pos) {
				file->line++;
				file->report_line++;
				file->line_start = pos + size + 1 + cr_offset;
			}
			pos += size + 1 + cr_offset;
		}
		/* Do I need to ga any farther? */
		else if (index == 0) {
			break;
		}
		/* Process a normal character */
		else {
			pos += size;
			index -= 1;
		}
	}
	return pos;
}

static int get_char(struct file_state *file, const char *pos)
{
	const char *end = file->buf + file->size;
	int c;
	c = -1;
	pos = next_char(file, pos, 0);
	if (pos < end) {
		/* Lookup the character */
		c = *pos;
		/* If it is a trigraph get the trigraph value */
		if (file->trigraphs &&
			(c == '?') && ((end - pos) >= 3) && (pos[1] == '?'))
		{
			switch(pos[2]) {
			case '=': c = '#'; break;
			case '/': c = '\\'; break;
			case '\'': c = '^'; break;
			case '(': c = '['; break;
			case ')': c = ']'; break;
			case '!': c = '!'; break;
			case '<': c = '{'; break;
			case '>': c = '}'; break;
			case '-': c = '~'; break;
			}
		}
	}
	return c;
}

static void eat_chars(struct file_state *file, const char *targ)
{
	const char *pos = file->pos;
	while(pos < targ) {
		/* Do we have a newline? */
		if (pos[0] == '\n') {
			file->line++;
			file->report_line++;
			file->line_start = pos + 1;
		}
		pos++;
	}
	file->pos = pos;
}


static size_t char_strlen(struct file_state *file, const char *src, const char *end)
{
	size_t len;
	len = 0;
	while(src < end) {
		src = next_char(file, src, 1);
		len++;
	}
	return len;
}

static void char_strcpy(char *dest,
	struct file_state *file, const char *src, const char *end)
{
	while(src < end) {
		int c;
		c = get_char(file, src);
		src = next_char(file, src, 1);
		*dest++ = c;
	}
}

static char *char_strdup(struct file_state *file,
	const char *start, const char *end, const char *id)
{
	char *str;
	size_t str_len;
	str_len = char_strlen(file, start, end);
	str = xcmalloc(str_len + 1, id);
	char_strcpy(str, file, start, end);
	str[str_len] = '\0';
	return str;
}

static const char *after_digits(struct file_state *file, const char *ptr)
{
	while(digitp(get_char(file, ptr))) {
		ptr = next_char(file, ptr, 1);
	}
	return ptr;
}

static const char *after_octdigits(struct file_state *file, const char *ptr)
{
	while(octdigitp(get_char(file, ptr))) {
		ptr = next_char(file, ptr, 1);
	}
	return ptr;
}

static const char *after_hexdigits(struct file_state *file, const char *ptr)
{
	while(hexdigitp(get_char(file, ptr))) {
		ptr = next_char(file, ptr, 1);
	}
	return ptr;
}

static const char *after_alnums(struct file_state *file, const char *ptr)
{
	int c;
	c = get_char(file, ptr);
	while(letterp(c) || digitp(c)) {
		ptr = next_char(file, ptr, 1);
		c = get_char(file, ptr);
	}
	return ptr;
}

static void save_string(struct file_state *file,
	struct token *tk, const char *start, const char *end, const char *id)
{
	char *str;

	/* Create a private copy of the string */
	str = char_strdup(file, start, end, id);

	/* Store the copy in the token */
	tk->val.str = str;
	tk->str_len = strlen(str);
}

static void raw_next_token(struct compile_state *state,
	struct file_state *file, struct token *tk)
{
	const char *token;
	int c, c1, c2, c3;
	const char *tokp;
	int eat;
	int tok;

	tk->str_len = 0;
	tk->ident = 0;
	token = tokp = next_char(file, file->pos, 0);
	tok = TOK_UNKNOWN;
	c  = get_char(file, tokp);
	tokp = next_char(file, tokp, 1);
	eat = 0;
	c1 = get_char(file, tokp);
	c2 = get_char(file, next_char(file, tokp, 1));
	c3 = get_char(file, next_char(file, tokp, 2));

	/* The end of the file */
	if (c == -1) {
		tok = TOK_EOF;
	}
	/* Whitespace */
	else if (spacep(c)) {
		tok = TOK_SPACE;
		while (spacep(get_char(file, tokp))) {
			tokp = next_char(file, tokp, 1);
		}
	}
	/* EOL Comments */
	else if ((c == '/') && (c1 == '/')) {
		tok = TOK_SPACE;
		tokp = next_char(file, tokp, 1);
		while((c = get_char(file, tokp)) != -1) {
			/* Advance to the next character only after we verify
			 * the current character is not a newline.
			 * EOL is special to the preprocessor so we don't
			 * want to loose any.
			 */
			if (c == '\n') {
				break;
			}
			tokp = next_char(file, tokp, 1);
		}
	}
	/* Comments */
	else if ((c == '/') && (c1 == '*')) {
		tokp = next_char(file, tokp, 2);
		c = c2;
		while((c1 = get_char(file, tokp)) != -1) {
			tokp = next_char(file, tokp, 1);
			if ((c == '*') && (c1 == '/')) {
				tok = TOK_SPACE;
				break;
			}
			c = c1;
		}
		if (tok == TOK_UNKNOWN) {
			error(state, 0, "unterminated comment");
		}
	}
	/* string constants */
	else if ((c == '"') || ((c == 'L') && (c1 == '"'))) {
		int multiline;

		multiline = 0;
		if (c == 'L') {
			tokp = next_char(file, tokp, 1);
		}
		while((c = get_char(file, tokp)) != -1) {
			tokp = next_char(file, tokp, 1);
			if (c == '\n') {
				multiline = 1;
			}
			else if (c == '\\') {
				tokp = next_char(file, tokp, 1);
			}
			else if (c == '"') {
				tok = TOK_LIT_STRING;
				break;
			}
		}
		if (tok == TOK_UNKNOWN) {
			error(state, 0, "unterminated string constant");
		}
		if (multiline) {
			warning(state, 0, "multiline string constant");
		}

		/* Save the string value */
		save_string(file, tk, token, tokp, "literal string");
	}
	/* character constants */
	else if ((c == '\'') || ((c == 'L') && (c1 == '\''))) {
		int multiline;

		multiline = 0;
		if (c == 'L') {
			tokp = next_char(file, tokp, 1);
		}
		while((c = get_char(file, tokp)) != -1) {
			tokp = next_char(file, tokp, 1);
			if (c == '\n') {
				multiline = 1;
			}
			else if (c == '\\') {
				tokp = next_char(file, tokp, 1);
			}
			else if (c == '\'') {
				tok = TOK_LIT_CHAR;
				break;
			}
		}
		if (tok == TOK_UNKNOWN) {
			error(state, 0, "unterminated character constant");
		}
		if (multiline) {
			warning(state, 0, "multiline character constant");
		}

		/* Save the character value */
		save_string(file, tk, token, tokp, "literal character");
	}
	/* integer and floating constants
	 * Integer Constants
	 * {digits}
	 * 0[Xx]{hexdigits}
	 * 0{octdigit}+
	 *
         * Floating constants
	 * {digits}.{digits}[Ee][+-]?{digits}
	 * {digits}.{digits}
	 * {digits}[Ee][+-]?{digits}
	 * .{digits}[Ee][+-]?{digits}
	 * .{digits}
	 */
	else if (digitp(c) || ((c == '.') && (digitp(c1)))) {
		const char *next;
		int is_float;
		int cn;
		is_float = 0;
		if (c != '.') {
			next = after_digits(file, tokp);
		}
		else {
			next = token;
		}
		cn = get_char(file, next);
		if (cn == '.') {
			next = next_char(file, next, 1);
			next = after_digits(file, next);
			is_float = 1;
		}
		cn = get_char(file, next);
		if ((cn == 'e') || (cn == 'E')) {
			const char *new;
			next = next_char(file, next, 1);
			cn = get_char(file, next);
			if ((cn == '+') || (cn == '-')) {
				next = next_char(file, next, 1);
			}
			new = after_digits(file, next);
			is_float |= (new != next);
			next = new;
		}
		if (is_float) {
			tok = TOK_LIT_FLOAT;
			cn = get_char(file, next);
			if ((cn  == 'f') || (cn == 'F') || (cn == 'l') || (cn == 'L')) {
				next = next_char(file, next, 1);
			}
		}
		if (!is_float && digitp(c)) {
			tok = TOK_LIT_INT;
			if ((c == '0') && ((c1 == 'x') || (c1 == 'X'))) {
				next = next_char(file, tokp, 1);
				next = after_hexdigits(file, next);
			}
			else if (c == '0') {
				next = after_octdigits(file, tokp);
			}
			else {
				next = after_digits(file, tokp);
			}
			/* crazy integer suffixes */
			cn = get_char(file, next);
			if ((cn == 'u') || (cn == 'U')) {
				next = next_char(file, next, 1);
				cn = get_char(file, next);
				if ((cn == 'l') || (cn == 'L')) {
					next = next_char(file, next, 1);
					cn = get_char(file, next);
				}
				if ((cn == 'l') || (cn == 'L')) {
					next = next_char(file, next, 1);
				}
			}
			else if ((cn == 'l') || (cn == 'L')) {
				next = next_char(file, next, 1);
				cn = get_char(file, next);
				if ((cn == 'l') || (cn == 'L')) {
					next = next_char(file, next, 1);
					cn = get_char(file, next);
				}
				if ((cn == 'u') || (cn == 'U')) {
					next = next_char(file, next, 1);
				}
			}
		}
		tokp = next;

		/* Save the integer/floating point value */
		save_string(file, tk, token, tokp, "literal number");
	}
	/* identifiers */
	else if (letterp(c)) {
		tok = TOK_IDENT;

		/* Find and save the identifier string */
		tokp = after_alnums(file, tokp);
		save_string(file, tk, token, tokp, "identifier");

		/* Look up to see which identifier it is */
		tk->ident = lookup(state, tk->val.str, tk->str_len);

		/* Free the identifier string */
		tk->str_len = 0;
		xfree(tk->val.str);

		/* See if this identifier can be macro expanded */
		tk->val.notmacro = 0;
		c = get_char(file, tokp);
		if (c == '$') {
			tokp = next_char(file, tokp, 1);
			tk->val.notmacro = 1;
		}
	}
	/* C99 alternate macro characters */
	else if ((c == '%') && (c1 == ':') && (c2 == '%') && (c3 == ':')) {
		eat += 3;
		tok = TOK_CONCATENATE;
	}
	else if ((c == '.') && (c1 == '.') && (c2 == '.')) { eat += 2; tok = TOK_DOTS; }
	else if ((c == '<') && (c1 == '<') && (c2 == '=')) { eat += 2; tok = TOK_SLEQ; }
	else if ((c == '>') && (c1 == '>') && (c2 == '=')) { eat += 2; tok = TOK_SREQ; }
	else if ((c == '*') && (c1 == '=')) { eat += 1; tok = TOK_TIMESEQ; }
	else if ((c == '/') && (c1 == '=')) { eat += 1; tok = TOK_DIVEQ; }
	else if ((c == '%') && (c1 == '=')) { eat += 1; tok = TOK_MODEQ; }
	else if ((c == '+') && (c1 == '=')) { eat += 1; tok = TOK_PLUSEQ; }
	else if ((c == '-') && (c1 == '=')) { eat += 1; tok = TOK_MINUSEQ; }
	else if ((c == '&') && (c1 == '=')) { eat += 1; tok = TOK_ANDEQ; }
	else if ((c == '^') && (c1 == '=')) { eat += 1; tok = TOK_XOREQ; }
	else if ((c == '|') && (c1 == '=')) { eat += 1; tok = TOK_OREQ; }
	else if ((c == '=') && (c1 == '=')) { eat += 1; tok = TOK_EQEQ; }
	else if ((c == '!') && (c1 == '=')) { eat += 1; tok = TOK_NOTEQ; }
	else if ((c == '|') && (c1 == '|')) { eat += 1; tok = TOK_LOGOR; }
	else if ((c == '&') && (c1 == '&')) { eat += 1; tok = TOK_LOGAND; }
	else if ((c == '<') && (c1 == '=')) { eat += 1; tok = TOK_LESSEQ; }
	else if ((c == '>') && (c1 == '=')) { eat += 1; tok = TOK_MOREEQ; }
	else if ((c == '<') && (c1 == '<')) { eat += 1; tok = TOK_SL; }
	else if ((c == '>') && (c1 == '>')) { eat += 1; tok = TOK_SR; }
	else if ((c == '+') && (c1 == '+')) { eat += 1; tok = TOK_PLUSPLUS; }
	else if ((c == '-') && (c1 == '-')) { eat += 1; tok = TOK_MINUSMINUS; }
	else if ((c == '-') && (c1 == '>')) { eat += 1; tok = TOK_ARROW; }
	else if ((c == '<') && (c1 == ':')) { eat += 1; tok = TOK_LBRACKET; }
	else if ((c == ':') && (c1 == '>')) { eat += 1; tok = TOK_RBRACKET; }
	else if ((c == '<') && (c1 == '%')) { eat += 1; tok = TOK_LBRACE; }
	else if ((c == '%') && (c1 == '>')) { eat += 1; tok = TOK_RBRACE; }
	else if ((c == '%') && (c1 == ':')) { eat += 1; tok = TOK_MACRO; }
	else if ((c == '#') && (c1 == '#')) { eat += 1; tok = TOK_CONCATENATE; }
	else if (c == ';') { tok = TOK_SEMI; }
	else if (c == '{') { tok = TOK_LBRACE; }
	else if (c == '}') { tok = TOK_RBRACE; }
	else if (c == ',') { tok = TOK_COMMA; }
	else if (c == '=') { tok = TOK_EQ; }
	else if (c == ':') { tok = TOK_COLON; }
	else if (c == '[') { tok = TOK_LBRACKET; }
	else if (c == ']') { tok = TOK_RBRACKET; }
	else if (c == '(') { tok = TOK_LPAREN; }
	else if (c == ')') { tok = TOK_RPAREN; }
	else if (c == '*') { tok = TOK_STAR; }
	else if (c == '>') { tok = TOK_MORE; }
	else if (c == '<') { tok = TOK_LESS; }
	else if (c == '?') { tok = TOK_QUEST; }
	else if (c == '|') { tok = TOK_OR; }
	else if (c == '&') { tok = TOK_AND; }
	else if (c == '^') { tok = TOK_XOR; }
	else if (c == '+') { tok = TOK_PLUS; }
	else if (c == '-') { tok = TOK_MINUS; }
	else if (c == '/') { tok = TOK_DIV; }
	else if (c == '%') { tok = TOK_MOD; }
	else if (c == '!') { tok = TOK_BANG; }
	else if (c == '.') { tok = TOK_DOT; }
	else if (c == '~') { tok = TOK_TILDE; }
	else if (c == '#') { tok = TOK_MACRO; }
	else if (c == '\n') { tok = TOK_EOL; }

	tokp = next_char(file, tokp, eat);
	eat_chars(file, tokp);
	tk->tok = tok;
	tk->pos = token;
}

static void check_tok(struct compile_state *state, struct token *tk, int tok)
{
	if (tk->tok != tok) {
		const char *name1, *name2;
		name1 = tokens[tk->tok];
		name2 = "";
		if ((tk->tok == TOK_IDENT) || (tk->tok == TOK_MIDENT)) {
			name2 = tk->ident->name;
		}
		error(state, 0, "\tfound %s %s expected %s",
			name1, name2, tokens[tok]);
	}
}

struct macro_arg_value {
	struct hash_entry *ident;
	char *value;
	size_t len;
};
static struct macro_arg_value *read_macro_args(
	struct compile_state *state, struct macro *macro,
	struct file_state *file, struct token *tk)
{
	struct macro_arg_value *argv;
	struct macro_arg *arg;
	int paren_depth;
	int i;

	if (macro->argc == 0) {
		do {
			raw_next_token(state, file, tk);
		} while(tk->tok == TOK_SPACE);
		return NULL;
	}
	argv = xcmalloc(sizeof(*argv) * macro->argc, "macro args");
	for(i = 0, arg = macro->args; arg; arg = arg->next, i++) {
		argv[i].value = 0;
		argv[i].len   = 0;
		argv[i].ident = arg->ident;
	}
	paren_depth = 0;
	i = 0;

	for(;;) {
		const char *start;
		size_t len;
		start = file->pos;
		raw_next_token(state, file, tk);

		if (!paren_depth && (tk->tok == TOK_COMMA) &&
			(argv[i].ident != state->i___VA_ARGS__))
		{
			i++;
			if (i >= macro->argc) {
				error(state, 0, "too many args to %s\n",
					macro->ident->name);
			}
			continue;
		}

		if (tk->tok == TOK_LPAREN) {
			paren_depth++;
		}

		if (tk->tok == TOK_RPAREN) {
			if (paren_depth == 0) {
				break;
			}
			paren_depth--;
		}
		if (tk->tok == TOK_EOF) {
			error(state, 0, "End of file encountered while parsing macro arguments");
		}

		len = char_strlen(file, start, file->pos);
		argv[i].value = xrealloc(
			argv[i].value, argv[i].len + len, "macro args");
		char_strcpy((char *)argv[i].value + argv[i].len, file, start, file->pos);
		argv[i].len += len;
	}
	if (i != macro->argc -1) {
		error(state, 0, "missing %s arg %d\n",
			macro->ident->name, i +2);
	}
	return argv;
}


static void free_macro_args(struct macro *macro, struct macro_arg_value *argv)
{
	int i;
	for(i = 0; i < macro->argc; i++) {
		xfree(argv[i].value);
	}
	xfree(argv);
}

struct macro_buf {
	char *str;
	size_t len, pos;
};

static void grow_macro_buf(struct compile_state *state,
	const char *id, struct macro_buf *buf,
	size_t grow)
{
	if ((buf->pos + grow) >= buf->len) {
		buf->str = xrealloc(buf->str, buf->len + grow, id);
		buf->len += grow;
	}
}

static void append_macro_text(struct compile_state *state,
	const char *id, struct macro_buf *buf,
	const char *fstart, size_t flen)
{
	grow_macro_buf(state, id, buf, flen);
	memcpy(buf->str + buf->pos, fstart, flen);
#if 0
	fprintf(state->errout, "append: `%*.*s' `%*.*s'\n",
		buf->pos, buf->pos, buf->str,
		flen, flen, buf->str + buf->pos);
#endif
	buf->pos += flen;
}


static void append_macro_chars(struct compile_state *state,
	const char *id, struct macro_buf *buf,
	struct file_state *file, const char *start, const char *end)
{
	size_t flen;
	flen = char_strlen(file, start, end);
	grow_macro_buf(state, id, buf, flen);
	char_strcpy(buf->str + buf->pos, file, start, end);
#if 0
	fprintf(state->errout, "append: `%*.*s' `%*.*s'\n",
		buf->pos, buf->pos, buf->str,
		flen, flen, buf->str + buf->pos);
#endif
	buf->pos += flen;
}

static int compile_macro(struct compile_state *state,
	struct file_state **filep, struct token *tk);

static void macro_expand_args(struct compile_state *state,
	struct macro *macro, struct macro_arg_value *argv, struct token *tk)
{
	int i;

	for(i = 0; i < macro->argc; i++) {
		struct file_state fmacro, *file;
		struct macro_buf buf;

		fmacro.prev        = 0;
		fmacro.basename    = argv[i].ident->name;
		fmacro.dirname     = "";
		fmacro.buf         = (char *)argv[i].value;
		fmacro.size        = argv[i].len;
		fmacro.pos         = fmacro.buf;
		fmacro.line        = 1;
		fmacro.line_start  = fmacro.buf;
		fmacro.report_line = 1;
		fmacro.report_name = fmacro.basename;
		fmacro.report_dir  = fmacro.dirname;
		fmacro.macro       = 1;
		fmacro.trigraphs   = 0;
		fmacro.join_lines  = 0;

		buf.len = argv[i].len;
		buf.str = xmalloc(buf.len, argv[i].ident->name);
		buf.pos = 0;

		file = &fmacro;
		for(;;) {
			raw_next_token(state, file, tk);

			/* If we have recursed into another macro body
			 * get out of it.
			 */
			if (tk->tok == TOK_EOF) {
				struct file_state *old;
				old = file;
				file = file->prev;
				if (!file) {
					break;
				}
				/* old->basename is used keep it */
				xfree(old->dirname);
				xfree(old->buf);
				xfree(old);
				continue;
			}
			else if (tk->ident && tk->ident->sym_define) {
				if (compile_macro(state, &file, tk)) {
					continue;
				}
			}

			append_macro_chars(state, macro->ident->name, &buf,
				file, tk->pos, file->pos);
		}

		xfree(argv[i].value);
		argv[i].value = buf.str;
		argv[i].len   = buf.pos;
	}
	return;
}

static void expand_macro(struct compile_state *state,
	struct macro *macro, struct macro_buf *buf,
	struct macro_arg_value *argv, struct token *tk)
{
	struct file_state fmacro;
	const char space[] = " ";
	const char *fstart;
	size_t flen;
	int i, j;

	/* Place the macro body in a dummy file */
	fmacro.prev        = 0;
	fmacro.basename    = macro->ident->name;
	fmacro.dirname     = "";
	fmacro.buf         = macro->buf;
	fmacro.size        = macro->buf_len;
	fmacro.pos         = fmacro.buf;
	fmacro.line        = 1;
	fmacro.line_start  = fmacro.buf;
	fmacro.report_line = 1;
	fmacro.report_name = fmacro.basename;
	fmacro.report_dir  = fmacro.dirname;
	fmacro.macro       = 1;
	fmacro.trigraphs   = 0;
	fmacro.join_lines  = 0;

	/* Allocate a buffer to hold the macro expansion */
	buf->len = macro->buf_len + 3;
	buf->str = xmalloc(buf->len, macro->ident->name);
	buf->pos = 0;

	fstart = fmacro.pos;
	raw_next_token(state, &fmacro, tk);
	while(tk->tok != TOK_EOF) {
		flen = fmacro.pos - fstart;
		switch(tk->tok) {
		case TOK_IDENT:
			if (macro->argc < 0) {
				break;
			}
			for(i = 0; i < macro->argc; i++) {
				if (argv[i].ident == tk->ident) {
					break;
				}
			}
			if (i >= macro->argc) {
				break;
			}
			/* Substitute macro parameter */
			fstart = argv[i].value;
			flen   = argv[i].len;
			break;
		case TOK_MACRO:
			if (macro->argc < 0) {
				break;
			}
			do {
				raw_next_token(state, &fmacro, tk);
			} while(tk->tok == TOK_SPACE);
			check_tok(state, tk, TOK_IDENT);
			for(i = 0; i < macro->argc; i++) {
				if (argv[i].ident == tk->ident) {
					break;
				}
			}
			if (i >= macro->argc) {
				error(state, 0, "parameter `%s' not found",
					tk->ident->name);
			}
			/* Stringize token */
			append_macro_text(state, macro->ident->name, buf, "\"", 1);
			for(j = 0; j < argv[i].len; j++) {
				char *str = argv[i].value + j;
				size_t len = 1;
				if (*str == '\\') {
					str = "\\";
					len = 2;
				}
				else if (*str == '"') {
					str = "\\\"";
					len = 2;
				}
				append_macro_text(state, macro->ident->name, buf, str, len);
			}
			append_macro_text(state, macro->ident->name, buf, "\"", 1);
			fstart = 0;
			flen   = 0;
			break;
		case TOK_CONCATENATE:
			/* Concatenate tokens */
			/* Delete the previous whitespace token */
			if (buf->str[buf->pos - 1] == ' ') {
				buf->pos -= 1;
			}
			/* Skip the next sequence of whitspace tokens */
			do {
				fstart = fmacro.pos;
				raw_next_token(state, &fmacro, tk);
			} while(tk->tok == TOK_SPACE);
			/* Restart at the top of the loop.
			 * I need to process the non white space token.
			 */
			continue;
			break;
		case TOK_SPACE:
			/* Collapse multiple spaces into one */
			if (buf->str[buf->pos - 1] != ' ') {
				fstart = space;
				flen   = 1;
			} else {
				fstart = 0;
				flen   = 0;
			}
			break;
		default:
			break;
		}

		append_macro_text(state, macro->ident->name, buf, fstart, flen);

		fstart = fmacro.pos;
		raw_next_token(state, &fmacro, tk);
	}
}

static void tag_macro_name(struct compile_state *state,
	struct macro *macro, struct macro_buf *buf,
	struct token *tk)
{
	/* Guard all instances of the macro name in the replacement
	 * text from further macro expansion.
	 */
	struct file_state fmacro;
	const char *fstart;
	size_t flen;

	/* Put the old macro expansion buffer in a file */
	fmacro.prev        = 0;
	fmacro.basename    = macro->ident->name;
	fmacro.dirname     = "";
	fmacro.buf         = buf->str;
	fmacro.size        = buf->pos;
	fmacro.pos         = fmacro.buf;
	fmacro.line        = 1;
	fmacro.line_start  = fmacro.buf;
	fmacro.report_line = 1;
	fmacro.report_name = fmacro.basename;
	fmacro.report_dir  = fmacro.dirname;
	fmacro.macro       = 1;
	fmacro.trigraphs   = 0;
	fmacro.join_lines  = 0;

	/* Allocate a new macro expansion buffer */
	buf->len = macro->buf_len + 3;
	buf->str = xmalloc(buf->len, macro->ident->name);
	buf->pos = 0;

	fstart = fmacro.pos;
	raw_next_token(state, &fmacro, tk);
	while(tk->tok != TOK_EOF) {
		flen = fmacro.pos - fstart;
		if ((tk->tok == TOK_IDENT) &&
			(tk->ident == macro->ident) &&
			(tk->val.notmacro == 0))
		{
			append_macro_text(state, macro->ident->name, buf, fstart, flen);
			fstart = "$";
			flen   = 1;
		}

		append_macro_text(state, macro->ident->name, buf, fstart, flen);

		fstart = fmacro.pos;
		raw_next_token(state, &fmacro, tk);
	}
	xfree(fmacro.buf);
}

static int compile_macro(struct compile_state *state,
	struct file_state **filep, struct token *tk)
{
	struct file_state *file;
	struct hash_entry *ident;
	struct macro *macro;
	struct macro_arg_value *argv;
	struct macro_buf buf;

#if 0
	fprintf(state->errout, "macro: %s\n", tk->ident->name);
#endif
	ident = tk->ident;
	macro = ident->sym_define;

	/* If this token comes from a macro expansion ignore it */
	if (tk->val.notmacro) {
		return 0;
	}
	/* If I am a function like macro and the identifier is not followed
	 * by a left parenthesis, do nothing.
	 */
	if ((macro->argc >= 0) && (get_char(*filep, (*filep)->pos) != '(')) {
		return 0;
	}

	/* Read in the macro arguments */
	argv = 0;
	if (macro->argc >= 0) {
		raw_next_token(state, *filep, tk);
		check_tok(state, tk, TOK_LPAREN);

		argv = read_macro_args(state, macro, *filep, tk);

		check_tok(state, tk, TOK_RPAREN);
	}
	/* Macro expand the macro arguments */
	macro_expand_args(state, macro, argv, tk);

	buf.str = 0;
	buf.len = 0;
	buf.pos = 0;
	if (ident == state->i___FILE__) {
		buf.len = strlen(state->file->basename) + 1 + 2 + 3;
		buf.str = xmalloc(buf.len, ident->name);
		sprintf(buf.str, "\"%s\"", state->file->basename);
		buf.pos = strlen(buf.str);
	}
	else if (ident == state->i___LINE__) {
		buf.len = 30;
		buf.str = xmalloc(buf.len, ident->name);
		sprintf(buf.str, "%d", state->file->line);
		buf.pos = strlen(buf.str);
	}
	else {
		expand_macro(state, macro, &buf, argv, tk);
	}
	/* Tag the macro name with a $ so it will no longer
	 * be regonized as a canidate for macro expansion.
	 */
	tag_macro_name(state, macro, &buf, tk);

#if 0
	fprintf(state->errout, "%s: %d -> `%*.*s'\n",
		ident->name, buf.pos, buf.pos, (int)(buf.pos), buf.str);
#endif

	free_macro_args(macro, argv);

	file = xmalloc(sizeof(*file), "file_state");
	file->prev        = *filep;
	file->basename    = xstrdup(ident->name);
	file->dirname     = xstrdup("");
	file->buf         = buf.str;
	file->size        = buf.pos;
	file->pos         = file->buf;
	file->line        = 1;
	file->line_start  = file->pos;
	file->report_line = 1;
	file->report_name = file->basename;
	file->report_dir  = file->dirname;
	file->macro       = 1;
	file->trigraphs   = 0;
	file->join_lines  = 0;
	*filep = file;
	return 1;
}

static void eat_tokens(struct compile_state *state, int targ_tok)
{
	if (state->eat_depth > 0) {
		internal_error(state, 0, "Already eating...");
	}
	state->eat_depth = state->if_depth;
	state->eat_targ = targ_tok;
}
static int if_eat(struct compile_state *state)
{
	return state->eat_depth > 0;
}
static int if_value(struct compile_state *state)
{
	int index, offset;
	index = state->if_depth / CHAR_BIT;
	offset = state->if_depth % CHAR_BIT;
	return !!(state->if_bytes[index] & (1 << (offset)));
}
static void set_if_value(struct compile_state *state, int value)
{
	int index, offset;
	index = state->if_depth / CHAR_BIT;
	offset = state->if_depth % CHAR_BIT;

	state->if_bytes[index] &= ~(1 << offset);
	if (value) {
		state->if_bytes[index] |= (1 << offset);
	}
}
static void in_if(struct compile_state *state, const char *name)
{
	if (state->if_depth <= 0) {
		error(state, 0, "%s without #if", name);
	}
}
static void enter_if(struct compile_state *state)
{
	state->if_depth += 1;
	if (state->if_depth > MAX_PP_IF_DEPTH) {
		error(state, 0, "#if depth too great");
	}
}
static void reenter_if(struct compile_state *state, const char *name)
{
	in_if(state, name);
	if ((state->eat_depth == state->if_depth) &&
		(state->eat_targ == TOK_MELSE)) {
		state->eat_depth = 0;
		state->eat_targ = 0;
	}
}
static void enter_else(struct compile_state *state, const char *name)
{
	in_if(state, name);
	if ((state->eat_depth == state->if_depth) &&
		(state->eat_targ == TOK_MELSE)) {
		state->eat_depth = 0;
		state->eat_targ = 0;
	}
}
static void exit_if(struct compile_state *state, const char *name)
{
	in_if(state, name);
	if (state->eat_depth == state->if_depth) {
		state->eat_depth = 0;
		state->eat_targ = 0;
	}
	state->if_depth -= 1;
}

static void raw_token(struct compile_state *state, struct token *tk)
{
	struct file_state *file;
	int rescan;

	file = state->file;
	raw_next_token(state, file, tk);
	do {
		rescan = 0;
		file = state->file;
		/* Exit out of an include directive or macro call */
		if ((tk->tok == TOK_EOF) &&
			(file != state->macro_file) && file->prev)
		{
			state->file = file->prev;
			/* file->basename is used keep it */
			xfree(file->dirname);
			xfree(file->buf);
			xfree(file);
			file = 0;
			raw_next_token(state, state->file, tk);
			rescan = 1;
		}
	} while(rescan);
}

static void pp_token(struct compile_state *state, struct token *tk)
{
	int rescan;

	raw_token(state, tk);
	do {
		rescan = 0;
		if (tk->tok == TOK_SPACE) {
			raw_token(state, tk);
			rescan = 1;
		}
		else if (tk->tok == TOK_IDENT) {
			if (state->token_base == 0) {
				ident_to_keyword(state, tk);
			} else {
				ident_to_macro(state, tk);
			}
		}
	} while(rescan);
}

static void preprocess(struct compile_state *state, struct token *tk);

static void token(struct compile_state *state, struct token *tk)
{
	int rescan;
	pp_token(state, tk);
	do {
		rescan = 0;
		/* Process a macro directive */
		if (tk->tok == TOK_MACRO) {
			/* Only match preprocessor directives at the start of a line */
			const char *ptr;
			ptr = state->file->line_start;
			while((ptr < tk->pos)
				&& spacep(get_char(state->file, ptr)))
			{
				ptr = next_char(state->file, ptr, 1);
			}
			if (ptr == tk->pos) {
				preprocess(state, tk);
				rescan = 1;
			}
		}
		/* Expand a macro call */
		else if (tk->ident && tk->ident->sym_define) {
			rescan = compile_macro(state, &state->file, tk);
			if (rescan) {
				pp_token(state, tk);
			}
		}
		/* Eat tokens disabled by the preprocessor
		 * (Unless we are parsing a preprocessor directive
		 */
		else if (if_eat(state) && (state->token_base == 0)) {
			pp_token(state, tk);
			rescan = 1;
		}
		/* Make certain EOL only shows up in preprocessor directives */
		else if ((tk->tok == TOK_EOL) && (state->token_base == 0)) {
			pp_token(state, tk);
			rescan = 1;
		}
		/* Error on unknown tokens */
		else if (tk->tok == TOK_UNKNOWN) {
			error(state, 0, "unknown token");
		}
	} while(rescan);
}


static inline struct token *get_token(struct compile_state *state, int offset)
{
	int index;
	index = state->token_base + offset;
	if (index >= sizeof(state->token)/sizeof(state->token[0])) {
		internal_error(state, 0, "token array to small");
	}
	return &state->token[index];
}

static struct token *do_eat_token(struct compile_state *state, int tok)
{
	struct token *tk;
	int i;
	check_tok(state, get_token(state, 1), tok);

	/* Free the old token value */
	tk = get_token(state, 0);
	if (tk->str_len) {
		memset((void *)tk->val.str, -1, tk->str_len);
		xfree(tk->val.str);
	}
	/* Overwrite the old token with newer tokens */
	for(i = state->token_base; i < sizeof(state->token)/sizeof(state->token[0]) - 1; i++) {
		state->token[i] = state->token[i + 1];
	}
	/* Clear the last token */
	memset(&state->token[i], 0, sizeof(state->token[i]));
	state->token[i].tok = -1;

	/* Return the token */
	return tk;
}

static int raw_peek(struct compile_state *state)
{
	struct token *tk1;
	tk1 = get_token(state, 1);
	if (tk1->tok == -1) {
		raw_token(state, tk1);
	}
	return tk1->tok;
}

static struct token *raw_eat(struct compile_state *state, int tok)
{
	raw_peek(state);
	return do_eat_token(state, tok);
}

static int pp_peek(struct compile_state *state)
{
	struct token *tk1;
	tk1 = get_token(state, 1);
	if (tk1->tok == -1) {
		pp_token(state, tk1);
	}
	return tk1->tok;
}

static struct token *pp_eat(struct compile_state *state, int tok)
{
	pp_peek(state);
	return do_eat_token(state, tok);
}

static int peek(struct compile_state *state)
{
	struct token *tk1;
	tk1 = get_token(state, 1);
	if (tk1->tok == -1) {
		token(state, tk1);
	}
	return tk1->tok;
}

static int peek2(struct compile_state *state)
{
	struct token *tk1, *tk2;
	tk1 = get_token(state, 1);
	tk2 = get_token(state, 2);
	if (tk1->tok == -1) {
		token(state, tk1);
	}
	if (tk2->tok == -1) {
		token(state, tk2);
	}
	return tk2->tok;
}

static struct token *eat(struct compile_state *state, int tok)
{
	peek(state);
	return do_eat_token(state, tok);
}

static void compile_file(struct compile_state *state, const char *filename, int local)
{
	char cwd[MAX_CWD_SIZE];
	const char *subdir, *base;
	int subdir_len;
	struct file_state *file;
	char *basename;
	file = xmalloc(sizeof(*file), "file_state");

	base = strrchr(filename, '/');
	subdir = filename;
	if (base != 0) {
		subdir_len = base - filename;
		base++;
	}
	else {
		base = filename;
		subdir_len = 0;
	}
	basename = xmalloc(strlen(base) +1, "basename");
	strcpy(basename, base);
	file->basename = basename;

	if (getcwd(cwd, sizeof(cwd)) == 0) {
		die("cwd buffer to small");
	}
	if ((subdir[0] == '/') || ((subdir[1] == ':') && ((subdir[2] == '/') || (subdir[2] == '\\')))) {
		file->dirname = xmalloc(subdir_len + 1, "dirname");
		memcpy(file->dirname, subdir, subdir_len);
		file->dirname[subdir_len] = '\0';
	}
	else {
		const char *dir;
		int dirlen;
		const char **path;
		/* Find the appropriate directory... */
		dir = 0;
		if (!state->file && exists(cwd, filename)) {
			dir = cwd;
		}
		if (local && state->file && exists(state->file->dirname, filename)) {
			dir = state->file->dirname;
		}
		for(path = state->compiler->include_paths; !dir && *path; path++) {
			if (exists(*path, filename)) {
				dir = *path;
			}
		}
		if (!dir) {
			error(state, 0, "Cannot open `%s'\n", filename);
		}
		dirlen = strlen(dir);
		file->dirname = xmalloc(dirlen + 1 + subdir_len + 1, "dirname");
		memcpy(file->dirname, dir, dirlen);
		file->dirname[dirlen] = '/';
		memcpy(file->dirname + dirlen + 1, subdir, subdir_len);
		file->dirname[dirlen + 1 + subdir_len] = '\0';
	}
	file->buf = slurp_file(file->dirname, file->basename, &file->size);

	file->pos = file->buf;
	file->line_start = file->pos;
	file->line = 1;

	file->report_line = 1;
	file->report_name = file->basename;
	file->report_dir  = file->dirname;
	file->macro       = 0;
	file->trigraphs   = (state->compiler->flags & COMPILER_TRIGRAPHS)? 1: 0;
	file->join_lines  = 1;

	file->prev = state->file;
	state->file = file;
}

static struct triple *constant_expr(struct compile_state *state);
static void integral(struct compile_state *state, struct triple *def);

static int mcexpr(struct compile_state *state)
{
	struct triple *cvalue;
	cvalue = constant_expr(state);
	integral(state, cvalue);
	if (cvalue->op != OP_INTCONST) {
		error(state, 0, "integer constant expected");
	}
	return cvalue->u.cval != 0;
}

static void preprocess(struct compile_state *state, struct token *current_token)
{
	/* Doing much more with the preprocessor would require
	 * a parser and a major restructuring.
	 * Postpone that for later.
	 */
	int old_token_base;
	int tok;

	state->macro_file = state->file;

	old_token_base = state->token_base;
	state->token_base = current_token - state->token;

	tok = pp_peek(state);
    	switch(tok) {
	case TOK_LIT_INT:
	{
		struct token *tk;
		int override_line;
		tk = pp_eat(state, TOK_LIT_INT);
		override_line = strtoul(tk->val.str, 0, 10);
		/* I have a preprocessor  line marker parse it */
		if (pp_peek(state) == TOK_LIT_STRING) {
			const char *token, *base;
			char *name, *dir;
			int name_len, dir_len;
			tk = pp_eat(state, TOK_LIT_STRING);
			name = xmalloc(tk->str_len, "report_name");
			token = tk->val.str + 1;
			base = strrchr(token, '/');
			name_len = tk->str_len -2;
			if (base != 0) {
				dir_len = base - token;
				base++;
				name_len -= base - token;
			} else {
				dir_len = 0;
				base = token;
			}
			memcpy(name, base, name_len);
			name[name_len] = '\0';
			dir = xmalloc(dir_len + 1, "report_dir");
			memcpy(dir, token, dir_len);
			dir[dir_len] = '\0';
			state->file->report_line = override_line - 1;
			state->file->report_name = name;
			state->file->report_dir = dir;
			state->file->macro      = 0;
		}
		break;
	}
	case TOK_MLINE:
	{
		struct token *tk;
		pp_eat(state, TOK_MLINE);
		tk = eat(state, TOK_LIT_INT);
		state->file->report_line = strtoul(tk->val.str, 0, 10) -1;
		if (pp_peek(state) == TOK_LIT_STRING) {
			const char *token, *base;
			char *name, *dir;
			int name_len, dir_len;
			tk = pp_eat(state, TOK_LIT_STRING);
			name = xmalloc(tk->str_len, "report_name");
			token = tk->val.str + 1;
			base = strrchr(token, '/');
			name_len = tk->str_len - 2;
			if (base != 0) {
				dir_len = base - token;
				base++;
				name_len -= base - token;
			} else {
				dir_len = 0;
				base = token;
			}
			memcpy(name, base, name_len);
			name[name_len] = '\0';
			dir = xmalloc(dir_len + 1, "report_dir");
			memcpy(dir, token, dir_len);
			dir[dir_len] = '\0';
			state->file->report_name = name;
			state->file->report_dir = dir;
			state->file->macro      = 0;
		}
		break;
	}
	case TOK_MUNDEF:
	{
		struct hash_entry *ident;
		pp_eat(state, TOK_MUNDEF);
		if (if_eat(state))  /* quit early when #if'd out */
			break;

		ident = pp_eat(state, TOK_MIDENT)->ident;

		undef_macro(state, ident);
		break;
	}
	case TOK_MPRAGMA:
		pp_eat(state, TOK_MPRAGMA);
		if (if_eat(state))  /* quit early when #if'd out */
			break;
		warning(state, 0, "Ignoring pragma");
		break;
	case TOK_MELIF:
		pp_eat(state, TOK_MELIF);
		reenter_if(state, "#elif");
		if (if_eat(state))   /* quit early when #if'd out */
			break;
		/* If the #if was taken the #elif just disables the following code */
		if (if_value(state)) {
			eat_tokens(state, TOK_MENDIF);
		}
		/* If the previous #if was not taken see if the #elif enables the
		 * trailing code.
		 */
		else {
			set_if_value(state, mcexpr(state));
			if (!if_value(state)) {
				eat_tokens(state, TOK_MELSE);
			}
		}
		break;
	case TOK_MIF:
		pp_eat(state, TOK_MIF);
		enter_if(state);
		if (if_eat(state))  /* quit early when #if'd out */
			break;
		set_if_value(state, mcexpr(state));
		if (!if_value(state)) {
			eat_tokens(state, TOK_MELSE);
		}
		break;
	case TOK_MIFNDEF:
	{
		struct hash_entry *ident;

		pp_eat(state, TOK_MIFNDEF);
		enter_if(state);
		if (if_eat(state))  /* quit early when #if'd out */
			break;
		ident = pp_eat(state, TOK_MIDENT)->ident;
		set_if_value(state, ident->sym_define == 0);
		if (!if_value(state)) {
			eat_tokens(state, TOK_MELSE);
		}
		break;
	}
	case TOK_MIFDEF:
	{
		struct hash_entry *ident;
		pp_eat(state, TOK_MIFDEF);
		enter_if(state);
		if (if_eat(state))  /* quit early when #if'd out */
			break;
		ident = pp_eat(state, TOK_MIDENT)->ident;
		set_if_value(state, ident->sym_define != 0);
		if (!if_value(state)) {
			eat_tokens(state, TOK_MELSE);
		}
		break;
	}
	case TOK_MELSE:
		pp_eat(state, TOK_MELSE);
		enter_else(state, "#else");
		if (!if_eat(state) && if_value(state)) {
			eat_tokens(state, TOK_MENDIF);
		}
		break;
	case TOK_MENDIF:
		pp_eat(state, TOK_MENDIF);
		exit_if(state, "#endif");
		break;
	case TOK_MDEFINE:
	{
		struct hash_entry *ident;
		struct macro_arg *args, **larg;
		const char *mstart, *mend;
		int argc;

		pp_eat(state, TOK_MDEFINE);
		if (if_eat(state))  /* quit early when #if'd out */
			break;
		ident = pp_eat(state, TOK_MIDENT)->ident;
		argc = -1;
		args = 0;
		larg = &args;

		/* Parse macro parameters */
		if (raw_peek(state) == TOK_LPAREN) {
			raw_eat(state, TOK_LPAREN);
			argc += 1;

			for(;;) {
				struct macro_arg *narg, *arg;
				struct hash_entry *aident;
				int tok;

				tok = pp_peek(state);
				if (!args && (tok == TOK_RPAREN)) {
					break;
				}
				else if (tok == TOK_DOTS) {
					pp_eat(state, TOK_DOTS);
					aident = state->i___VA_ARGS__;
				}
				else {
					aident = pp_eat(state, TOK_MIDENT)->ident;
				}

				narg = xcmalloc(sizeof(*arg), "macro arg");
				narg->ident = aident;

				/* Verify I don't have a duplicate identifier */
				for(arg = args; arg; arg = arg->next) {
					if (arg->ident == narg->ident) {
						error(state, 0, "Duplicate macro arg `%s'",
							narg->ident->name);
					}
				}
				/* Add the new argument to the end of the list */
				*larg = narg;
				larg = &narg->next;
				argc += 1;

				if ((aident == state->i___VA_ARGS__) ||
					(pp_peek(state) != TOK_COMMA)) {
					break;
				}
				pp_eat(state, TOK_COMMA);
			}
			pp_eat(state, TOK_RPAREN);
		}
		/* Remove leading whitespace */
		while(raw_peek(state) == TOK_SPACE) {
			raw_eat(state, TOK_SPACE);
		}

		/* Remember the start of the macro body */
		raw_peek(state);
		mend = mstart = get_token(state, 1)->pos;

		/* Find the end of the macro */
		for(tok = raw_peek(state); tok != TOK_EOL; tok = raw_peek(state)) {
			raw_eat(state, tok);
			/* Remember the end of the last non space token */
			raw_peek(state);
			if (tok != TOK_SPACE) {
				mend = get_token(state, 1)->pos;
			}
		}

		/* Now that I have found the body defined the token */
		do_define_macro(state, ident,
			char_strdup(state->file, mstart, mend, "macro buf"),
			argc, args);
		break;
	}
	case TOK_MERROR:
	{
		const char *start, *end;
		int len;

		pp_eat(state, TOK_MERROR);
		/* Find the start of the line */
		raw_peek(state);
		start = get_token(state, 1)->pos;

		/* Find the end of the line */
		while((tok = raw_peek(state)) != TOK_EOL) {
			raw_eat(state, tok);
		}
		end = get_token(state, 1)->pos;
		len = end - start;
		if (!if_eat(state)) {
			error(state, 0, "%*.*s", len, len, start);
		}
		break;
	}
	case TOK_MWARNING:
	{
		const char *start, *end;
		int len;

		pp_eat(state, TOK_MWARNING);

		/* Find the start of the line */
		raw_peek(state);
		start = get_token(state, 1)->pos;

		/* Find the end of the line */
		while((tok = raw_peek(state)) != TOK_EOL) {
			raw_eat(state, tok);
		}
		end = get_token(state, 1)->pos;
		len = end - start;
		if (!if_eat(state)) {
			warning(state, 0, "%*.*s", len, len, start);
		}
		break;
	}
	case TOK_MINCLUDE:
	{
		char *name;
		int local;
		name = 0;

		pp_eat(state, TOK_MINCLUDE);
		if (if_eat(state)) {
			/* Find the end of the line */
			while((tok = raw_peek(state)) != TOK_EOL) {
				raw_eat(state, tok);
			}
			break;
		}
		tok = peek(state);
		if (tok == TOK_LIT_STRING) {
			struct token *tk;
			const char *token;
			int name_len;
			tk = eat(state, TOK_LIT_STRING);
			name = xmalloc(tk->str_len, "include");
			token = tk->val.str +1;
			name_len = tk->str_len -2;
			if (*token == '"') {
				token++;
				name_len--;
			}
			memcpy(name, token, name_len);
			name[name_len] = '\0';
			local = 1;
		}
		else if (tok == TOK_LESS) {
			struct macro_buf buf;
			eat(state, TOK_LESS);

			buf.len = 40;
			buf.str = xmalloc(buf.len, "include");
			buf.pos = 0;

			tok = peek(state);
			while((tok != TOK_MORE) &&
				(tok != TOK_EOL) && (tok != TOK_EOF))
			{
				struct token *tk;
				tk = eat(state, tok);
				append_macro_chars(state, "include", &buf,
					state->file, tk->pos, state->file->pos);
				tok = peek(state);
			}
			append_macro_text(state, "include", &buf, "\0", 1);
			if (peek(state) != TOK_MORE) {
				error(state, 0, "Unterminated include directive");
			}
			eat(state, TOK_MORE);
			local = 0;
			name = buf.str;
		}
		else {
			error(state, 0, "Invalid include directive");
		}
		/* Error if there are any tokens after the include */
		if (pp_peek(state) != TOK_EOL) {
			error(state, 0, "garbage after include directive");
		}
		if (!if_eat(state)) {
			compile_file(state, name, local);
		}
		xfree(name);
		break;
	}
	case TOK_EOL:
		/* Ignore # without a following ident */
		break;
	default:
	{
		const char *name1, *name2;
		name1 = tokens[tok];
		name2 = "";
		if (tok == TOK_MIDENT) {
			name2 = get_token(state, 1)->ident->name;
		}
		error(state, 0, "Invalid preprocessor directive: %s %s",
			name1, name2);
		break;
	}
	}
	/* Consume the rest of the macro line */
	do {
		tok = pp_peek(state);
		pp_eat(state, tok);
	} while((tok != TOK_EOF) && (tok != TOK_EOL));
	state->token_base = old_token_base;
	state->macro_file = NULL;
	return;
}

/* Type helper functions */

static struct type *new_type(
	unsigned int type, struct type *left, struct type *right)
{
	struct type *result;
	result = xmalloc(sizeof(*result), "type");
	result->type = type;
	result->left = left;
	result->right = right;
	result->field_ident = 0;
	result->type_ident = 0;
	result->elements = 0;
	return result;
}

static struct type *clone_type(unsigned int specifiers, struct type *old)
{
	struct type *result;
	result = xmalloc(sizeof(*result), "type");
	memcpy(result, old, sizeof(*result));
	result->type &= TYPE_MASK;
	result->type |= specifiers;
	return result;
}

static struct type *dup_type(struct compile_state *state, struct type *orig)
{
	struct type *new;
	new = xcmalloc(sizeof(*new), "type");
	new->type = orig->type;
	new->field_ident = orig->field_ident;
	new->type_ident  = orig->type_ident;
	new->elements    = orig->elements;
	if (orig->left) {
		new->left = dup_type(state, orig->left);
	}
	if (orig->right) {
		new->right = dup_type(state, orig->right);
	}
	return new;
}


static struct type *invalid_type(struct compile_state *state, struct type *type)
{
	struct type *invalid, *member;
	invalid = 0;
	if (!type) {
		internal_error(state, 0, "type missing?");
	}
	switch(type->type & TYPE_MASK) {
	case TYPE_VOID:
	case TYPE_CHAR:		case TYPE_UCHAR:
	case TYPE_SHORT:	case TYPE_USHORT:
	case TYPE_INT:		case TYPE_UINT:
	case TYPE_LONG:		case TYPE_ULONG:
	case TYPE_LLONG:	case TYPE_ULLONG:
	case TYPE_POINTER:
	case TYPE_ENUM:
		break;
	case TYPE_BITFIELD:
		invalid = invalid_type(state, type->left);
		break;
	case TYPE_ARRAY:
		invalid = invalid_type(state, type->left);
		break;
	case TYPE_STRUCT:
	case TYPE_TUPLE:
		member = type->left;
		while(member && (invalid == 0) &&
			((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
			invalid = invalid_type(state, member->left);
			member = member->right;
		}
		if (!invalid) {
			invalid = invalid_type(state, member);
		}
		break;
	case TYPE_UNION:
	case TYPE_JOIN:
		member = type->left;
		while(member && (invalid == 0) &&
			((member->type & TYPE_MASK) == TYPE_OVERLAP)) {
			invalid = invalid_type(state, member->left);
			member = member->right;
		}
		if (!invalid) {
			invalid = invalid_type(state, member);
		}
		break;
	default:
		invalid = type;
		break;
	}
	return invalid;

}

static struct type void_type    = { .type  = TYPE_VOID };
static struct type char_type    = { .type  = TYPE_CHAR };
static struct type uchar_type   = { .type  = TYPE_UCHAR };
#if DEBUG_ROMCC_WARNING
static struct type short_type   = { .type  = TYPE_SHORT };
#endif
static struct type ushort_type  = { .type  = TYPE_USHORT };
static struct type int_type     = { .type  = TYPE_INT };
static struct type uint_type    = { .type  = TYPE_UINT };
static struct type long_type    = { .type  = TYPE_LONG };
static struct type ulong_type   = { .type  = TYPE_ULONG };
static struct type unknown_type = { .type  = TYPE_UNKNOWN };

static struct type void_ptr_type  = {
	.type = TYPE_POINTER,
	.left = &void_type,
};

#if DEBUG_ROMCC_WARNING
static struct type void_func_type = {
	.type  = TYPE_FUNCTION,
	.left  = &void_type,
	.right = &void_type,
};
#endif

static size_t bits_to_bytes(size_t size)
{
	return (size + SIZEOF_CHAR - 1)/SIZEOF_CHAR;
}

static struct triple *variable(struct compile_state *state, struct type *type)
{
	struct triple *result;
	if ((type->type & STOR_MASK) != STOR_PERM) {
		result = triple(state, OP_ADECL, type, 0, 0);
		generate_lhs_pieces(state, result);
	}
	else {
		result = triple(state, OP_SDECL, type, 0, 0);
	}
	return result;
}

static void stor_of(FILE *fp, struct type *type)
{
	switch(type->type & STOR_MASK) {
	case STOR_AUTO:
		fprintf(fp, "auto ");
		break;
	case STOR_STATIC:
		fprintf(fp, "static ");
		break;
	case STOR_LOCAL:
		fprintf(fp, "local ");
		break;
	case STOR_EXTERN:
		fprintf(fp, "extern ");
		break;
	case STOR_REGISTER:
		fprintf(fp, "register ");
		break;
	case STOR_TYPEDEF:
		fprintf(fp, "typedef ");
		break;
	case STOR_INLINE | STOR_LOCAL:
		fprintf(fp, "inline ");
		break;
	case STOR_INLINE | STOR_STATIC:
		fprintf(fp, "static inline");
		break;
	case STOR_INLINE | STOR_EXTERN:
		fprintf(fp, "extern inline");
		break;
	default:
		fprintf(fp, "stor:%x", type->type & STOR_MASK);
		break;
	}
}
static void qual_of(FILE *fp, struct type *type)
{
	if (type->type & QUAL_CONST) {
		fprintf(fp, " const");
	}
	if (type->type & QUAL_VOLATILE) {
		fprintf(fp, " volatile");
	}
	if (type->type & QUAL_RESTRICT) {
		fprintf(fp, " restrict");
	}
}

static void name_of(FILE *fp, struct type *type)
{
	unsigned int base_type;
	base_type = type->type & TYPE_MASK;
	if ((base_type != TYPE_PRODUCT) && (base_type != TYPE_OVERLAP)) {
		stor_of(fp, type);
	}
	switch(base_type) {
	case TYPE_VOID:
		fprintf(fp, "void");
		qual_of(fp, type);
		break;
	case TYPE_CHAR:
		fprintf(fp, "signed char");
		qual_of(fp, type);
		break;
	case TYPE_UCHAR:
		fprintf(fp, "unsigned char");
		qual_of(fp, type);
		break;
	case TYPE_SHORT:
		fprintf(fp, "signed short");
		qual_of(fp, type);
		break;
	case TYPE_USHORT:
		fprintf(fp, "unsigned short");
		qual_of(fp, type);
		break;
	case TYPE_INT:
		fprintf(fp, "signed int");
		qual_of(fp, type);
		break;
	case TYPE_UINT:
		fprintf(fp, "unsigned int");
		qual_of(fp, type);
		break;
	case TYPE_LONG:
		fprintf(fp, "signed long");
		qual_of(fp, type);
		break;
	case TYPE_ULONG:
		fprintf(fp, "unsigned long");
		qual_of(fp, type);
		break;
	case TYPE_POINTER:
		name_of(fp, type->left);
		fprintf(fp, " * ");
		qual_of(fp, type);
		break;
	case TYPE_PRODUCT:
		name_of(fp, type->left);
		fprintf(fp, ", ");
		name_of(fp, type->right);
		break;
	case TYPE_OVERLAP:
		name_of(fp, type->left);
		fprintf(fp, ",| ");
		name_of(fp, type->right);
		break;
	case TYPE_ENUM:
		fprintf(fp, "enum %s",
			(type->type_ident)? type->type_ident->name : "");
		qual_of(fp, type);
		break;
	case TYPE_STRUCT:
		fprintf(fp, "struct %s { ",
			(type->type_ident)? type->type_ident->name : "");
		name_of(fp, type->left);
		fprintf(fp, " } ");
		qual_of(fp, type);
		break;
	case TYPE_UNION:
		fprintf(fp, "union %s { ",
			(type->type_ident)? type->type_ident->name : "");
		name_of(fp, type->left);
		fprintf(fp, " } ");
		qual_of(fp, type);
		break;
	case TYPE_FUNCTION:
		name_of(fp, type->left);
		fprintf(fp, " (*)(");
		name_of(fp, type->right);
		fprintf(fp, ")");
		break;
	case TYPE_ARRAY:
		name_of(fp, type->left);
		fprintf(fp, " [%ld]", (long)(type->elements));
		break;
	case TYPE_TUPLE:
		fprintf(fp, "tuple { ");
		name_of(fp, type->left);
		fprintf(fp, " } ");
		qual_of(fp, type);
		break;
	case TYPE_JOIN:
		fprintf(fp, "join { ");
		name_of(fp, type->left);
		fprintf(fp, " } ");
		qual_of(fp, type);
		break;
	case TYPE_BITFIELD:
		name_of(fp, type->left);
		fprintf(fp, " : %d ", type->elements);
		qual_of(fp, type);
		break;
	case TYPE_UNKNOWN:
		fprintf(fp, "unknown_t");
		break;
	default:
		fprintf(fp, "????: %x", base_type);
		break;
	}
	if (type->field_ident && type->field_ident->name) {
		fprintf(fp, " .%s", type->field_ident->name);
	}
}

static size_t align_of(struct compile_state *state, struct type *type)
{
	size_t align;
	switch(type->type & TYPE_MASK) {
	case TYPE_VOID:
		align = 1;
		break;
	case TYPE_BITFIELD:
		align = 1;
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		align = ALIGNOF_CHAR;
		break;
	case TYPE_SHORT:
	case TYPE_USHORT:
		align = ALIGNOF_SHORT;
		break;
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_ENUM:
		align = ALIGNOF_INT;
		break;
	case TYPE_LONG:
	case TYPE_ULONG:
		align = ALIGNOF_LONG;
		break;
	case TYPE_POINTER:
		align = ALIGNOF_POINTER;
		break;
	case TYPE_PRODUCT:
	case TYPE_OVERLAP:
	{
		size_t left_align, right_align;
		left_align  = align_of(state, type->left);
		right_align = align_of(state, type->right);
		align = (left_align >= right_align) ? left_align : right_align;
		break;
	}
	case TYPE_ARRAY:
		align = align_of(state, type->left);
		break;
	case TYPE_STRUCT:
	case TYPE_TUPLE:
	case TYPE_UNION:
	case TYPE_JOIN:
		align = align_of(state, type->left);
		break;
	default:
		error(state, 0, "alignof not yet defined for type\n");
		break;
	}
	return align;
}

static size_t reg_align_of(struct compile_state *state, struct type *type)
{
	size_t align;
	switch(type->type & TYPE_MASK) {
	case TYPE_VOID:
		align = 1;
		break;
	case TYPE_BITFIELD:
		align = 1;
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		align = REG_ALIGNOF_CHAR;
		break;
	case TYPE_SHORT:
	case TYPE_USHORT:
		align = REG_ALIGNOF_SHORT;
		break;
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_ENUM:
		align = REG_ALIGNOF_INT;
		break;
	case TYPE_LONG:
	case TYPE_ULONG:
		align = REG_ALIGNOF_LONG;
		break;
	case TYPE_POINTER:
		align = REG_ALIGNOF_POINTER;
		break;
	case TYPE_PRODUCT:
	case TYPE_OVERLAP:
	{
		size_t left_align, right_align;
		left_align  = reg_align_of(state, type->left);
		right_align = reg_align_of(state, type->right);
		align = (left_align >= right_align) ? left_align : right_align;
		break;
	}
	case TYPE_ARRAY:
		align = reg_align_of(state, type->left);
		break;
	case TYPE_STRUCT:
	case TYPE_UNION:
	case TYPE_TUPLE:
	case TYPE_JOIN:
		align = reg_align_of(state, type->left);
		break;
	default:
		error(state, 0, "alignof not yet defined for type\n");
		break;
	}
	return align;
}

static size_t align_of_in_bytes(struct compile_state *state, struct type *type)
{
	return bits_to_bytes(align_of(state, type));
}
static size_t size_of(struct compile_state *state, struct type *type);
static size_t reg_size_of(struct compile_state *state, struct type *type);

static size_t needed_padding(struct compile_state *state,
	struct type *type, size_t offset)
{
        size_t padding, align;
	align = align_of(state, type);
	/* Align to the next machine word if the bitfield does completely
	 * fit into the current word.
	 */
	if ((type->type & TYPE_MASK) == TYPE_BITFIELD) {
		size_t size;
		size = size_of(state, type);
		if ((offset + type->elements)/size != offset/size) {
			align = size;
		}
	}
	padding = 0;
	if (offset % align) {
		padding = align - (offset % align);
	}
	return padding;
}

static size_t reg_needed_padding(struct compile_state *state,
	struct type *type, size_t offset)
{
        size_t padding, align;
	align = reg_align_of(state, type);
	/* Align to the next register word if the bitfield does completely
	 * fit into the current register.
	 */
	if (((type->type & TYPE_MASK) == TYPE_BITFIELD) &&
		(((offset + type->elements)/REG_SIZEOF_REG) != (offset/REG_SIZEOF_REG)))
	{
		align = REG_SIZEOF_REG;
	}
	padding = 0;
	if (offset % align) {
		padding = align - (offset % align);
	}
	return padding;
}

static size_t size_of(struct compile_state *state, struct type *type)
{
	size_t size;
	switch(type->type & TYPE_MASK) {
	case TYPE_VOID:
		size = 0;
		break;
	case TYPE_BITFIELD:
		size = type->elements;
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		size = SIZEOF_CHAR;
		break;
	case TYPE_SHORT:
	case TYPE_USHORT:
		size = SIZEOF_SHORT;
		break;
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_ENUM:
		size = SIZEOF_INT;
		break;
	case TYPE_LONG:
	case TYPE_ULONG:
		size = SIZEOF_LONG;
		break;
	case TYPE_POINTER:
		size = SIZEOF_POINTER;
		break;
	case TYPE_PRODUCT:
	{
		size_t pad;
		size = 0;
		while((type->type & TYPE_MASK) == TYPE_PRODUCT) {
			pad = needed_padding(state, type->left, size);
			size = size + pad + size_of(state, type->left);
			type = type->right;
		}
		pad = needed_padding(state, type, size);
		size = size + pad + size_of(state, type);
		break;
	}
	case TYPE_OVERLAP:
	{
		size_t size_left, size_right;
		size_left = size_of(state, type->left);
		size_right = size_of(state, type->right);
		size = (size_left >= size_right)? size_left : size_right;
		break;
	}
	case TYPE_ARRAY:
		if (type->elements == ELEMENT_COUNT_UNSPECIFIED) {
			internal_error(state, 0, "Invalid array type");
		} else {
			size = size_of(state, type->left) * type->elements;
		}
		break;
	case TYPE_STRUCT:
	case TYPE_TUPLE:
	{
		size_t pad;
		size = size_of(state, type->left);
		/* Pad structures so their size is a multiples of their alignment */
		pad = needed_padding(state, type, size);
		size = size + pad;
		break;
	}
	case TYPE_UNION:
	case TYPE_JOIN:
	{
		size_t pad;
		size = size_of(state, type->left);
		/* Pad unions so their size is a multiple of their alignment */
		pad = needed_padding(state, type, size);
		size = size + pad;
		break;
	}
	default:
		internal_error(state, 0, "sizeof not yet defined for type");
		break;
	}
	return size;
}

static size_t reg_size_of(struct compile_state *state, struct type *type)
{
	size_t size;
	switch(type->type & TYPE_MASK) {
	case TYPE_VOID:
		size = 0;
		break;
	case TYPE_BITFIELD:
		size = type->elements;
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		size = REG_SIZEOF_CHAR;
		break;
	case TYPE_SHORT:
	case TYPE_USHORT:
		size = REG_SIZEOF_SHORT;
		break;
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_ENUM:
		size = REG_SIZEOF_INT;
		break;
	case TYPE_LONG:
	case TYPE_ULONG:
		size = REG_SIZEOF_LONG;
		break;
	case TYPE_POINTER:
		size = REG_SIZEOF_POINTER;
		break;
	case TYPE_PRODUCT:
	{
		size_t pad;
		size = 0;
		while((type->type & TYPE_MASK) == TYPE_PRODUCT) {
			pad = reg_needed_padding(state, type->left, size);
			size = size + pad + reg_size_of(state, type->left);
			type = type->right;
		}
		pad = reg_needed_padding(state, type, size);
		size = size + pad + reg_size_of(state, type);
		break;
	}
	case TYPE_OVERLAP:
	{
		size_t size_left, size_right;
		size_left  = reg_size_of(state, type->left);
		size_right = reg_size_of(state, type->right);
		size = (size_left >= size_right)? size_left : size_right;
		break;
	}
	case TYPE_ARRAY:
		if (type->elements == ELEMENT_COUNT_UNSPECIFIED) {
			internal_error(state, 0, "Invalid array type");
		} else {
			size = reg_size_of(state, type->left) * type->elements;
		}
		break;
	case TYPE_STRUCT:
	case TYPE_TUPLE:
	{
		size_t pad;
		size = reg_size_of(state, type->left);
		/* Pad structures so their size is a multiples of their alignment */
		pad = reg_needed_padding(state, type, size);
		size = size + pad;
		break;
	}
	case TYPE_UNION:
	case TYPE_JOIN:
	{
		size_t pad;
		size = reg_size_of(state, type->left);
		/* Pad unions so their size is a multiple of their alignment */
		pad = reg_needed_padding(state, type, size);
		size = size + pad;
		break;
	}
	default:
		internal_error(state, 0, "sizeof not yet defined for type");
		break;
	}
	return size;
}

static size_t registers_of(struct compile_state *state, struct type *type)
{
	size_t registers;
	registers = reg_size_of(state, type);
	registers += REG_SIZEOF_REG - 1;
	registers /= REG_SIZEOF_REG;
	return registers;
}

static size_t size_of_in_bytes(struct compile_state *state, struct type *type)
{
	return bits_to_bytes(size_of(state, type));
}

static size_t field_offset(struct compile_state *state,
	struct type *type, struct hash_entry *field)
{
	struct type *member;
	size_t size;

	size = 0;
	member = 0;
	if ((type->type & TYPE_MASK) == TYPE_STRUCT) {
		member = type->left;
		while(member && ((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
			size += needed_padding(state, member->left, size);
			if (member->left->field_ident == field) {
				member = member->left;
				break;
			}
			size += size_of(state, member->left);
			member = member->right;
		}
		size += needed_padding(state, member, size);
	}
	else if ((type->type & TYPE_MASK) == TYPE_UNION) {
		member = type->left;
		while(member && ((member->type & TYPE_MASK) == TYPE_OVERLAP)) {
			if (member->left->field_ident == field) {
				member = member->left;
				break;
			}
			member = member->right;
		}
	}
	else {
		internal_error(state, 0, "field_offset only works on structures and unions");
	}

	if (!member || (member->field_ident != field)) {
		error(state, 0, "member %s not present", field->name);
	}
	return size;
}

static size_t field_reg_offset(struct compile_state *state,
	struct type *type, struct hash_entry *field)
{
	struct type *member;
	size_t size;

	size = 0;
	member = 0;
	if ((type->type & TYPE_MASK) == TYPE_STRUCT) {
		member = type->left;
		while(member && ((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
			size += reg_needed_padding(state, member->left, size);
			if (member->left->field_ident == field) {
				member = member->left;
				break;
			}
			size += reg_size_of(state, member->left);
			member = member->right;
		}
	}
	else if ((type->type & TYPE_MASK) == TYPE_UNION) {
		member = type->left;
		while(member && ((member->type & TYPE_MASK) == TYPE_OVERLAP)) {
			if (member->left->field_ident == field) {
				member = member->left;
				break;
			}
			member = member->right;
		}
	}
	else {
		internal_error(state, 0, "field_reg_offset only works on structures and unions");
	}

	if (!member || (member->field_ident != field)) {
		error(state, 0, "member %s not present", field->name);
	}
	size += reg_needed_padding(state, member, size);
	return size;
}

static struct type *field_type(struct compile_state *state,
	struct type *type, struct hash_entry *field)
{
	struct type *member;

	member = 0;
	if ((type->type & TYPE_MASK) == TYPE_STRUCT) {
		member = type->left;
		while(member && ((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
			if (member->left->field_ident == field) {
				member = member->left;
				break;
			}
			member = member->right;
		}
	}
	else if ((type->type & TYPE_MASK) == TYPE_UNION) {
		member = type->left;
		while(member && ((member->type & TYPE_MASK) == TYPE_OVERLAP)) {
			if (member->left->field_ident == field) {
				member = member->left;
				break;
			}
			member = member->right;
		}
	}
	else {
		internal_error(state, 0, "field_type only works on structures and unions");
	}

	if (!member || (member->field_ident != field)) {
		error(state, 0, "member %s not present", field->name);
	}
	return member;
}

static size_t index_offset(struct compile_state *state,
	struct type *type, ulong_t index)
{
	struct type *member;
	size_t size;
	size = 0;
	if ((type->type & TYPE_MASK) == TYPE_ARRAY) {
		size = size_of(state, type->left) * index;
	}
	else if ((type->type & TYPE_MASK) == TYPE_TUPLE) {
		ulong_t i;
		member = type->left;
		i = 0;
		while(member && ((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
			size += needed_padding(state, member->left, size);
			if (i == index) {
				member = member->left;
				break;
			}
			size += size_of(state, member->left);
			i++;
			member = member->right;
		}
		size += needed_padding(state, member, size);
		if (i != index) {
			internal_error(state, 0, "Missing member index: %u", index);
		}
	}
	else if ((type->type & TYPE_MASK) == TYPE_JOIN) {
		ulong_t i;
		size = 0;
		member = type->left;
		i = 0;
		while(member && ((member->type & TYPE_MASK) == TYPE_OVERLAP)) {
			if (i == index) {
				break;
			}
			i++;
			member = member->right;
		}
		if (i != index) {
			internal_error(state, 0, "Missing member index: %u", index);
		}
	}
	else {
		internal_error(state, 0,
			"request for index %u in something not an array, tuple or join",
			index);
	}
	return size;
}

static size_t index_reg_offset(struct compile_state *state,
	struct type *type, ulong_t index)
{
	struct type *member;
	size_t size;
	size = 0;
	if ((type->type & TYPE_MASK) == TYPE_ARRAY) {
		size = reg_size_of(state, type->left) * index;
	}
	else if ((type->type & TYPE_MASK) == TYPE_TUPLE) {
		ulong_t i;
		member = type->left;
		i = 0;
		while(member && ((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
			size += reg_needed_padding(state, member->left, size);
			if (i == index) {
				member = member->left;
				break;
			}
			size += reg_size_of(state, member->left);
			i++;
			member = member->right;
		}
		if (i != index) {
			internal_error(state, 0, "Missing member index: %u", index);
		}
		size += reg_needed_padding(state, member, size);
	}
	else if ((type->type & TYPE_MASK) == TYPE_JOIN) {
		ulong_t i;
		size = 0;
		member = type->left;
		i = 0;
		while(member && ((member->type & TYPE_MASK) == TYPE_OVERLAP)) {
			if (i == index) {
				break;
			}
			i++;
			member = member->right;
		}
		if (i != index) {
			internal_error(state, 0, "Missing member index: %u", index);
		}
	}
	else {
		internal_error(state, 0,
			"request for index %u in something not an array, tuple or join",
			index);
	}
	return size;
}

static struct type *index_type(struct compile_state *state,
	struct type *type, ulong_t index)
{
	struct type *member;
	if (index >= type->elements) {
		internal_error(state, 0, "Invalid element %u requested", index);
	}
	if ((type->type & TYPE_MASK) == TYPE_ARRAY) {
		member = type->left;
	}
	else if ((type->type & TYPE_MASK) == TYPE_TUPLE) {
		ulong_t i;
		member = type->left;
		i = 0;
		while(member && ((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
			if (i == index) {
				member = member->left;
				break;
			}
			i++;
			member = member->right;
		}
		if (i != index) {
			internal_error(state, 0, "Missing member index: %u", index);
		}
	}
	else if ((type->type & TYPE_MASK) == TYPE_JOIN) {
		ulong_t i;
		member = type->left;
		i = 0;
		while(member && ((member->type & TYPE_MASK) == TYPE_OVERLAP)) {
			if (i == index) {
				member = member->left;
				break;
			}
			i++;
			member = member->right;
		}
		if (i != index) {
			internal_error(state, 0, "Missing member index: %u", index);
		}
	}
	else {
		member = 0;
		internal_error(state, 0,
			"request for index %u in something not an array, tuple or join",
			index);
	}
	return member;
}

static struct type *unpack_type(struct compile_state *state, struct type *type)
{
	/* If I have a single register compound type not a bit-field
	 * find the real type.
	 */
	struct type *start_type;
	size_t size;
	/* Get out early if I need multiple registers for this type */
	size = reg_size_of(state, type);
	if (size > REG_SIZEOF_REG) {
		return type;
	}
	/* Get out early if I don't need any registers for this type */
	if (size == 0) {
		return &void_type;
	}
	/* Loop until I have no more layers I can remove */
	do {
		start_type = type;
		switch(type->type & TYPE_MASK) {
		case TYPE_ARRAY:
			/* If I have a single element the unpacked type
			 * is that element.
			 */
			if (type->elements == 1) {
				type = type->left;
			}
			break;
		case TYPE_STRUCT:
		case TYPE_TUPLE:
			/* If I have a single element the unpacked type
			 * is that element.
			 */
			if (type->elements == 1) {
				type = type->left;
			}
			/* If I have multiple elements the unpacked
			 * type is the non-void element.
			 */
			else {
				struct type *next, *member;
				struct type *sub_type;
				sub_type = 0;
				next = type->left;
				while(next) {
					member = next;
					next = 0;
					if ((member->type & TYPE_MASK) == TYPE_PRODUCT) {
						next = member->right;
						member = member->left;
					}
					if (reg_size_of(state, member) > 0) {
						if (sub_type) {
							internal_error(state, 0, "true compound type in a register");
						}
						sub_type = member;
					}
				}
				if (sub_type) {
					type = sub_type;
				}
			}
			break;

		case TYPE_UNION:
		case TYPE_JOIN:
			/* If I have a single element the unpacked type
			 * is that element.
			 */
			if (type->elements == 1) {
				type = type->left;
			}
			/* I can't in general unpack union types */
			break;
		default:
			/* If I'm not a compound type I can't unpack it */
			break;
		}
	} while(start_type != type);
	switch(type->type & TYPE_MASK) {
	case TYPE_STRUCT:
	case TYPE_ARRAY:
	case TYPE_TUPLE:
		internal_error(state, 0, "irredicible type?");
		break;
	}
	return type;
}

static int equiv_types(struct type *left, struct type *right);
static int is_compound_type(struct type *type);

static struct type *reg_type(
	struct compile_state *state, struct type *type, int reg_offset)
{
	struct type *member;
	size_t size;
#if 1
	struct type *invalid;
	invalid = invalid_type(state, type);
	if (invalid) {
		fprintf(state->errout, "type: ");
		name_of(state->errout, type);
		fprintf(state->errout, "\n");
		fprintf(state->errout, "invalid: ");
		name_of(state->errout, invalid);
		fprintf(state->errout, "\n");
		internal_error(state, 0, "bad input type?");
	}
#endif

	size = reg_size_of(state, type);
	if (reg_offset > size) {
		member = 0;
		fprintf(state->errout, "type: ");
		name_of(state->errout, type);
		fprintf(state->errout, "\n");
		internal_error(state, 0, "offset outside of type");
	}
	else {
		switch(type->type & TYPE_MASK) {
			/* Don't do anything with the basic types */
		case TYPE_VOID:
		case TYPE_CHAR:		case TYPE_UCHAR:
		case TYPE_SHORT:	case TYPE_USHORT:
		case TYPE_INT:		case TYPE_UINT:
		case TYPE_LONG:		case TYPE_ULONG:
		case TYPE_LLONG:	case TYPE_ULLONG:
		case TYPE_FLOAT:	case TYPE_DOUBLE:
		case TYPE_LDOUBLE:
		case TYPE_POINTER:
		case TYPE_ENUM:
		case TYPE_BITFIELD:
			member = type;
			break;
		case TYPE_ARRAY:
			member = type->left;
			size = reg_size_of(state, member);
			if (size > REG_SIZEOF_REG) {
				member = reg_type(state, member, reg_offset % size);
			}
			break;
		case TYPE_STRUCT:
		case TYPE_TUPLE:
		{
			size_t offset;
			offset = 0;
			member = type->left;
			while(member && ((member->type & TYPE_MASK) == TYPE_PRODUCT)) {
				size = reg_size_of(state, member->left);
				offset += reg_needed_padding(state, member->left, offset);
				if ((offset + size) > reg_offset) {
					member = member->left;
					break;
				}
				offset += size;
				member = member->right;
			}
			offset += reg_needed_padding(state, member, offset);
			member = reg_type(state, member, reg_offset - offset);
			break;
		}
		case TYPE_UNION:
		case TYPE_JOIN:
		{
			struct type *join, **jnext, *mnext;
			join = new_type(TYPE_JOIN, 0, 0);
			jnext = &join->left;
			mnext = type->left;
			while(mnext) {
				size_t size;
				member = mnext;
				mnext = 0;
				if ((member->type & TYPE_MASK) == TYPE_OVERLAP) {
					mnext = member->right;
					member = member->left;
				}
				size = reg_size_of(state, member);
				if (size > reg_offset) {
					struct type *part, *hunt;
					part = reg_type(state, member, reg_offset);
					/* See if this type is already in the union */
					hunt = join->left;
					while(hunt) {
						struct type *test = hunt;
						hunt = 0;
						if ((test->type & TYPE_MASK) == TYPE_OVERLAP) {
							hunt = test->right;
							test = test->left;
						}
						if (equiv_types(part, test)) {
							goto next;
						}
					}
					/* Nope add it */
					if (!*jnext) {
						*jnext = part;
					} else {
						*jnext = new_type(TYPE_OVERLAP, *jnext, part);
						jnext = &(*jnext)->right;
					}
					join->elements++;
				}
			next:
				;
			}
			if (join->elements == 0) {
				internal_error(state, 0, "No elements?");
			}
			member = join;
			break;
		}
		default:
			member = 0;
			fprintf(state->errout, "type: ");
			name_of(state->errout, type);
			fprintf(state->errout, "\n");
			internal_error(state, 0, "reg_type not yet defined for type");

		}
	}
	/* If I have a single register compound type not a bit-field
	 * find the real type.
	 */
	member = unpack_type(state, member);
		;
	size  = reg_size_of(state, member);
	if (size > REG_SIZEOF_REG) {
		internal_error(state, 0, "Cannot find type of single register");
	}
#if 1
	invalid = invalid_type(state, member);
	if (invalid) {
		fprintf(state->errout, "type: ");
		name_of(state->errout, member);
		fprintf(state->errout, "\n");
		fprintf(state->errout, "invalid: ");
		name_of(state->errout, invalid);
		fprintf(state->errout, "\n");
		internal_error(state, 0, "returning bad type?");
	}
#endif
	return member;
}

static struct type *next_field(struct compile_state *state,
	struct type *type, struct type *prev_member)
{
	struct type *member;
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		internal_error(state, 0, "next_field only works on structures");
	}
	member = type->left;
	while((member->type & TYPE_MASK) == TYPE_PRODUCT) {
		if (!prev_member) {
			member = member->left;
			break;
		}
		if (member->left == prev_member) {
			prev_member = 0;
		}
		member = member->right;
	}
	if (member == prev_member) {
		prev_member = 0;
	}
	if (prev_member) {
		internal_error(state, 0, "prev_member %s not present",
			prev_member->field_ident->name);
	}
	return member;
}

typedef void (*walk_type_fields_cb_t)(struct compile_state *state, struct type *type,
	size_t ret_offset, size_t mem_offset, void *arg);

static void walk_type_fields(struct compile_state *state,
	struct type *type, size_t reg_offset, size_t mem_offset,
	walk_type_fields_cb_t cb, void *arg);

static void walk_struct_fields(struct compile_state *state,
	struct type *type, size_t reg_offset, size_t mem_offset,
	walk_type_fields_cb_t cb, void *arg)
{
	struct type *tptr;
	ulong_t i;
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		internal_error(state, 0, "walk_struct_fields only works on structures");
	}
	tptr = type->left;
	for(i = 0; i < type->elements; i++) {
		struct type *mtype;
		mtype = tptr;
		if ((mtype->type & TYPE_MASK) == TYPE_PRODUCT) {
			mtype = mtype->left;
		}
		walk_type_fields(state, mtype,
			reg_offset +
			field_reg_offset(state, type, mtype->field_ident),
			mem_offset +
			field_offset(state, type, mtype->field_ident),
			cb, arg);
		tptr = tptr->right;
	}

}

static void walk_type_fields(struct compile_state *state,
	struct type *type, size_t reg_offset, size_t mem_offset,
	walk_type_fields_cb_t cb, void *arg)
{
	switch(type->type & TYPE_MASK) {
	case TYPE_STRUCT:
		walk_struct_fields(state, type, reg_offset, mem_offset, cb, arg);
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
	case TYPE_SHORT:
	case TYPE_USHORT:
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_LONG:
	case TYPE_ULONG:
		cb(state, type, reg_offset, mem_offset, arg);
		break;
	case TYPE_VOID:
		break;
	default:
		internal_error(state, 0, "walk_type_fields not yet implemented for type");
	}
}

static void arrays_complete(struct compile_state *state, struct type *type)
{
	if ((type->type & TYPE_MASK) == TYPE_ARRAY) {
		if (type->elements == ELEMENT_COUNT_UNSPECIFIED) {
			error(state, 0, "array size not specified");
		}
		arrays_complete(state, type->left);
	}
}

static unsigned int get_basic_type(struct type *type)
{
	unsigned int basic;
	basic = type->type & TYPE_MASK;
	/* Convert enums to ints */
	if (basic == TYPE_ENUM) {
		basic = TYPE_INT;
	}
	/* Convert bitfields to standard types */
	else if (basic == TYPE_BITFIELD) {
		if (type->elements <= SIZEOF_CHAR) {
			basic = TYPE_CHAR;
		}
		else if (type->elements <= SIZEOF_SHORT) {
			basic = TYPE_SHORT;
		}
		else if (type->elements <= SIZEOF_INT) {
			basic = TYPE_INT;
		}
		else if (type->elements <= SIZEOF_LONG) {
			basic = TYPE_LONG;
		}
		if (!TYPE_SIGNED(type->left->type)) {
			basic += 1;
		}
	}
	return basic;
}

static unsigned int do_integral_promotion(unsigned int type)
{
	if (TYPE_INTEGER(type) && (TYPE_RANK(type) < TYPE_RANK(TYPE_INT))) {
		type = TYPE_INT;
	}
	return type;
}

static unsigned int do_arithmetic_conversion(
	unsigned int left, unsigned int right)
{
	if ((left == TYPE_LDOUBLE) || (right == TYPE_LDOUBLE)) {
		return TYPE_LDOUBLE;
	}
	else if ((left == TYPE_DOUBLE) || (right == TYPE_DOUBLE)) {
		return TYPE_DOUBLE;
	}
	else if ((left == TYPE_FLOAT) || (right == TYPE_FLOAT)) {
		return TYPE_FLOAT;
	}
	left = do_integral_promotion(left);
	right = do_integral_promotion(right);
	/* If both operands have the same size done */
	if (left == right) {
		return left;
	}
	/* If both operands have the same signedness pick the larger */
	else if (!!TYPE_UNSIGNED(left) == !!TYPE_UNSIGNED(right)) {
		return (TYPE_RANK(left) >= TYPE_RANK(right)) ? left : right;
	}
	/* If the signed type can hold everything use it */
	else if (TYPE_SIGNED(left) && (TYPE_RANK(left) > TYPE_RANK(right))) {
		return left;
	}
	else if (TYPE_SIGNED(right) && (TYPE_RANK(right) > TYPE_RANK(left))) {
		return right;
	}
	/* Convert to the unsigned type with the same rank as the signed type */
	else if (TYPE_SIGNED(left)) {
		return TYPE_MKUNSIGNED(left);
	}
	else {
		return TYPE_MKUNSIGNED(right);
	}
}

/* see if two types are the same except for qualifiers */
static int equiv_types(struct type *left, struct type *right)
{
	unsigned int type;
	/* Error if the basic types do not match */
	if ((left->type & TYPE_MASK) != (right->type & TYPE_MASK)) {
		return 0;
	}
	type = left->type & TYPE_MASK;
	/* If the basic types match and it is a void type we are done */
	if (type == TYPE_VOID) {
		return 1;
	}
	/* For bitfields we need to compare the sizes */
	else if (type == TYPE_BITFIELD) {
		return (left->elements == right->elements) &&
			(TYPE_SIGNED(left->left->type) == TYPE_SIGNED(right->left->type));
	}
	/* if the basic types match and it is an arithmetic type we are done */
	else if (TYPE_ARITHMETIC(type)) {
		return 1;
	}
	/* If it is a pointer type recurse and keep testing */
	else if (type == TYPE_POINTER) {
		return equiv_types(left->left, right->left);
	}
	else if (type == TYPE_ARRAY) {
		return (left->elements == right->elements) &&
			equiv_types(left->left, right->left);
	}
	/* test for struct equality */
	else if (type == TYPE_STRUCT) {
		return left->type_ident == right->type_ident;
	}
	/* test for union equality */
	else if (type == TYPE_UNION) {
		return left->type_ident == right->type_ident;
	}
	/* Test for equivalent functions */
	else if (type == TYPE_FUNCTION) {
		return equiv_types(left->left, right->left) &&
			equiv_types(left->right, right->right);
	}
	/* We only see TYPE_PRODUCT as part of function equivalence matching */
	/* We also see TYPE_PRODUCT as part of of tuple equivalence matchin */
	else if (type == TYPE_PRODUCT) {
		return equiv_types(left->left, right->left) &&
			equiv_types(left->right, right->right);
	}
	/* We should see TYPE_OVERLAP when comparing joins */
	else if (type == TYPE_OVERLAP) {
		return equiv_types(left->left, right->left) &&
			equiv_types(left->right, right->right);
	}
	/* Test for equivalence of tuples */
	else if (type == TYPE_TUPLE) {
		return (left->elements == right->elements) &&
			equiv_types(left->left, right->left);
	}
	/* Test for equivalence of joins */
	else if (type == TYPE_JOIN) {
		return (left->elements == right->elements) &&
			equiv_types(left->left, right->left);
	}
	else {
		return 0;
	}
}

static int equiv_ptrs(struct type *left, struct type *right)
{
	if (((left->type & TYPE_MASK) != TYPE_POINTER) ||
		((right->type & TYPE_MASK) != TYPE_POINTER)) {
		return 0;
	}
	return equiv_types(left->left, right->left);
}

static struct type *compatible_types(struct type *left, struct type *right)
{
	struct type *result;
	unsigned int type, qual_type;
	/* Error if the basic types do not match */
	if ((left->type & TYPE_MASK) != (right->type & TYPE_MASK)) {
		return 0;
	}
	type = left->type & TYPE_MASK;
	qual_type = (left->type & ~STOR_MASK) | (right->type & ~STOR_MASK);
	result = 0;
	/* if the basic types match and it is an arithmetic type we are done */
	if (TYPE_ARITHMETIC(type)) {
		result = new_type(qual_type, 0, 0);
	}
	/* If it is a pointer type recurse and keep testing */
	else if (type == TYPE_POINTER) {
		result = compatible_types(left->left, right->left);
		if (result) {
			result = new_type(qual_type, result, 0);
		}
	}
	/* test for struct equality */
	else if (type == TYPE_STRUCT) {
		if (left->type_ident == right->type_ident) {
			result = left;
		}
	}
	/* test for union equality */
	else if (type == TYPE_UNION) {
		if (left->type_ident == right->type_ident) {
			result = left;
		}
	}
	/* Test for equivalent functions */
	else if (type == TYPE_FUNCTION) {
		struct type *lf, *rf;
		lf = compatible_types(left->left, right->left);
		rf = compatible_types(left->right, right->right);
		if (lf && rf) {
			result = new_type(qual_type, lf, rf);
		}
	}
	/* We only see TYPE_PRODUCT as part of function equivalence matching */
	else if (type == TYPE_PRODUCT) {
		struct type *lf, *rf;
		lf = compatible_types(left->left, right->left);
		rf = compatible_types(left->right, right->right);
		if (lf && rf) {
			result = new_type(qual_type, lf, rf);
		}
	}
	else {
		/* Nothing else is compatible */
	}
	return result;
}

/* See if left is a equivalent to right or right is a union member of left */
static int is_subset_type(struct type *left, struct type *right)
{
	if (equiv_types(left, right)) {
		return 1;
	}
	if ((left->type & TYPE_MASK) == TYPE_JOIN) {
		struct type *member, *mnext;
		mnext = left->left;
		while(mnext) {
			member = mnext;
			mnext = 0;
			if ((member->type & TYPE_MASK) == TYPE_OVERLAP) {
				mnext = member->right;
				member = member->left;
			}
			if (is_subset_type( member, right)) {
				return 1;
			}
		}
	}
	return 0;
}

static struct type *compatible_ptrs(struct type *left, struct type *right)
{
	struct type *result;
	if (((left->type & TYPE_MASK) != TYPE_POINTER) ||
		((right->type & TYPE_MASK) != TYPE_POINTER)) {
		return 0;
	}
	result = compatible_types(left->left, right->left);
	if (result) {
		unsigned int qual_type;
		qual_type = (left->type & ~STOR_MASK) | (right->type & ~STOR_MASK);
		result = new_type(qual_type, result, 0);
	}
	return result;

}
static struct triple *integral_promotion(
	struct compile_state *state, struct triple *def)
{
	struct type *type;
	type = def->type;
	/* As all operations are carried out in registers
	 * the values are converted on load I just convert
	 * logical type of the operand.
	 */
	if (TYPE_INTEGER(type->type)) {
		unsigned int int_type;
		int_type = type->type & ~TYPE_MASK;
		int_type |= do_integral_promotion(get_basic_type(type));
		if (int_type != type->type) {
			if (def->op != OP_LOAD) {
				def->type = new_type(int_type, 0, 0);
			}
			else {
				def = triple(state, OP_CONVERT,
					new_type(int_type, 0, 0), def, 0);
			}
		}
	}
	return def;
}


static void arithmetic(struct compile_state *state, struct triple *def)
{
	if (!TYPE_ARITHMETIC(def->type->type)) {
		error(state, 0, "arithmetic type expexted");
	}
}

static void ptr_arithmetic(struct compile_state *state, struct triple *def)
{
	if (!TYPE_PTR(def->type->type) && !TYPE_ARITHMETIC(def->type->type)) {
		error(state, def, "pointer or arithmetic type expected");
	}
}

static int is_integral(struct triple *ins)
{
	return TYPE_INTEGER(ins->type->type);
}

static void integral(struct compile_state *state, struct triple *def)
{
	if (!is_integral(def)) {
		error(state, 0, "integral type expected");
	}
}


static void bool(struct compile_state *state, struct triple *def)
{
	if (!TYPE_ARITHMETIC(def->type->type) &&
		((def->type->type & TYPE_MASK) != TYPE_POINTER)) {
		error(state, 0, "arithmetic or pointer type expected");
	}
}

static int is_signed(struct type *type)
{
	if ((type->type & TYPE_MASK) == TYPE_BITFIELD) {
		type = type->left;
	}
	return !!TYPE_SIGNED(type->type);
}
static int is_compound_type(struct type *type)
{
	int is_compound;
	switch((type->type & TYPE_MASK)) {
	case TYPE_ARRAY:
	case TYPE_STRUCT:
	case TYPE_TUPLE:
	case TYPE_UNION:
	case TYPE_JOIN:
		is_compound = 1;
		break;
	default:
		is_compound = 0;
		break;
	}
	return is_compound;
}

/* Is this value located in a register otherwise it must be in memory */
static int is_in_reg(struct compile_state *state, struct triple *def)
{
	int in_reg;
	if (def->op == OP_ADECL) {
		in_reg = 1;
	}
	else if ((def->op == OP_SDECL) || (def->op == OP_DEREF)) {
		in_reg = 0;
	}
	else if (triple_is_part(state, def)) {
		in_reg = is_in_reg(state, MISC(def, 0));
	}
	else {
		internal_error(state, def, "unknown expr storage location");
		in_reg = -1;
	}
	return in_reg;
}

/* Is this an auto or static variable location? Something that can
 * be assigned to.  Otherwise it must must be a pure value, a temporary.
 */
static int is_lvalue(struct compile_state *state, struct triple *def)
{
	int ret;
	ret = 0;
	if (!def) {
		return 0;
	}
	if ((def->op == OP_ADECL) ||
		(def->op == OP_SDECL) ||
		(def->op == OP_DEREF) ||
		(def->op == OP_BLOBCONST) ||
		(def->op == OP_LIST)) {
		ret = 1;
	}
	else if (triple_is_part(state, def)) {
		ret = is_lvalue(state, MISC(def, 0));
	}
	return ret;
}

static void clvalue(struct compile_state *state, struct triple *def)
{
	if (!def) {
		internal_error(state, def, "nothing where lvalue expected?");
	}
	if (!is_lvalue(state, def)) {
		error(state, def, "lvalue expected");
	}
}
static void lvalue(struct compile_state *state, struct triple *def)
{
	clvalue(state, def);
	if (def->type->type & QUAL_CONST) {
		error(state, def, "modifable lvalue expected");
	}
}

static int is_pointer(struct triple *def)
{
	return (def->type->type & TYPE_MASK) == TYPE_POINTER;
}

static void pointer(struct compile_state *state, struct triple *def)
{
	if (!is_pointer(def)) {
		error(state, def, "pointer expected");
	}
}

static struct triple *int_const(
	struct compile_state *state, struct type *type, ulong_t value)
{
	struct triple *result;
	switch(type->type & TYPE_MASK) {
	case TYPE_CHAR:
	case TYPE_INT:   case TYPE_UINT:
	case TYPE_LONG:  case TYPE_ULONG:
		break;
	default:
		internal_error(state, 0, "constant for unknown type");
	}
	result = triple(state, OP_INTCONST, type, 0, 0);
	result->u.cval = value;
	return result;
}


static struct triple *read_expr(struct compile_state *state, struct triple *def);

static struct triple *do_mk_addr_expr(struct compile_state *state,
	struct triple *expr, struct type *type, ulong_t offset)
{
	struct triple *result;
	clvalue(state, expr);


	result = 0;
	if (expr->op == OP_ADECL) {
		error(state, expr, "address of auto variables not supported");
	}
	else if (expr->op == OP_SDECL) {
		struct type *ptr_type;
		ptr_type = new_type(TYPE_POINTER | (type->type & QUAL_MASK), type, 0);

		result = triple(state, OP_ADDRCONST, ptr_type, 0, 0);
		MISC(result, 0) = expr;
		result->u.cval = offset;
	}
	else if (expr->op == OP_DEREF) {
		struct type *ptr_type;
		ptr_type = new_type(TYPE_POINTER | (type->type & QUAL_MASK), type, 0);

		result = triple(state, OP_ADD, ptr_type,
			RHS(expr, 0),
			int_const(state, &ulong_type, offset));
	}
	else if (expr->op == OP_BLOBCONST) {
		FINISHME();
		internal_error(state, expr, "not yet implemented");
	}
	else if (expr->op == OP_LIST) {
		error(state, 0, "Function addresses not supported");
	}
	else if (triple_is_part(state, expr)) {
		struct triple *part;
		part = expr;
		expr = MISC(expr, 0);
		if (part->op == OP_DOT) {
			offset += bits_to_bytes(
				field_offset(state, expr->type, part->u.field));
		}
		else if (part->op == OP_INDEX) {
			offset += bits_to_bytes(
				index_offset(state, expr->type, part->u.cval));
		}
		else {
			internal_error(state, part, "unhandled part type");
		}
		result = do_mk_addr_expr(state, expr, type, offset);
	}
	if (!result) {
		internal_error(state, expr, "cannot take address of expression");
	}
	return result;
}

static struct triple *mk_addr_expr(
	struct compile_state *state, struct triple *expr, ulong_t offset)
{
	return do_mk_addr_expr(state, expr, expr->type, offset);
}

static struct triple *mk_deref_expr(
	struct compile_state *state, struct triple *expr)
{
	struct type *base_type;
	pointer(state, expr);
	base_type = expr->type->left;
	return triple(state, OP_DEREF, base_type, expr, 0);
}

/* lvalue conversions always apply except when certain operators
 * are applied.  So I apply apply it when I know no more
 * operators will be applied.
 */
static struct triple *lvalue_conversion(struct compile_state *state, struct triple *def)
{
	/* Tranform an array to a pointer to the first element */
	if ((def->type->type & TYPE_MASK) == TYPE_ARRAY) {
		struct type *type;
		type = new_type(
			TYPE_POINTER | (def->type->type & QUAL_MASK),
			def->type->left, 0);
		if ((def->op == OP_SDECL) || IS_CONST_OP(def->op)) {
			struct triple *addrconst;
			if ((def->op != OP_SDECL) && (def->op != OP_BLOBCONST)) {
				internal_error(state, def, "bad array constant");
			}
			addrconst = triple(state, OP_ADDRCONST, type, 0, 0);
			MISC(addrconst, 0) = def;
			def = addrconst;
		}
		else {
			def = triple(state, OP_CONVERT, type, def, 0);
		}
	}
	/* Transform a function to a pointer to it */
	else if ((def->type->type & TYPE_MASK) == TYPE_FUNCTION) {
		def = mk_addr_expr(state, def, 0);
	}
	return def;
}

static struct triple *deref_field(
	struct compile_state *state, struct triple *expr, struct hash_entry *field)
{
	struct triple *result;
	struct type *type, *member;
	ulong_t offset;
	if (!field) {
		internal_error(state, 0, "No field passed to deref_field");
	}
	result = 0;
	type = expr->type;
	if (((type->type & TYPE_MASK) != TYPE_STRUCT) &&
		((type->type & TYPE_MASK) != TYPE_UNION)) {
		error(state, 0, "request for member %s in something not a struct or union",
			field->name);
	}
	member = field_type(state, type, field);
	if ((type->type & STOR_MASK) == STOR_PERM) {
		/* Do the pointer arithmetic to get a deref the field */
		offset = bits_to_bytes(field_offset(state, type, field));
		result = do_mk_addr_expr(state, expr, member, offset);
		result = mk_deref_expr(state, result);
	}
	else {
		/* Find the variable for the field I want. */
		result = triple(state, OP_DOT, member, expr, 0);
		result->u.field = field;
	}
	return result;
}

static struct triple *deref_index(
	struct compile_state *state, struct triple *expr, size_t index)
{
	struct triple *result;
	struct type *type, *member;
	ulong_t offset;

	result = 0;
	type = expr->type;
	member = index_type(state, type, index);

	if ((type->type & STOR_MASK) == STOR_PERM) {
		offset = bits_to_bytes(index_offset(state, type, index));
		result = do_mk_addr_expr(state, expr, member, offset);
		result = mk_deref_expr(state, result);
	}
	else {
		result = triple(state, OP_INDEX, member, expr, 0);
		result->u.cval = index;
	}
	return result;
}

static struct triple *read_expr(struct compile_state *state, struct triple *def)
{
	int op;
	if  (!def) {
		return 0;
	}
#if DEBUG_ROMCC_WARNINGS
#warning "CHECK_ME is this the only place I need to do lvalue conversions?"
#endif
	/* Transform lvalues into something we can read */
	def = lvalue_conversion(state, def);
	if (!is_lvalue(state, def)) {
		return def;
	}
	if (is_in_reg(state, def)) {
		op = OP_READ;
	} else {
		if (def->op == OP_SDECL) {
			def = mk_addr_expr(state, def, 0);
			def = mk_deref_expr(state, def);
		}
		op = OP_LOAD;
	}
	def = triple(state, op, def->type, def, 0);
	if (def->type->type & QUAL_VOLATILE) {
		def->id |= TRIPLE_FLAG_VOLATILE;
	}
	return def;
}

int is_write_compatible(struct compile_state *state,
	struct type *dest, struct type *rval)
{
	int compatible = 0;
	/* Both operands have arithmetic type */
	if (TYPE_ARITHMETIC(dest->type) && TYPE_ARITHMETIC(rval->type)) {
		compatible = 1;
	}
	/* One operand is a pointer and the other is a pointer to void */
	else if (((dest->type & TYPE_MASK) == TYPE_POINTER) &&
		((rval->type & TYPE_MASK) == TYPE_POINTER) &&
		(((dest->left->type & TYPE_MASK) == TYPE_VOID) ||
			((rval->left->type & TYPE_MASK) == TYPE_VOID))) {
		compatible = 1;
	}
	/* If both types are the same without qualifiers we are good */
	else if (equiv_ptrs(dest, rval)) {
		compatible = 1;
	}
	/* test for struct/union equality  */
	else if (equiv_types(dest, rval)) {
		compatible = 1;
	}
	return compatible;
}

static void write_compatible(struct compile_state *state,
	struct type *dest, struct type *rval)
{
	if (!is_write_compatible(state, dest, rval)) {
		FILE *fp = state->errout;
		fprintf(fp, "dest: ");
		name_of(fp, dest);
		fprintf(fp,"\nrval: ");
		name_of(fp, rval);
		fprintf(fp, "\n");
		error(state, 0, "Incompatible types in assignment");
	}
}

static int is_init_compatible(struct compile_state *state,
	struct type *dest, struct type *rval)
{
	int compatible = 0;
	if (is_write_compatible(state, dest, rval)) {
		compatible = 1;
	}
	else if (equiv_types(dest, rval)) {
		compatible = 1;
	}
	return compatible;
}

static struct triple *write_expr(
	struct compile_state *state, struct triple *dest, struct triple *rval)
{
	struct triple *def;

	def = 0;
	if (!rval) {
		internal_error(state, 0, "missing rval");
	}

	if (rval->op == OP_LIST) {
		internal_error(state, 0, "expression of type OP_LIST?");
	}
	if (!is_lvalue(state, dest)) {
		internal_error(state, 0, "writing to a non lvalue?");
	}
	if (dest->type->type & QUAL_CONST) {
		internal_error(state, 0, "modifable lvalue expexted");
	}

	write_compatible(state, dest->type, rval->type);
	if (!equiv_types(dest->type, rval->type)) {
		rval = triple(state, OP_CONVERT, dest->type, rval, 0);
	}

	/* Now figure out which assignment operator to use */
	if (is_in_reg(state, dest)) {
		def = triple(state, OP_WRITE, dest->type, rval, dest);
		if (MISC(def, 0) != dest) {
			internal_error(state, def, "huh?");
		}
		if (RHS(def, 0) != rval) {
			internal_error(state, def, "huh?");
		}
	} else {
		def = triple(state, OP_STORE, dest->type, dest, rval);
	}
	if (def->type->type & QUAL_VOLATILE) {
		def->id |= TRIPLE_FLAG_VOLATILE;
	}
	return def;
}

static struct triple *init_expr(
	struct compile_state *state, struct triple *dest, struct triple *rval)
{
	struct triple *def;

	def = 0;
	if (!rval) {
		internal_error(state, 0, "missing rval");
	}
	if ((dest->type->type & STOR_MASK) != STOR_PERM) {
		rval = read_expr(state, rval);
		def = write_expr(state, dest, rval);
	}
	else {
		/* Fill in the array size if necessary */
		if (((dest->type->type & TYPE_MASK) == TYPE_ARRAY) &&
			((rval->type->type & TYPE_MASK) == TYPE_ARRAY)) {
			if (dest->type->elements == ELEMENT_COUNT_UNSPECIFIED) {
				dest->type->elements = rval->type->elements;
			}
		}
		if (!equiv_types(dest->type, rval->type)) {
			error(state, 0, "Incompatible types in inializer");
		}
		MISC(dest, 0) = rval;
		insert_triple(state, dest, rval);
		rval->id |= TRIPLE_FLAG_FLATTENED;
		use_triple(MISC(dest, 0), dest);
	}
	return def;
}

struct type *arithmetic_result(
	struct compile_state *state, struct triple *left, struct triple *right)
{
	struct type *type;
	/* Sanity checks to ensure I am working with arithmetic types */
	arithmetic(state, left);
	arithmetic(state, right);
	type = new_type(
		do_arithmetic_conversion(
			get_basic_type(left->type),
			get_basic_type(right->type)),
		0, 0);
	return type;
}

struct type *ptr_arithmetic_result(
	struct compile_state *state, struct triple *left, struct triple *right)
{
	struct type *type;
	/* Sanity checks to ensure I am working with the proper types */
	ptr_arithmetic(state, left);
	arithmetic(state, right);
	if (TYPE_ARITHMETIC(left->type->type) &&
		TYPE_ARITHMETIC(right->type->type)) {
		type = arithmetic_result(state, left, right);
	}
	else if (TYPE_PTR(left->type->type)) {
		type = left->type;
	}
	else {
		internal_error(state, 0, "huh?");
		type = 0;
	}
	return type;
}

/* boolean helper function */

static struct triple *ltrue_expr(struct compile_state *state,
	struct triple *expr)
{
	switch(expr->op) {
	case OP_LTRUE:   case OP_LFALSE:  case OP_EQ:      case OP_NOTEQ:
	case OP_SLESS:   case OP_ULESS:   case OP_SMORE:   case OP_UMORE:
	case OP_SLESSEQ: case OP_ULESSEQ: case OP_SMOREEQ: case OP_UMOREEQ:
		/* If the expression is already boolean do nothing */
		break;
	default:
		expr = triple(state, OP_LTRUE, &int_type, expr, 0);
		break;
	}
	return expr;
}

static struct triple *lfalse_expr(struct compile_state *state,
	struct triple *expr)
{
	return triple(state, OP_LFALSE, &int_type, expr, 0);
}

static struct triple *mkland_expr(
	struct compile_state *state,
	struct triple *left, struct triple *right)
{
	struct triple *def, *val, *var, *jmp, *mid, *end;
	struct triple *lstore, *rstore;

	/* Generate some intermediate triples */
	end = label(state);
	var = variable(state, &int_type);

	/* Store the left hand side value */
	lstore = write_expr(state, var, left);

	/* Jump if the value is false */
	jmp =  branch(state, end,
		lfalse_expr(state, read_expr(state, var)));
	mid = label(state);

	/* Store the right hand side value */
	rstore = write_expr(state, var, right);

	/* An expression for the computed value */
	val = read_expr(state, var);

	/* Generate the prog for a logical and */
	def = mkprog(state, var, lstore, jmp, mid, rstore, end, val, 0UL);

	return def;
}

static struct triple *mklor_expr(
	struct compile_state *state,
	struct triple *left, struct triple *right)
{
	struct triple *def, *val, *var, *jmp, *mid, *end;

	/* Generate some intermediate triples */
	end = label(state);
	var = variable(state, &int_type);

	/* Store the left hand side value */
	left = write_expr(state, var, left);

	/* Jump if the value is true */
	jmp = branch(state, end, read_expr(state, var));
	mid = label(state);

	/* Store the right hand side value */
	right = write_expr(state, var, right);

	/* An expression for the computed value*/
	val = read_expr(state, var);

	/* Generate the prog for a logical or */
	def = mkprog(state, var, left, jmp, mid, right, end, val, 0UL);

	return def;
}

static struct triple *mkcond_expr(
	struct compile_state *state,
	struct triple *test, struct triple *left, struct triple *right)
{
	struct triple *def, *val, *var, *jmp1, *jmp2, *top, *mid, *end;
	struct type *result_type;
	unsigned int left_type, right_type;
	bool(state, test);
	left_type = left->type->type;
	right_type = right->type->type;
	result_type = 0;
	/* Both operands have arithmetic type */
	if (TYPE_ARITHMETIC(left_type) && TYPE_ARITHMETIC(right_type)) {
		result_type = arithmetic_result(state, left, right);
	}
	/* Both operands have void type */
	else if (((left_type & TYPE_MASK) == TYPE_VOID) &&
		((right_type & TYPE_MASK) == TYPE_VOID)) {
		result_type = &void_type;
	}
	/* pointers to the same type... */
	else if ((result_type = compatible_ptrs(left->type, right->type))) {
		;
	}
	/* Both operands are pointers and left is a pointer to void */
	else if (((left_type & TYPE_MASK) == TYPE_POINTER) &&
		((right_type & TYPE_MASK) == TYPE_POINTER) &&
		((left->type->left->type & TYPE_MASK) == TYPE_VOID)) {
		result_type = right->type;
	}
	/* Both operands are pointers and right is a pointer to void */
	else if (((left_type & TYPE_MASK) == TYPE_POINTER) &&
		((right_type & TYPE_MASK) == TYPE_POINTER) &&
		((right->type->left->type & TYPE_MASK) == TYPE_VOID)) {
		result_type = left->type;
	}
	if (!result_type) {
		error(state, 0, "Incompatible types in conditional expression");
	}
	/* Generate some intermediate triples */
	mid = label(state);
	end = label(state);
	var = variable(state, result_type);

	/* Branch if the test is false */
	jmp1 = branch(state, mid, lfalse_expr(state, read_expr(state, test)));
	top = label(state);

	/* Store the left hand side value */
	left = write_expr(state, var, left);

	/* Branch to the end */
	jmp2 = branch(state, end, 0);

	/* Store the right hand side value */
	right = write_expr(state, var, right);

	/* An expression for the computed value */
	val = read_expr(state, var);

	/* Generate the prog for a conditional expression */
	def = mkprog(state, var, jmp1, top, left, jmp2, mid, right, end, val, 0UL);

	return def;
}


static int expr_depth(struct compile_state *state, struct triple *ins)
{
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME move optimal ordering of subexpressions into the optimizer"
#endif
	int count;
	count = 0;
	if (!ins || (ins->id & TRIPLE_FLAG_FLATTENED)) {
		count = 0;
	}
	else if (ins->op == OP_DEREF) {
		count = expr_depth(state, RHS(ins, 0)) - 1;
	}
	else if (ins->op == OP_VAL) {
		count = expr_depth(state, RHS(ins, 0)) - 1;
	}
	else if (ins->op == OP_FCALL) {
		/* Don't figure the depth of a call just guess it is huge */
		count = 1000;
	}
	else {
		struct triple **expr;
		expr = triple_rhs(state, ins, 0);
		for(;expr; expr = triple_rhs(state, ins, expr)) {
			if (*expr) {
				int depth;
				depth = expr_depth(state, *expr);
				if (depth > count) {
					count = depth;
				}
			}
		}
	}
	return count + 1;
}

static struct triple *flatten_generic(
	struct compile_state *state, struct triple *first, struct triple *ptr,
	int ignored)
{
	struct rhs_vector {
		int depth;
		struct triple **ins;
	} vector[MAX_RHS];
	int i, rhs, lhs;
	/* Only operations with just a rhs and a lhs should come here */
	rhs = ptr->rhs;
	lhs = ptr->lhs;
	if (TRIPLE_SIZE(ptr) != lhs + rhs + ignored) {
		internal_error(state, ptr, "unexpected args for: %d %s",
			ptr->op, tops(ptr->op));
	}
	/* Find the depth of the rhs elements */
	for(i = 0; i < rhs; i++) {
		vector[i].ins = &RHS(ptr, i);
		vector[i].depth = expr_depth(state, *vector[i].ins);
	}
	/* Selection sort the rhs */
	for(i = 0; i < rhs; i++) {
		int j, max = i;
		for(j = i + 1; j < rhs; j++ ) {
			if (vector[j].depth > vector[max].depth) {
				max = j;
			}
		}
		if (max != i) {
			struct rhs_vector tmp;
			tmp = vector[i];
			vector[i] = vector[max];
			vector[max] = tmp;
		}
	}
	/* Now flatten the rhs elements */
	for(i = 0; i < rhs; i++) {
		*vector[i].ins = flatten(state, first, *vector[i].ins);
		use_triple(*vector[i].ins, ptr);
	}
	if (lhs) {
		insert_triple(state, first, ptr);
		ptr->id |= TRIPLE_FLAG_FLATTENED;
		ptr->id &= ~TRIPLE_FLAG_LOCAL;

		/* Now flatten the lhs elements */
		for(i = 0; i < lhs; i++) {
			struct triple **ins = &LHS(ptr, i);
			*ins = flatten(state, first, *ins);
			use_triple(*ins, ptr);
		}
	}
	return ptr;
}

static struct triple *flatten_prog(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	struct triple *head, *body, *val;
	head = RHS(ptr, 0);
	RHS(ptr, 0) = 0;
	val  = head->prev;
	body = head->next;
	release_triple(state, head);
	release_triple(state, ptr);
	val->next        = first;
	body->prev       = first->prev;
	body->prev->next = body;
	val->next->prev  = val;

	if (triple_is_cbranch(state, body->prev) ||
		triple_is_call(state, body->prev)) {
		unuse_triple(first, body->prev);
		use_triple(body, body->prev);
	}

	if (!(val->id & TRIPLE_FLAG_FLATTENED)) {
		internal_error(state, val, "val not flattened?");
	}

	return val;
}


static struct triple *flatten_part(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	if (!triple_is_part(state, ptr)) {
		internal_error(state, ptr,  "not a part");
	}
	if (ptr->rhs || ptr->lhs || ptr->targ || (ptr->misc != 1)) {
		internal_error(state, ptr, "unexpected args for: %d %s",
			ptr->op, tops(ptr->op));
	}
	MISC(ptr, 0) = flatten(state, first, MISC(ptr, 0));
	use_triple(MISC(ptr, 0), ptr);
	return flatten_generic(state, first, ptr, 1);
}

static struct triple *flatten(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	struct triple *orig_ptr;
	if (!ptr)
		return 0;
	do {
		orig_ptr = ptr;
		/* Only flatten triples once */
		if (ptr->id & TRIPLE_FLAG_FLATTENED) {
			return ptr;
		}
		switch(ptr->op) {
		case OP_VAL:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			return MISC(ptr, 0);
			break;
		case OP_PROG:
			ptr = flatten_prog(state, first, ptr);
			break;
		case OP_FCALL:
			ptr = flatten_generic(state, first, ptr, 1);
			insert_triple(state, first, ptr);
			ptr->id |= TRIPLE_FLAG_FLATTENED;
			ptr->id &= ~TRIPLE_FLAG_LOCAL;
			if (ptr->next != ptr) {
				use_triple(ptr->next, ptr);
			}
			break;
		case OP_READ:
		case OP_LOAD:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			use_triple(RHS(ptr, 0), ptr);
			break;
		case OP_WRITE:
			ptr = flatten_generic(state, first, ptr, 1);
			MISC(ptr, 0) = flatten(state, first, MISC(ptr, 0));
			use_triple(MISC(ptr, 0), ptr);
			break;
		case OP_BRANCH:
			use_triple(TARG(ptr, 0), ptr);
			break;
		case OP_CBRANCH:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			use_triple(RHS(ptr, 0), ptr);
			use_triple(TARG(ptr, 0), ptr);
			insert_triple(state, first, ptr);
			ptr->id |= TRIPLE_FLAG_FLATTENED;
			ptr->id &= ~TRIPLE_FLAG_LOCAL;
			if (ptr->next != ptr) {
				use_triple(ptr->next, ptr);
			}
			break;
		case OP_CALL:
			MISC(ptr, 0) = flatten(state, first, MISC(ptr, 0));
			use_triple(MISC(ptr, 0), ptr);
			use_triple(TARG(ptr, 0), ptr);
			insert_triple(state, first, ptr);
			ptr->id |= TRIPLE_FLAG_FLATTENED;
			ptr->id &= ~TRIPLE_FLAG_LOCAL;
			if (ptr->next != ptr) {
				use_triple(ptr->next, ptr);
			}
			break;
		case OP_RET:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			use_triple(RHS(ptr, 0), ptr);
			break;
		case OP_BLOBCONST:
			insert_triple(state, state->global_pool, ptr);
			ptr->id |= TRIPLE_FLAG_FLATTENED;
			ptr->id &= ~TRIPLE_FLAG_LOCAL;
			ptr = triple(state, OP_SDECL, ptr->type, ptr, 0);
			use_triple(MISC(ptr, 0), ptr);
			break;
		case OP_DEREF:
			/* Since OP_DEREF is just a marker delete it when I flatten it */
			ptr = RHS(ptr, 0);
			RHS(orig_ptr, 0) = 0;
			free_triple(state, orig_ptr);
			break;
		case OP_DOT:
			if (RHS(ptr, 0)->op == OP_DEREF) {
				struct triple *base, *left;
				ulong_t offset;
				base = MISC(ptr, 0);
				offset = bits_to_bytes(field_offset(state, base->type, ptr->u.field));
				left = RHS(base, 0);
				ptr = triple(state, OP_ADD, left->type,
					read_expr(state, left),
					int_const(state, &ulong_type, offset));
				free_triple(state, base);
			}
			else {
				ptr = flatten_part(state, first, ptr);
			}
			break;
		case OP_INDEX:
			if (RHS(ptr, 0)->op == OP_DEREF) {
				struct triple *base, *left;
				ulong_t offset;
				base = MISC(ptr, 0);
				offset = bits_to_bytes(index_offset(state, base->type, ptr->u.cval));
				left = RHS(base, 0);
				ptr = triple(state, OP_ADD, left->type,
					read_expr(state, left),
					int_const(state, &long_type, offset));
				free_triple(state, base);
			}
			else {
				ptr = flatten_part(state, first, ptr);
			}
			break;
		case OP_PIECE:
			ptr = flatten_part(state, first, ptr);
			use_triple(ptr, MISC(ptr, 0));
			break;
		case OP_ADDRCONST:
			MISC(ptr, 0) = flatten(state, first, MISC(ptr, 0));
			use_triple(MISC(ptr, 0), ptr);
			break;
		case OP_SDECL:
			first = state->global_pool;
			MISC(ptr, 0) = flatten(state, first, MISC(ptr, 0));
			use_triple(MISC(ptr, 0), ptr);
			insert_triple(state, first, ptr);
			ptr->id |= TRIPLE_FLAG_FLATTENED;
			ptr->id &= ~TRIPLE_FLAG_LOCAL;
			return ptr;
		case OP_ADECL:
			ptr = flatten_generic(state, first, ptr, 0);
			break;
		default:
			/* Flatten the easy cases we don't override */
			ptr = flatten_generic(state, first, ptr, 0);
			break;
		}
	} while(ptr && (ptr != orig_ptr));
	if (ptr && !(ptr->id & TRIPLE_FLAG_FLATTENED)) {
		insert_triple(state, first, ptr);
		ptr->id |= TRIPLE_FLAG_FLATTENED;
		ptr->id &= ~TRIPLE_FLAG_LOCAL;
	}
	return ptr;
}

static void release_expr(struct compile_state *state, struct triple *expr)
{
	struct triple *head;
	head = label(state);
	flatten(state, head, expr);
	while(head->next != head) {
		release_triple(state, head->next);
	}
	free_triple(state, head);
}

static int replace_rhs_use(struct compile_state *state,
	struct triple *orig, struct triple *new, struct triple *use)
{
	struct triple **expr;
	int found;
	found = 0;
	expr = triple_rhs(state, use, 0);
	for(;expr; expr = triple_rhs(state, use, expr)) {
		if (*expr == orig) {
			*expr = new;
			found = 1;
		}
	}
	if (found) {
		unuse_triple(orig, use);
		use_triple(new, use);
	}
	return found;
}

static int replace_lhs_use(struct compile_state *state,
	struct triple *orig, struct triple *new, struct triple *use)
{
	struct triple **expr;
	int found;
	found = 0;
	expr = triple_lhs(state, use, 0);
	for(;expr; expr = triple_lhs(state, use, expr)) {
		if (*expr == orig) {
			*expr = new;
			found = 1;
		}
	}
	if (found) {
		unuse_triple(orig, use);
		use_triple(new, use);
	}
	return found;
}

static int replace_misc_use(struct compile_state *state,
	struct triple *orig, struct triple *new, struct triple *use)
{
	struct triple **expr;
	int found;
	found = 0;
	expr = triple_misc(state, use, 0);
	for(;expr; expr = triple_misc(state, use, expr)) {
		if (*expr == orig) {
			*expr = new;
			found = 1;
		}
	}
	if (found) {
		unuse_triple(orig, use);
		use_triple(new, use);
	}
	return found;
}

static int replace_targ_use(struct compile_state *state,
	struct triple *orig, struct triple *new, struct triple *use)
{
	struct triple **expr;
	int found;
	found = 0;
	expr = triple_targ(state, use, 0);
	for(;expr; expr = triple_targ(state, use, expr)) {
		if (*expr == orig) {
			*expr = new;
			found = 1;
		}
	}
	if (found) {
		unuse_triple(orig, use);
		use_triple(new, use);
	}
	return found;
}

static void replace_use(struct compile_state *state,
	struct triple *orig, struct triple *new, struct triple *use)
{
	int found;
	found = 0;
	found |= replace_rhs_use(state, orig, new, use);
	found |= replace_lhs_use(state, orig, new, use);
	found |= replace_misc_use(state, orig, new, use);
	found |= replace_targ_use(state, orig, new, use);
	if (!found) {
		internal_error(state, use, "use without use");
	}
}

static void propagate_use(struct compile_state *state,
	struct triple *orig, struct triple *new)
{
	struct triple_set *user, *next;
	for(user = orig->use; user; user = next) {
		/* Careful replace_use modifies the use chain and
		 * removes use.  So we must get a copy of the next
		 * entry early.
		 */
		next = user->next;
		replace_use(state, orig, new, user->member);
	}
	if (orig->use) {
		internal_error(state, orig, "used after propagate_use");
	}
}

/*
 * Code generators
 * ===========================
 */

static struct triple *mk_cast_expr(
	struct compile_state *state, struct type *type, struct triple *expr)
{
	struct triple *def;
	def = read_expr(state, expr);
	def = triple(state, OP_CONVERT, type, def, 0);
	return def;
}

static struct triple *mk_add_expr(
	struct compile_state *state, struct triple *left, struct triple *right)
{
	struct type *result_type;
	/* Put pointer operands on the left */
	if (is_pointer(right)) {
		struct triple *tmp;
		tmp = left;
		left = right;
		right = tmp;
	}
	left  = read_expr(state, left);
	right = read_expr(state, right);
	result_type = ptr_arithmetic_result(state, left, right);
	if (is_pointer(left)) {
		struct type *ptr_math;
		int op;
		if (is_signed(right->type)) {
			ptr_math = &long_type;
			op = OP_SMUL;
		} else {
			ptr_math = &ulong_type;
			op = OP_UMUL;
		}
		if (!equiv_types(right->type, ptr_math)) {
			right = mk_cast_expr(state, ptr_math, right);
		}
		right = triple(state, op, ptr_math, right,
			int_const(state, ptr_math,
				size_of_in_bytes(state, left->type->left)));
	}
	return triple(state, OP_ADD, result_type, left, right);
}

static struct triple *mk_sub_expr(
	struct compile_state *state, struct triple *left, struct triple *right)
{
	struct type *result_type;
	result_type = ptr_arithmetic_result(state, left, right);
	left  = read_expr(state, left);
	right = read_expr(state, right);
	if (is_pointer(left)) {
		struct type *ptr_math;
		int op;
		if (is_signed(right->type)) {
			ptr_math = &long_type;
			op = OP_SMUL;
		} else {
			ptr_math = &ulong_type;
			op = OP_UMUL;
		}
		if (!equiv_types(right->type, ptr_math)) {
			right = mk_cast_expr(state, ptr_math, right);
		}
		right = triple(state, op, ptr_math, right,
			int_const(state, ptr_math,
				size_of_in_bytes(state, left->type->left)));
	}
	return triple(state, OP_SUB, result_type, left, right);
}

static struct triple *mk_pre_inc_expr(
	struct compile_state *state, struct triple *def)
{
	struct triple *val;
	lvalue(state, def);
	val = mk_add_expr(state, def, int_const(state, &int_type, 1));
	return triple(state, OP_VAL, def->type,
		write_expr(state, def, val),
		val);
}

static struct triple *mk_pre_dec_expr(
	struct compile_state *state, struct triple *def)
{
	struct triple *val;
	lvalue(state, def);
	val = mk_sub_expr(state, def, int_const(state, &int_type, 1));
	return triple(state, OP_VAL, def->type,
		write_expr(state, def, val),
		val);
}

static struct triple *mk_post_inc_expr(
	struct compile_state *state, struct triple *def)
{
	struct triple *val;
	lvalue(state, def);
	val = read_expr(state, def);
	return triple(state, OP_VAL, def->type,
		write_expr(state, def,
			mk_add_expr(state, val, int_const(state, &int_type, 1)))
		, val);
}

static struct triple *mk_post_dec_expr(
	struct compile_state *state, struct triple *def)
{
	struct triple *val;
	lvalue(state, def);
	val = read_expr(state, def);
	return triple(state, OP_VAL, def->type,
		write_expr(state, def,
			mk_sub_expr(state, val, int_const(state, &int_type, 1)))
		, val);
}

static struct triple *mk_subscript_expr(
	struct compile_state *state, struct triple *left, struct triple *right)
{
	left  = read_expr(state, left);
	right = read_expr(state, right);
	if (!is_pointer(left) && !is_pointer(right)) {
		error(state, left, "subscripted value is not a pointer");
	}
	return mk_deref_expr(state, mk_add_expr(state, left, right));
}


/*
 * Compile time evaluation
 * ===========================
 */
static int is_const(struct triple *ins)
{
	return IS_CONST_OP(ins->op);
}

static int is_simple_const(struct triple *ins)
{
	/* Is this a constant that u.cval has the value.
	 * Or equivalently is this a constant that read_const
	 * works on.
	 * So far only OP_INTCONST qualifies.
	 */
	return (ins->op == OP_INTCONST);
}

static int constants_equal(struct compile_state *state,
	struct triple *left, struct triple *right)
{
	int equal;
	if ((left->op == OP_UNKNOWNVAL) || (right->op == OP_UNKNOWNVAL)) {
		equal = 0;
	}
	else if (!is_const(left) || !is_const(right)) {
		equal = 0;
	}
	else if (left->op != right->op) {
		equal = 0;
	}
	else if (!equiv_types(left->type, right->type)) {
		equal = 0;
	}
	else {
		equal = 0;
		switch(left->op) {
		case OP_INTCONST:
			if (left->u.cval == right->u.cval) {
				equal = 1;
			}
			break;
		case OP_BLOBCONST:
		{
			size_t lsize, rsize, bytes;
			lsize = size_of(state, left->type);
			rsize = size_of(state, right->type);
			if (lsize != rsize) {
				break;
			}
			bytes = bits_to_bytes(lsize);
			if (memcmp(left->u.blob, right->u.blob, bytes) == 0) {
				equal = 1;
			}
			break;
		}
		case OP_ADDRCONST:
			if ((MISC(left, 0) == MISC(right, 0)) &&
				(left->u.cval == right->u.cval)) {
				equal = 1;
			}
			break;
		default:
			internal_error(state, left, "uknown constant type");
			break;
		}
	}
	return equal;
}

static int is_zero(struct triple *ins)
{
	return is_simple_const(ins) && (ins->u.cval == 0);
}

static int is_one(struct triple *ins)
{
	return is_simple_const(ins) && (ins->u.cval == 1);
}

#if DEBUG_ROMCC_WARNING
static long_t bit_count(ulong_t value)
{
	int count;
	int i;
	count = 0;
	for(i = (sizeof(ulong_t)*8) -1; i >= 0; i--) {
		ulong_t mask;
		mask = 1;
		mask <<= i;
		if (value & mask) {
			count++;
		}
	}
	return count;

}
#endif

static long_t bsr(ulong_t value)
{
	int i;
	for(i = (sizeof(ulong_t)*8) -1; i >= 0; i--) {
		ulong_t mask;
		mask = 1;
		mask <<= i;
		if (value & mask) {
			return i;
		}
	}
	return -1;
}

static long_t bsf(ulong_t value)
{
	int i;
	for(i = 0; i < (sizeof(ulong_t)*8); i++) {
		ulong_t mask;
		mask = 1;
		mask <<= 1;
		if (value & mask) {
			return i;
		}
	}
	return -1;
}

static long_t ilog2(ulong_t value)
{
	return bsr(value);
}

static long_t tlog2(struct triple *ins)
{
	return ilog2(ins->u.cval);
}

static int is_pow2(struct triple *ins)
{
	ulong_t value, mask;
	long_t log;
	if (!is_const(ins)) {
		return 0;
	}
	value = ins->u.cval;
	log = ilog2(value);
	if (log == -1) {
		return 0;
	}
	mask = 1;
	mask <<= log;
	return  ((value & mask) == value);
}

static ulong_t read_const(struct compile_state *state,
	struct triple *ins, struct triple *rhs)
{
	switch(rhs->type->type &TYPE_MASK) {
	case TYPE_CHAR:
	case TYPE_SHORT:
	case TYPE_INT:
	case TYPE_LONG:
	case TYPE_UCHAR:
	case TYPE_USHORT:
	case TYPE_UINT:
	case TYPE_ULONG:
	case TYPE_POINTER:
	case TYPE_BITFIELD:
		break;
	default:
		fprintf(state->errout, "type: ");
		name_of(state->errout, rhs->type);
		fprintf(state->errout, "\n");
		internal_warning(state, rhs, "bad type to read_const");
		break;
	}
	if (!is_simple_const(rhs)) {
		internal_error(state, rhs, "bad op to read_const");
	}
	return rhs->u.cval;
}

static long_t read_sconst(struct compile_state *state,
	struct triple *ins, struct triple *rhs)
{
	return (long_t)(rhs->u.cval);
}

int const_ltrue(struct compile_state *state, struct triple *ins, struct triple *rhs)
{
	if (!is_const(rhs)) {
		internal_error(state, 0, "non const passed to const_true");
	}
	return !is_zero(rhs);
}

int const_eq(struct compile_state *state, struct triple *ins,
	struct triple *left, struct triple *right)
{
	int result;
	if (!is_const(left) || !is_const(right)) {
		internal_warning(state, ins, "non const passed to const_eq");
		result = -1;
	}
	else if (left == right) {
		result = 1;
	}
	else if (is_simple_const(left) && is_simple_const(right)) {
		ulong_t lval, rval;
		lval = read_const(state, ins, left);
		rval = read_const(state, ins, right);
		result = (lval == rval);
	}
	else if ((left->op == OP_ADDRCONST) &&
		(right->op == OP_ADDRCONST)) {
		result = (MISC(left, 0) == MISC(right, 0)) &&
			(left->u.cval == right->u.cval);
	}
	else {
		internal_warning(state, ins, "incomparable constants passed to const_eq");
		result = -1;
	}
	return result;

}

int const_ucmp(struct compile_state *state, struct triple *ins,
	struct triple *left, struct triple *right)
{
	int result;
	if (!is_const(left) || !is_const(right)) {
		internal_warning(state, ins, "non const past to const_ucmp");
		result = -2;
	}
	else if (left == right) {
		result = 0;
	}
	else if (is_simple_const(left) && is_simple_const(right)) {
		ulong_t lval, rval;
		lval = read_const(state, ins, left);
		rval = read_const(state, ins, right);
		result = 0;
		if (lval > rval) {
			result = 1;
		} else if (rval > lval) {
			result = -1;
		}
	}
	else if ((left->op == OP_ADDRCONST) &&
		(right->op == OP_ADDRCONST) &&
		(MISC(left, 0) == MISC(right, 0))) {
		result = 0;
		if (left->u.cval > right->u.cval) {
			result = 1;
		} else if (left->u.cval < right->u.cval) {
			result = -1;
		}
	}
	else {
		internal_warning(state, ins, "incomparable constants passed to const_ucmp");
		result = -2;
	}
	return result;
}

int const_scmp(struct compile_state *state, struct triple *ins,
	struct triple *left, struct triple *right)
{
	int result;
	if (!is_const(left) || !is_const(right)) {
		internal_warning(state, ins, "non const past to ucmp_const");
		result = -2;
	}
	else if (left == right) {
		result = 0;
	}
	else if (is_simple_const(left) && is_simple_const(right)) {
		long_t lval, rval;
		lval = read_sconst(state, ins, left);
		rval = read_sconst(state, ins, right);
		result = 0;
		if (lval > rval) {
			result = 1;
		} else if (rval > lval) {
			result = -1;
		}
	}
	else {
		internal_warning(state, ins, "incomparable constants passed to const_scmp");
		result = -2;
	}
	return result;
}

static void unuse_rhs(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	expr = triple_rhs(state, ins, 0);
	for(;expr;expr = triple_rhs(state, ins, expr)) {
		if (*expr) {
			unuse_triple(*expr, ins);
			*expr = 0;
		}
	}
}

static void unuse_lhs(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	expr = triple_lhs(state, ins, 0);
	for(;expr;expr = triple_lhs(state, ins, expr)) {
		unuse_triple(*expr, ins);
		*expr = 0;
	}
}

#if DEBUG_ROMCC_WARNING
static void unuse_misc(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	expr = triple_misc(state, ins, 0);
	for(;expr;expr = triple_misc(state, ins, expr)) {
		unuse_triple(*expr, ins);
		*expr = 0;
	}
}

static void unuse_targ(struct compile_state *state, struct triple *ins)
{
	int i;
	struct triple **slot;
	slot = &TARG(ins, 0);
	for(i = 0; i < ins->targ; i++) {
		unuse_triple(slot[i], ins);
		slot[i] = 0;
	}
}

static void check_lhs(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	expr = triple_lhs(state, ins, 0);
	for(;expr;expr = triple_lhs(state, ins, expr)) {
		internal_error(state, ins, "unexpected lhs");
	}

}
#endif

static void check_misc(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	expr = triple_misc(state, ins, 0);
	for(;expr;expr = triple_misc(state, ins, expr)) {
		if (*expr) {
			internal_error(state, ins, "unexpected misc");
		}
	}
}

static void check_targ(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	expr = triple_targ(state, ins, 0);
	for(;expr;expr = triple_targ(state, ins, expr)) {
		internal_error(state, ins, "unexpected targ");
	}
}

static void wipe_ins(struct compile_state *state, struct triple *ins)
{
	/* Becareful which instructions you replace the wiped
	 * instruction with, as there are not enough slots
	 * in all instructions to hold all others.
	 */
	check_targ(state, ins);
	check_misc(state, ins);
	unuse_rhs(state, ins);
	unuse_lhs(state, ins);
	ins->lhs  = 0;
	ins->rhs  = 0;
	ins->misc = 0;
	ins->targ = 0;
}

#if DEBUG_ROMCC_WARNING
static void wipe_branch(struct compile_state *state, struct triple *ins)
{
	/* Becareful which instructions you replace the wiped
	 * instruction with, as there are not enough slots
	 * in all instructions to hold all others.
	 */
	unuse_rhs(state, ins);
	unuse_lhs(state, ins);
	unuse_misc(state, ins);
	unuse_targ(state, ins);
	ins->lhs  = 0;
	ins->rhs  = 0;
	ins->misc = 0;
	ins->targ = 0;
}
#endif

static void mkcopy(struct compile_state *state,
	struct triple *ins, struct triple *rhs)
{
	struct block *block;
	if (!equiv_types(ins->type, rhs->type)) {
		FILE *fp = state->errout;
		fprintf(fp, "src type: ");
		name_of(fp, rhs->type);
		fprintf(fp, "\ndst type: ");
		name_of(fp, ins->type);
		fprintf(fp, "\n");
		internal_error(state, ins, "mkcopy type mismatch");
	}
	block = block_of_triple(state, ins);
	wipe_ins(state, ins);
	ins->op = OP_COPY;
	ins->rhs  = 1;
	ins->u.block = block;
	RHS(ins, 0) = rhs;
	use_triple(RHS(ins, 0), ins);
}

static void mkconst(struct compile_state *state,
	struct triple *ins, ulong_t value)
{
	if (!is_integral(ins) && !is_pointer(ins)) {
		fprintf(state->errout, "type: ");
		name_of(state->errout, ins->type);
		fprintf(state->errout, "\n");
		internal_error(state, ins, "unknown type to make constant value: %ld",
			value);
	}
	wipe_ins(state, ins);
	ins->op = OP_INTCONST;
	ins->u.cval = value;
}

static void mkaddr_const(struct compile_state *state,
	struct triple *ins, struct triple *sdecl, ulong_t value)
{
	if ((sdecl->op != OP_SDECL) && (sdecl->op != OP_LABEL)) {
		internal_error(state, ins, "bad base for addrconst");
	}
	wipe_ins(state, ins);
	ins->op = OP_ADDRCONST;
	ins->misc = 1;
	MISC(ins, 0) = sdecl;
	ins->u.cval = value;
	use_triple(sdecl, ins);
}

#if DEBUG_DECOMPOSE_PRINT_TUPLES
static void print_tuple(struct compile_state *state,
	struct triple *ins, struct triple *tuple)
{
	FILE *fp = state->dbgout;
	fprintf(fp, "%5s %p tuple: %p ", tops(ins->op), ins, tuple);
	name_of(fp, tuple->type);
	if (tuple->lhs > 0) {
		fprintf(fp, " lhs: ");
		name_of(fp, LHS(tuple, 0)->type);
	}
	fprintf(fp, "\n");

}
#endif

static struct triple *decompose_with_tuple(struct compile_state *state,
	struct triple *ins, struct triple *tuple)
{
	struct triple *next;
	next = ins->next;
	flatten(state, next, tuple);
#if DEBUG_DECOMPOSE_PRINT_TUPLES
	print_tuple(state, ins, tuple);
#endif

	if (!is_compound_type(tuple->type) && (tuple->lhs > 0)) {
		struct triple *tmp;
		if (tuple->lhs != 1) {
			internal_error(state, tuple, "plain type in multiple registers?");
		}
		tmp = LHS(tuple, 0);
		release_triple(state, tuple);
		tuple = tmp;
	}

	propagate_use(state, ins, tuple);
	release_triple(state, ins);

	return next;
}

static struct triple *decompose_unknownval(struct compile_state *state,
	struct triple *ins)
{
	struct triple *tuple;
	ulong_t i;

#if DEBUG_DECOMPOSE_HIRES
	FILE *fp = state->dbgout;
	fprintf(fp, "unknown type: ");
	name_of(fp, ins->type);
	fprintf(fp, "\n");
#endif

	get_occurrence(ins->occurrence);
	tuple = alloc_triple(state, OP_TUPLE, ins->type, -1, -1,
		ins->occurrence);

	for(i = 0; i < tuple->lhs; i++) {
		struct type *piece_type;
		struct triple *unknown;

		piece_type = reg_type(state, ins->type, i * REG_SIZEOF_REG);
		get_occurrence(tuple->occurrence);
		unknown = alloc_triple(state, OP_UNKNOWNVAL, piece_type, 0, 0,
			tuple->occurrence);
		LHS(tuple, i) = unknown;
	}
	return decompose_with_tuple(state, ins, tuple);
}


static struct triple *decompose_read(struct compile_state *state,
	struct triple *ins)
{
	struct triple *tuple, *lval;
	ulong_t i;

	lval = RHS(ins, 0);

	if (lval->op == OP_PIECE) {
		return ins->next;
	}
	get_occurrence(ins->occurrence);
	tuple = alloc_triple(state, OP_TUPLE, lval->type, -1, -1,
		ins->occurrence);

	if ((tuple->lhs != lval->lhs) &&
		(!triple_is_def(state, lval) || (tuple->lhs != 1)))
	{
		internal_error(state, ins, "lhs size inconsistency?");
	}
	for(i = 0; i < tuple->lhs; i++) {
		struct triple *piece, *read, *bitref;
		if ((i != 0) || !triple_is_def(state, lval)) {
			piece = LHS(lval, i);
		} else {
			piece = lval;
		}

		/* See if the piece is really a bitref */
		bitref = 0;
		if (piece->op == OP_BITREF) {
			bitref = piece;
			piece = RHS(bitref, 0);
		}

		get_occurrence(tuple->occurrence);
		read = alloc_triple(state, OP_READ, piece->type, -1, -1,
			tuple->occurrence);
		RHS(read, 0) = piece;

		if (bitref) {
			struct triple *extract;
			int op;
			if (is_signed(bitref->type->left)) {
				op = OP_SEXTRACT;
			} else {
				op = OP_UEXTRACT;
			}
			get_occurrence(tuple->occurrence);
			extract = alloc_triple(state, op, bitref->type, -1, -1,
				tuple->occurrence);
			RHS(extract, 0) = read;
			extract->u.bitfield.size   = bitref->u.bitfield.size;
			extract->u.bitfield.offset = bitref->u.bitfield.offset;

			read = extract;
		}

		LHS(tuple, i) = read;
	}
	return decompose_with_tuple(state, ins, tuple);
}

static struct triple *decompose_write(struct compile_state *state,
	struct triple *ins)
{
	struct triple *tuple, *lval, *val;
	ulong_t i;

	lval = MISC(ins, 0);
	val = RHS(ins, 0);
	get_occurrence(ins->occurrence);
	tuple = alloc_triple(state, OP_TUPLE, ins->type, -1, -1,
		ins->occurrence);

	if ((tuple->lhs != lval->lhs) &&
		(!triple_is_def(state, lval) || tuple->lhs != 1))
	{
		internal_error(state, ins, "lhs size inconsistency?");
	}
	for(i = 0; i < tuple->lhs; i++) {
		struct triple *piece, *write, *pval, *bitref;
		if ((i != 0) || !triple_is_def(state, lval)) {
			piece = LHS(lval, i);
		} else {
			piece = lval;
		}
		if ((i == 0) && (tuple->lhs == 1) && (val->lhs == 0)) {
			pval = val;
		}
		else {
			if (i > val->lhs) {
				internal_error(state, ins, "lhs size inconsistency?");
			}
			pval = LHS(val, i);
		}

		/* See if the piece is really a bitref */
		bitref = 0;
		if (piece->op == OP_BITREF) {
			struct triple *read, *deposit;
			bitref = piece;
			piece = RHS(bitref, 0);

			/* Read the destination register */
			get_occurrence(tuple->occurrence);
			read = alloc_triple(state, OP_READ, piece->type, -1, -1,
				tuple->occurrence);
			RHS(read, 0) = piece;

			/* Deposit the new bitfield value */
			get_occurrence(tuple->occurrence);
			deposit = alloc_triple(state, OP_DEPOSIT, piece->type, -1, -1,
				tuple->occurrence);
			RHS(deposit, 0) = read;
			RHS(deposit, 1) = pval;
			deposit->u.bitfield.size   = bitref->u.bitfield.size;
			deposit->u.bitfield.offset = bitref->u.bitfield.offset;

			/* Now write the newly generated value */
			pval = deposit;
		}

		get_occurrence(tuple->occurrence);
		write = alloc_triple(state, OP_WRITE, piece->type, -1, -1,
			tuple->occurrence);
		MISC(write, 0) = piece;
		RHS(write, 0) = pval;
		LHS(tuple, i) = write;
	}
	return decompose_with_tuple(state, ins, tuple);
}

struct decompose_load_info {
	struct occurrence *occurrence;
	struct triple *lval;
	struct triple *tuple;
};
static void decompose_load_cb(struct compile_state *state,
	struct type *type, size_t reg_offset, size_t mem_offset, void *arg)
{
	struct decompose_load_info *info = arg;
	struct triple *load;

	if (reg_offset > info->tuple->lhs) {
		internal_error(state, info->tuple, "lhs to small?");
	}
	get_occurrence(info->occurrence);
	load = alloc_triple(state, OP_LOAD, type, -1, -1, info->occurrence);
	RHS(load, 0) = mk_addr_expr(state, info->lval, mem_offset);
	LHS(info->tuple, reg_offset/REG_SIZEOF_REG) = load;
}

static struct triple *decompose_load(struct compile_state *state,
	struct triple *ins)
{
	struct triple *tuple;
	struct decompose_load_info info;

	if (!is_compound_type(ins->type)) {
		return ins->next;
	}
	get_occurrence(ins->occurrence);
	tuple = alloc_triple(state, OP_TUPLE, ins->type, -1, -1,
		ins->occurrence);

	info.occurrence = ins->occurrence;
	info.lval      = RHS(ins, 0);
	info.tuple     = tuple;
	walk_type_fields(state, ins->type, 0, 0, decompose_load_cb, &info);

	return decompose_with_tuple(state, ins, tuple);
}


struct decompose_store_info {
	struct occurrence *occurrence;
	struct triple *lval;
	struct triple *val;
	struct triple *tuple;
};
static void decompose_store_cb(struct compile_state *state,
	struct type *type, size_t reg_offset, size_t mem_offset, void *arg)
{
	struct decompose_store_info *info = arg;
	struct triple *store;

	if (reg_offset > info->tuple->lhs) {
		internal_error(state, info->tuple, "lhs to small?");
	}
	get_occurrence(info->occurrence);
	store = alloc_triple(state, OP_STORE, type, -1, -1, info->occurrence);
	RHS(store, 0) = mk_addr_expr(state, info->lval, mem_offset);
	RHS(store, 1) = LHS(info->val, reg_offset);
	LHS(info->tuple, reg_offset/REG_SIZEOF_REG) = store;
}

static struct triple *decompose_store(struct compile_state *state,
	struct triple *ins)
{
	struct triple *tuple;
	struct decompose_store_info info;

	if (!is_compound_type(ins->type)) {
		return ins->next;
	}
	get_occurrence(ins->occurrence);
	tuple = alloc_triple(state, OP_TUPLE, ins->type, -1, -1,
		ins->occurrence);

	info.occurrence = ins->occurrence;
	info.lval      = RHS(ins, 0);
	info.val       = RHS(ins, 1);
	info.tuple     = tuple;
	walk_type_fields(state, ins->type, 0, 0, decompose_store_cb, &info);

	return decompose_with_tuple(state, ins, tuple);
}

static struct triple *decompose_dot(struct compile_state *state,
	struct triple *ins)
{
	struct triple *tuple, *lval;
	struct type *type;
	size_t reg_offset;
	int i, idx;

	lval = MISC(ins, 0);
	reg_offset = field_reg_offset(state, lval->type, ins->u.field);
	idx  = reg_offset/REG_SIZEOF_REG;
	type = field_type(state, lval->type, ins->u.field);
#if DEBUG_DECOMPOSE_HIRES
	{
		FILE *fp = state->dbgout;
		fprintf(fp, "field type: ");
		name_of(fp, type);
		fprintf(fp, "\n");
	}
#endif

	get_occurrence(ins->occurrence);
	tuple = alloc_triple(state, OP_TUPLE, type, -1, -1,
		ins->occurrence);

	if (((ins->type->type & TYPE_MASK) == TYPE_BITFIELD) &&
		(tuple->lhs != 1))
	{
		internal_error(state, ins, "multi register bitfield?");
	}

	for(i = 0; i < tuple->lhs; i++, idx++) {
		struct triple *piece;
		if (!triple_is_def(state, lval)) {
			if (idx > lval->lhs) {
				internal_error(state, ins, "inconsistent lhs count");
			}
			piece = LHS(lval, idx);
		} else {
			if (idx != 0) {
				internal_error(state, ins, "bad reg_offset into def");
			}
			if (i != 0) {
				internal_error(state, ins, "bad reg count from def");
			}
			piece = lval;
		}

		/* Remember the offset of the bitfield */
		if ((type->type & TYPE_MASK) == TYPE_BITFIELD) {
			get_occurrence(ins->occurrence);
			piece = build_triple(state, OP_BITREF, type, piece, 0,
				ins->occurrence);
			piece->u.bitfield.size   = size_of(state, type);
			piece->u.bitfield.offset = reg_offset % REG_SIZEOF_REG;
		}
		else if ((reg_offset % REG_SIZEOF_REG) != 0) {
			internal_error(state, ins,
				"request for a nonbitfield sub register?");
		}

		LHS(tuple, i) = piece;
	}

	return decompose_with_tuple(state, ins, tuple);
}

static struct triple *decompose_index(struct compile_state *state,
	struct triple *ins)
{
	struct triple *tuple, *lval;
	struct type *type;
	int i, idx;

	lval = MISC(ins, 0);
	idx = index_reg_offset(state, lval->type, ins->u.cval)/REG_SIZEOF_REG;
	type = index_type(state, lval->type, ins->u.cval);
#if DEBUG_DECOMPOSE_HIRES
{
	FILE *fp = state->dbgout;
	fprintf(fp, "index type: ");
	name_of(fp, type);
	fprintf(fp, "\n");
}
#endif

	get_occurrence(ins->occurrence);
	tuple = alloc_triple(state, OP_TUPLE, type, -1, -1,
		ins->occurrence);

	for(i = 0; i < tuple->lhs; i++, idx++) {
		struct triple *piece;
		if (!triple_is_def(state, lval)) {
			if (idx > lval->lhs) {
				internal_error(state, ins, "inconsistent lhs count");
			}
			piece = LHS(lval, idx);
		} else {
			if (idx != 0) {
				internal_error(state, ins, "bad reg_offset into def");
			}
			if (i != 0) {
				internal_error(state, ins, "bad reg count from def");
			}
			piece = lval;
		}
		LHS(tuple, i) = piece;
	}

	return decompose_with_tuple(state, ins, tuple);
}

static void decompose_compound_types(struct compile_state *state)
{
	struct triple *ins, *next, *first;
	first = state->first;

	/* Pass one expand compound values into pseudo registers.
	 */
	next = first;
	do {
		ins = next;
		next = ins->next;
		switch(ins->op) {
		case OP_UNKNOWNVAL:
			next = decompose_unknownval(state, ins);
			break;

		case OP_READ:
			next = decompose_read(state, ins);
			break;

		case OP_WRITE:
			next = decompose_write(state, ins);
			break;


		/* Be very careful with the load/store logic. These
		 * operations must convert from the in register layout
		 * to the in memory layout, which is nontrivial.
		 */
		case OP_LOAD:
			next = decompose_load(state, ins);
			break;
		case OP_STORE:
			next = decompose_store(state, ins);
			break;

		case OP_DOT:
			next = decompose_dot(state, ins);
			break;
		case OP_INDEX:
			next = decompose_index(state, ins);
			break;

		}
#if DEBUG_DECOMPOSE_HIRES
		fprintf(fp, "decompose next: %p\n", next);
		fflush(fp);
		fprintf(fp, "next->op: %d %s\n",
			next->op, tops(next->op));
		/* High resolution debugging mode */
		print_triples(state);
#endif
	} while (next != first);

	/* Pass two remove the tuples.
	 */
	ins = first;
	do {
		next = ins->next;
		if (ins->op == OP_TUPLE) {
			if (ins->use) {
				internal_error(state, ins, "tuple used");
			}
			else {
				release_triple(state, ins);
			}
		}
		ins = next;
	} while(ins != first);
	ins = first;
	do {
		next = ins->next;
		if (ins->op == OP_BITREF) {
			if (ins->use) {
				internal_error(state, ins, "bitref used");
			}
			else {
				release_triple(state, ins);
			}
		}
		ins = next;
	} while(ins != first);

	/* Pass three verify the state and set ->id to 0.
	 */
	next = first;
	do {
		ins = next;
		next = ins->next;
		ins->id &= ~TRIPLE_FLAG_FLATTENED;
		if (triple_stores_block(state, ins)) {
			ins->u.block = 0;
		}
		if (triple_is_def(state, ins)) {
			if (reg_size_of(state, ins->type) > REG_SIZEOF_REG) {
				internal_error(state, ins, "multi register value remains?");
			}
		}
		if (ins->op == OP_DOT) {
			internal_error(state, ins, "OP_DOT remains?");
		}
		if (ins->op == OP_INDEX) {
			internal_error(state, ins, "OP_INDEX remains?");
		}
		if (ins->op == OP_BITREF) {
			internal_error(state, ins, "OP_BITREF remains?");
		}
		if (ins->op == OP_TUPLE) {
			internal_error(state, ins, "OP_TUPLE remains?");
		}
	} while(next != first);
}

/* For those operations that cannot be simplified */
static void simplify_noop(struct compile_state *state, struct triple *ins)
{
	return;
}

static void simplify_smul(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && !is_const(RHS(ins, 1))) {
		struct triple *tmp;
		tmp = RHS(ins, 0);
		RHS(ins, 0) = RHS(ins, 1);
		RHS(ins, 1) = tmp;
	}
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(state, ins, RHS(ins, 0));
		right = read_sconst(state, ins, RHS(ins, 1));
		mkconst(state, ins, left * right);
	}
	else if (is_zero(RHS(ins, 1))) {
		mkconst(state, ins, 0);
	}
	else if (is_one(RHS(ins, 1))) {
		mkcopy(state, ins, RHS(ins, 0));
	}
	else if (is_pow2(RHS(ins, 1))) {
		struct triple *val;
		val = int_const(state, ins->type, tlog2(RHS(ins, 1)));
		ins->op = OP_SL;
		insert_triple(state, state->global_pool, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_umul(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && !is_const(RHS(ins, 1))) {
		struct triple *tmp;
		tmp = RHS(ins, 0);
		RHS(ins, 0) = RHS(ins, 1);
		RHS(ins, 1) = tmp;
	}
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left * right);
	}
	else if (is_zero(RHS(ins, 1))) {
		mkconst(state, ins, 0);
	}
	else if (is_one(RHS(ins, 1))) {
		mkcopy(state, ins, RHS(ins, 0));
	}
	else if (is_pow2(RHS(ins, 1))) {
		struct triple *val;
		val = int_const(state, ins->type, tlog2(RHS(ins, 1)));
		ins->op = OP_SL;
		insert_triple(state, state->global_pool, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_sdiv(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(state, ins, RHS(ins, 0));
		right = read_sconst(state, ins, RHS(ins, 1));
		mkconst(state, ins, left / right);
	}
	else if (is_zero(RHS(ins, 0))) {
		mkconst(state, ins, 0);
	}
	else if (is_zero(RHS(ins, 1))) {
		error(state, ins, "division by zero");
	}
	else if (is_one(RHS(ins, 1))) {
		mkcopy(state, ins, RHS(ins, 0));
	}
	else if (is_pow2(RHS(ins, 1))) {
		struct triple *val;
		val = int_const(state, ins->type, tlog2(RHS(ins, 1)));
		ins->op = OP_SSR;
		insert_triple(state, state->global_pool, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_udiv(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left / right);
	}
	else if (is_zero(RHS(ins, 0))) {
		mkconst(state, ins, 0);
	}
	else if (is_zero(RHS(ins, 1))) {
		error(state, ins, "division by zero");
	}
	else if (is_one(RHS(ins, 1))) {
		mkcopy(state, ins, RHS(ins, 0));
	}
	else if (is_pow2(RHS(ins, 1))) {
		struct triple *val;
		val = int_const(state, ins->type, tlog2(RHS(ins, 1)));
		ins->op = OP_USR;
		insert_triple(state, state->global_pool, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_smod(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left % right);
	}
	else if (is_zero(RHS(ins, 0))) {
		mkconst(state, ins, 0);
	}
	else if (is_zero(RHS(ins, 1))) {
		error(state, ins, "division by zero");
	}
	else if (is_one(RHS(ins, 1))) {
		mkconst(state, ins, 0);
	}
	else if (is_pow2(RHS(ins, 1))) {
		struct triple *val;
		val = int_const(state, ins->type, RHS(ins, 1)->u.cval - 1);
		ins->op = OP_AND;
		insert_triple(state, state->global_pool, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_umod(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left % right);
	}
	else if (is_zero(RHS(ins, 0))) {
		mkconst(state, ins, 0);
	}
	else if (is_zero(RHS(ins, 1))) {
		error(state, ins, "division by zero");
	}
	else if (is_one(RHS(ins, 1))) {
		mkconst(state, ins, 0);
	}
	else if (is_pow2(RHS(ins, 1))) {
		struct triple *val;
		val = int_const(state, ins->type, RHS(ins, 1)->u.cval - 1);
		ins->op = OP_AND;
		insert_triple(state, state->global_pool, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_add(struct compile_state *state, struct triple *ins)
{
	/* start with the pointer on the left */
	if (is_pointer(RHS(ins, 1))) {
		struct triple *tmp;
		tmp = RHS(ins, 0);
		RHS(ins, 0) = RHS(ins, 1);
		RHS(ins, 1) = tmp;
	}
	if (is_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		if (RHS(ins, 0)->op == OP_INTCONST) {
			ulong_t left, right;
			left  = read_const(state, ins, RHS(ins, 0));
			right = read_const(state, ins, RHS(ins, 1));
			mkconst(state, ins, left + right);
		}
		else if (RHS(ins, 0)->op == OP_ADDRCONST) {
			struct triple *sdecl;
			ulong_t left, right;
			sdecl = MISC(RHS(ins, 0), 0);
			left  = RHS(ins, 0)->u.cval;
			right = RHS(ins, 1)->u.cval;
			mkaddr_const(state, ins, sdecl, left + right);
		}
		else {
			internal_warning(state, ins, "Optimize me!");
		}
	}
	else if (is_const(RHS(ins, 0)) && !is_const(RHS(ins, 1))) {
		struct triple *tmp;
		tmp = RHS(ins, 1);
		RHS(ins, 1) = RHS(ins, 0);
		RHS(ins, 0) = tmp;
	}
}

static void simplify_sub(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		if (RHS(ins, 0)->op == OP_INTCONST) {
			ulong_t left, right;
			left  = read_const(state, ins, RHS(ins, 0));
			right = read_const(state, ins, RHS(ins, 1));
			mkconst(state, ins, left - right);
		}
		else if (RHS(ins, 0)->op == OP_ADDRCONST) {
			struct triple *sdecl;
			ulong_t left, right;
			sdecl = MISC(RHS(ins, 0), 0);
			left  = RHS(ins, 0)->u.cval;
			right = RHS(ins, 1)->u.cval;
			mkaddr_const(state, ins, sdecl, left - right);
		}
		else {
			internal_warning(state, ins, "Optimize me!");
		}
	}
}

static void simplify_sl(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, RHS(ins, 1));
		if (right >= (size_of(state, ins->type))) {
			warning(state, ins, "left shift count >= width of type");
		}
	}
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins,  left << right);
	}
}

static void simplify_usr(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, RHS(ins, 1));
		if (right >= (size_of(state, ins->type))) {
			warning(state, ins, "right shift count >= width of type");
		}
	}
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left >> right);
	}
}

static void simplify_ssr(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, RHS(ins, 1));
		if (right >= (size_of(state, ins->type))) {
			warning(state, ins, "right shift count >= width of type");
		}
	}
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(state, ins, RHS(ins, 0));
		right = read_sconst(state, ins, RHS(ins, 1));
		mkconst(state, ins, left >> right);
	}
}

static void simplify_and(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_simple_const(left) && is_simple_const(right)) {
		ulong_t lval, rval;
		lval = read_const(state, ins, left);
		rval = read_const(state, ins, right);
		mkconst(state, ins, lval & rval);
	}
	else if (is_zero(right) || is_zero(left)) {
		mkconst(state, ins, 0);
	}
}

static void simplify_or(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_simple_const(left) && is_simple_const(right)) {
		ulong_t lval, rval;
		lval = read_const(state, ins, left);
		rval = read_const(state, ins, right);
		mkconst(state, ins, lval | rval);
	}
#if 0 /* I need to handle type mismatches here... */
	else if (is_zero(right)) {
		mkcopy(state, ins, left);
	}
	else if (is_zero(left)) {
		mkcopy(state, ins, right);
	}
#endif
}

static void simplify_xor(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left ^ right);
	}
}

static void simplify_pos(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		mkconst(state, ins, RHS(ins, 0)->u.cval);
	}
	else {
		mkcopy(state, ins, RHS(ins, 0));
	}
}

static void simplify_neg(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, RHS(ins, 0));
		mkconst(state, ins, -left);
	}
	else if (RHS(ins, 0)->op == OP_NEG) {
		mkcopy(state, ins, RHS(RHS(ins, 0), 0));
	}
}

static void simplify_invert(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, RHS(ins, 0));
		mkconst(state, ins, ~left);
	}
}

static void simplify_eq(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_eq(state, ins, left, right);
		if (val >= 0) {
			mkconst(state, ins, val == 1);
		}
	}
	else if (left == right) {
		mkconst(state, ins, 1);
	}
}

static void simplify_noteq(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_eq(state, ins, left, right);
		if (val >= 0) {
			mkconst(state, ins, val != 1);
		}
	}
	if (left == right) {
		mkconst(state, ins, 0);
	}
}

static void simplify_sless(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_scmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val < 0);
		}
	}
	else if (left == right) {
		mkconst(state, ins, 0);
	}
}

static void simplify_uless(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_ucmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val < 0);
		}
	}
	else if (is_zero(right)) {
		mkconst(state, ins, 0);
	}
	else if (left == right) {
		mkconst(state, ins, 0);
	}
}

static void simplify_smore(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_scmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val > 0);
		}
	}
	else if (left == right) {
		mkconst(state, ins, 0);
	}
}

static void simplify_umore(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_ucmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val > 0);
		}
	}
	else if (is_zero(left)) {
		mkconst(state, ins, 0);
	}
	else if (left == right) {
		mkconst(state, ins, 0);
	}
}


static void simplify_slesseq(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_scmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val <= 0);
		}
	}
	else if (left == right) {
		mkconst(state, ins, 1);
	}
}

static void simplify_ulesseq(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_ucmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val <= 0);
		}
	}
	else if (is_zero(left)) {
		mkconst(state, ins, 1);
	}
	else if (left == right) {
		mkconst(state, ins, 1);
	}
}

static void simplify_smoreeq(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_scmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val >= 0);
		}
	}
	else if (left == right) {
		mkconst(state, ins, 1);
	}
}

static void simplify_umoreeq(struct compile_state *state, struct triple *ins)
{
	struct triple *left, *right;
	left = RHS(ins, 0);
	right = RHS(ins, 1);

	if (is_const(left) && is_const(right)) {
		int val;
		val = const_ucmp(state, ins, left, right);
		if ((val >= -1) && (val <= 1)) {
			mkconst(state, ins, val >= 0);
		}
	}
	else if (is_zero(right)) {
		mkconst(state, ins, 1);
	}
	else if (left == right) {
		mkconst(state, ins, 1);
	}
}

static void simplify_lfalse(struct compile_state *state, struct triple *ins)
{
	struct triple *rhs;
	rhs = RHS(ins, 0);

	if (is_const(rhs)) {
		mkconst(state, ins, !const_ltrue(state, ins, rhs));
	}
	/* Otherwise if I am the only user... */
	else if ((rhs->use) &&
		(rhs->use->member == ins) && (rhs->use->next == 0)) {
		int need_copy = 1;
		/* Invert a boolean operation */
		switch(rhs->op) {
		case OP_LTRUE:   rhs->op = OP_LFALSE;  break;
		case OP_LFALSE:  rhs->op = OP_LTRUE;   break;
		case OP_EQ:      rhs->op = OP_NOTEQ;   break;
		case OP_NOTEQ:   rhs->op = OP_EQ;      break;
		case OP_SLESS:   rhs->op = OP_SMOREEQ; break;
		case OP_ULESS:   rhs->op = OP_UMOREEQ; break;
		case OP_SMORE:   rhs->op = OP_SLESSEQ; break;
		case OP_UMORE:   rhs->op = OP_ULESSEQ; break;
		case OP_SLESSEQ: rhs->op = OP_SMORE;   break;
		case OP_ULESSEQ: rhs->op = OP_UMORE;   break;
		case OP_SMOREEQ: rhs->op = OP_SLESS;   break;
		case OP_UMOREEQ: rhs->op = OP_ULESS;   break;
		default:
			need_copy = 0;
			break;
		}
		if (need_copy) {
			mkcopy(state, ins, rhs);
		}
	}
}

static void simplify_ltrue (struct compile_state *state, struct triple *ins)
{
	struct triple *rhs;
	rhs = RHS(ins, 0);

	if (is_const(rhs)) {
		mkconst(state, ins, const_ltrue(state, ins, rhs));
	}
	else switch(rhs->op) {
	case OP_LTRUE:   case OP_LFALSE:  case OP_EQ:      case OP_NOTEQ:
	case OP_SLESS:   case OP_ULESS:   case OP_SMORE:   case OP_UMORE:
	case OP_SLESSEQ: case OP_ULESSEQ: case OP_SMOREEQ: case OP_UMOREEQ:
		mkcopy(state, ins, rhs);
	}

}

static void simplify_load(struct compile_state *state, struct triple *ins)
{
	struct triple *addr, *sdecl, *blob;

	/* If I am doing a load with a constant pointer from a constant
	 * table get the value.
	 */
	addr = RHS(ins, 0);
	if ((addr->op == OP_ADDRCONST) && (sdecl = MISC(addr, 0)) &&
		(sdecl->op == OP_SDECL) && (blob = MISC(sdecl, 0)) &&
		(blob->op == OP_BLOBCONST)) {
		unsigned char buffer[SIZEOF_WORD];
		size_t reg_size, mem_size;
		const char *src, *end;
		ulong_t val;
		reg_size = reg_size_of(state, ins->type);
		if (reg_size > REG_SIZEOF_REG) {
			internal_error(state, ins, "load size greater than register");
		}
		mem_size = size_of(state, ins->type);
		end = blob->u.blob;
		end += bits_to_bytes(size_of(state, sdecl->type));
		src = blob->u.blob;
		src += addr->u.cval;

		if (src > end) {
			error(state, ins, "Load address out of bounds");
		}

		memset(buffer, 0, sizeof(buffer));
		memcpy(buffer, src, bits_to_bytes(mem_size));

		switch(mem_size) {
		case SIZEOF_I8:  val = *((uint8_t *) buffer); break;
		case SIZEOF_I16: val = *((uint16_t *)buffer); break;
		case SIZEOF_I32: val = *((uint32_t *)buffer); break;
		case SIZEOF_I64: val = *((uint64_t *)buffer); break;
		default:
			internal_error(state, ins, "mem_size: %d not handled",
				mem_size);
			val = 0;
			break;
		}
		mkconst(state, ins, val);
	}
}

static void simplify_uextract(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0))) {
		ulong_t val;
		ulong_t mask;
		val = read_const(state, ins, RHS(ins, 0));
		mask = 1;
		mask <<= ins->u.bitfield.size;
		mask -= 1;
		val >>= ins->u.bitfield.offset;
		val &= mask;
		mkconst(state, ins, val);
	}
}

static void simplify_sextract(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0))) {
		ulong_t val;
		ulong_t mask;
		long_t sval;
		val = read_const(state, ins, RHS(ins, 0));
		mask = 1;
		mask <<= ins->u.bitfield.size;
		mask -= 1;
		val >>= ins->u.bitfield.offset;
		val &= mask;
		val <<= (SIZEOF_LONG - ins->u.bitfield.size);
		sval = val;
		sval >>= (SIZEOF_LONG - ins->u.bitfield.size);
		mkconst(state, ins, sval);
	}
}

static void simplify_deposit(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0)) && is_simple_const(RHS(ins, 1))) {
		ulong_t targ, val;
		ulong_t mask;
		targ = read_const(state, ins, RHS(ins, 0));
		val  = read_const(state, ins, RHS(ins, 1));
		mask = 1;
		mask <<= ins->u.bitfield.size;
		mask -= 1;
		mask <<= ins->u.bitfield.offset;
		targ &= ~mask;
		val <<= ins->u.bitfield.offset;
		val &= mask;
		targ |= val;
		mkconst(state, ins, targ);
	}
}

static void simplify_copy(struct compile_state *state, struct triple *ins)
{
	struct triple *right;
	right = RHS(ins, 0);
	if (is_subset_type(ins->type, right->type)) {
		ins->type = right->type;
	}
	if (equiv_types(ins->type, right->type)) {
		ins->op = OP_COPY;/* I don't need to convert if the types match */
	} else {
		if (ins->op == OP_COPY) {
			internal_error(state, ins, "type mismatch on copy");
		}
	}
	if (is_const(right) && (right->op == OP_ADDRCONST) && is_pointer(ins)) {
		struct triple *sdecl;
		ulong_t offset;
		sdecl  = MISC(right, 0);
		offset = right->u.cval;
		mkaddr_const(state, ins, sdecl, offset);
	}
	else if (is_const(right) && is_write_compatible(state, ins->type, right->type)) {
		switch(right->op) {
		case OP_INTCONST:
		{
			ulong_t left;
			left = read_const(state, ins, right);
			/* Ensure I have not overflowed the destination. */
			if (size_of(state, right->type) > size_of(state, ins->type)) {
				ulong_t mask;
				mask = 1;
				mask <<= size_of(state, ins->type);
				mask -= 1;
				left &= mask;
			}
			/* Ensure I am properly sign extended */
			if (size_of(state, right->type) < size_of(state, ins->type) &&
				is_signed(right->type)) {
				uint64_t val;
				int shift;
				shift = SIZEOF_LONG - size_of(state, right->type);
				val = left;
				val <<= shift;
				val >>= shift;
				left = (ulong_t)val;
			}
			mkconst(state, ins, left);
			break;
		}
		default:
			internal_error(state, ins, "uknown constant");
			break;
		}
	}
}

static int phi_present(struct block *block)
{
	struct triple *ptr;
	if (!block) {
		return 0;
	}
	ptr = block->first;
	do {
		if (ptr->op == OP_PHI) {
			return 1;
		}
		ptr = ptr->next;
	} while(ptr != block->last);
	return 0;
}

static int phi_dependency(struct block *block)
{
	/* A block has a phi dependency if a phi function
	 * depends on that block to exist, and makes a block
	 * that is otherwise useless unsafe to remove.
	 */
	if (block) {
		struct block_set *edge;
		for(edge = block->edges; edge; edge = edge->next) {
			if (phi_present(edge->member)) {
				return 1;
			}
		}
	}
	return 0;
}

static struct triple *branch_target(struct compile_state *state, struct triple *ins)
{
	struct triple *targ;
	targ = TARG(ins, 0);
	/* During scc_transform temporary triples are allocated that
	 * loop back onto themselves. If I see one don't advance the
	 * target.
	 */
	while(triple_is_structural(state, targ) &&
		(targ->next != targ) && (targ->next != state->first)) {
		targ = targ->next;
	}
	return targ;
}


static void simplify_branch(struct compile_state *state, struct triple *ins)
{
	int simplified, loops;
	if ((ins->op != OP_BRANCH) && (ins->op != OP_CBRANCH)) {
		internal_error(state, ins, "not branch");
	}
	if (ins->use != 0) {
		internal_error(state, ins, "branch use");
	}
	/* The challenge here with simplify branch is that I need to
	 * make modifications to the control flow graph as well
	 * as to the branch instruction itself.  That is handled
	 * by rebuilding the basic blocks after simplify all is called.
	 */

	/* If we have a branch to an unconditional branch update
	 * our target.  But watch out for dependencies from phi
	 * functions.
	 * Also only do this a limited number of times so
	 * we don't get into an infinite loop.
	 */
	loops = 0;
	do {
		struct triple *targ;
		simplified = 0;
		targ = branch_target(state, ins);
		if ((targ != ins) && (targ->op == OP_BRANCH) &&
			!phi_dependency(targ->u.block))
		{
			unuse_triple(TARG(ins, 0), ins);
			TARG(ins, 0) = TARG(targ, 0);
			use_triple(TARG(ins, 0), ins);
			simplified = 1;
		}
	} while(simplified && (++loops < 20));

	/* If we have a conditional branch with a constant condition
	 * make it an unconditional branch.
	 */
	if ((ins->op == OP_CBRANCH) && is_simple_const(RHS(ins, 0))) {
		struct triple *targ;
		ulong_t value;
		value = read_const(state, ins, RHS(ins, 0));
		unuse_triple(RHS(ins, 0), ins);
		targ = TARG(ins, 0);
		ins->rhs  = 0;
		ins->targ = 1;
		ins->op = OP_BRANCH;
		if (value) {
			unuse_triple(ins->next, ins);
			TARG(ins, 0) = targ;
		}
		else {
			unuse_triple(targ, ins);
			TARG(ins, 0) = ins->next;
		}
	}

	/* If we have a branch to the next instruction,
	 * make it a noop.
	 */
	if (TARG(ins, 0) == ins->next) {
		unuse_triple(TARG(ins, 0), ins);
		if (ins->op == OP_CBRANCH) {
			unuse_triple(RHS(ins, 0), ins);
			unuse_triple(ins->next, ins);
		}
		ins->lhs = 0;
		ins->rhs = 0;
		ins->misc = 0;
		ins->targ = 0;
		ins->op = OP_NOOP;
		if (ins->use) {
			internal_error(state, ins, "noop use != 0");
		}
	}
}

static void simplify_label(struct compile_state *state, struct triple *ins)
{
	/* Ignore volatile labels */
	if (!triple_is_pure(state, ins, ins->id)) {
		return;
	}
	if (ins->use == 0) {
		ins->op = OP_NOOP;
	}
	else if (ins->prev->op == OP_LABEL) {
		/* In general it is not safe to merge one label that
		 * imediately follows another.  The problem is that the empty
		 * looking block may have phi functions that depend on it.
		 */
		if (!phi_dependency(ins->prev->u.block)) {
			struct triple_set *user, *next;
			ins->op = OP_NOOP;
			for(user = ins->use; user; user = next) {
				struct triple *use, **expr;
				next = user->next;
				use = user->member;
				expr = triple_targ(state, use, 0);
				for(;expr; expr = triple_targ(state, use, expr)) {
					if (*expr == ins) {
						*expr = ins->prev;
						unuse_triple(ins, use);
						use_triple(ins->prev, use);
					}

				}
			}
			if (ins->use) {
				internal_error(state, ins, "noop use != 0");
			}
		}
	}
}

static void simplify_phi(struct compile_state *state, struct triple *ins)
{
	struct triple **slot;
	struct triple *value;
	int zrhs, i;
	ulong_t cvalue;
	slot = &RHS(ins, 0);
	zrhs = ins->rhs;
	if (zrhs == 0) {
		return;
	}
	/* See if all of the rhs members of a phi have the same value */
	if (slot[0] && is_simple_const(slot[0])) {
		cvalue = read_const(state, ins, slot[0]);
		for(i = 1; i < zrhs; i++) {
			if (	!slot[i] ||
				!is_simple_const(slot[i]) ||
				!equiv_types(slot[0]->type, slot[i]->type) ||
				(cvalue != read_const(state, ins, slot[i]))) {
				break;
			}
		}
		if (i == zrhs) {
			mkconst(state, ins, cvalue);
			return;
		}
	}

	/* See if all of rhs members of a phi are the same */
	value = slot[0];
	for(i = 1; i < zrhs; i++) {
		if (slot[i] != value) {
			break;
		}
	}
	if (i == zrhs) {
		/* If the phi has a single value just copy it */
		if (!is_subset_type(ins->type, value->type)) {
			internal_error(state, ins, "bad input type to phi");
		}
		/* Make the types match */
		if (!equiv_types(ins->type, value->type)) {
			ins->type = value->type;
		}
		/* Now make the actual copy */
		mkcopy(state, ins, value);
		return;
	}
}


static void simplify_bsf(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, RHS(ins, 0));
		mkconst(state, ins, bsf(left));
	}
}

static void simplify_bsr(struct compile_state *state, struct triple *ins)
{
	if (is_simple_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, RHS(ins, 0));
		mkconst(state, ins, bsr(left));
	}
}


typedef void (*simplify_t)(struct compile_state *state, struct triple *ins);
static const struct simplify_table {
	simplify_t func;
	unsigned long flag;
} table_simplify[] = {
#define simplify_sdivt    simplify_noop
#define simplify_udivt    simplify_noop
#define simplify_piece    simplify_noop

[OP_SDIVT      ] = { simplify_sdivt,    COMPILER_SIMPLIFY_ARITH },
[OP_UDIVT      ] = { simplify_udivt,	COMPILER_SIMPLIFY_ARITH },
[OP_SMUL       ] = { simplify_smul,	COMPILER_SIMPLIFY_ARITH },
[OP_UMUL       ] = { simplify_umul,	COMPILER_SIMPLIFY_ARITH },
[OP_SDIV       ] = { simplify_sdiv,	COMPILER_SIMPLIFY_ARITH },
[OP_UDIV       ] = { simplify_udiv,	COMPILER_SIMPLIFY_ARITH },
[OP_SMOD       ] = { simplify_smod,	COMPILER_SIMPLIFY_ARITH },
[OP_UMOD       ] = { simplify_umod,	COMPILER_SIMPLIFY_ARITH },
[OP_ADD        ] = { simplify_add,	COMPILER_SIMPLIFY_ARITH },
[OP_SUB        ] = { simplify_sub,	COMPILER_SIMPLIFY_ARITH },
[OP_SL         ] = { simplify_sl,	COMPILER_SIMPLIFY_SHIFT },
[OP_USR        ] = { simplify_usr,	COMPILER_SIMPLIFY_SHIFT },
[OP_SSR        ] = { simplify_ssr,	COMPILER_SIMPLIFY_SHIFT },
[OP_AND        ] = { simplify_and,	COMPILER_SIMPLIFY_BITWISE },
[OP_XOR        ] = { simplify_xor,	COMPILER_SIMPLIFY_BITWISE },
[OP_OR         ] = { simplify_or,	COMPILER_SIMPLIFY_BITWISE },
[OP_POS        ] = { simplify_pos,	COMPILER_SIMPLIFY_ARITH },
[OP_NEG        ] = { simplify_neg,	COMPILER_SIMPLIFY_ARITH },
[OP_INVERT     ] = { simplify_invert,	COMPILER_SIMPLIFY_BITWISE },

[OP_EQ         ] = { simplify_eq,	COMPILER_SIMPLIFY_LOGICAL },
[OP_NOTEQ      ] = { simplify_noteq,	COMPILER_SIMPLIFY_LOGICAL },
[OP_SLESS      ] = { simplify_sless,	COMPILER_SIMPLIFY_LOGICAL },
[OP_ULESS      ] = { simplify_uless,	COMPILER_SIMPLIFY_LOGICAL },
[OP_SMORE      ] = { simplify_smore,	COMPILER_SIMPLIFY_LOGICAL },
[OP_UMORE      ] = { simplify_umore,	COMPILER_SIMPLIFY_LOGICAL },
[OP_SLESSEQ    ] = { simplify_slesseq, 	COMPILER_SIMPLIFY_LOGICAL },
[OP_ULESSEQ    ] = { simplify_ulesseq,	COMPILER_SIMPLIFY_LOGICAL },
[OP_SMOREEQ    ] = { simplify_smoreeq,	COMPILER_SIMPLIFY_LOGICAL },
[OP_UMOREEQ    ] = { simplify_umoreeq,	COMPILER_SIMPLIFY_LOGICAL },
[OP_LFALSE     ] = { simplify_lfalse,	COMPILER_SIMPLIFY_LOGICAL },
[OP_LTRUE      ] = { simplify_ltrue,	COMPILER_SIMPLIFY_LOGICAL },

[OP_LOAD       ] = { simplify_load,	COMPILER_SIMPLIFY_OP },
[OP_STORE      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_UEXTRACT   ] = { simplify_uextract, COMPILER_SIMPLIFY_BITFIELD },
[OP_SEXTRACT   ] = { simplify_sextract, COMPILER_SIMPLIFY_BITFIELD },
[OP_DEPOSIT    ] = { simplify_deposit,  COMPILER_SIMPLIFY_BITFIELD },

[OP_NOOP       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_INTCONST   ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_BLOBCONST  ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_ADDRCONST  ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_UNKNOWNVAL ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_WRITE      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_READ       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_COPY       ] = { simplify_copy,	COMPILER_SIMPLIFY_COPY },
[OP_CONVERT    ] = { simplify_copy,	COMPILER_SIMPLIFY_COPY },
[OP_PIECE      ] = { simplify_piece,	COMPILER_SIMPLIFY_OP },
[OP_ASM        ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_DOT        ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_INDEX      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_LIST       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_BRANCH     ] = { simplify_branch,	COMPILER_SIMPLIFY_BRANCH },
[OP_CBRANCH    ] = { simplify_branch,	COMPILER_SIMPLIFY_BRANCH },
[OP_CALL       ] = { simplify_noop,	COMPILER_SIMPLIFY_BRANCH },
[OP_RET        ] = { simplify_noop,	COMPILER_SIMPLIFY_BRANCH },
[OP_LABEL      ] = { simplify_label,	COMPILER_SIMPLIFY_LABEL },
[OP_ADECL      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_SDECL      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_PHI        ] = { simplify_phi,	COMPILER_SIMPLIFY_PHI },

[OP_INB        ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_INW        ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_INL        ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_OUTB       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_OUTW       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_OUTL       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_BSF        ] = { simplify_bsf,	COMPILER_SIMPLIFY_OP },
[OP_BSR        ] = { simplify_bsr,	COMPILER_SIMPLIFY_OP },
[OP_RDMSR      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_WRMSR      ] = { simplify_noop,     COMPILER_SIMPLIFY_OP },
[OP_HLT        ] = { simplify_noop,     COMPILER_SIMPLIFY_OP },
};

static inline void debug_simplify(struct compile_state *state,
	simplify_t do_simplify, struct triple *ins)
{
#if DEBUG_SIMPLIFY_HIRES
		if (state->functions_joined && (do_simplify != simplify_noop)) {
			/* High resolution debugging mode */
			fprintf(state->dbgout, "simplifing: ");
			display_triple(state->dbgout, ins);
		}
#endif
		do_simplify(state, ins);
#if DEBUG_SIMPLIFY_HIRES
		if (state->functions_joined && (do_simplify != simplify_noop)) {
			/* High resolution debugging mode */
			fprintf(state->dbgout, "simplified: ");
			display_triple(state->dbgout, ins);
		}
#endif
}
static void simplify(struct compile_state *state, struct triple *ins)
{
	int op;
	simplify_t do_simplify;
	if (ins == &unknown_triple) {
		internal_error(state, ins, "simplifying the unknown triple?");
	}
	do {
		op = ins->op;
		do_simplify = 0;
		if ((op < 0) || (op >= sizeof(table_simplify)/sizeof(table_simplify[0]))) {
			do_simplify = 0;
		}
		else {
			do_simplify = table_simplify[op].func;
		}
		if (do_simplify &&
			!(state->compiler->flags & table_simplify[op].flag)) {
			do_simplify = simplify_noop;
		}
		if (do_simplify && (ins->id & TRIPLE_FLAG_VOLATILE)) {
			do_simplify = simplify_noop;
		}

		if (!do_simplify) {
			internal_error(state, ins, "cannot simplify op: %d %s",
				op, tops(op));
			return;
		}
		debug_simplify(state, do_simplify, ins);
	} while(ins->op != op);
}

static void rebuild_ssa_form(struct compile_state *state);

static void simplify_all(struct compile_state *state)
{
	struct triple *ins, *first;
	if (!(state->compiler->flags & COMPILER_SIMPLIFY)) {
		return;
	}
	first = state->first;
	ins = first->prev;
	do {
		simplify(state, ins);
		ins = ins->prev;
	} while(ins != first->prev);
	ins = first;
	do {
		simplify(state, ins);
		ins = ins->next;
	}while(ins != first);
	rebuild_ssa_form(state);

	print_blocks(state, __func__, state->dbgout);
}

/*
 * Builtins....
 * ============================
 */

static void register_builtin_function(struct compile_state *state,
	const char *name, int op, struct type *rtype, ...)
{
	struct type *ftype, *atype, *ctype, *crtype, *param, **next;
	struct triple *def, *result, *work, *first, *retvar, *ret;
	struct hash_entry *ident;
	struct file_state file;
	int parameters;
	int name_len;
	va_list args;
	int i;

	/* Dummy file state to get debug handling right */
	memset(&file, 0, sizeof(file));
	file.basename = "<built-in>";
	file.line = 1;
	file.report_line = 1;
	file.report_name = file.basename;
	file.prev = state->file;
	state->file = &file;
	state->function = name;

	/* Find the Parameter count */
	valid_op(state, op);
	parameters = table_ops[op].rhs;
	if (parameters < 0 ) {
		internal_error(state, 0, "Invalid builtin parameter count");
	}

	/* Find the function type */
	ftype = new_type(TYPE_FUNCTION | STOR_INLINE | STOR_STATIC, rtype, 0);
	ftype->elements = parameters;
	next = &ftype->right;
	va_start(args, rtype);
	for(i = 0; i < parameters; i++) {
		atype = va_arg(args, struct type *);
		if (!*next) {
			*next = atype;
		} else {
			*next = new_type(TYPE_PRODUCT, *next, atype);
			next = &((*next)->right);
		}
	}
	if (!*next) {
		*next = &void_type;
	}
	va_end(args);

	/* Get the initial closure type */
	ctype = new_type(TYPE_JOIN, &void_type, 0);
	ctype->elements = 1;

	/* Get the return type */
	crtype = new_type(TYPE_TUPLE, new_type(TYPE_PRODUCT, ctype, rtype), 0);
	crtype->elements = 2;

	/* Generate the needed triples */
	def = triple(state, OP_LIST, ftype, 0, 0);
	first = label(state);
	RHS(def, 0) = first;
	result = flatten(state, first, variable(state, crtype));
	retvar = flatten(state, first, variable(state, &void_ptr_type));
	ret = triple(state, OP_RET, &void_type, read_expr(state, retvar), 0);

	/* Now string them together */
	param = ftype->right;
	for(i = 0; i < parameters; i++) {
		if ((param->type & TYPE_MASK) == TYPE_PRODUCT) {
			atype = param->left;
		} else {
			atype = param;
		}
		flatten(state, first, variable(state, atype));
		param = param->right;
	}
	work = new_triple(state, op, rtype, -1, parameters);
	generate_lhs_pieces(state, work);
	for(i = 0; i < parameters; i++) {
		RHS(work, i) = read_expr(state, farg(state, def, i));
	}
	if ((rtype->type & TYPE_MASK) != TYPE_VOID) {
		work = write_expr(state, deref_index(state, result, 1), work);
	}
	flatten(state, first, work);
	flatten(state, first, label(state));
	flatten(state, first, ret);
	name_len = strlen(name);
	ident = lookup(state, name, name_len);
	ftype->type_ident = ident;
	symbol(state, ident, &ident->sym_ident, def, ftype);

	state->file = file.prev;
	state->function = 0;
	state->main_function = 0;

	if (!state->functions) {
		state->functions = def;
	} else {
		insert_triple(state, state->functions, def);
	}
	if (state->compiler->debug & DEBUG_INLINE) {
		FILE *fp = state->dbgout;
		fprintf(fp, "\n");
		loc(fp, state, 0);
		fprintf(fp, "\n__________ %s _________\n", __FUNCTION__);
		display_func(state, fp, def);
		fprintf(fp, "__________ %s _________ done\n\n", __FUNCTION__);
	}
}

static struct type *partial_struct(struct compile_state *state,
	const char *field_name, struct type *type, struct type *rest)
{
	struct hash_entry *field_ident;
	struct type *result;
	int field_name_len;

	field_name_len = strlen(field_name);
	field_ident = lookup(state, field_name, field_name_len);

	result = clone_type(0, type);
	result->field_ident = field_ident;

	if (rest) {
		result = new_type(TYPE_PRODUCT, result, rest);
	}
	return result;
}

static struct type *register_builtin_type(struct compile_state *state,
	const char *name, struct type *type)
{
	struct hash_entry *ident;
	int name_len;

	name_len = strlen(name);
	ident = lookup(state, name, name_len);

	if ((type->type & TYPE_MASK) == TYPE_PRODUCT) {
		ulong_t elements = 0;
		struct type *field;
		type = new_type(TYPE_STRUCT, type, 0);
		field = type->left;
		while((field->type & TYPE_MASK) == TYPE_PRODUCT) {
			elements++;
			field = field->right;
		}
		elements++;
		symbol(state, ident, &ident->sym_tag, 0, type);
		type->type_ident = ident;
		type->elements = elements;
	}
	symbol(state, ident, &ident->sym_ident, 0, type);
	ident->tok = TOK_TYPE_NAME;
	return type;
}


static void register_builtins(struct compile_state *state)
{
	struct type *div_type, *ldiv_type;
	struct type *udiv_type, *uldiv_type;
	struct type *msr_type;

	div_type = register_builtin_type(state, "__builtin_div_t",
		partial_struct(state, "quot", &int_type,
		partial_struct(state, "rem",  &int_type, 0)));
	ldiv_type = register_builtin_type(state, "__builtin_ldiv_t",
		partial_struct(state, "quot", &long_type,
		partial_struct(state, "rem",  &long_type, 0)));
	udiv_type = register_builtin_type(state, "__builtin_udiv_t",
		partial_struct(state, "quot", &uint_type,
		partial_struct(state, "rem",  &uint_type, 0)));
	uldiv_type = register_builtin_type(state, "__builtin_uldiv_t",
		partial_struct(state, "quot", &ulong_type,
		partial_struct(state, "rem",  &ulong_type, 0)));

	register_builtin_function(state, "__builtin_div",   OP_SDIVT, div_type,
		&int_type, &int_type);
	register_builtin_function(state, "__builtin_ldiv",  OP_SDIVT, ldiv_type,
		&long_type, &long_type);
	register_builtin_function(state, "__builtin_udiv",  OP_UDIVT, udiv_type,
		&uint_type, &uint_type);
	register_builtin_function(state, "__builtin_uldiv", OP_UDIVT, uldiv_type,
		&ulong_type, &ulong_type);

	register_builtin_function(state, "__builtin_inb", OP_INB, &uchar_type,
		&ushort_type);
	register_builtin_function(state, "__builtin_inw", OP_INW, &ushort_type,
		&ushort_type);
	register_builtin_function(state, "__builtin_inl", OP_INL, &uint_type,
		&ushort_type);

	register_builtin_function(state, "__builtin_outb", OP_OUTB, &void_type,
		&uchar_type, &ushort_type);
	register_builtin_function(state, "__builtin_outw", OP_OUTW, &void_type,
		&ushort_type, &ushort_type);
	register_builtin_function(state, "__builtin_outl", OP_OUTL, &void_type,
		&uint_type, &ushort_type);

	register_builtin_function(state, "__builtin_bsf", OP_BSF, &int_type,
		&int_type);
	register_builtin_function(state, "__builtin_bsr", OP_BSR, &int_type,
		&int_type);

	msr_type = register_builtin_type(state, "__builtin_msr_t",
		partial_struct(state, "lo", &ulong_type,
		partial_struct(state, "hi", &ulong_type, 0)));

	register_builtin_function(state, "__builtin_rdmsr", OP_RDMSR, msr_type,
		&ulong_type);
	register_builtin_function(state, "__builtin_wrmsr", OP_WRMSR, &void_type,
		&ulong_type, &ulong_type, &ulong_type);

	register_builtin_function(state, "__builtin_hlt", OP_HLT, &void_type,
		&void_type);
}

static struct type *declarator(
	struct compile_state *state, struct type *type,
	struct hash_entry **ident, int need_ident);
static void decl(struct compile_state *state, struct triple *first);
static struct type *specifier_qualifier_list(struct compile_state *state);
#if DEBUG_ROMCC_WARNING
static int isdecl_specifier(int tok);
#endif
static struct type *decl_specifiers(struct compile_state *state);
static int istype(int tok);
static struct triple *expr(struct compile_state *state);
static struct triple *assignment_expr(struct compile_state *state);
static struct type *type_name(struct compile_state *state);
static void statement(struct compile_state *state, struct triple *first);

static struct triple *call_expr(
	struct compile_state *state, struct triple *func)
{
	struct triple *def;
	struct type *param, *type;
	ulong_t pvals, index;

	if ((func->type->type & TYPE_MASK) != TYPE_FUNCTION) {
		error(state, 0, "Called object is not a function");
	}
	if (func->op != OP_LIST) {
		internal_error(state, 0, "improper function");
	}
	eat(state, TOK_LPAREN);
	/* Find the return type without any specifiers */
	type = clone_type(0, func->type->left);
	/* Count the number of rhs entries for OP_FCALL */
	param = func->type->right;
	pvals = 0;
	while((param->type & TYPE_MASK) == TYPE_PRODUCT) {
		pvals++;
		param = param->right;
	}
	if ((param->type & TYPE_MASK) != TYPE_VOID) {
		pvals++;
	}
	def = new_triple(state, OP_FCALL, type, -1, pvals);
	MISC(def, 0) = func;

	param = func->type->right;
	for(index = 0; index < pvals; index++) {
		struct triple *val;
		struct type *arg_type;
		val = read_expr(state, assignment_expr(state));
		arg_type = param;
		if ((param->type & TYPE_MASK) == TYPE_PRODUCT) {
			arg_type = param->left;
		}
		write_compatible(state, arg_type, val->type);
		RHS(def, index) = val;
		if (index != (pvals - 1)) {
			eat(state, TOK_COMMA);
			param = param->right;
		}
	}
	eat(state, TOK_RPAREN);
	return def;
}


static struct triple *character_constant(struct compile_state *state)
{
	struct triple *def;
	struct token *tk;
	const signed char *str, *end;
	int c;
	int str_len;
	tk = eat(state, TOK_LIT_CHAR);
	str = (signed char *)tk->val.str + 1;
	str_len = tk->str_len - 2;
	if (str_len <= 0) {
		error(state, 0, "empty character constant");
	}
	end = str + str_len;
	c = char_value(state, &str, end);
	if (str != end) {
		error(state, 0, "multibyte character constant not supported");
	}
	def = int_const(state, &char_type, (ulong_t)((long_t)c));
	return def;
}

static struct triple *string_constant(struct compile_state *state)
{
	struct triple *def;
	struct token *tk;
	struct type *type;
	const signed char *str, *end;
	signed char *buf, *ptr;
	int str_len;

	buf = 0;
	type = new_type(TYPE_ARRAY, &char_type, 0);
	type->elements = 0;
	/* The while loop handles string concatenation */
	do {
		tk = eat(state, TOK_LIT_STRING);
		str = (signed char *)tk->val.str + 1;
		str_len = tk->str_len - 2;
		if (str_len < 0) {
			error(state, 0, "negative string constant length");
		}
		/* ignore empty string tokens */
		if ('"' == *str && 0 == str[1])
			continue;
		end = str + str_len;
		ptr = buf;
		buf = xmalloc(type->elements + str_len + 1, "string_constant");
		memcpy(buf, ptr, type->elements);
		free(ptr);
		ptr = buf + type->elements;
		do {
			*ptr++ = char_value(state, &str, end);
		} while(str < end);
		type->elements = ptr - buf;
	} while(peek(state) == TOK_LIT_STRING);
	*ptr = '\0';
	type->elements += 1;
	def = triple(state, OP_BLOBCONST, type, 0, 0);
	def->u.blob = buf;

	return def;
}


static struct triple *integer_constant(struct compile_state *state)
{
	struct triple *def;
	unsigned long val;
	struct token *tk;
	char *end;
	int u, l, decimal;
	struct type *type;

	tk = eat(state, TOK_LIT_INT);
	errno = 0;
	decimal = (tk->val.str[0] != '0');
	val = strtoul(tk->val.str, &end, 0);
	if (errno == ERANGE) {
		error(state, 0, "Integer constant out of range");
	}
	u = l = 0;
	if ((*end == 'u') || (*end == 'U')) {
		u = 1;
			end++;
	}
	if ((*end == 'l') || (*end == 'L')) {
		l = 1;
		end++;
	}
	if ((*end == 'u') || (*end == 'U')) {
		u = 1;
		end++;
	}
	if (*end) {
		error(state, 0, "Junk at end of integer constant");
	}
	if (u && l)  {
		type = &ulong_type;
	}
	else if (l) {
		type = &long_type;
		if (!decimal && (val > LONG_T_MAX)) {
			type = &ulong_type;
		}
	}
	else if (u) {
		type = &uint_type;
		if (val > UINT_T_MAX) {
			type = &ulong_type;
		}
	}
	else {
		type = &int_type;
		if (!decimal && (val > INT_T_MAX) && (val <= UINT_T_MAX)) {
			type = &uint_type;
		}
		else if (!decimal && (val > LONG_T_MAX)) {
			type = &ulong_type;
		}
		else if (val > INT_T_MAX) {
			type = &long_type;
		}
	}
	def = int_const(state, type, val);
	return def;
}

static struct triple *primary_expr(struct compile_state *state)
{
	struct triple *def;
	int tok;
	tok = peek(state);
	switch(tok) {
	case TOK_IDENT:
	{
		struct hash_entry *ident;
		/* Here ident is either:
		 * a varable name
		 * a function name
		 */
		ident = eat(state, TOK_IDENT)->ident;
		if (!ident->sym_ident) {
			error(state, 0, "%s undeclared", ident->name);
		}
		def = ident->sym_ident->def;
		break;
	}
	case TOK_ENUM_CONST:
	{
		struct hash_entry *ident;
		/* Here ident is an enumeration constant */
		ident = eat(state, TOK_ENUM_CONST)->ident;
		if (!ident->sym_ident) {
			error(state, 0, "%s undeclared", ident->name);
		}
		def = ident->sym_ident->def;
		break;
	}
	case TOK_MIDENT:
	{
		struct hash_entry *ident;
		ident = eat(state, TOK_MIDENT)->ident;
		warning(state, 0, "Replacing undefined macro: %s with 0",
			ident->name);
		def = int_const(state, &int_type, 0);
		break;
	}
	case TOK_LPAREN:
		eat(state, TOK_LPAREN);
		def = expr(state);
		eat(state, TOK_RPAREN);
		break;
	case TOK_LIT_INT:
		def = integer_constant(state);
		break;
	case TOK_LIT_FLOAT:
		eat(state, TOK_LIT_FLOAT);
		error(state, 0, "Floating point constants not supported");
		def = 0;
		FINISHME();
		break;
	case TOK_LIT_CHAR:
		def = character_constant(state);
		break;
	case TOK_LIT_STRING:
		def = string_constant(state);
		break;
	default:
		def = 0;
		error(state, 0, "Unexpected token: %s\n", tokens[tok]);
	}
	return def;
}

static struct triple *postfix_expr(struct compile_state *state)
{
	struct triple *def;
	int postfix;
	def = primary_expr(state);
	do {
		struct triple *left;
		int tok;
		postfix = 1;
		left = def;
		switch((tok = peek(state))) {
		case TOK_LBRACKET:
			eat(state, TOK_LBRACKET);
			def = mk_subscript_expr(state, left, expr(state));
			eat(state, TOK_RBRACKET);
			break;
		case TOK_LPAREN:
			def = call_expr(state, def);
			break;
		case TOK_DOT:
		{
			struct hash_entry *field;
			eat(state, TOK_DOT);
			field = eat(state, TOK_IDENT)->ident;
			def = deref_field(state, def, field);
			break;
		}
		case TOK_ARROW:
		{
			struct hash_entry *field;
			eat(state, TOK_ARROW);
			field = eat(state, TOK_IDENT)->ident;
			def = mk_deref_expr(state, read_expr(state, def));
			def = deref_field(state, def, field);
			break;
		}
		case TOK_PLUSPLUS:
			eat(state, TOK_PLUSPLUS);
			def = mk_post_inc_expr(state, left);
			break;
		case TOK_MINUSMINUS:
			eat(state, TOK_MINUSMINUS);
			def = mk_post_dec_expr(state, left);
			break;
		default:
			postfix = 0;
			break;
		}
	} while(postfix);
	return def;
}

static struct triple *cast_expr(struct compile_state *state);

static struct triple *unary_expr(struct compile_state *state)
{
	struct triple *def, *right;
	int tok;
	switch((tok = peek(state))) {
	case TOK_PLUSPLUS:
		eat(state, TOK_PLUSPLUS);
		def = mk_pre_inc_expr(state, unary_expr(state));
		break;
	case TOK_MINUSMINUS:
		eat(state, TOK_MINUSMINUS);
		def = mk_pre_dec_expr(state, unary_expr(state));
		break;
	case TOK_AND:
		eat(state, TOK_AND);
		def = mk_addr_expr(state, cast_expr(state), 0);
		break;
	case TOK_STAR:
		eat(state, TOK_STAR);
		def = mk_deref_expr(state, read_expr(state, cast_expr(state)));
		break;
	case TOK_PLUS:
		eat(state, TOK_PLUS);
		right = read_expr(state, cast_expr(state));
		arithmetic(state, right);
		def = integral_promotion(state, right);
		break;
	case TOK_MINUS:
		eat(state, TOK_MINUS);
		right = read_expr(state, cast_expr(state));
		arithmetic(state, right);
		def = integral_promotion(state, right);
		def = triple(state, OP_NEG, def->type, def, 0);
		break;
	case TOK_TILDE:
		eat(state, TOK_TILDE);
		right = read_expr(state, cast_expr(state));
		integral(state, right);
		def = integral_promotion(state, right);
		def = triple(state, OP_INVERT, def->type, def, 0);
		break;
	case TOK_BANG:
		eat(state, TOK_BANG);
		right = read_expr(state, cast_expr(state));
		bool(state, right);
		def = lfalse_expr(state, right);
		break;
	case TOK_SIZEOF:
	{
		struct type *type;
		int tok1, tok2;
		eat(state, TOK_SIZEOF);
		tok1 = peek(state);
		tok2 = peek2(state);
		if ((tok1 == TOK_LPAREN) && istype(tok2)) {
			eat(state, TOK_LPAREN);
			type = type_name(state);
			eat(state, TOK_RPAREN);
		}
		else {
			struct triple *expr;
			expr = unary_expr(state);
			type = expr->type;
			release_expr(state, expr);
		}
		def = int_const(state, &ulong_type, size_of_in_bytes(state, type));
		break;
	}
	case TOK_ALIGNOF:
	{
		struct type *type;
		int tok1, tok2;
		eat(state, TOK_ALIGNOF);
		tok1 = peek(state);
		tok2 = peek2(state);
		if ((tok1 == TOK_LPAREN) && istype(tok2)) {
			eat(state, TOK_LPAREN);
			type = type_name(state);
			eat(state, TOK_RPAREN);
		}
		else {
			struct triple *expr;
			expr = unary_expr(state);
			type = expr->type;
			release_expr(state, expr);
		}
		def = int_const(state, &ulong_type, align_of_in_bytes(state, type));
		break;
	}
	case TOK_MDEFINED:
	{
		/* We only come here if we are called from the preprocessor */
		struct hash_entry *ident;
		int parens;
		eat(state, TOK_MDEFINED);
		parens = 0;
		if (pp_peek(state) == TOK_LPAREN) {
			pp_eat(state, TOK_LPAREN);
			parens = 1;
		}
		ident = pp_eat(state, TOK_MIDENT)->ident;
		if (parens) {
			eat(state, TOK_RPAREN);
		}
		def = int_const(state, &int_type, ident->sym_define != 0);
		break;
	}
	default:
		def = postfix_expr(state);
		break;
	}
	return def;
}

static struct triple *cast_expr(struct compile_state *state)
{
	struct triple *def;
	int tok1, tok2;
	tok1 = peek(state);
	tok2 = peek2(state);
	if ((tok1 == TOK_LPAREN) && istype(tok2)) {
		struct type *type;
		eat(state, TOK_LPAREN);
		type = type_name(state);
		eat(state, TOK_RPAREN);
		def = mk_cast_expr(state, type, cast_expr(state));
	}
	else {
		def = unary_expr(state);
	}
	return def;
}

static struct triple *mult_expr(struct compile_state *state)
{
	struct triple *def;
	int done;
	def = cast_expr(state);
	do {
		struct triple *left, *right;
		struct type *result_type;
		int tok, op, sign;
		done = 0;
		tok = peek(state);
		switch(tok) {
		case TOK_STAR:
		case TOK_DIV:
		case TOK_MOD:
			left = read_expr(state, def);
			arithmetic(state, left);

			eat(state, tok);

			right = read_expr(state, cast_expr(state));
			arithmetic(state, right);

			result_type = arithmetic_result(state, left, right);
			sign = is_signed(result_type);
			op = -1;
			switch(tok) {
			case TOK_STAR: op = sign? OP_SMUL : OP_UMUL; break;
			case TOK_DIV:  op = sign? OP_SDIV : OP_UDIV; break;
			case TOK_MOD:  op = sign? OP_SMOD : OP_UMOD; break;
			}
			def = triple(state, op, result_type, left, right);
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return def;
}

static struct triple *add_expr(struct compile_state *state)
{
	struct triple *def;
	int done;
	def = mult_expr(state);
	do {
		done = 0;
		switch( peek(state)) {
		case TOK_PLUS:
			eat(state, TOK_PLUS);
			def = mk_add_expr(state, def, mult_expr(state));
			break;
		case TOK_MINUS:
			eat(state, TOK_MINUS);
			def = mk_sub_expr(state, def, mult_expr(state));
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return def;
}

static struct triple *shift_expr(struct compile_state *state)
{
	struct triple *def;
	int done;
	def = add_expr(state);
	do {
		struct triple *left, *right;
		int tok, op;
		done = 0;
		switch((tok = peek(state))) {
		case TOK_SL:
		case TOK_SR:
			left = read_expr(state, def);
			integral(state, left);
			left = integral_promotion(state, left);

			eat(state, tok);

			right = read_expr(state, add_expr(state));
			integral(state, right);
			right = integral_promotion(state, right);

			op = (tok == TOK_SL)? OP_SL :
				is_signed(left->type)? OP_SSR: OP_USR;

			def = triple(state, op, left->type, left, right);
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return def;
}

static struct triple *relational_expr(struct compile_state *state)
{
#if DEBUG_ROMCC_WARNINGS
#warning "Extend relational exprs to work on more than arithmetic types"
#endif
	struct triple *def;
	int done;
	def = shift_expr(state);
	do {
		struct triple *left, *right;
		struct type *arg_type;
		int tok, op, sign;
		done = 0;
		switch((tok = peek(state))) {
		case TOK_LESS:
		case TOK_MORE:
		case TOK_LESSEQ:
		case TOK_MOREEQ:
			left = read_expr(state, def);
			arithmetic(state, left);

			eat(state, tok);

			right = read_expr(state, shift_expr(state));
			arithmetic(state, right);

			arg_type = arithmetic_result(state, left, right);
			sign = is_signed(arg_type);
			op = -1;
			switch(tok) {
			case TOK_LESS:   op = sign? OP_SLESS : OP_ULESS; break;
			case TOK_MORE:   op = sign? OP_SMORE : OP_UMORE; break;
			case TOK_LESSEQ: op = sign? OP_SLESSEQ : OP_ULESSEQ; break;
			case TOK_MOREEQ: op = sign? OP_SMOREEQ : OP_UMOREEQ; break;
			}
			def = triple(state, op, &int_type, left, right);
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return def;
}

static struct triple *equality_expr(struct compile_state *state)
{
#if DEBUG_ROMCC_WARNINGS
#warning "Extend equality exprs to work on more than arithmetic types"
#endif
	struct triple *def;
	int done;
	def = relational_expr(state);
	do {
		struct triple *left, *right;
		int tok, op;
		done = 0;
		switch((tok = peek(state))) {
		case TOK_EQEQ:
		case TOK_NOTEQ:
			left = read_expr(state, def);
			arithmetic(state, left);
			eat(state, tok);
			right = read_expr(state, relational_expr(state));
			arithmetic(state, right);
			op = (tok == TOK_EQEQ) ? OP_EQ: OP_NOTEQ;
			def = triple(state, op, &int_type, left, right);
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return def;
}

static struct triple *and_expr(struct compile_state *state)
{
	struct triple *def;
	def = equality_expr(state);
	while(peek(state) == TOK_AND) {
		struct triple *left, *right;
		struct type *result_type;
		left = read_expr(state, def);
		integral(state, left);
		eat(state, TOK_AND);
		right = read_expr(state, equality_expr(state));
		integral(state, right);
		result_type = arithmetic_result(state, left, right);
		def = triple(state, OP_AND, result_type, left, right);
	}
	return def;
}

static struct triple *xor_expr(struct compile_state *state)
{
	struct triple *def;
	def = and_expr(state);
	while(peek(state) == TOK_XOR) {
		struct triple *left, *right;
		struct type *result_type;
		left = read_expr(state, def);
		integral(state, left);
		eat(state, TOK_XOR);
		right = read_expr(state, and_expr(state));
		integral(state, right);
		result_type = arithmetic_result(state, left, right);
		def = triple(state, OP_XOR, result_type, left, right);
	}
	return def;
}

static struct triple *or_expr(struct compile_state *state)
{
	struct triple *def;
	def = xor_expr(state);
	while(peek(state) == TOK_OR) {
		struct triple *left, *right;
		struct type *result_type;
		left = read_expr(state, def);
		integral(state, left);
		eat(state, TOK_OR);
		right = read_expr(state, xor_expr(state));
		integral(state, right);
		result_type = arithmetic_result(state, left, right);
		def = triple(state, OP_OR, result_type, left, right);
	}
	return def;
}

static struct triple *land_expr(struct compile_state *state)
{
	struct triple *def;
	def = or_expr(state);
	while(peek(state) == TOK_LOGAND) {
		struct triple *left, *right;
		left = read_expr(state, def);
		bool(state, left);
		eat(state, TOK_LOGAND);
		right = read_expr(state, or_expr(state));
		bool(state, right);

		def = mkland_expr(state,
			ltrue_expr(state, left),
			ltrue_expr(state, right));
	}
	return def;
}

static struct triple *lor_expr(struct compile_state *state)
{
	struct triple *def;
	def = land_expr(state);
	while(peek(state) == TOK_LOGOR) {
		struct triple *left, *right;
		left = read_expr(state, def);
		bool(state, left);
		eat(state, TOK_LOGOR);
		right = read_expr(state, land_expr(state));
		bool(state, right);

		def = mklor_expr(state,
			ltrue_expr(state, left),
			ltrue_expr(state, right));
	}
	return def;
}

static struct triple *conditional_expr(struct compile_state *state)
{
	struct triple *def;
	def = lor_expr(state);
	if (peek(state) == TOK_QUEST) {
		struct triple *test, *left, *right;
		bool(state, def);
		test = ltrue_expr(state, read_expr(state, def));
		eat(state, TOK_QUEST);
		left = read_expr(state, expr(state));
		eat(state, TOK_COLON);
		right = read_expr(state, conditional_expr(state));

		def = mkcond_expr(state, test, left, right);
	}
	return def;
}

struct cv_triple {
	struct triple *val;
	int id;
};

static void set_cv(struct compile_state *state, struct cv_triple *cv,
	struct triple *dest, struct triple *val)
{
	if (cv[dest->id].val) {
		free_triple(state, cv[dest->id].val);
	}
	cv[dest->id].val = val;
}
static struct triple *get_cv(struct compile_state *state, struct cv_triple *cv,
	struct triple *src)
{
	return cv[src->id].val;
}

static struct triple *eval_const_expr(
	struct compile_state *state, struct triple *expr)
{
	struct triple *def;
	if (is_const(expr)) {
		def = expr;
	}
	else {
		/* If we don't start out as a constant simplify into one */
		struct triple *head, *ptr;
		struct cv_triple *cv;
		int i, count;
		head = label(state); /* dummy initial triple */
		flatten(state, head, expr);
		count = 1;
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			count++;
		}
		cv = xcmalloc(sizeof(struct cv_triple)*count, "const value vector");
		i = 1;
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			cv[i].val = 0;
			cv[i].id  = ptr->id;
			ptr->id   = i;
			i++;
		}
		ptr = head->next;
		do {
			valid_ins(state, ptr);
			if ((ptr->op == OP_PHI) || (ptr->op == OP_LIST)) {
				internal_error(state, ptr,
					"unexpected %s in constant expression",
					tops(ptr->op));
			}
			else if (ptr->op == OP_LIST) {
			}
			else if (triple_is_structural(state, ptr)) {
				ptr = ptr->next;
			}
			else if (triple_is_ubranch(state, ptr)) {
				ptr = TARG(ptr, 0);
			}
			else if (triple_is_cbranch(state, ptr)) {
				struct triple *cond_val;
				cond_val = get_cv(state, cv, RHS(ptr, 0));
				if (!cond_val || !is_const(cond_val) ||
					(cond_val->op != OP_INTCONST))
				{
					internal_error(state, ptr, "bad branch condition");
				}
				if (cond_val->u.cval == 0) {
					ptr = ptr->next;
				} else {
					ptr = TARG(ptr, 0);
				}
			}
			else if (triple_is_branch(state, ptr)) {
				error(state, ptr, "bad branch type in constant expression");
			}
			else if (ptr->op == OP_WRITE) {
				struct triple *val;
				val = get_cv(state, cv, RHS(ptr, 0));

				set_cv(state, cv, MISC(ptr, 0),
					copy_triple(state, val));
				set_cv(state, cv, ptr,
					copy_triple(state, val));
				ptr = ptr->next;
			}
			else if (ptr->op == OP_READ) {
				set_cv(state, cv, ptr,
					copy_triple(state,
						get_cv(state, cv, RHS(ptr, 0))));
				ptr = ptr->next;
			}
			else if (triple_is_pure(state, ptr, cv[ptr->id].id)) {
				struct triple *val, **rhs;
				val = copy_triple(state, ptr);
				rhs = triple_rhs(state, val, 0);
				for(; rhs; rhs = triple_rhs(state, val, rhs)) {
					if (!*rhs) {
						internal_error(state, ptr, "Missing rhs");
					}
					*rhs = get_cv(state, cv, *rhs);
				}
				simplify(state, val);
				set_cv(state, cv, ptr, val);
				ptr = ptr->next;
			}
			else {
				error(state, ptr, "impure operation in constant expression");
			}

		} while(ptr != head);

		/* Get the result value */
		def = get_cv(state, cv, head->prev);
		cv[head->prev->id].val = 0;

		/* Free the temporary values */
		for(i = 0; i < count; i++) {
			if (cv[i].val) {
				free_triple(state, cv[i].val);
				cv[i].val = 0;
			}
		}
		xfree(cv);
		/* Free the intermediate expressions */
		while(head->next != head) {
			release_triple(state, head->next);
		}
		free_triple(state, head);
	}
	if (!is_const(def)) {
		error(state, expr, "Not a constant expression");
	}
	return def;
}

static struct triple *constant_expr(struct compile_state *state)
{
	return eval_const_expr(state, conditional_expr(state));
}

static struct triple *assignment_expr(struct compile_state *state)
{
	struct triple *def, *left, *right;
	int tok, op, sign;
	/* The C grammer in K&R shows assignment expressions
	 * only taking unary expressions as input on their
	 * left hand side.  But specifies the precedence of
	 * assignemnt as the lowest operator except for comma.
	 *
	 * Allowing conditional expressions on the left hand side
	 * of an assignement results in a grammar that accepts
	 * a larger set of statements than standard C.   As long
	 * as the subset of the grammar that is standard C behaves
	 * correctly this should cause no problems.
	 *
	 * For the extra token strings accepted by the grammar
	 * none of them should produce a valid lvalue, so they
	 * should not produce functioning programs.
	 *
	 * GCC has this bug as well, so surprises should be minimal.
	 */
	def = conditional_expr(state);
	left = def;
	switch((tok = peek(state))) {
	case TOK_EQ:
		lvalue(state, left);
		eat(state, TOK_EQ);
		def = write_expr(state, left,
			read_expr(state, assignment_expr(state)));
		break;
	case TOK_TIMESEQ:
	case TOK_DIVEQ:
	case TOK_MODEQ:
		lvalue(state, left);
		arithmetic(state, left);
		eat(state, tok);
		right = read_expr(state, assignment_expr(state));
		arithmetic(state, right);

		sign = is_signed(left->type);
		op = -1;
		switch(tok) {
		case TOK_TIMESEQ: op = sign? OP_SMUL : OP_UMUL; break;
		case TOK_DIVEQ:   op = sign? OP_SDIV : OP_UDIV; break;
		case TOK_MODEQ:   op = sign? OP_SMOD : OP_UMOD; break;
		}
		def = write_expr(state, left,
			triple(state, op, left->type,
				read_expr(state, left), right));
		break;
	case TOK_PLUSEQ:
		lvalue(state, left);
		eat(state, TOK_PLUSEQ);
		def = write_expr(state, left,
			mk_add_expr(state, left, assignment_expr(state)));
		break;
	case TOK_MINUSEQ:
		lvalue(state, left);
		eat(state, TOK_MINUSEQ);
		def = write_expr(state, left,
			mk_sub_expr(state, left, assignment_expr(state)));
		break;
	case TOK_SLEQ:
	case TOK_SREQ:
	case TOK_ANDEQ:
	case TOK_XOREQ:
	case TOK_OREQ:
		lvalue(state, left);
		integral(state, left);
		eat(state, tok);
		right = read_expr(state, assignment_expr(state));
		integral(state, right);
		right = integral_promotion(state, right);
		sign = is_signed(left->type);
		op = -1;
		switch(tok) {
		case TOK_SLEQ:  op = OP_SL; break;
		case TOK_SREQ:  op = sign? OP_SSR: OP_USR; break;
		case TOK_ANDEQ: op = OP_AND; break;
		case TOK_XOREQ: op = OP_XOR; break;
		case TOK_OREQ:  op = OP_OR; break;
		}
		def = write_expr(state, left,
			triple(state, op, left->type,
				read_expr(state, left), right));
		break;
	}
	return def;
}

static struct triple *expr(struct compile_state *state)
{
	struct triple *def;
	def = assignment_expr(state);
	while(peek(state) == TOK_COMMA) {
		eat(state, TOK_COMMA);
		def = mkprog(state, def, assignment_expr(state), 0UL);
	}
	return def;
}

static void expr_statement(struct compile_state *state, struct triple *first)
{
	if (peek(state) != TOK_SEMI) {
		/* lvalue conversions always apply except when certian operators
		 * are applied.  I apply the lvalue conversions here
		 * as I know no more operators will be applied.
		 */
		flatten(state, first, lvalue_conversion(state, expr(state)));
	}
	eat(state, TOK_SEMI);
}

static void if_statement(struct compile_state *state, struct triple *first)
{
	struct triple *test, *jmp1, *jmp2, *middle, *end;

	jmp1 = jmp2 = middle = 0;
	eat(state, TOK_IF);
	eat(state, TOK_LPAREN);
	test = expr(state);
	bool(state, test);
	/* Cleanup and invert the test */
	test = lfalse_expr(state, read_expr(state, test));
	eat(state, TOK_RPAREN);
	/* Generate the needed pieces */
	middle = label(state);
	jmp1 = branch(state, middle, test);
	/* Thread the pieces together */
	flatten(state, first, test);
	flatten(state, first, jmp1);
	flatten(state, first, label(state));
	statement(state, first);
	if (peek(state) == TOK_ELSE) {
		eat(state, TOK_ELSE);
		/* Generate the rest of the pieces */
		end = label(state);
		jmp2 = branch(state, end, 0);
		/* Thread them together */
		flatten(state, first, jmp2);
		flatten(state, first, middle);
		statement(state, first);
		flatten(state, first, end);
	}
	else {
		flatten(state, first, middle);
	}
}

static void for_statement(struct compile_state *state, struct triple *first)
{
	struct triple *head, *test, *tail, *jmp1, *jmp2, *end;
	struct triple *label1, *label2, *label3;
	struct hash_entry *ident;

	eat(state, TOK_FOR);
	eat(state, TOK_LPAREN);
	head = test = tail = jmp1 = jmp2 = 0;
	if (peek(state) != TOK_SEMI) {
		head = expr(state);
	}
	eat(state, TOK_SEMI);
	if (peek(state) != TOK_SEMI) {
		test = expr(state);
		bool(state, test);
		test = ltrue_expr(state, read_expr(state, test));
	}
	eat(state, TOK_SEMI);
	if (peek(state) != TOK_RPAREN) {
		tail = expr(state);
	}
	eat(state, TOK_RPAREN);
	/* Generate the needed pieces */
	label1 = label(state);
	label2 = label(state);
	label3 = label(state);
	if (test) {
		jmp1 = branch(state, label3, 0);
		jmp2 = branch(state, label1, test);
	}
	else {
		jmp2 = branch(state, label1, 0);
	}
	end = label(state);
	/* Remember where break and continue go */
	start_scope(state);
	ident = state->i_break;
	symbol(state, ident, &ident->sym_ident, end, end->type);
	ident = state->i_continue;
	symbol(state, ident, &ident->sym_ident, label2, label2->type);
	/* Now include the body */
	flatten(state, first, head);
	flatten(state, first, jmp1);
	flatten(state, first, label1);
	statement(state, first);
	flatten(state, first, label2);
	flatten(state, first, tail);
	flatten(state, first, label3);
	flatten(state, first, test);
	flatten(state, first, jmp2);
	flatten(state, first, end);
	/* Cleanup the break/continue scope */
	end_scope(state);
}

static void while_statement(struct compile_state *state, struct triple *first)
{
	struct triple *label1, *test, *label2, *jmp1, *jmp2, *end;
	struct hash_entry *ident;
	eat(state, TOK_WHILE);
	eat(state, TOK_LPAREN);
	test = expr(state);
	bool(state, test);
	test = ltrue_expr(state, read_expr(state, test));
	eat(state, TOK_RPAREN);
	/* Generate the needed pieces */
	label1 = label(state);
	label2 = label(state);
	jmp1 = branch(state, label2, 0);
	jmp2 = branch(state, label1, test);
	end = label(state);
	/* Remember where break and continue go */
	start_scope(state);
	ident = state->i_break;
	symbol(state, ident, &ident->sym_ident, end, end->type);
	ident = state->i_continue;
	symbol(state, ident, &ident->sym_ident, label2, label2->type);
	/* Thread them together */
	flatten(state, first, jmp1);
	flatten(state, first, label1);
	statement(state, first);
	flatten(state, first, label2);
	flatten(state, first, test);
	flatten(state, first, jmp2);
	flatten(state, first, end);
	/* Cleanup the break/continue scope */
	end_scope(state);
}

static void do_statement(struct compile_state *state, struct triple *first)
{
	struct triple *label1, *label2, *test, *end;
	struct hash_entry *ident;
	eat(state, TOK_DO);
	/* Generate the needed pieces */
	label1 = label(state);
	label2 = label(state);
	end = label(state);
	/* Remember where break and continue go */
	start_scope(state);
	ident = state->i_break;
	symbol(state, ident, &ident->sym_ident, end, end->type);
	ident = state->i_continue;
	symbol(state, ident, &ident->sym_ident, label2, label2->type);
	/* Now include the body */
	flatten(state, first, label1);
	statement(state, first);
	/* Cleanup the break/continue scope */
	end_scope(state);
	/* Eat the rest of the loop */
	eat(state, TOK_WHILE);
	eat(state, TOK_LPAREN);
	test = read_expr(state, expr(state));
	bool(state, test);
	eat(state, TOK_RPAREN);
	eat(state, TOK_SEMI);
	/* Thread the pieces together */
	test = ltrue_expr(state, test);
	flatten(state, first, label2);
	flatten(state, first, test);
	flatten(state, first, branch(state, label1, test));
	flatten(state, first, end);
}


static void return_statement(struct compile_state *state, struct triple *first)
{
	struct triple *jmp, *mv, *dest, *var, *val;
	int last;
	eat(state, TOK_RETURN);

#if DEBUG_ROMCC_WARNINGS
#warning "FIXME implement a more general excess branch elimination"
#endif
	val = 0;
	/* If we have a return value do some more work */
	if (peek(state) != TOK_SEMI) {
		val = read_expr(state, expr(state));
	}
	eat(state, TOK_SEMI);

	/* See if this last statement in a function */
	last = ((peek(state) == TOK_RBRACE) &&
		(state->scope_depth == GLOBAL_SCOPE_DEPTH +2));

	/* Find the return variable */
	var = fresult(state, state->main_function);

	/* Find the return destination */
	dest = state->i_return->sym_ident->def;
	mv = jmp = 0;
	/* If needed generate a jump instruction */
	if (!last) {
		jmp = branch(state, dest, 0);
	}
	/* If needed generate an assignment instruction */
	if (val) {
		mv = write_expr(state, deref_index(state, var, 1), val);
	}
	/* Now put the code together */
	if (mv) {
		flatten(state, first, mv);
		flatten(state, first, jmp);
	}
	else if (jmp) {
		flatten(state, first, jmp);
	}
}

static void break_statement(struct compile_state *state, struct triple *first)
{
	struct triple *dest;
	eat(state, TOK_BREAK);
	eat(state, TOK_SEMI);
	if (!state->i_break->sym_ident) {
		error(state, 0, "break statement not within loop or switch");
	}
	dest = state->i_break->sym_ident->def;
	flatten(state, first, branch(state, dest, 0));
}

static void continue_statement(struct compile_state *state, struct triple *first)
{
	struct triple *dest;
	eat(state, TOK_CONTINUE);
	eat(state, TOK_SEMI);
	if (!state->i_continue->sym_ident) {
		error(state, 0, "continue statement outside of a loop");
	}
	dest = state->i_continue->sym_ident->def;
	flatten(state, first, branch(state, dest, 0));
}

static void goto_statement(struct compile_state *state, struct triple *first)
{
	struct hash_entry *ident;
	eat(state, TOK_GOTO);
	ident = eat(state, TOK_IDENT)->ident;
	if (!ident->sym_label) {
		/* If this is a forward branch allocate the label now,
		 * it will be flattend in the appropriate location later.
		 */
		struct triple *ins;
		ins = label(state);
		label_symbol(state, ident, ins, FUNCTION_SCOPE_DEPTH);
 	}
	eat(state, TOK_SEMI);

	flatten(state, first, branch(state, ident->sym_label->def, 0));
}

static void labeled_statement(struct compile_state *state, struct triple *first)
{
	struct triple *ins;
	struct hash_entry *ident;

	ident = eat(state, TOK_IDENT)->ident;
	if (ident->sym_label && ident->sym_label->def) {
		ins = ident->sym_label->def;
		put_occurrence(ins->occurrence);
		ins->occurrence = new_occurrence(state);
	}
	else {
		ins = label(state);
		label_symbol(state, ident, ins, FUNCTION_SCOPE_DEPTH);
	}
	if (ins->id & TRIPLE_FLAG_FLATTENED) {
		error(state, 0, "label %s already defined", ident->name);
	}
	flatten(state, first, ins);

	eat(state, TOK_COLON);
	statement(state, first);
}

static void switch_statement(struct compile_state *state, struct triple *first)
{
	struct triple *value, *top, *end, *dbranch;
	struct hash_entry *ident;

	/* See if we have a valid switch statement */
	eat(state, TOK_SWITCH);
	eat(state, TOK_LPAREN);
	value = expr(state);
	integral(state, value);
	value = read_expr(state, value);
	eat(state, TOK_RPAREN);
	/* Generate the needed pieces */
	top = label(state);
	end = label(state);
	dbranch = branch(state, end, 0);
	/* Remember where case branches and break goes */
	start_scope(state);
	ident = state->i_switch;
	symbol(state, ident, &ident->sym_ident, value, value->type);
	ident = state->i_case;
	symbol(state, ident, &ident->sym_ident, top, top->type);
	ident = state->i_break;
	symbol(state, ident, &ident->sym_ident, end, end->type);
	ident = state->i_default;
	symbol(state, ident, &ident->sym_ident, dbranch, dbranch->type);
	/* Thread them together */
	flatten(state, first, value);
	flatten(state, first, top);
	flatten(state, first, dbranch);
	statement(state, first);
	flatten(state, first, end);
	/* Cleanup the switch scope */
	end_scope(state);
}

static void case_statement(struct compile_state *state, struct triple *first)
{
	struct triple *cvalue, *dest, *test, *jmp;
	struct triple *ptr, *value, *top, *dbranch;

	/* See if w have a valid case statement */
	eat(state, TOK_CASE);
	cvalue = constant_expr(state);
	integral(state, cvalue);
	if (cvalue->op != OP_INTCONST) {
		error(state, 0, "integer constant expected");
	}
	eat(state, TOK_COLON);
	if (!state->i_case->sym_ident) {
		error(state, 0, "case statement not within a switch");
	}

	/* Lookup the interesting pieces */
	top = state->i_case->sym_ident->def;
	value = state->i_switch->sym_ident->def;
	dbranch = state->i_default->sym_ident->def;

	/* See if this case label has already been used */
	for(ptr = top; ptr != dbranch; ptr = ptr->next) {
		if (ptr->op != OP_EQ) {
			continue;
		}
		if (RHS(ptr, 1)->u.cval == cvalue->u.cval) {
			error(state, 0, "duplicate case %d statement",
				cvalue->u.cval);
		}
	}
	/* Generate the needed pieces */
	dest = label(state);
	test = triple(state, OP_EQ, &int_type, value, cvalue);
	jmp = branch(state, dest, test);
	/* Thread the pieces together */
	flatten(state, dbranch, test);
	flatten(state, dbranch, jmp);
	flatten(state, dbranch, label(state));
	flatten(state, first, dest);
	statement(state, first);
}

static void default_statement(struct compile_state *state, struct triple *first)
{
	struct triple *dest;
	struct triple *dbranch, *end;

	/* See if we have a valid default statement */
	eat(state, TOK_DEFAULT);
	eat(state, TOK_COLON);

	if (!state->i_case->sym_ident) {
		error(state, 0, "default statement not within a switch");
	}

	/* Lookup the interesting pieces */
	dbranch = state->i_default->sym_ident->def;
	end = state->i_break->sym_ident->def;

	/* See if a default statement has already happened */
	if (TARG(dbranch, 0) != end) {
		error(state, 0, "duplicate default statement");
	}

	/* Generate the needed pieces */
	dest = label(state);

	/* Blame the branch on the default statement */
	put_occurrence(dbranch->occurrence);
	dbranch->occurrence = new_occurrence(state);

	/* Thread the pieces together */
	TARG(dbranch, 0) = dest;
	use_triple(dest, dbranch);
	flatten(state, first, dest);
	statement(state, first);
}

static void asm_statement(struct compile_state *state, struct triple *first)
{
	struct asm_info *info;
	struct {
		struct triple *constraint;
		struct triple *expr;
	} out_param[MAX_LHS], in_param[MAX_RHS], clob_param[MAX_LHS];
	struct triple *def, *asm_str;
	int out, in, clobbers, more, colons, i;
	int flags;

	flags = 0;
	eat(state, TOK_ASM);
	/* For now ignore the qualifiers */
	switch(peek(state)) {
	case TOK_CONST:
		eat(state, TOK_CONST);
		break;
	case TOK_VOLATILE:
		eat(state, TOK_VOLATILE);
		flags |= TRIPLE_FLAG_VOLATILE;
		break;
	}
	eat(state, TOK_LPAREN);
	asm_str = string_constant(state);

	colons = 0;
	out = in = clobbers = 0;
	/* Outputs */
	if ((colons == 0) && (peek(state) == TOK_COLON)) {
		eat(state, TOK_COLON);
		colons++;
		more = (peek(state) == TOK_LIT_STRING);
		while(more) {
			struct triple *var;
			struct triple *constraint;
			char *str;
			more = 0;
			if (out > MAX_LHS) {
				error(state, 0, "Maximum output count exceeded.");
			}
			constraint = string_constant(state);
			str = constraint->u.blob;
			if (str[0] != '=') {
				error(state, 0, "Output constraint does not start with =");
			}
			constraint->u.blob = str + 1;
			eat(state, TOK_LPAREN);
			var = conditional_expr(state);
			eat(state, TOK_RPAREN);

			lvalue(state, var);
			out_param[out].constraint = constraint;
			out_param[out].expr       = var;
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
				more = 1;
			}
			out++;
		}
	}
	/* Inputs */
	if ((colons == 1) && (peek(state) == TOK_COLON)) {
		eat(state, TOK_COLON);
		colons++;
		more = (peek(state) == TOK_LIT_STRING);
		while(more) {
			struct triple *val;
			struct triple *constraint;
			char *str;
			more = 0;
			if (in > MAX_RHS) {
				error(state, 0, "Maximum input count exceeded.");
			}
			constraint = string_constant(state);
			str = constraint->u.blob;
			if (digitp(str[0] && str[1] == '\0')) {
				int val;
				val = digval(str[0]);
				if ((val < 0) || (val >= out)) {
					error(state, 0, "Invalid input constraint %d", val);
				}
			}
			eat(state, TOK_LPAREN);
			val = conditional_expr(state);
			eat(state, TOK_RPAREN);

			in_param[in].constraint = constraint;
			in_param[in].expr       = val;
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
				more = 1;
			}
			in++;
		}
	}

	/* Clobber */
	if ((colons == 2) && (peek(state) == TOK_COLON)) {
		eat(state, TOK_COLON);
		colons++;
		more = (peek(state) == TOK_LIT_STRING);
		while(more) {
			struct triple *clobber;
			more = 0;
			if ((clobbers + out) > MAX_LHS) {
				error(state, 0, "Maximum clobber limit exceeded.");
			}
			clobber = string_constant(state);

			clob_param[clobbers].constraint = clobber;
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
				more = 1;
			}
			clobbers++;
		}
	}
	eat(state, TOK_RPAREN);
	eat(state, TOK_SEMI);


	info = xcmalloc(sizeof(*info), "asm_info");
	info->str = asm_str->u.blob;
	free_triple(state, asm_str);

	def = new_triple(state, OP_ASM, &void_type, clobbers + out, in);
	def->u.ainfo = info;
	def->id |= flags;

	/* Find the register constraints */
	for(i = 0; i < out; i++) {
		struct triple *constraint;
		constraint = out_param[i].constraint;
		info->tmpl.lhs[i] = arch_reg_constraint(state,
			out_param[i].expr->type, constraint->u.blob);
		free_triple(state, constraint);
	}
	for(; i - out < clobbers; i++) {
		struct triple *constraint;
		constraint = clob_param[i - out].constraint;
		info->tmpl.lhs[i] = arch_reg_clobber(state, constraint->u.blob);
		free_triple(state, constraint);
	}
	for(i = 0; i < in; i++) {
		struct triple *constraint;
		const char *str;
		constraint = in_param[i].constraint;
		str = constraint->u.blob;
		if (digitp(str[0]) && str[1] == '\0') {
			struct reg_info cinfo;
			int val;
			val = digval(str[0]);
			cinfo.reg = info->tmpl.lhs[val].reg;
			cinfo.regcm = arch_type_to_regcm(state, in_param[i].expr->type);
			cinfo.regcm &= info->tmpl.lhs[val].regcm;
			if (cinfo.reg == REG_UNSET) {
				cinfo.reg = REG_VIRT0 + val;
			}
			if (cinfo.regcm == 0) {
				error(state, 0, "No registers for %d", val);
			}
			info->tmpl.lhs[val] = cinfo;
			info->tmpl.rhs[i]   = cinfo;

		} else {
			info->tmpl.rhs[i] = arch_reg_constraint(state,
				in_param[i].expr->type, str);
		}
		free_triple(state, constraint);
	}

	/* Now build the helper expressions */
	for(i = 0; i < in; i++) {
		RHS(def, i) = read_expr(state, in_param[i].expr);
	}
	flatten(state, first, def);
	for(i = 0; i < (out + clobbers); i++) {
		struct type *type;
		struct triple *piece;
		if (i < out) {
			type = out_param[i].expr->type;
		} else {
			size_t size = arch_reg_size(info->tmpl.lhs[i].reg);
			if (size >= SIZEOF_LONG) {
				type = &ulong_type;
			}
			else if (size >= SIZEOF_INT) {
				type = &uint_type;
			}
			else if (size >= SIZEOF_SHORT) {
				type = &ushort_type;
			}
			else {
				type = &uchar_type;
			}
		}
		piece = triple(state, OP_PIECE, type, def, 0);
		piece->u.cval = i;
		LHS(def, i) = piece;
		flatten(state, first, piece);
	}
	/* And write the helpers to their destinations */
	for(i = 0; i < out; i++) {
		struct triple *piece;
		piece = LHS(def, i);
		flatten(state, first,
			write_expr(state, out_param[i].expr, piece));
	}
}


static int isdecl(int tok)
{
	switch(tok) {
	case TOK_AUTO:
	case TOK_REGISTER:
	case TOK_STATIC:
	case TOK_EXTERN:
	case TOK_TYPEDEF:
	case TOK_CONST:
	case TOK_RESTRICT:
	case TOK_VOLATILE:
	case TOK_VOID:
	case TOK_CHAR:
	case TOK_SHORT:
	case TOK_INT:
	case TOK_LONG:
	case TOK_FLOAT:
	case TOK_DOUBLE:
	case TOK_SIGNED:
	case TOK_UNSIGNED:
	case TOK_STRUCT:
	case TOK_UNION:
	case TOK_ENUM:
	case TOK_TYPE_NAME: /* typedef name */
		return 1;
	default:
		return 0;
	}
}

static void compound_statement(struct compile_state *state, struct triple *first)
{
	eat(state, TOK_LBRACE);
	start_scope(state);

	/* statement-list opt */
	while (peek(state) != TOK_RBRACE) {
		statement(state, first);
	}
	end_scope(state);
	eat(state, TOK_RBRACE);
}

static void statement(struct compile_state *state, struct triple *first)
{
	int tok;
	tok = peek(state);
	if (tok == TOK_LBRACE) {
		compound_statement(state, first);
	}
	else if (tok == TOK_IF) {
		if_statement(state, first);
	}
	else if (tok == TOK_FOR) {
		for_statement(state, first);
	}
	else if (tok == TOK_WHILE) {
		while_statement(state, first);
	}
	else if (tok == TOK_DO) {
		do_statement(state, first);
	}
	else if (tok == TOK_RETURN) {
		return_statement(state, first);
	}
	else if (tok == TOK_BREAK) {
		break_statement(state, first);
	}
	else if (tok == TOK_CONTINUE) {
		continue_statement(state, first);
	}
	else if (tok == TOK_GOTO) {
		goto_statement(state, first);
	}
	else if (tok == TOK_SWITCH) {
		switch_statement(state, first);
	}
	else if (tok == TOK_ASM) {
		asm_statement(state, first);
	}
	else if ((tok == TOK_IDENT) && (peek2(state) == TOK_COLON)) {
		labeled_statement(state, first);
	}
	else if (tok == TOK_CASE) {
 		case_statement(state, first);
	}
	else if (tok == TOK_DEFAULT) {
		default_statement(state, first);
	}
	else if (isdecl(tok)) {
		/* This handles C99 intermixing of statements and decls */
		decl(state, first);
	}
	else {
		expr_statement(state, first);
	}
}

static struct type *param_decl(struct compile_state *state)
{
	struct type *type;
	struct hash_entry *ident;
	/* Cheat so the declarator will know we are not global */
	start_scope(state);
	ident = 0;
	type = decl_specifiers(state);
	type = declarator(state, type, &ident, 0);
	type->field_ident = ident;
	end_scope(state);
	return type;
}

static struct type *param_type_list(struct compile_state *state, struct type *type)
{
	struct type *ftype, **next;
	ftype = new_type(TYPE_FUNCTION | (type->type & STOR_MASK), type, param_decl(state));
	next = &ftype->right;
	ftype->elements = 1;
	while(peek(state) == TOK_COMMA) {
		eat(state, TOK_COMMA);
		if (peek(state) == TOK_DOTS) {
			eat(state, TOK_DOTS);
			error(state, 0, "variadic functions not supported");
		}
		else {
			*next = new_type(TYPE_PRODUCT, *next, param_decl(state));
			next = &((*next)->right);
			ftype->elements++;
		}
	}
	return ftype;
}

static struct type *type_name(struct compile_state *state)
{
	struct type *type;
	type = specifier_qualifier_list(state);
	/* abstract-declarator (may consume no tokens) */
	type = declarator(state, type, 0, 0);
	return type;
}

static struct type *direct_declarator(
	struct compile_state *state, struct type *type,
	struct hash_entry **pident, int need_ident)
{
	struct hash_entry *ident;
	struct type *outer;
	int op;
	outer = 0;
	arrays_complete(state, type);
	switch(peek(state)) {
	case TOK_IDENT:
		ident = eat(state, TOK_IDENT)->ident;
		if (!ident) {
			error(state, 0, "Unexpected identifier found");
		}
		/* The name of what we are declaring */
		*pident = ident;
		break;
	case TOK_LPAREN:
		eat(state, TOK_LPAREN);
		outer = declarator(state, type, pident, need_ident);
		eat(state, TOK_RPAREN);
		break;
	default:
		if (need_ident) {
			error(state, 0, "Identifier expected");
		}
		break;
	}
	do {
		op = 1;
		arrays_complete(state, type);
		switch(peek(state)) {
		case TOK_LPAREN:
			eat(state, TOK_LPAREN);
			type = param_type_list(state, type);
			eat(state, TOK_RPAREN);
			break;
		case TOK_LBRACKET:
		{
			unsigned int qualifiers;
			struct triple *value;
			value = 0;
			eat(state, TOK_LBRACKET);
			if (peek(state) != TOK_RBRACKET) {
				value = constant_expr(state);
				integral(state, value);
			}
			eat(state, TOK_RBRACKET);

			qualifiers = type->type & (QUAL_MASK | STOR_MASK);
			type = new_type(TYPE_ARRAY | qualifiers, type, 0);
			if (value) {
				type->elements = value->u.cval;
				free_triple(state, value);
			} else {
				type->elements = ELEMENT_COUNT_UNSPECIFIED;
				op = 0;
			}
		}
			break;
		default:
			op = 0;
			break;
		}
	} while(op);
	if (outer) {
		struct type *inner;
		arrays_complete(state, type);
		FINISHME();
		for(inner = outer; inner->left; inner = inner->left)
			;
		inner->left = type;
		type = outer;
	}
	return type;
}

static struct type *declarator(
	struct compile_state *state, struct type *type,
	struct hash_entry **pident, int need_ident)
{
	while(peek(state) == TOK_STAR) {
		eat(state, TOK_STAR);
		type = new_type(TYPE_POINTER | (type->type & STOR_MASK), type, 0);
	}
	type = direct_declarator(state, type, pident, need_ident);
	return type;
}

static struct type *typedef_name(
	struct compile_state *state, unsigned int specifiers)
{
	struct hash_entry *ident;
	struct type *type;
	ident = eat(state, TOK_TYPE_NAME)->ident;
	type = ident->sym_ident->type;
	specifiers |= type->type & QUAL_MASK;
	if ((specifiers & (STOR_MASK | QUAL_MASK)) !=
		(type->type & (STOR_MASK | QUAL_MASK))) {
		type = clone_type(specifiers, type);
	}
	return type;
}

static struct type *enum_specifier(
	struct compile_state *state, unsigned int spec)
{
	struct hash_entry *ident;
	ulong_t base;
	int tok;
	struct type *enum_type;
	enum_type = 0;
	ident = 0;
	eat(state, TOK_ENUM);
	tok = peek(state);
	if ((tok == TOK_IDENT) || (tok == TOK_ENUM_CONST) || (tok == TOK_TYPE_NAME)) {
		ident = eat(state, tok)->ident;
	}
	base = 0;
	if (!ident || (peek(state) == TOK_LBRACE)) {
		struct type **next;
		eat(state, TOK_LBRACE);
		enum_type = new_type(TYPE_ENUM | spec, 0, 0);
		enum_type->type_ident = ident;
		next = &enum_type->right;
		do {
			struct hash_entry *eident;
			struct triple *value;
			struct type *entry;
			eident = eat(state, TOK_IDENT)->ident;
			if (eident->sym_ident) {
				error(state, 0, "%s already declared",
					eident->name);
			}
			eident->tok = TOK_ENUM_CONST;
			if (peek(state) == TOK_EQ) {
				struct triple *val;
				eat(state, TOK_EQ);
				val = constant_expr(state);
				integral(state, val);
				base = val->u.cval;
			}
			value = int_const(state, &int_type, base);
			symbol(state, eident, &eident->sym_ident, value, &int_type);
			entry = new_type(TYPE_LIST, 0, 0);
			entry->field_ident = eident;
			*next = entry;
			next = &entry->right;
			base += 1;
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
			}
		} while(peek(state) != TOK_RBRACE);
		eat(state, TOK_RBRACE);
		if (ident) {
			symbol(state, ident, &ident->sym_tag, 0, enum_type);
		}
	}
	if (ident && ident->sym_tag &&
		ident->sym_tag->type &&
		((ident->sym_tag->type->type & TYPE_MASK) == TYPE_ENUM)) {
		enum_type = clone_type(spec, ident->sym_tag->type);
	}
	else if (ident && !enum_type) {
		error(state, 0, "enum %s undeclared", ident->name);
	}
	return enum_type;
}

static struct type *struct_declarator(
	struct compile_state *state, struct type *type, struct hash_entry **ident)
{
	if (peek(state) != TOK_COLON) {
		type = declarator(state, type, ident, 1);
	}
	if (peek(state) == TOK_COLON) {
		struct triple *value;
		eat(state, TOK_COLON);
		value = constant_expr(state);
		if (value->op != OP_INTCONST) {
			error(state, 0, "Invalid constant expression");
		}
		if (value->u.cval > size_of(state, type)) {
			error(state, 0, "bitfield larger than base type");
		}
		if (!TYPE_INTEGER(type->type) || ((type->type & TYPE_MASK) == TYPE_BITFIELD)) {
			error(state, 0, "bitfield base not an integer type");
		}
		type = new_type(TYPE_BITFIELD, type, 0);
		type->elements = value->u.cval;
	}
	return type;
}

static struct type *struct_or_union_specifier(
	struct compile_state *state, unsigned int spec)
{
	struct type *struct_type;
	struct hash_entry *ident;
	unsigned int type_main;
	unsigned int type_join;
	int tok;
	struct_type = 0;
	ident = 0;
	switch(peek(state)) {
	case TOK_STRUCT:
		eat(state, TOK_STRUCT);
		type_main = TYPE_STRUCT;
		type_join = TYPE_PRODUCT;
		break;
	case TOK_UNION:
		eat(state, TOK_UNION);
		type_main = TYPE_UNION;
		type_join = TYPE_OVERLAP;
		break;
	default:
		eat(state, TOK_STRUCT);
		type_main = TYPE_STRUCT;
		type_join = TYPE_PRODUCT;
		break;
	}
	tok = peek(state);
	if ((tok == TOK_IDENT) || (tok == TOK_ENUM_CONST) || (tok == TOK_TYPE_NAME)) {
		ident = eat(state, tok)->ident;
	}
	if (!ident || (peek(state) == TOK_LBRACE)) {
		ulong_t elements;
		struct type **next;
		elements = 0;
		eat(state, TOK_LBRACE);
		next = &struct_type;
		do {
			struct type *base_type;
			int done;
			base_type = specifier_qualifier_list(state);
			do {
				struct type *type;
				struct hash_entry *fident;
				done = 1;
				type = struct_declarator(state, base_type, &fident);
				elements++;
				if (peek(state) == TOK_COMMA) {
					done = 0;
					eat(state, TOK_COMMA);
				}
				type = clone_type(0, type);
				type->field_ident = fident;
				if (*next) {
					*next = new_type(type_join, *next, type);
					next = &((*next)->right);
				} else {
					*next = type;
				}
			} while(!done);
			eat(state, TOK_SEMI);
		} while(peek(state) != TOK_RBRACE);
		eat(state, TOK_RBRACE);
		struct_type = new_type(type_main | spec, struct_type, 0);
		struct_type->type_ident = ident;
		struct_type->elements = elements;
		if (ident) {
			symbol(state, ident, &ident->sym_tag, 0, struct_type);
		}
	}
	if (ident && ident->sym_tag &&
		ident->sym_tag->type &&
		((ident->sym_tag->type->type & TYPE_MASK) == type_main)) {
		struct_type = clone_type(spec, ident->sym_tag->type);
	}
	else if (ident && !struct_type) {
		error(state, 0, "%s %s undeclared",
			(type_main == TYPE_STRUCT)?"struct" : "union",
			ident->name);
	}
	return struct_type;
}

static unsigned int storage_class_specifier_opt(struct compile_state *state)
{
	unsigned int specifiers;
	switch(peek(state)) {
	case TOK_AUTO:
		eat(state, TOK_AUTO);
		specifiers = STOR_AUTO;
		break;
	case TOK_REGISTER:
		eat(state, TOK_REGISTER);
		specifiers = STOR_REGISTER;
		break;
	case TOK_STATIC:
		eat(state, TOK_STATIC);
		specifiers = STOR_STATIC;
		break;
	case TOK_EXTERN:
		eat(state, TOK_EXTERN);
		specifiers = STOR_EXTERN;
		break;
	case TOK_TYPEDEF:
		eat(state, TOK_TYPEDEF);
		specifiers = STOR_TYPEDEF;
		break;
	default:
		if (state->scope_depth <= GLOBAL_SCOPE_DEPTH) {
			specifiers = STOR_LOCAL;
		}
		else {
			specifiers = STOR_AUTO;
		}
	}
	return specifiers;
}

static unsigned int function_specifier_opt(struct compile_state *state)
{
	/* Ignore the inline keyword */
	unsigned int specifiers;
	specifiers = 0;
	switch(peek(state)) {
	case TOK_INLINE:
		eat(state, TOK_INLINE);
		specifiers = STOR_INLINE;
	}
	return specifiers;
}

static unsigned int attrib(struct compile_state *state, unsigned int attributes)
{
	int tok = peek(state);
	switch(tok) {
	case TOK_COMMA:
	case TOK_LPAREN:
		/* The empty attribute ignore it */
		break;
	case TOK_IDENT:
	case TOK_ENUM_CONST:
	case TOK_TYPE_NAME:
	{
		struct hash_entry *ident;
		ident = eat(state, TOK_IDENT)->ident;

		if (ident == state->i_noinline) {
			if (attributes & ATTRIB_ALWAYS_INLINE) {
				error(state, 0, "both always_inline and noinline attribtes");
			}
			attributes |= ATTRIB_NOINLINE;
		}
		else if (ident == state->i_always_inline) {
			if (attributes & ATTRIB_NOINLINE) {
				error(state, 0, "both noinline and always_inline attribtes");
			}
			attributes |= ATTRIB_ALWAYS_INLINE;
		}
		else if (ident == state->i_noreturn) {
			// attribute((noreturn)) does nothing (yet?)
		}
		else if (ident == state->i_unused) {
			// attribute((unused)) does nothing (yet?)
		}
		else if (ident == state->i_packed) {
			// attribute((packed)) does nothing (yet?)
		}
		else {
			error(state, 0, "Unknown attribute:%s", ident->name);
		}
		break;
	}
	default:
		error(state, 0, "Unexpected token: %s\n", tokens[tok]);
		break;
	}
	return attributes;
}

static unsigned int attribute_list(struct compile_state *state, unsigned type)
{
	type = attrib(state, type);
	while(peek(state) == TOK_COMMA) {
		eat(state, TOK_COMMA);
		type = attrib(state, type);
	}
	return type;
}

static unsigned int attributes_opt(struct compile_state *state, unsigned type)
{
	if (peek(state) == TOK_ATTRIBUTE) {
		eat(state, TOK_ATTRIBUTE);
		eat(state, TOK_LPAREN);
		eat(state, TOK_LPAREN);
		type = attribute_list(state, type);
		eat(state, TOK_RPAREN);
		eat(state, TOK_RPAREN);
	}
	return type;
}

static unsigned int type_qualifiers(struct compile_state *state)
{
	unsigned int specifiers;
	int done;
	done = 0;
	specifiers = QUAL_NONE;
	do {
		switch(peek(state)) {
		case TOK_CONST:
			eat(state, TOK_CONST);
			specifiers |= QUAL_CONST;
			break;
		case TOK_VOLATILE:
			eat(state, TOK_VOLATILE);
			specifiers |= QUAL_VOLATILE;
			break;
		case TOK_RESTRICT:
			eat(state, TOK_RESTRICT);
			specifiers |= QUAL_RESTRICT;
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return specifiers;
}

static struct type *type_specifier(
	struct compile_state *state, unsigned int spec)
{
	struct type *type;
	int tok;
	type = 0;
	switch((tok = peek(state))) {
	case TOK_VOID:
		eat(state, TOK_VOID);
		type = new_type(TYPE_VOID | spec, 0, 0);
		break;
	case TOK_CHAR:
		eat(state, TOK_CHAR);
		type = new_type(TYPE_CHAR | spec, 0, 0);
		break;
	case TOK_SHORT:
		eat(state, TOK_SHORT);
		if (peek(state) == TOK_INT) {
			eat(state, TOK_INT);
		}
		type = new_type(TYPE_SHORT | spec, 0, 0);
		break;
	case TOK_INT:
		eat(state, TOK_INT);
		type = new_type(TYPE_INT | spec, 0, 0);
		break;
	case TOK_LONG:
		eat(state, TOK_LONG);
		switch(peek(state)) {
		case TOK_LONG:
			eat(state, TOK_LONG);
			error(state, 0, "long long not supported");
			break;
		case TOK_DOUBLE:
			eat(state, TOK_DOUBLE);
			error(state, 0, "long double not supported");
			break;
		case TOK_INT:
			eat(state, TOK_INT);
			type = new_type(TYPE_LONG | spec, 0, 0);
			break;
		default:
			type = new_type(TYPE_LONG | spec, 0, 0);
			break;
		}
		break;
	case TOK_FLOAT:
		eat(state, TOK_FLOAT);
		error(state, 0, "type float not supported");
		break;
	case TOK_DOUBLE:
		eat(state, TOK_DOUBLE);
		error(state, 0, "type double not supported");
		break;
	case TOK_SIGNED:
		eat(state, TOK_SIGNED);
		switch(peek(state)) {
		case TOK_LONG:
			eat(state, TOK_LONG);
			switch(peek(state)) {
			case TOK_LONG:
				eat(state, TOK_LONG);
				error(state, 0, "type long long not supported");
				break;
			case TOK_INT:
				eat(state, TOK_INT);
				type = new_type(TYPE_LONG | spec, 0, 0);
				break;
			default:
				type = new_type(TYPE_LONG | spec, 0, 0);
				break;
			}
			break;
		case TOK_INT:
			eat(state, TOK_INT);
			type = new_type(TYPE_INT | spec, 0, 0);
			break;
		case TOK_SHORT:
			eat(state, TOK_SHORT);
			type = new_type(TYPE_SHORT | spec, 0, 0);
			break;
		case TOK_CHAR:
			eat(state, TOK_CHAR);
			type = new_type(TYPE_CHAR | spec, 0, 0);
			break;
		default:
			type = new_type(TYPE_INT | spec, 0, 0);
			break;
		}
		break;
	case TOK_UNSIGNED:
		eat(state, TOK_UNSIGNED);
		switch(peek(state)) {
		case TOK_LONG:
			eat(state, TOK_LONG);
			switch(peek(state)) {
			case TOK_LONG:
				eat(state, TOK_LONG);
				error(state, 0, "unsigned long long not supported");
				break;
			case TOK_INT:
				eat(state, TOK_INT);
				type = new_type(TYPE_ULONG | spec, 0, 0);
				break;
			default:
				type = new_type(TYPE_ULONG | spec, 0, 0);
				break;
			}
			break;
		case TOK_INT:
			eat(state, TOK_INT);
			type = new_type(TYPE_UINT | spec, 0, 0);
			break;
		case TOK_SHORT:
			eat(state, TOK_SHORT);
			type = new_type(TYPE_USHORT | spec, 0, 0);
			break;
		case TOK_CHAR:
			eat(state, TOK_CHAR);
			type = new_type(TYPE_UCHAR | spec, 0, 0);
			break;
		default:
			type = new_type(TYPE_UINT | spec, 0, 0);
			break;
		}
		break;
		/* struct or union specifier */
	case TOK_STRUCT:
	case TOK_UNION:
		type = struct_or_union_specifier(state, spec);
		break;
		/* enum-spefifier */
	case TOK_ENUM:
		type = enum_specifier(state, spec);
		break;
		/* typedef name */
	case TOK_TYPE_NAME:
		type = typedef_name(state, spec);
		break;
	default:
		error(state, 0, "bad type specifier %s",
			tokens[tok]);
		break;
	}
	return type;
}

static int istype(int tok)
{
	switch(tok) {
	case TOK_CONST:
	case TOK_RESTRICT:
	case TOK_VOLATILE:
	case TOK_VOID:
	case TOK_CHAR:
	case TOK_SHORT:
	case TOK_INT:
	case TOK_LONG:
	case TOK_FLOAT:
	case TOK_DOUBLE:
	case TOK_SIGNED:
	case TOK_UNSIGNED:
	case TOK_STRUCT:
	case TOK_UNION:
	case TOK_ENUM:
	case TOK_TYPE_NAME:
		return 1;
	default:
		return 0;
	}
}


static struct type *specifier_qualifier_list(struct compile_state *state)
{
	struct type *type;
	unsigned int specifiers = 0;

	/* type qualifiers */
	specifiers |= type_qualifiers(state);

	/* type specifier */
	type = type_specifier(state, specifiers);

	return type;
}

#if DEBUG_ROMCC_WARNING
static int isdecl_specifier(int tok)
{
	switch(tok) {
		/* storage class specifier */
	case TOK_AUTO:
	case TOK_REGISTER:
	case TOK_STATIC:
	case TOK_EXTERN:
	case TOK_TYPEDEF:
		/* type qualifier */
	case TOK_CONST:
	case TOK_RESTRICT:
	case TOK_VOLATILE:
		/* type specifiers */
	case TOK_VOID:
	case TOK_CHAR:
	case TOK_SHORT:
	case TOK_INT:
	case TOK_LONG:
	case TOK_FLOAT:
	case TOK_DOUBLE:
	case TOK_SIGNED:
	case TOK_UNSIGNED:
		/* struct or union specifier */
	case TOK_STRUCT:
	case TOK_UNION:
		/* enum-spefifier */
	case TOK_ENUM:
		/* typedef name */
	case TOK_TYPE_NAME:
		/* function specifiers */
	case TOK_INLINE:
		return 1;
	default:
		return 0;
	}
}
#endif

static struct type *decl_specifiers(struct compile_state *state)
{
	struct type *type;
	unsigned int specifiers;
	/* I am overly restrictive in the arragement of specifiers supported.
	 * C is overly flexible in this department it makes interpreting
	 * the parse tree difficult.
	 */
	specifiers = 0;

	/* storage class specifier */
	specifiers |= storage_class_specifier_opt(state);

	/* function-specifier */
	specifiers |= function_specifier_opt(state);

	/* attributes */
	specifiers |= attributes_opt(state, 0);

	/* type qualifier */
	specifiers |= type_qualifiers(state);

	/* type specifier */
	type = type_specifier(state, specifiers);
	return type;
}

struct field_info {
	struct type *type;
	size_t offset;
};

static struct field_info designator(struct compile_state *state, struct type *type)
{
	int tok;
	struct field_info info;
	info.offset = ~0U;
	info.type = 0;
	do {
		switch(peek(state)) {
		case TOK_LBRACKET:
		{
			struct triple *value;
			if ((type->type & TYPE_MASK) != TYPE_ARRAY) {
				error(state, 0, "Array designator not in array initializer");
			}
			eat(state, TOK_LBRACKET);
			value = constant_expr(state);
			eat(state, TOK_RBRACKET);

			info.type = type->left;
			info.offset = value->u.cval * size_of(state, info.type);
			break;
		}
		case TOK_DOT:
		{
			struct hash_entry *field;
			if (((type->type & TYPE_MASK) != TYPE_STRUCT) &&
				((type->type & TYPE_MASK) != TYPE_UNION))
			{
				error(state, 0, "Struct designator not in struct initializer");
			}
			eat(state, TOK_DOT);
			field = eat(state, TOK_IDENT)->ident;
			info.offset = field_offset(state, type, field);
			info.type   = field_type(state, type, field);
			break;
		}
		default:
			error(state, 0, "Invalid designator");
		}
		tok = peek(state);
	} while((tok == TOK_LBRACKET) || (tok == TOK_DOT));
	eat(state, TOK_EQ);
	return info;
}

static struct triple *initializer(
	struct compile_state *state, struct type *type)
{
	struct triple *result;
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME more consistent initializer handling (where should eval_const_expr go?"
#endif
	if (peek(state) != TOK_LBRACE) {
		result = assignment_expr(state);
		if (((type->type & TYPE_MASK) == TYPE_ARRAY) &&
			(type->elements == ELEMENT_COUNT_UNSPECIFIED) &&
			((result->type->type & TYPE_MASK) == TYPE_ARRAY) &&
			(result->type->elements != ELEMENT_COUNT_UNSPECIFIED) &&
			(equiv_types(type->left, result->type->left))) {
			type->elements = result->type->elements;
		}
		if (is_lvalue(state, result) &&
			((result->type->type & TYPE_MASK) == TYPE_ARRAY) &&
			(type->type & TYPE_MASK) != TYPE_ARRAY)
		{
			result = lvalue_conversion(state, result);
		}
		if (!is_init_compatible(state, type, result->type)) {
			error(state, 0, "Incompatible types in initializer");
		}
		if (!equiv_types(type, result->type)) {
			result = mk_cast_expr(state, type, result);
		}
	}
	else {
		int comma;
		size_t max_offset;
		struct field_info info;
		void *buf;
		if (((type->type & TYPE_MASK) != TYPE_ARRAY) &&
			((type->type & TYPE_MASK) != TYPE_STRUCT)) {
			internal_error(state, 0, "unknown initializer type");
		}
		info.offset = 0;
		info.type = type->left;
		if ((type->type & TYPE_MASK) == TYPE_STRUCT) {
			info.type = next_field(state, type, 0);
		}
		if (type->elements == ELEMENT_COUNT_UNSPECIFIED) {
			max_offset = 0;
		} else {
			max_offset = size_of(state, type);
		}
		buf = xcmalloc(bits_to_bytes(max_offset), "initializer");
		eat(state, TOK_LBRACE);
		do {
			struct triple *value;
			struct type *value_type;
			size_t value_size;
			void *dest;
			int tok;
			comma = 0;
			tok = peek(state);
			if ((tok == TOK_LBRACKET) || (tok == TOK_DOT)) {
				info = designator(state, type);
			}
			if ((type->elements != ELEMENT_COUNT_UNSPECIFIED) &&
				(info.offset >= max_offset)) {
				error(state, 0, "element beyond bounds");
			}
			value_type = info.type;
			value = eval_const_expr(state, initializer(state, value_type));
			value_size = size_of(state, value_type);
			if (((type->type & TYPE_MASK) == TYPE_ARRAY) &&
				(type->elements == ELEMENT_COUNT_UNSPECIFIED) &&
				(max_offset <= info.offset)) {
				void *old_buf;
				size_t old_size;
				old_buf = buf;
				old_size = max_offset;
				max_offset = info.offset + value_size;
				buf = xmalloc(bits_to_bytes(max_offset), "initializer");
				memcpy(buf, old_buf, bits_to_bytes(old_size));
				xfree(old_buf);
			}
			dest = ((char *)buf) + bits_to_bytes(info.offset);
#if DEBUG_INITIALIZER
			fprintf(state->errout, "dest = buf + %d max_offset: %d value_size: %d op: %d\n",
				dest - buf,
				bits_to_bytes(max_offset),
				bits_to_bytes(value_size),
				value->op);
#endif
			if (value->op == OP_BLOBCONST) {
				memcpy(dest, value->u.blob, bits_to_bytes(value_size));
			}
			else if ((value->op == OP_INTCONST) && (value_size == SIZEOF_I8)) {
#if DEBUG_INITIALIZER
				fprintf(state->errout, "byte: %02x\n", value->u.cval & 0xff);
#endif
				*((uint8_t *)dest) = value->u.cval & 0xff;
			}
			else if ((value->op == OP_INTCONST) && (value_size == SIZEOF_I16)) {
				*((uint16_t *)dest) = value->u.cval & 0xffff;
			}
			else if ((value->op == OP_INTCONST) && (value_size == SIZEOF_I32)) {
				*((uint32_t *)dest) = value->u.cval & 0xffffffff;
			}
			else {
				internal_error(state, 0, "unhandled constant initializer");
			}
			free_triple(state, value);
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
				comma = 1;
			}
			info.offset += value_size;
			if ((type->type & TYPE_MASK) == TYPE_STRUCT) {
				info.type = next_field(state, type, info.type);
				info.offset = field_offset(state, type,
					info.type->field_ident);
			}
		} while(comma && (peek(state) != TOK_RBRACE));
		if ((type->elements == ELEMENT_COUNT_UNSPECIFIED) &&
			((type->type & TYPE_MASK) == TYPE_ARRAY)) {
			type->elements = max_offset / size_of(state, type->left);
		}
		eat(state, TOK_RBRACE);
		result = triple(state, OP_BLOBCONST, type, 0, 0);
		result->u.blob = buf;
	}
	return result;
}

static void resolve_branches(struct compile_state *state, struct triple *first)
{
	/* Make a second pass and finish anything outstanding
	 * with respect to branches.  The only outstanding item
	 * is to see if there are goto to labels that have not
	 * been defined and to error about them.
	 */
	int i;
	struct triple *ins;
	/* Also error on branches that do not use their targets */
	ins = first;
	do {
		if (!triple_is_ret(state, ins)) {
			struct triple **expr ;
			struct triple_set *set;
			expr = triple_targ(state, ins, 0);
			for(; expr; expr = triple_targ(state, ins, expr)) {
				struct triple *targ;
				targ = *expr;
				for(set = targ?targ->use:0; set; set = set->next) {
					if (set->member == ins) {
						break;
					}
				}
				if (!set) {
					internal_error(state, ins, "targ not used");
				}
			}
		}
		ins = ins->next;
	} while(ins != first);
	/* See if there are goto to labels that have not been defined */
	for(i = 0; i < HASH_TABLE_SIZE; i++) {
		struct hash_entry *entry;
		for(entry = state->hash_table[i]; entry; entry = entry->next) {
			struct triple *ins;
			if (!entry->sym_label) {
				continue;
			}
			ins = entry->sym_label->def;
			if (!(ins->id & TRIPLE_FLAG_FLATTENED)) {
				error(state, ins, "label `%s' used but not defined",
					entry->name);
			}
		}
	}
}

static struct triple *function_definition(
	struct compile_state *state, struct type *type)
{
	struct triple *def, *tmp, *first, *end, *retvar, *ret;
	struct triple *fname;
	struct type *fname_type;
	struct hash_entry *ident;
	struct type *param, *crtype, *ctype;
	int i;
	if ((type->type &TYPE_MASK) != TYPE_FUNCTION) {
		error(state, 0, "Invalid function header");
	}

	/* Verify the function type */
	if (((type->right->type & TYPE_MASK) != TYPE_VOID)  &&
		((type->right->type & TYPE_MASK) != TYPE_PRODUCT) &&
		(type->right->field_ident == 0)) {
		error(state, 0, "Invalid function parameters");
	}
	param = type->right;
	i = 0;
	while((param->type & TYPE_MASK) == TYPE_PRODUCT) {
		i++;
		if (!param->left->field_ident) {
			error(state, 0, "No identifier for parameter %d\n", i);
		}
		param = param->right;
	}
	i++;
	if (((param->type & TYPE_MASK) != TYPE_VOID) && !param->field_ident) {
		error(state, 0, "No identifier for parameter %d\n", i);
	}

	/* Get a list of statements for this function. */
	def = triple(state, OP_LIST, type, 0, 0);

	/* Start a new scope for the passed parameters */
	start_scope(state);

	/* Put a label at the very start of a function */
	first = label(state);
	RHS(def, 0) = first;

	/* Put a label at the very end of a function */
	end = label(state);
	flatten(state, first, end);
	/* Remember where return goes */
	ident = state->i_return;
	symbol(state, ident, &ident->sym_ident, end, end->type);

	/* Get the initial closure type */
	ctype = new_type(TYPE_JOIN, &void_type, 0);
	ctype->elements = 1;

	/* Add a variable for the return value */
	crtype = new_type(TYPE_TUPLE,
		/* Remove all type qualifiers from the return type */
		new_type(TYPE_PRODUCT, ctype, clone_type(0, type->left)), 0);
	crtype->elements = 2;
	flatten(state, end, variable(state, crtype));

	/* Allocate a variable for the return address */
	retvar = flatten(state, end, variable(state, &void_ptr_type));

	/* Add in the return instruction */
	ret = triple(state, OP_RET, &void_type, read_expr(state, retvar), 0);
	flatten(state, first, ret);

	/* Walk through the parameters and create symbol table entries
	 * for them.
	 */
	param = type->right;
	while((param->type & TYPE_MASK) == TYPE_PRODUCT) {
		ident = param->left->field_ident;
		tmp = variable(state, param->left);
		var_symbol(state, ident, tmp);
		flatten(state, end, tmp);
		param = param->right;
	}
	if ((param->type & TYPE_MASK) != TYPE_VOID) {
		/* And don't forget the last parameter */
		ident = param->field_ident;
		tmp = variable(state, param);
		symbol(state, ident, &ident->sym_ident, tmp, tmp->type);
		flatten(state, end, tmp);
	}

	/* Add the declaration static const char __func__ [] = "func-name"  */
	fname_type = new_type(TYPE_ARRAY,
		clone_type(QUAL_CONST | STOR_STATIC, &char_type), 0);
	fname_type->type |= QUAL_CONST | STOR_STATIC;
	fname_type->elements = strlen(state->function) + 1;

	fname = triple(state, OP_BLOBCONST, fname_type, 0, 0);
	fname->u.blob = (void *)state->function;
	fname = flatten(state, end, fname);

	ident = state->i___func__;
	symbol(state, ident, &ident->sym_ident, fname, fname_type);

	/* Remember which function I am compiling.
	 * Also assume the last defined function is the main function.
	 */
	state->main_function = def;

	/* Now get the actual function definition */
	compound_statement(state, end);

	/* Finish anything unfinished with branches */
	resolve_branches(state, first);

	/* Remove the parameter scope */
	end_scope(state);


	/* Remember I have defined a function */
	if (!state->functions) {
		state->functions = def;
	} else {
		insert_triple(state, state->functions, def);
	}
	if (state->compiler->debug & DEBUG_INLINE) {
		FILE *fp = state->dbgout;
		fprintf(fp, "\n");
		loc(fp, state, 0);
		fprintf(fp, "\n__________ %s _________\n", __FUNCTION__);
		display_func(state, fp, def);
		fprintf(fp, "__________ %s _________ done\n\n", __FUNCTION__);
	}

	return def;
}

static struct triple *do_decl(struct compile_state *state,
	struct type *type, struct hash_entry *ident)
{
	struct triple *def;
	def = 0;
	/* Clean up the storage types used */
	switch (type->type & STOR_MASK) {
	case STOR_AUTO:
	case STOR_STATIC:
		/* These are the good types I am aiming for */
		break;
	case STOR_REGISTER:
		type->type &= ~STOR_MASK;
		type->type |= STOR_AUTO;
		break;
	case STOR_LOCAL:
	case STOR_EXTERN:
		type->type &= ~STOR_MASK;
		type->type |= STOR_STATIC;
		break;
	case STOR_TYPEDEF:
		if (!ident) {
			error(state, 0, "typedef without name");
		}
		symbol(state, ident, &ident->sym_ident, 0, type);
		ident->tok = TOK_TYPE_NAME;
		return 0;
		break;
	default:
		internal_error(state, 0, "Undefined storage class");
	}
	if ((type->type & TYPE_MASK) == TYPE_FUNCTION) {
		// ignore function prototypes
		return def;
	}
	if (ident &&
		((type->type & TYPE_MASK) == TYPE_ARRAY) &&
		((type->type & STOR_MASK) != STOR_STATIC))
		error(state, 0, "non static arrays not supported");
	if (ident &&
		((type->type & STOR_MASK) == STOR_STATIC) &&
		((type->type & QUAL_CONST) == 0)) {
		error(state, 0, "non const static variables not supported");
	}
	if (ident) {
		def = variable(state, type);
		var_symbol(state, ident, def);
	}
	return def;
}

static void decl(struct compile_state *state, struct triple *first)
{
	struct type *base_type, *type;
	struct hash_entry *ident;
	struct triple *def;
	int global;
	global = (state->scope_depth <= GLOBAL_SCOPE_DEPTH);
	base_type = decl_specifiers(state);
	ident = 0;
	type = declarator(state, base_type, &ident, 0);
	type->type = attributes_opt(state, type->type);
	if (global && ident && (peek(state) == TOK_LBRACE)) {
		/* function */
		type->type_ident = ident;
		state->function = ident->name;
		def = function_definition(state, type);
		symbol(state, ident, &ident->sym_ident, def, type);
		state->function = 0;
	}
	else {
		int done;
		flatten(state, first, do_decl(state, type, ident));
		/* type or variable definition */
		do {
			done = 1;
			if (peek(state) == TOK_EQ) {
				if (!ident) {
					error(state, 0, "cannot assign to a type");
				}
				eat(state, TOK_EQ);
				flatten(state, first,
					init_expr(state,
						ident->sym_ident->def,
						initializer(state, type)));
			}
			arrays_complete(state, type);
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
				ident = 0;
				type = declarator(state, base_type, &ident, 0);
				flatten(state, first, do_decl(state, type, ident));
				done = 0;
			}
		} while(!done);
		eat(state, TOK_SEMI);
	}
}

static void decls(struct compile_state *state)
{
	struct triple *list;
	int tok;
	list = label(state);
	while(1) {
		tok = peek(state);
		if (tok == TOK_EOF) {
			return;
		}
		if (tok == TOK_SPACE) {
			eat(state, TOK_SPACE);
		}
		decl(state, list);
		if (list->next != list) {
			error(state, 0, "global variables not supported");
		}
	}
}

/*
 * Function inlining
 */
struct triple_reg_set {
	struct triple_reg_set *next;
	struct triple *member;
	struct triple *new;
};
struct reg_block {
	struct block *block;
	struct triple_reg_set *in;
	struct triple_reg_set *out;
	int vertex;
};
static void setup_basic_blocks(struct compile_state *, struct basic_blocks *bb);
static void analyze_basic_blocks(struct compile_state *state, struct basic_blocks *bb);
static void free_basic_blocks(struct compile_state *, struct basic_blocks *bb);
static int tdominates(struct compile_state *state, struct triple *dom, struct triple *sub);
static void walk_blocks(struct compile_state *state, struct basic_blocks *bb,
	void (*cb)(struct compile_state *state, struct block *block, void *arg),
	void *arg);
static void print_block(
	struct compile_state *state, struct block *block, void *arg);
static int do_triple_set(struct triple_reg_set **head,
	struct triple *member, struct triple *new_member);
static void do_triple_unset(struct triple_reg_set **head, struct triple *member);
static struct reg_block *compute_variable_lifetimes(
	struct compile_state *state, struct basic_blocks *bb);
static void free_variable_lifetimes(struct compile_state *state,
	struct basic_blocks *bb, struct reg_block *blocks);
#if DEBUG_EXPLICIT_CLOSURES
static void print_live_variables(struct compile_state *state,
	struct basic_blocks *bb, struct reg_block *rb, FILE *fp);
#endif


static struct triple *call(struct compile_state *state,
	struct triple *retvar, struct triple *ret_addr,
	struct triple *targ, struct triple *ret)
{
	struct triple *call;

	if (!retvar || !is_lvalue(state, retvar)) {
		internal_error(state, 0, "writing to a non lvalue?");
	}
	write_compatible(state, retvar->type, &void_ptr_type);

	call = new_triple(state, OP_CALL, &void_type, 1, 0);
	TARG(call, 0) = targ;
	MISC(call, 0) = ret;
	if (!targ || (targ->op != OP_LABEL)) {
		internal_error(state, 0, "call not to a label");
	}
	if (!ret || (ret->op != OP_RET)) {
		internal_error(state, 0, "call not matched with return");
	}
	return call;
}

static void walk_functions(struct compile_state *state,
	void (*cb)(struct compile_state *state, struct triple *func, void *arg),
	void *arg)
{
	struct triple *func, *first;
	func = first = state->functions;
	do {
		cb(state, func, arg);
		func = func->next;
	} while(func != first);
}

static void reverse_walk_functions(struct compile_state *state,
	void (*cb)(struct compile_state *state, struct triple *func, void *arg),
	void *arg)
{
	struct triple *func, *first;
	func = first = state->functions;
	do {
		func = func->prev;
		cb(state, func, arg);
	} while(func != first);
}


static void mark_live(struct compile_state *state, struct triple *func, void *arg)
{
	struct triple *ptr, *first;
	if (func->u.cval == 0) {
		return;
	}
	ptr = first = RHS(func, 0);
	do {
		if (ptr->op == OP_FCALL) {
			struct triple *called_func;
			called_func = MISC(ptr, 0);
			/* Mark the called function as used */
			if (!(func->id & TRIPLE_FLAG_FLATTENED)) {
				called_func->u.cval++;
			}
			/* Remove the called function from the list */
			called_func->prev->next = called_func->next;
			called_func->next->prev = called_func->prev;

			/* Place the called function before me on the list */
			called_func->next       = func;
			called_func->prev       = func->prev;
			called_func->prev->next = called_func;
			called_func->next->prev = called_func;
		}
		ptr = ptr->next;
	} while(ptr != first);
	func->id |= TRIPLE_FLAG_FLATTENED;
}

static void mark_live_functions(struct compile_state *state)
{
	/* Ensure state->main_function is the last function in
	 * the list of functions.
	 */
	if ((state->main_function->next != state->functions) ||
		(state->functions->prev != state->main_function)) {
		internal_error(state, 0,
			"state->main_function is not at the end of the function list ");
	}
	state->main_function->u.cval = 1;
	reverse_walk_functions(state, mark_live, 0);
}

static int local_triple(struct compile_state *state,
	struct triple *func, struct triple *ins)
{
	int local = (ins->id & TRIPLE_FLAG_LOCAL);
#if 0
	if (!local) {
		FILE *fp = state->errout;
		fprintf(fp, "global: ");
		display_triple(fp, ins);
	}
#endif
	return local;
}

struct triple *copy_func(struct compile_state *state, struct triple *ofunc,
	struct occurrence *base_occurrence)
{
	struct triple *nfunc;
	struct triple *nfirst, *ofirst;
	struct triple *new, *old;

	if (state->compiler->debug & DEBUG_INLINE) {
		FILE *fp = state->dbgout;
		fprintf(fp, "\n");
		loc(fp, state, 0);
		fprintf(fp, "\n__________ %s _________\n", __FUNCTION__);
		display_func(state, fp, ofunc);
		fprintf(fp, "__________ %s _________ done\n\n", __FUNCTION__);
	}

	/* Make a new copy of the old function */
	nfunc = triple(state, OP_LIST, ofunc->type, 0, 0);
	nfirst = 0;
	ofirst = old = RHS(ofunc, 0);
	do {
		struct triple *new;
		struct occurrence *occurrence;
		int old_lhs, old_rhs;
		old_lhs = old->lhs;
		old_rhs = old->rhs;
		occurrence = inline_occurrence(state, base_occurrence, old->occurrence);
		if (ofunc->u.cval && (old->op == OP_FCALL)) {
			MISC(old, 0)->u.cval += 1;
		}
		new = alloc_triple(state, old->op, old->type, old_lhs, old_rhs,
			occurrence);
		if (!triple_stores_block(state, new)) {
			memcpy(&new->u, &old->u, sizeof(new->u));
		}
		if (!nfirst) {
			RHS(nfunc, 0) = nfirst = new;
		}
		else {
			insert_triple(state, nfirst, new);
		}
		new->id |= TRIPLE_FLAG_FLATTENED;
		new->id |= old->id & TRIPLE_FLAG_COPY;

		/* During the copy remember new as user of old */
		use_triple(old, new);

		/* Remember which instructions are local */
		old->id |= TRIPLE_FLAG_LOCAL;
		old = old->next;
	} while(old != ofirst);

	/* Make a second pass to fix up any unresolved references */
	old = ofirst;
	new = nfirst;
	do {
		struct triple **oexpr, **nexpr;
		int count, i;
		/* Lookup where the copy is, to join pointers */
		count = TRIPLE_SIZE(old);
		for(i = 0; i < count; i++) {
			oexpr = &old->param[i];
			nexpr = &new->param[i];
			if (*oexpr && !*nexpr) {
				if (!local_triple(state, ofunc, *oexpr)) {
					*nexpr = *oexpr;
				}
				else if ((*oexpr)->use) {
					*nexpr = (*oexpr)->use->member;
				}
				if (*nexpr == old) {
					internal_error(state, 0, "new == old?");
				}
				use_triple(*nexpr, new);
			}
			if (!*nexpr && *oexpr) {
				internal_error(state, 0, "Could not copy %d", i);
			}
		}
		old = old->next;
		new = new->next;
	} while((old != ofirst) && (new != nfirst));

	/* Make a third pass to cleanup the extra useses */
	old = ofirst;
	new = nfirst;
	do {
		unuse_triple(old, new);
		/* Forget which instructions are local */
		old->id &= ~TRIPLE_FLAG_LOCAL;
		old = old->next;
		new = new->next;
	} while ((old != ofirst) && (new != nfirst));
	return nfunc;
}

static void expand_inline_call(
	struct compile_state *state, struct triple *me, struct triple *fcall)
{
	/* Inline the function call */
	struct type *ptype;
	struct triple *ofunc, *nfunc, *nfirst, *result, *retvar, *ins;
	struct triple *end, *nend;
	int pvals, i;

	/* Find the triples */
	ofunc = MISC(fcall, 0);
	if (ofunc->op != OP_LIST) {
		internal_error(state, 0, "improper function");
	}
	nfunc = copy_func(state, ofunc, fcall->occurrence);
	/* Prepend the parameter reading into the new function list */
	ptype = nfunc->type->right;
	pvals = fcall->rhs;
	for(i = 0; i < pvals; i++) {
		struct type *atype;
		struct triple *arg, *param;
		atype = ptype;
		if ((ptype->type & TYPE_MASK) == TYPE_PRODUCT) {
			atype = ptype->left;
		}
		param = farg(state, nfunc, i);
		if ((param->type->type & TYPE_MASK) != (atype->type & TYPE_MASK)) {
			internal_error(state, fcall, "param %d type mismatch", i);
		}
		arg = RHS(fcall, i);
		flatten(state, fcall, write_expr(state, param, arg));
		ptype = ptype->right;
	}
	result = 0;
	if ((nfunc->type->left->type & TYPE_MASK) != TYPE_VOID) {
		result = read_expr(state,
			deref_index(state, fresult(state, nfunc), 1));
	}
	if (state->compiler->debug & DEBUG_INLINE) {
		FILE *fp = state->dbgout;
		fprintf(fp, "\n");
		loc(fp, state, 0);
		fprintf(fp, "\n__________ %s _________\n", __FUNCTION__);
		display_func(state, fp, nfunc);
		fprintf(fp, "__________ %s _________ done\n\n", __FUNCTION__);
	}

	/*
	 * Get rid of the extra triples
	 */
	/* Remove the read of the return address */
	ins = RHS(nfunc, 0)->prev->prev;
	if ((ins->op != OP_READ) || (RHS(ins, 0) != fretaddr(state, nfunc))) {
		internal_error(state, ins, "Not return address read?");
	}
	release_triple(state, ins);
	/* Remove the return instruction */
	ins = RHS(nfunc, 0)->prev;
	if (ins->op != OP_RET) {
		internal_error(state, ins, "Not return?");
	}
	release_triple(state, ins);
	/* Remove the retaddres variable */
	retvar = fretaddr(state, nfunc);
	if ((retvar->lhs != 1) ||
		(retvar->op != OP_ADECL) ||
		(retvar->next->op != OP_PIECE) ||
		(MISC(retvar->next, 0) != retvar)) {
		internal_error(state, retvar, "Not the return address?");
	}
	release_triple(state, retvar->next);
	release_triple(state, retvar);

	/* Remove the label at the start of the function */
	ins = RHS(nfunc, 0);
	if (ins->op != OP_LABEL) {
		internal_error(state, ins, "Not label?");
	}
	nfirst = ins->next;
	free_triple(state, ins);
	/* Release the new function header */
	RHS(nfunc, 0) = 0;
	free_triple(state, nfunc);

	/* Append the new function list onto the return list */
	end = fcall->prev;
	nend = nfirst->prev;
	end->next    = nfirst;
	nfirst->prev = end;
	nend->next   = fcall;
	fcall->prev  = nend;

	/* Now the result reading code */
	if (result) {
		result = flatten(state, fcall, result);
		propagate_use(state, fcall, result);
	}

	/* Release the original fcall instruction */
	release_triple(state, fcall);

	return;
}

/*
 *
 * Type of the result variable.
 *
 *                                     result
 *                                        |
 *                             +----------+------------+
 *                             |                       |
 *                     union of closures         result_type
 *                             |
 *          +------------------+---------------+
 *          |                                  |
 *       closure1                    ...   closuerN
 *          |                                  |
 *  +----+--+-+--------+-----+       +----+----+---+-----+
 *  |    |    |        |     |       |    |        |     |
 * var1 var2 var3 ... varN result   var1 var2 ... varN result
 *                           |
 *                  +--------+---------+
 *                  |                  |
 *          union of closures     result_type
 *                  |
 *            +-----+-------------------+
 *            |                         |
 *         closure1            ...  closureN
 *            |                         |
 *  +-----+---+----+----+      +----+---+----+-----+
 *  |     |        |    |      |    |        |     |
 * var1 var2 ... varN result  var1 var2 ... varN result
 */

static int add_closure_type(struct compile_state *state,
	struct triple *func, struct type *closure_type)
{
	struct type *type, *ctype, **next;
	struct triple *var, *new_var;
	int i;

#if 0
	FILE *fp = state->errout;
	fprintf(fp, "original_type: ");
	name_of(fp, fresult(state, func)->type);
	fprintf(fp, "\n");
#endif
	/* find the original type */
	var = fresult(state, func);
	type = var->type;
	if (type->elements != 2) {
		internal_error(state, var, "bad return type");
	}

	/* Find the complete closure type and update it */
	ctype = type->left->left;
	next = &ctype->left;
	while(((*next)->type & TYPE_MASK) == TYPE_OVERLAP) {
		next = &(*next)->right;
	}
	*next = new_type(TYPE_OVERLAP, *next, dup_type(state, closure_type));
	ctype->elements += 1;

#if 0
	fprintf(fp, "new_type: ");
	name_of(fp, type);
	fprintf(fp, "\n");
	fprintf(fp, "ctype: %p %d bits: %d ",
		ctype, ctype->elements, reg_size_of(state, ctype));
	name_of(fp, ctype);
	fprintf(fp, "\n");
#endif

	/* Regenerate the variable with the new type definition */
	new_var = pre_triple(state, var, OP_ADECL, type, 0, 0);
	new_var->id |= TRIPLE_FLAG_FLATTENED;
	for(i = 0; i < new_var->lhs; i++) {
		LHS(new_var, i)->id |= TRIPLE_FLAG_FLATTENED;
	}

	/* Point everyone at the new variable */
	propagate_use(state, var, new_var);

	/* Release the original variable */
	for(i = 0; i < var->lhs; i++) {
		release_triple(state, LHS(var, i));
	}
	release_triple(state, var);

	/* Return the index of the added closure type */
	return ctype->elements - 1;
}

static struct triple *closure_expr(struct compile_state *state,
	struct triple *func, int closure_idx, int var_idx)
{
	return deref_index(state,
		deref_index(state,
			deref_index(state, fresult(state, func), 0),
			closure_idx),
		var_idx);
}


static void insert_triple_set(
	struct triple_reg_set **head, struct triple *member)
{
	struct triple_reg_set *new;
	new = xcmalloc(sizeof(*new), "triple_set");
	new->member = member;
	new->new    = 0;
	new->next   = *head;
	*head       = new;
}

static int ordered_triple_set(
	struct triple_reg_set **head, struct triple *member)
{
	struct triple_reg_set **ptr;
	if (!member)
		return 0;
	ptr = head;
	while(*ptr) {
		if (member == (*ptr)->member) {
			return 0;
		}
		/* keep the list ordered */
		if (member->id < (*ptr)->member->id) {
			break;
		}
		ptr = &(*ptr)->next;
	}
	insert_triple_set(ptr, member);
	return 1;
}


static void free_closure_variables(struct compile_state *state,
	struct triple_reg_set **enclose)
{
	struct triple_reg_set *entry, *next;
	for(entry = *enclose; entry; entry = next) {
		next = entry->next;
		do_triple_unset(enclose, entry->member);
	}
}

static int lookup_closure_index(struct compile_state *state,
	struct triple *me, struct triple *val)
{
	struct triple *first, *ins, *next;
	first = RHS(me, 0);
	next = first;
	do {
		struct triple *result;
		struct triple *index0, *index1, *index2, *read, *write;
		ins = next;
		next = ins->next;
		if (ins->op != OP_CALL) {
			continue;
		}
		/* I am at a previous call point examine it closely */
		if (ins->next->op != OP_LABEL) {
			internal_error(state, ins, "call not followed by label");
		}
		/* Does this call does not enclose any variables? */
		if ((ins->next->next->op != OP_INDEX) ||
			(ins->next->next->u.cval != 0) ||
			(result = MISC(ins->next->next, 0)) ||
			(result->id & TRIPLE_FLAG_LOCAL)) {
			continue;
		}
		/* The pattern is:
		 * 0 index result < 0 >
		 * 1 index 0 < ? >
		 * 2 index 1 < ? >
		 * 3 read  2
		 * 4 write 3 var
		 */
		for(index0 = ins->next->next;
			(index0->op == OP_INDEX) &&
				(MISC(index0, 0) == result) &&
				(index0->u.cval == 0) ;
			index0 = write->next)
		{
			index1 = index0->next;
			index2 = index1->next;
			read   = index2->next;
			write  = read->next;
			if ((index0->op != OP_INDEX) ||
				(index1->op != OP_INDEX) ||
				(index2->op != OP_INDEX) ||
				(read->op != OP_READ) ||
				(write->op != OP_WRITE) ||
				(MISC(index1, 0) != index0) ||
				(MISC(index2, 0) != index1) ||
				(RHS(read, 0) != index2) ||
				(RHS(write, 0) != read)) {
				internal_error(state, index0, "bad var read");
			}
			if (MISC(write, 0) == val) {
				return index2->u.cval;
			}
		}
	} while(next != first);
	return -1;
}

static inline int enclose_triple(struct triple *ins)
{
	return (ins && ((ins->type->type & TYPE_MASK) != TYPE_VOID));
}

static void compute_closure_variables(struct compile_state *state,
	struct triple *me, struct triple *fcall, struct triple_reg_set **enclose)
{
	struct triple_reg_set *set, *vars, **last_var;
	struct basic_blocks bb;
	struct reg_block *rb;
	struct block *block;
	struct triple *old_result, *first, *ins;
	size_t count, idx;
	unsigned long used_indicies;
	int i, max_index;
#define MAX_INDICIES (sizeof(used_indicies)*CHAR_BIT)
#define ID_BITS(X) ((X) & (TRIPLE_FLAG_LOCAL -1))
	struct {
		unsigned id;
		int index;
	} *info;


	/* Find the basic blocks of this function */
	bb.func = me;
	bb.first = RHS(me, 0);
	old_result = 0;
	if (!triple_is_ret(state, bb.first->prev)) {
		bb.func = 0;
	} else {
		old_result = fresult(state, me);
	}
	analyze_basic_blocks(state, &bb);

	/* Find which variables are currently alive in a given block */
	rb = compute_variable_lifetimes(state, &bb);

	/* Find the variables that are currently alive */
	block = block_of_triple(state, fcall);
	if (!block || (block->vertex <= 0) || (block->vertex > bb.last_vertex)) {
		internal_error(state, fcall, "No reg block? block: %p", block);
	}

#if DEBUG_EXPLICIT_CLOSURES
	print_live_variables(state, &bb, rb, state->dbgout);
	fflush(state->dbgout);
#endif

	/* Count the number of triples in the function */
	first = RHS(me, 0);
	ins = first;
	count = 0;
	do {
		count++;
		ins = ins->next;
	} while(ins != first);

	/* Allocate some memory to temporary hold the id info */
	info = xcmalloc(sizeof(*info) * (count +1), "info");

	/* Mark the local function */
	first = RHS(me, 0);
	ins = first;
	idx = 1;
	do {
		info[idx].id = ins->id;
		ins->id = TRIPLE_FLAG_LOCAL | idx;
		idx++;
		ins = ins->next;
	} while(ins != first);

	/*
	 * Build the list of variables to enclose.
	 *
	 * A target it to put the same variable in the
	 * same slot for ever call of a given function.
	 * After coloring this removes all of the variable
	 * manipulation code.
	 *
	 * The list of variables to enclose is built ordered
	 * program order because except in corner cases this
	 * gives me the stability of assignment I need.
	 *
	 * To gurantee that stability I lookup the variables
	 * to see where they have been used before and
	 * I build my final list with the assigned indicies.
	 */
	vars = 0;
	if (enclose_triple(old_result)) {
		ordered_triple_set(&vars, old_result);
	}
	for(set = rb[block->vertex].out; set; set = set->next) {
		if (!enclose_triple(set->member)) {
			continue;
		}
		if ((set->member == fcall) || (set->member == old_result)) {
			continue;
		}
		if (!local_triple(state, me, set->member)) {
			internal_error(state, set->member, "not local?");
		}
		ordered_triple_set(&vars, set->member);
	}

	/* Lookup the current indicies of the live varialbe */
	used_indicies = 0;
	max_index = -1;
	for(set = vars; set ; set = set->next) {
		struct triple *ins;
		int index;
		ins = set->member;
		index  = lookup_closure_index(state, me, ins);
		info[ID_BITS(ins->id)].index = index;
		if (index < 0) {
			continue;
		}
		if (index >= MAX_INDICIES) {
			internal_error(state, ins, "index unexpectedly large");
		}
		if (used_indicies & (1 << index)) {
			internal_error(state, ins, "index previously used?");
		}
		/* Remember which indicies have been used */
		used_indicies |= (1 << index);
		if (index > max_index) {
			max_index = index;
		}
	}

	/* Walk through the live variables and make certain
	 * everything is assigned an index.
	 */
	for(set = vars; set; set = set->next) {
		struct triple *ins;
		int index;
		ins = set->member;
		index = info[ID_BITS(ins->id)].index;
		if (index >= 0) {
			continue;
		}
		/* Find the lowest unused index value */
		for(index = 0; index < MAX_INDICIES; index++) {
			if (!(used_indicies & ((uint64_t)1 << index))) {
				break;
			}
		}
		if (index == MAX_INDICIES) {
			internal_error(state, ins, "no free indicies?");
		}
		info[ID_BITS(ins->id)].index = index;
		/* Remember which indicies have been used */
		used_indicies |= (1 << index);
		if (index > max_index) {
			max_index = index;
		}
	}

	/* Build the return list of variables with positions matching
	 * their indicies.
	 */
	*enclose = 0;
	last_var = enclose;
	for(i = 0; i <= max_index; i++) {
		struct triple *var;
		var = 0;
		if (used_indicies & (1 << i)) {
			for(set = vars; set; set = set->next) {
				int index;
				index = info[ID_BITS(set->member->id)].index;
				if (index == i) {
					var = set->member;
					break;
				}
			}
			if (!var) {
				internal_error(state, me, "missing variable");
			}
		}
		insert_triple_set(last_var, var);
		last_var = &(*last_var)->next;
	}

#if DEBUG_EXPLICIT_CLOSURES
	/* Print out the variables to be enclosed */
	loc(state->dbgout, state, fcall);
	fprintf(state->dbgout, "Alive:\n");
	for(set = *enclose; set; set = set->next) {
		display_triple(state->dbgout, set->member);
	}
	fflush(state->dbgout);
#endif

	/* Clear the marks */
	ins = first;
	do {
		ins->id = info[ID_BITS(ins->id)].id;
		ins = ins->next;
	} while(ins != first);

	/* Release the ordered list of live variables */
	free_closure_variables(state, &vars);

	/* Release the storage of the old ids */
	xfree(info);

	/* Release the variable lifetime information */
	free_variable_lifetimes(state, &bb, rb);

	/* Release the basic blocks of this function */
	free_basic_blocks(state, &bb);
}

static void expand_function_call(
	struct compile_state *state, struct triple *me, struct triple *fcall)
{
	/* Generate an ordinary function call */
	struct type *closure_type, **closure_next;
	struct triple *func, *func_first, *func_last, *retvar;
	struct triple *first;
	struct type *ptype, *rtype;
	struct triple *ret_addr, *ret_loc;
	struct triple_reg_set *enclose, *set;
	int closure_idx, pvals, i;

#if DEBUG_EXPLICIT_CLOSURES
	FILE *fp = state->dbgout;
	fprintf(fp, "\ndisplay_func(me) ptr: %p\n", fcall);
	display_func(state, fp, MISC(fcall, 0));
	display_func(state, fp, me);
	fprintf(fp, "__________ %s _________ done\n\n", __FUNCTION__);
#endif

	/* Find the triples */
	func = MISC(fcall, 0);
	func_first = RHS(func, 0);
	retvar = fretaddr(state, func);
	func_last  = func_first->prev;
	first = fcall->next;

	/* Find what I need to enclose */
	compute_closure_variables(state, me, fcall, &enclose);

	/* Compute the closure type */
	closure_type = new_type(TYPE_TUPLE, 0, 0);
	closure_type->elements = 0;
	closure_next = &closure_type->left;
	for(set = enclose; set ; set = set->next) {
		struct type *type;
		type = &void_type;
		if (set->member) {
			type = set->member->type;
		}
		if (!*closure_next) {
			*closure_next = type;
		} else {
			*closure_next = new_type(TYPE_PRODUCT, *closure_next,
				type);
			closure_next = &(*closure_next)->right;
		}
		closure_type->elements += 1;
	}
	if (closure_type->elements == 0) {
		closure_type->type = TYPE_VOID;
	}


#if DEBUG_EXPLICIT_CLOSURES
	fprintf(state->dbgout, "closure type: ");
	name_of(state->dbgout, closure_type);
	fprintf(state->dbgout, "\n");
#endif

	/* Update the called functions closure variable */
	closure_idx = add_closure_type(state, func, closure_type);
	free(closure_type);
	closure_type = NULL;

	/* Generate some needed triples */
	ret_loc = label(state);
	ret_addr = triple(state, OP_ADDRCONST, &void_ptr_type, ret_loc, 0);

	/* Pass the parameters to the new function */
	ptype = func->type->right;
	pvals = fcall->rhs;
	for(i = 0; i < pvals; i++) {
		struct type *atype;
		struct triple *arg, *param;
		atype = ptype;
		if ((ptype->type & TYPE_MASK) == TYPE_PRODUCT) {
			atype = ptype->left;
		}
		param = farg(state, func, i);
		if ((param->type->type & TYPE_MASK) != (atype->type & TYPE_MASK)) {
			internal_error(state, fcall, "param type mismatch");
		}
		arg = RHS(fcall, i);
		flatten(state, first, write_expr(state, param, arg));
		ptype = ptype->right;
	}
	rtype = func->type->left;

	/* Thread the triples together */
	ret_loc       = flatten(state, first, ret_loc);

	/* Save the active variables in the result variable */
	for(i = 0, set = enclose; set ; set = set->next, i++) {
		if (!set->member) {
			continue;
		}
		flatten(state, ret_loc,
			write_expr(state,
				closure_expr(state, func, closure_idx, i),
				read_expr(state, set->member)));
	}

	/* Initialize the return value */
	if ((rtype->type & TYPE_MASK) != TYPE_VOID) {
		flatten(state, ret_loc,
			write_expr(state,
				deref_index(state, fresult(state, func), 1),
				new_triple(state, OP_UNKNOWNVAL, rtype,  0, 0)));
	}

	ret_addr      = flatten(state, ret_loc, ret_addr);
	flatten(state, ret_loc, write_expr(state, retvar, ret_addr));
	flatten(state, ret_loc,
		call(state, retvar, ret_addr, func_first, func_last));

	/* Find the result */
	if ((rtype->type & TYPE_MASK) != TYPE_VOID) {
		struct triple * result;
		result = flatten(state, first,
			read_expr(state,
				deref_index(state, fresult(state, func), 1)));

		propagate_use(state, fcall, result);
	}

	/* Release the original fcall instruction */
	release_triple(state, fcall);

	/* Restore the active variables from the result variable */
	for(i = 0, set = enclose; set ; set = set->next, i++) {
		struct triple_set *use, *next;
		struct triple *new;
		struct basic_blocks bb;
		if (!set->member || (set->member == fcall)) {
			continue;
		}
		/* Generate an expression for the value */
		new = flatten(state, first,
			read_expr(state,
				closure_expr(state, func, closure_idx, i)));


		/* If the original is an lvalue restore the preserved value */
		if (is_lvalue(state, set->member)) {
			flatten(state, first,
				write_expr(state, set->member, new));
			continue;
		}
		/*
		 * If the original is a value update the dominated uses.
		 */

		/* Analyze the basic blocks so I can see who dominates whom */
		bb.func = me;
		bb.first = RHS(me, 0);
		if (!triple_is_ret(state, bb.first->prev)) {
			bb.func = 0;
		}
		analyze_basic_blocks(state, &bb);


#if DEBUG_EXPLICIT_CLOSURES
		fprintf(state->errout, "Updating domindated uses: %p -> %p\n",
			set->member, new);
#endif
		/* If fcall dominates the use update the expression */
		for(use = set->member->use; use; use = next) {
			/* Replace use modifies the use chain and
			 * removes use, so I must take a copy of the
			 * next entry early.
			 */
			next = use->next;
			if (!tdominates(state, fcall, use->member)) {
				continue;
			}
			replace_use(state, set->member, new, use->member);
		}

		/* Release the basic blocks, the instructions will be
		 * different next time, and flatten/insert_triple does
		 * not update the block values so I can't cache the analysis.
		 */
		free_basic_blocks(state, &bb);
	}

	/* Release the closure variable list */
	free_closure_variables(state, &enclose);

	if (state->compiler->debug & DEBUG_INLINE) {
		FILE *fp = state->dbgout;
		fprintf(fp, "\n");
		loc(fp, state, 0);
		fprintf(fp, "\n__________ %s _________\n", __FUNCTION__);
		display_func(state, fp, func);
		display_func(state, fp, me);
		fprintf(fp, "__________ %s _________ done\n\n", __FUNCTION__);
	}

	return;
}

static int do_inline(struct compile_state *state, struct triple *func)
{
	int do_inline;
	int policy;

	policy = state->compiler->flags & COMPILER_INLINE_MASK;
	switch(policy) {
	case COMPILER_INLINE_ALWAYS:
		do_inline = 1;
		if (func->type->type & ATTRIB_NOINLINE) {
			error(state, func, "noinline with always_inline compiler option");
		}
		break;
	case COMPILER_INLINE_NEVER:
		do_inline = 0;
		if (func->type->type & ATTRIB_ALWAYS_INLINE) {
			error(state, func, "always_inline with noinline compiler option");
		}
		break;
	case COMPILER_INLINE_DEFAULTON:
		switch(func->type->type & STOR_MASK) {
		case STOR_STATIC | STOR_INLINE:
		case STOR_LOCAL  | STOR_INLINE:
		case STOR_EXTERN | STOR_INLINE:
			do_inline = 1;
			break;
		default:
			do_inline = 1;
			break;
		}
		break;
	case COMPILER_INLINE_DEFAULTOFF:
		switch(func->type->type & STOR_MASK) {
		case STOR_STATIC | STOR_INLINE:
		case STOR_LOCAL  | STOR_INLINE:
		case STOR_EXTERN | STOR_INLINE:
			do_inline = 1;
			break;
		default:
			do_inline = 0;
			break;
		}
		break;
	case COMPILER_INLINE_NOPENALTY:
		switch(func->type->type & STOR_MASK) {
		case STOR_STATIC | STOR_INLINE:
		case STOR_LOCAL  | STOR_INLINE:
		case STOR_EXTERN | STOR_INLINE:
			do_inline = 1;
			break;
		default:
			do_inline = (func->u.cval == 1);
			break;
		}
		break;
	default:
		internal_error(state, 0, "Unimplemented inline policy");
		break;
	}
	/* Force inlining */
	if (func->type->type & ATTRIB_NOINLINE) {
		do_inline = 0;
	}
	if (func->type->type & ATTRIB_ALWAYS_INLINE) {
		do_inline = 1;
	}
	return do_inline;
}

static void inline_function(struct compile_state *state, struct triple *me, void *arg)
{
	struct triple *first, *ptr, *next;
	/* If the function is not used don't bother */
	if (me->u.cval <= 0) {
		return;
	}
	if (state->compiler->debug & DEBUG_CALLS2) {
		FILE *fp = state->dbgout;
		fprintf(fp, "in: %s\n",
			me->type->type_ident->name);
	}

	first = RHS(me, 0);
	next = first;
	do {
		struct triple *func, *prev;
		ptr = next;
		prev = ptr->prev;
		next = ptr->next;
		if (ptr->op != OP_FCALL) {
			continue;
		}
		func = MISC(ptr, 0);
		/* See if the function should be inlined */
		if (!do_inline(state, func)) {
			/* Put a label after the fcall */
			post_triple(state, ptr, OP_LABEL, &void_type, 0, 0);
			continue;
		}
		if (state->compiler->debug & DEBUG_CALLS) {
			FILE *fp = state->dbgout;
			if (state->compiler->debug & DEBUG_CALLS2) {
				loc(fp, state, ptr);
			}
			fprintf(fp, "inlining %s\n",
				func->type->type_ident->name);
			fflush(fp);
		}

		/* Update the function use counts */
		func->u.cval -= 1;

		/* Replace the fcall with the called function */
		expand_inline_call(state, me, ptr);

		next = prev->next;
	} while (next != first);

	next = first;
	do {
		struct triple *prev, *func;
		ptr = next;
		prev = ptr->prev;
		next = ptr->next;
		if (ptr->op != OP_FCALL) {
			continue;
		}
		func = MISC(ptr, 0);
		if (state->compiler->debug & DEBUG_CALLS) {
			FILE *fp = state->dbgout;
			if (state->compiler->debug & DEBUG_CALLS2) {
				loc(fp, state, ptr);
			}
			fprintf(fp, "calling %s\n",
				func->type->type_ident->name);
			fflush(fp);
		}
		/* Replace the fcall with the instruction sequence
		 * needed to make the call.
		 */
		expand_function_call(state, me, ptr);
		next = prev->next;
	} while(next != first);
}

static void inline_functions(struct compile_state *state, struct triple *func)
{
	inline_function(state, func, 0);
	reverse_walk_functions(state, inline_function, 0);
}

static void insert_function(struct compile_state *state,
	struct triple *func, void *arg)
{
	struct triple *first, *end, *ffirst, *fend;

	if (state->compiler->debug & DEBUG_INLINE) {
		FILE *fp = state->errout;
		fprintf(fp, "%s func count: %d\n",
			func->type->type_ident->name, func->u.cval);
	}
	if (func->u.cval == 0) {
		return;
	}

	/* Find the end points of the lists */
	first  = arg;
	end    = first->prev;
	ffirst = RHS(func, 0);
	fend   = ffirst->prev;

	/* splice the lists together */
	end->next    = ffirst;
	ffirst->prev = end;
	fend->next   = first;
	first->prev  = fend;
}

struct triple *input_asm(struct compile_state *state)
{
	struct asm_info *info;
	struct triple *def;
	int i, out;

	info = xcmalloc(sizeof(*info), "asm_info");
	info->str = "";

	out = sizeof(arch_input_regs)/sizeof(arch_input_regs[0]);
	memcpy(&info->tmpl.lhs, arch_input_regs, sizeof(arch_input_regs));

	def = new_triple(state, OP_ASM, &void_type, out, 0);
	def->u.ainfo = info;
	def->id |= TRIPLE_FLAG_VOLATILE;

	for(i = 0; i < out; i++) {
		struct triple *piece;
		piece = triple(state, OP_PIECE, &int_type, def, 0);
		piece->u.cval = i;
		LHS(def, i) = piece;
	}

	return def;
}

struct triple *output_asm(struct compile_state *state)
{
	struct asm_info *info;
	struct triple *def;
	int in;

	info = xcmalloc(sizeof(*info), "asm_info");
	info->str = "";

	in = sizeof(arch_output_regs)/sizeof(arch_output_regs[0]);
	memcpy(&info->tmpl.rhs, arch_output_regs, sizeof(arch_output_regs));

	def = new_triple(state, OP_ASM, &void_type, 0, in);
	def->u.ainfo = info;
	def->id |= TRIPLE_FLAG_VOLATILE;

	return def;
}

static void join_functions(struct compile_state *state)
{
	struct triple *start, *end, *call, *in, *out, *func;
	struct file_state file;
	struct type *pnext, *param;
	struct type *result_type, *args_type;
	int idx;

	/* Be clear the functions have not been joined yet */
	state->functions_joined = 0;

	/* Dummy file state to get debug handing right */
	memset(&file, 0, sizeof(file));
	file.basename = "";
	file.line = 0;
	file.report_line = 0;
	file.report_name = file.basename;
	file.prev = state->file;
	state->file = &file;
	state->function = "";

	if (!state->main_function) {
		error(state, 0, "No functions to compile\n");
	}

	/* The type of arguments */
	args_type   = state->main_function->type->right;
	/* The return type without any specifiers */
	result_type = clone_type(0, state->main_function->type->left);


	/* Verify the external arguments */
	if (registers_of(state, args_type) > ARCH_INPUT_REGS) {
		error(state, state->main_function,
			"Too many external input arguments");
	}
	if (registers_of(state, result_type) > ARCH_OUTPUT_REGS) {
		error(state, state->main_function,
			"Too many external output arguments");
	}

	/* Lay down the basic program structure */
	end           = label(state);
	start         = label(state);
	start         = flatten(state, state->first, start);
	end           = flatten(state, state->first, end);
	in            = input_asm(state);
	out           = output_asm(state);
	call          = new_triple(state, OP_FCALL, result_type, -1, registers_of(state, args_type));
	MISC(call, 0) = state->main_function;
	in            = flatten(state, state->first, in);
	call          = flatten(state, state->first, call);
	out           = flatten(state, state->first, out);


	/* Read the external input arguments */
	pnext = args_type;
	idx = 0;
	while(pnext && ((pnext->type & TYPE_MASK) != TYPE_VOID)) {
		struct triple *expr;
		param = pnext;
		pnext = 0;
		if ((param->type & TYPE_MASK) == TYPE_PRODUCT) {
			pnext = param->right;
			param = param->left;
		}
		if (registers_of(state, param) != 1) {
			error(state, state->main_function,
				"Arg: %d %s requires multiple registers",
				idx + 1, param->field_ident->name);
		}
		expr = read_expr(state, LHS(in, idx));
		RHS(call, idx) = expr;
		expr = flatten(state, call, expr);
		use_triple(expr, call);

		idx++;
	}


	/* Write the external output arguments */
	pnext = result_type;
	if ((pnext->type & TYPE_MASK) == TYPE_STRUCT) {
		pnext = result_type->left;
	}
	for(idx = 0; idx < out->rhs; idx++) {
		struct triple *expr;
		param = pnext;
		pnext = 0;
		if (param && ((param->type & TYPE_MASK) == TYPE_PRODUCT)) {
			pnext = param->right;
			param = param->left;
		}
		if (param && ((param->type & TYPE_MASK) == TYPE_VOID)) {
			param = 0;
		}
		if (param) {
			if (registers_of(state, param) != 1) {
				error(state, state->main_function,
					"Result: %d %s requires multiple registers",
					idx, param->field_ident->name);
			}
			expr = read_expr(state, call);
			if ((result_type->type & TYPE_MASK) == TYPE_STRUCT) {
				expr = deref_field(state, expr, param->field_ident);
			}
		} else {
			expr = triple(state, OP_UNKNOWNVAL, &int_type, 0, 0);
		}
		flatten(state, out, expr);
		RHS(out, idx) = expr;
		use_triple(expr, out);
	}

	/* Allocate a dummy containing function */
	func = triple(state, OP_LIST,
		new_type(TYPE_FUNCTION, &void_type, &void_type), 0, 0);
	func->type->type_ident = lookup(state, "", 0);
	RHS(func, 0) = state->first;
	func->u.cval = 1;

	/* See which functions are called, and how often */
	mark_live_functions(state);
	inline_functions(state, func);
	walk_functions(state, insert_function, end);

	if (start->next != end) {
		flatten(state, start, branch(state, end, 0));
	}

	/* OK now the functions have been joined. */
	state->functions_joined = 1;

	/* Done now cleanup */
	state->file = file.prev;
	state->function = 0;
}

/*
 * Data structurs for optimation.
 */


static int do_use_block(
	struct block *used, struct block_set **head, struct block *user,
	int front)
{
	struct block_set **ptr, *new;
	if (!used)
		return 0;
	if (!user)
		return 0;
	ptr = head;
	while(*ptr) {
		if ((*ptr)->member == user) {
			return 0;
		}
		ptr = &(*ptr)->next;
	}
	new = xcmalloc(sizeof(*new), "block_set");
	new->member = user;
	if (front) {
		new->next = *head;
		*head = new;
	}
	else {
		new->next = 0;
		*ptr = new;
	}
	return 1;
}
static int do_unuse_block(
	struct block *used, struct block_set **head, struct block *unuser)
{
	struct block_set *use, **ptr;
	int count;
	count = 0;
	ptr = head;
	while(*ptr) {
		use = *ptr;
		if (use->member == unuser) {
			*ptr = use->next;
			memset(use, -1, sizeof(*use));
			xfree(use);
			count += 1;
		}
		else {
			ptr = &use->next;
		}
	}
	return count;
}

static void use_block(struct block *used, struct block *user)
{
	int count;
	/* Append new to the head of the list, print_block
	 * depends on this.
	 */
	count = do_use_block(used, &used->use, user, 1);
	used->users += count;
}
static void unuse_block(struct block *used, struct block *unuser)
{
	int count;
	count = do_unuse_block(used, &used->use, unuser);
	used->users -= count;
}

static void add_block_edge(struct block *block, struct block *edge, int front)
{
	int count;
	count = do_use_block(block, &block->edges, edge, front);
	block->edge_count += count;
}

static void remove_block_edge(struct block *block, struct block *edge)
{
	int count;
	count = do_unuse_block(block, &block->edges, edge);
	block->edge_count -= count;
}

static void idom_block(struct block *idom, struct block *user)
{
	do_use_block(idom, &idom->idominates, user, 0);
}

static void unidom_block(struct block *idom, struct block *unuser)
{
	do_unuse_block(idom, &idom->idominates, unuser);
}

static void domf_block(struct block *block, struct block *domf)
{
	do_use_block(block, &block->domfrontier, domf, 0);
}

static void undomf_block(struct block *block, struct block *undomf)
{
	do_unuse_block(block, &block->domfrontier, undomf);
}

static void ipdom_block(struct block *ipdom, struct block *user)
{
	do_use_block(ipdom, &ipdom->ipdominates, user, 0);
}

static void unipdom_block(struct block *ipdom, struct block *unuser)
{
	do_unuse_block(ipdom, &ipdom->ipdominates, unuser);
}

static void ipdomf_block(struct block *block, struct block *ipdomf)
{
	do_use_block(block, &block->ipdomfrontier, ipdomf, 0);
}

static void unipdomf_block(struct block *block, struct block *unipdomf)
{
	do_unuse_block(block, &block->ipdomfrontier, unipdomf);
}

static int walk_triples(
	struct compile_state *state,
	int (*cb)(struct compile_state *state, struct triple *ptr, void *arg),
	void *arg)
{
	struct triple *ptr;
	int result;
	ptr = state->first;
	do {
		result = cb(state, ptr, arg);
		if (ptr->next->prev != ptr) {
			internal_error(state, ptr->next, "bad prev");
		}
		ptr = ptr->next;
	} while((result == 0) && (ptr != state->first));
	return result;
}

#define PRINT_LIST 1
static int do_print_triple(struct compile_state *state, struct triple *ins, void *arg)
{
	FILE *fp = arg;
	int op;
	op = ins->op;
	if (op == OP_LIST) {
#if !PRINT_LIST
		return 0;
#endif
	}
	if ((op == OP_LABEL) && (ins->use)) {
		fprintf(fp, "\n%p:\n", ins);
	}
	display_triple(fp, ins);

	if (triple_is_branch(state, ins) && ins->use &&
		(ins->op != OP_RET) && (ins->op != OP_FCALL)) {
		internal_error(state, ins, "branch used?");
	}
	if (triple_is_branch(state, ins)) {
		fprintf(fp, "\n");
	}
	return 0;
}

static void print_triples(struct compile_state *state)
{
	if (state->compiler->debug & DEBUG_TRIPLES) {
		FILE *fp = state->dbgout;
		fprintf(fp, "--------------- triples ---------------\n");
		walk_triples(state, do_print_triple, fp);
		fprintf(fp, "\n");
	}
}

struct cf_block {
	struct block *block;
};
static void find_cf_blocks(struct cf_block *cf, struct block *block)
{
	struct block_set *edge;
	if (!block || (cf[block->vertex].block == block)) {
		return;
	}
	cf[block->vertex].block = block;
	for(edge = block->edges; edge; edge = edge->next) {
		find_cf_blocks(cf, edge->member);
	}
}

static void print_control_flow(struct compile_state *state,
	FILE *fp, struct basic_blocks *bb)
{
	struct cf_block *cf;
	int i;
	fprintf(fp, "\ncontrol flow\n");
	cf = xcmalloc(sizeof(*cf) * (bb->last_vertex + 1), "cf_block");
	find_cf_blocks(cf, bb->first_block);

	for(i = 1; i <= bb->last_vertex; i++) {
		struct block *block;
		struct block_set *edge;
		block = cf[i].block;
		if (!block)
			continue;
		fprintf(fp, "(%p) %d:", block, block->vertex);
		for(edge = block->edges; edge; edge = edge->next) {
			fprintf(fp, " %d", edge->member->vertex);
		}
		fprintf(fp, "\n");
	}

	xfree(cf);
}

static void free_basic_block(struct compile_state *state, struct block *block)
{
	struct block_set *edge, *entry;
	struct block *child;
	if (!block) {
		return;
	}
	if (block->vertex == -1) {
		return;
	}
	block->vertex = -1;
	for(edge = block->edges; edge; edge = edge->next) {
		if (edge->member) {
			unuse_block(edge->member, block);
		}
	}
	if (block->idom) {
		unidom_block(block->idom, block);
	}
	block->idom = 0;
	if (block->ipdom) {
		unipdom_block(block->ipdom, block);
	}
	block->ipdom = 0;
	while((entry = block->use)) {
		child = entry->member;
		unuse_block(block, child);
		if (child && (child->vertex != -1)) {
			for(edge = child->edges; edge; edge = edge->next) {
				edge->member = 0;
			}
		}
	}
	while((entry = block->idominates)) {
		child = entry->member;
		unidom_block(block, child);
		if (child && (child->vertex != -1)) {
			child->idom = 0;
		}
	}
	while((entry = block->domfrontier)) {
		child = entry->member;
		undomf_block(block, child);
	}
	while((entry = block->ipdominates)) {
		child = entry->member;
		unipdom_block(block, child);
		if (child && (child->vertex != -1)) {
			child->ipdom = 0;
		}
	}
	while((entry = block->ipdomfrontier)) {
		child = entry->member;
		unipdomf_block(block, child);
	}
	if (block->users != 0) {
		internal_error(state, 0, "block still has users");
	}
	while((edge = block->edges)) {
		child = edge->member;
		remove_block_edge(block, child);

		if (child && (child->vertex != -1)) {
			free_basic_block(state, child);
		}
	}
	memset(block, -1, sizeof(*block));
}

static void free_basic_blocks(struct compile_state *state,
	struct basic_blocks *bb)
{
	struct triple *first, *ins;
	free_basic_block(state, bb->first_block);
	bb->last_vertex = 0;
	bb->first_block = bb->last_block = 0;
	first = bb->first;
	ins = first;
	do {
		if (triple_stores_block(state, ins)) {
			ins->u.block = 0;
		}
		ins = ins->next;
	} while(ins != first);

}

static struct block *basic_block(struct compile_state *state,
	struct basic_blocks *bb, struct triple *first)
{
	struct block *block;
	struct triple *ptr;
	if (!triple_is_label(state, first)) {
		internal_error(state, first, "block does not start with a label");
	}
	/* See if this basic block has already been setup */
	if (first->u.block != 0) {
		return first->u.block;
	}
	/* Allocate another basic block structure */
	bb->last_vertex += 1;
	block = xcmalloc(sizeof(*block), "block");
	block->first = block->last = first;
	block->vertex = bb->last_vertex;
	ptr = first;
	do {
		if ((ptr != first) && triple_is_label(state, ptr) && (ptr->use)) {
			break;
		}
		block->last = ptr;
		/* If ptr->u is not used remember where the baic block is */
		if (triple_stores_block(state, ptr)) {
			ptr->u.block = block;
		}
		if (triple_is_branch(state, ptr)) {
			break;
		}
		ptr = ptr->next;
	} while (ptr != bb->first);
	if ((ptr == bb->first) ||
		((ptr->next == bb->first) && (
			triple_is_end(state, ptr) ||
			triple_is_ret(state, ptr))))
	{
		/* The block has no outflowing edges */
	}
	else if (triple_is_label(state, ptr)) {
		struct block *next;
		next = basic_block(state, bb, ptr);
		add_block_edge(block, next, 0);
		use_block(next, block);
	}
	else if (triple_is_branch(state, ptr)) {
		struct triple **expr, *first;
		struct block *child;
		/* Find the branch targets.
		 * I special case the first branch as that magically
		 * avoids some difficult cases for the register allocator.
		 */
		expr = triple_edge_targ(state, ptr, 0);
		if (!expr) {
			internal_error(state, ptr, "branch without targets");
		}
		first = *expr;
		expr = triple_edge_targ(state, ptr, expr);
		for(; expr; expr = triple_edge_targ(state, ptr, expr)) {
			if (!*expr) continue;
			child = basic_block(state, bb, *expr);
			use_block(child, block);
			add_block_edge(block, child, 0);
		}
		if (first) {
			child = basic_block(state, bb, first);
			use_block(child, block);
			add_block_edge(block, child, 1);

			/* Be certain the return block of a call is
			 * in a basic block.  When it is not find
			 * start of the block, insert a label if
			 * necessary and build the basic block.
			 * Then add a fake edge from the start block
			 * to the return block of the function.
			 */
			if (state->functions_joined && triple_is_call(state, ptr)
				&& !block_of_triple(state, MISC(ptr, 0))) {
				struct block *tail;
				struct triple *start;
				start = triple_to_block_start(state, MISC(ptr, 0));
				if (!triple_is_label(state, start)) {
					start = pre_triple(state,
						start, OP_LABEL, &void_type, 0, 0);
				}
				tail = basic_block(state, bb, start);
				add_block_edge(child, tail, 0);
				use_block(tail, child);
			}
		}
	}
	else {
		internal_error(state, 0, "Bad basic block split");
	}
#if 0
{
	struct block_set *edge;
	FILE *fp = state->errout;
	fprintf(fp, "basic_block: %10p [%2d] ( %10p - %10p )",
		block, block->vertex,
		block->first, block->last);
	for(edge = block->edges; edge; edge = edge->next) {
		fprintf(fp, " %10p [%2d]",
			edge->member ? edge->member->first : 0,
			edge->member ? edge->member->vertex : -1);
	}
	fprintf(fp, "\n");
}
#endif
	return block;
}


static void walk_blocks(struct compile_state *state, struct basic_blocks *bb,
	void (*cb)(struct compile_state *state, struct block *block, void *arg),
	void *arg)
{
	struct triple *ptr, *first;
	struct block *last_block;
	last_block = 0;
	first = bb->first;
	ptr = first;
	do {
		if (triple_stores_block(state, ptr)) {
			struct block *block;
			block = ptr->u.block;
			if (block && (block != last_block)) {
				cb(state, block, arg);
			}
			last_block = block;
		}
		ptr = ptr->next;
	} while(ptr != first);
}

static void print_block(
	struct compile_state *state, struct block *block, void *arg)
{
	struct block_set *user, *edge;
	struct triple *ptr;
	FILE *fp = arg;

	fprintf(fp, "\nblock: %p (%d) ",
		block,
		block->vertex);

	for(edge = block->edges; edge; edge = edge->next) {
		fprintf(fp, " %p<-%p",
			edge->member,
			(edge->member && edge->member->use)?
			edge->member->use->member : 0);
	}
	fprintf(fp, "\n");
	if (block->first->op == OP_LABEL) {
		fprintf(fp, "%p:\n", block->first);
	}
	for(ptr = block->first; ; ) {
		display_triple(fp, ptr);
		if (ptr == block->last)
			break;
		ptr = ptr->next;
		if (ptr == block->first) {
			internal_error(state, 0, "missing block last?");
		}
	}
	fprintf(fp, "users %d: ", block->users);
	for(user = block->use; user; user = user->next) {
		fprintf(fp, "%p (%d) ",
			user->member,
			user->member->vertex);
	}
	fprintf(fp,"\n\n");
}


static void romcc_print_blocks(struct compile_state *state, FILE *fp)
{
	fprintf(fp, "--------------- blocks ---------------\n");
	walk_blocks(state, &state->bb, print_block, fp);
}
static void print_blocks(struct compile_state *state, const char *func, FILE *fp)
{
	if (state->compiler->debug & DEBUG_BASIC_BLOCKS) {
		fprintf(fp, "After %s\n", func);
		romcc_print_blocks(state, fp);
		if (state->compiler->debug & DEBUG_FDOMINATORS) {
			print_dominators(state, fp, &state->bb);
			print_dominance_frontiers(state, fp, &state->bb);
		}
		print_control_flow(state, fp, &state->bb);
	}
}

static void prune_nonblock_triples(struct compile_state *state,
	struct basic_blocks *bb)
{
	struct block *block;
	struct triple *first, *ins, *next;
	/* Delete the triples not in a basic block */
	block = 0;
	first = bb->first;
	ins = first;
	do {
		next = ins->next;
		if (ins->op == OP_LABEL) {
			block = ins->u.block;
		}
		if (!block) {
			struct triple_set *use;
			for(use = ins->use; use; use = use->next) {
				struct block *block;
				block = block_of_triple(state, use->member);
				if (block != 0) {
					internal_error(state, ins, "pruning used ins?");
				}
			}
			release_triple(state, ins);
		}
		if (block && block->last == ins) {
			block = 0;
		}
		ins = next;
	} while(ins != first);
}

static void setup_basic_blocks(struct compile_state *state,
	struct basic_blocks *bb)
{
	if (!triple_stores_block(state, bb->first)) {
		internal_error(state, 0, "ins will not store block?");
	}
	/* Initialize the state */
	bb->first_block = bb->last_block = 0;
	bb->last_vertex = 0;
	free_basic_blocks(state, bb);

	/* Find the basic blocks */
	bb->first_block = basic_block(state, bb, bb->first);

	/* Be certain the last instruction of a function, or the
	 * entire program is in a basic block.  When it is not find
	 * the start of the block, insert a label if necessary and build
	 * basic block.  Then add a fake edge from the start block
	 * to the final block.
	 */
	if (!block_of_triple(state, bb->first->prev)) {
		struct triple *start;
		struct block *tail;
		start = triple_to_block_start(state, bb->first->prev);
		if (!triple_is_label(state, start)) {
			start = pre_triple(state,
				start, OP_LABEL, &void_type, 0, 0);
		}
		tail = basic_block(state, bb, start);
		add_block_edge(bb->first_block, tail, 0);
		use_block(tail, bb->first_block);
	}

	/* Find the last basic block.
	 */
	bb->last_block = block_of_triple(state, bb->first->prev);

	/* Delete the triples not in a basic block */
	prune_nonblock_triples(state, bb);

#if 0
	/* If we are debugging print what I have just done */
	if (state->compiler->debug & DEBUG_BASIC_BLOCKS) {
		print_blocks(state, state->dbgout);
		print_control_flow(state, bb);
	}
#endif
}


struct sdom_block {
	struct block *block;
	struct sdom_block *sdominates;
	struct sdom_block *sdom_next;
	struct sdom_block *sdom;
	struct sdom_block *label;
	struct sdom_block *parent;
	struct sdom_block *ancestor;
	int vertex;
};


static void unsdom_block(struct sdom_block *block)
{
	struct sdom_block **ptr;
	if (!block->sdom_next) {
		return;
	}
	ptr = &block->sdom->sdominates;
	while(*ptr) {
		if ((*ptr) == block) {
			*ptr = block->sdom_next;
			return;
		}
		ptr = &(*ptr)->sdom_next;
	}
}

static void sdom_block(struct sdom_block *sdom, struct sdom_block *block)
{
	unsdom_block(block);
	block->sdom = sdom;
	block->sdom_next = sdom->sdominates;
	sdom->sdominates = block;
}



static int initialize_sdblock(struct sdom_block *sd,
	struct block *parent, struct block *block, int vertex)
{
	struct block_set *edge;
	if (!block || (sd[block->vertex].block == block)) {
		return vertex;
	}
	vertex += 1;
	/* Renumber the blocks in a convenient fashion */
	block->vertex = vertex;
	sd[vertex].block    = block;
	sd[vertex].sdom     = &sd[vertex];
	sd[vertex].label    = &sd[vertex];
	sd[vertex].parent   = parent? &sd[parent->vertex] : 0;
	sd[vertex].ancestor = 0;
	sd[vertex].vertex   = vertex;
	for(edge = block->edges; edge; edge = edge->next) {
		vertex = initialize_sdblock(sd, block, edge->member, vertex);
	}
	return vertex;
}

static int initialize_spdblock(
	struct compile_state *state, struct sdom_block *sd,
	struct block *parent, struct block *block, int vertex)
{
	struct block_set *user;
	if (!block || (sd[block->vertex].block == block)) {
		return vertex;
	}
	vertex += 1;
	/* Renumber the blocks in a convenient fashion */
	block->vertex = vertex;
	sd[vertex].block    = block;
	sd[vertex].sdom     = &sd[vertex];
	sd[vertex].label    = &sd[vertex];
	sd[vertex].parent   = parent? &sd[parent->vertex] : 0;
	sd[vertex].ancestor = 0;
	sd[vertex].vertex   = vertex;
	for(user = block->use; user; user = user->next) {
		vertex = initialize_spdblock(state, sd, block, user->member, vertex);
	}
	return vertex;
}

static int setup_spdblocks(struct compile_state *state,
	struct basic_blocks *bb, struct sdom_block *sd)
{
	struct block *block;
	int vertex;
	/* Setup as many sdpblocks as possible without using fake edges */
	vertex = initialize_spdblock(state, sd, 0, bb->last_block, 0);

	/* Walk through the graph and find unconnected blocks.  Add a
	 * fake edge from the unconnected blocks to the end of the
	 * graph.
	 */
	block = bb->first_block->last->next->u.block;
	for(; block && block != bb->first_block; block = block->last->next->u.block) {
		if (sd[block->vertex].block == block) {
			continue;
		}
#if DEBUG_SDP_BLOCKS
		{
			FILE *fp = state->errout;
			fprintf(fp, "Adding %d\n", vertex +1);
		}
#endif
		add_block_edge(block, bb->last_block, 0);
		use_block(bb->last_block, block);

		vertex = initialize_spdblock(state, sd, bb->last_block, block, vertex);
	}
	return vertex;
}

static void compress_ancestors(struct sdom_block *v)
{
	/* This procedure assumes ancestor(v) != 0 */
	/* if (ancestor(ancestor(v)) != 0) {
	 *	compress(ancestor(ancestor(v)));
	 * 	if (semi(label(ancestor(v))) < semi(label(v))) {
	 *		label(v) = label(ancestor(v));
	 *	}
	 *	ancestor(v) = ancestor(ancestor(v));
	 * }
	 */
	if (!v->ancestor) {
		return;
	}
	if (v->ancestor->ancestor) {
		compress_ancestors(v->ancestor->ancestor);
		if (v->ancestor->label->sdom->vertex < v->label->sdom->vertex) {
			v->label = v->ancestor->label;
		}
		v->ancestor = v->ancestor->ancestor;
	}
}

static void compute_sdom(struct compile_state *state,
	struct basic_blocks *bb, struct sdom_block *sd)
{
	int i;
	/* // step 2
	 *  for each v <= pred(w) {
	 * 	u = EVAL(v);
	 * 	if (semi[u] < semi[w] {
	 *		semi[w] = semi[u];
	 *	}
	 * }
	 * add w to bucket(vertex(semi[w]));
	 * LINK(parent(w), w);
	 *
	 * // step 3
	 * for each v <= bucket(parent(w)) {
	 * 	delete v from bucket(parent(w));
	 * 	u = EVAL(v);
	 * 	dom(v) = (semi[u] < semi[v]) ? u : parent(w);
	 * }
	 */
	for(i = bb->last_vertex; i >= 2; i--) {
		struct sdom_block *v, *parent, *next;
		struct block_set *user;
		struct block *block;
		block = sd[i].block;
		parent = sd[i].parent;
		/* Step 2 */
		for(user = block->use; user; user = user->next) {
			struct sdom_block *v, *u;
			v = &sd[user->member->vertex];
			u = !(v->ancestor)? v : (compress_ancestors(v), v->label);
			if (u->sdom->vertex < sd[i].sdom->vertex) {
				sd[i].sdom = u->sdom;
			}
		}
		sdom_block(sd[i].sdom, &sd[i]);
		sd[i].ancestor = parent;
		/* Step 3 */
		for(v = parent->sdominates; v; v = next) {
			struct sdom_block *u;
			next = v->sdom_next;
			unsdom_block(v);
			u = (!v->ancestor) ? v : (compress_ancestors(v), v->label);
			v->block->idom = (u->sdom->vertex < v->sdom->vertex)?
				u->block : parent->block;
		}
	}
}

static void compute_spdom(struct compile_state *state,
	struct basic_blocks *bb, struct sdom_block *sd)
{
	int i;
	/* // step 2
	 *  for each v <= pred(w) {
	 * 	u = EVAL(v);
	 * 	if (semi[u] < semi[w] {
	 *		semi[w] = semi[u];
	 *	}
	 * }
	 * add w to bucket(vertex(semi[w]));
	 * LINK(parent(w), w);
	 *
	 * // step 3
	 * for each v <= bucket(parent(w)) {
	 * 	delete v from bucket(parent(w));
	 * 	u = EVAL(v);
	 * 	dom(v) = (semi[u] < semi[v]) ? u : parent(w);
	 * }
	 */
	for(i = bb->last_vertex; i >= 2; i--) {
		struct sdom_block *u, *v, *parent, *next;
		struct block_set *edge;
		struct block *block;
		block = sd[i].block;
		parent = sd[i].parent;
		/* Step 2 */
		for(edge = block->edges; edge; edge = edge->next) {
			v = &sd[edge->member->vertex];
			u = !(v->ancestor)? v : (compress_ancestors(v), v->label);
			if (u->sdom->vertex < sd[i].sdom->vertex) {
				sd[i].sdom = u->sdom;
			}
		}
		sdom_block(sd[i].sdom, &sd[i]);
		sd[i].ancestor = parent;
		/* Step 3 */
		for(v = parent->sdominates; v; v = next) {
			struct sdom_block *u;
			next = v->sdom_next;
			unsdom_block(v);
			u = (!v->ancestor) ? v : (compress_ancestors(v), v->label);
			v->block->ipdom = (u->sdom->vertex < v->sdom->vertex)?
				u->block : parent->block;
		}
	}
}

static void compute_idom(struct compile_state *state,
	struct basic_blocks *bb, struct sdom_block *sd)
{
	int i;
	for(i = 2; i <= bb->last_vertex; i++) {
		struct block *block;
		block = sd[i].block;
		if (block->idom->vertex != sd[i].sdom->vertex) {
			block->idom = block->idom->idom;
		}
		idom_block(block->idom, block);
	}
	sd[1].block->idom = 0;
}

static void compute_ipdom(struct compile_state *state,
	struct basic_blocks *bb, struct sdom_block *sd)
{
	int i;
	for(i = 2; i <= bb->last_vertex; i++) {
		struct block *block;
		block = sd[i].block;
		if (block->ipdom->vertex != sd[i].sdom->vertex) {
			block->ipdom = block->ipdom->ipdom;
		}
		ipdom_block(block->ipdom, block);
	}
	sd[1].block->ipdom = 0;
}

	/* Theorem 1:
	 *   Every vertex of a flowgraph G = (V, E, r) except r has
	 *   a unique immediate dominator.
	 *   The edges {(idom(w), w) |w <= V - {r}} form a directed tree
	 *   rooted at r, called the dominator tree of G, such that
	 *   v dominates w if and only if v is a proper ancestor of w in
	 *   the dominator tree.
	 */
	/* Lemma 1:
	 *   If v and w are vertices of G such that v <= w,
	 *   than any path from v to w must contain a common ancestor
	 *   of v and w in T.
	 */
	/* Lemma 2:  For any vertex w != r, idom(w) -> w */
	/* Lemma 3:  For any vertex w != r, sdom(w) -> w */
	/* Lemma 4:  For any vertex w != r, idom(w) -> sdom(w) */
	/* Theorem 2:
	 *   Let w != r.  Suppose every u for which sdom(w) -> u -> w satisfies
	 *   sdom(u) >= sdom(w).  Then idom(w) = sdom(w).
	 */
	/* Theorem 3:
	 *   Let w != r and let u be a vertex for which sdom(u) is
	 *   minimum among vertices u satisfying sdom(w) -> u -> w.
	 *   Then sdom(u) <= sdom(w) and idom(u) = idom(w).
	 */
	/* Lemma 5:  Let vertices v,w satisfy v -> w.
	 *           Then v -> idom(w) or idom(w) -> idom(v)
	 */

static void find_immediate_dominators(struct compile_state *state,
	struct basic_blocks *bb)
{
	struct sdom_block *sd;
	/* w->sdom = min{v| there is a path v = v0,v1,...,vk = w such that:
	 *           vi > w for (1 <= i <= k - 1}
	 */
	/* Theorem 4:
	 *   For any vertex w != r.
	 *   sdom(w) = min(
	 *                 {v|(v,w) <= E  and v < w } U
	 *                 {sdom(u) | u > w and there is an edge (v, w) such that u -> v})
	 */
	/* Corollary 1:
	 *   Let w != r and let u be a vertex for which sdom(u) is
	 *   minimum among vertices u satisfying sdom(w) -> u -> w.
	 *   Then:
	 *                   { sdom(w) if sdom(w) = sdom(u),
	 *        idom(w) = {
	 *                   { idom(u) otherwise
	 */
	/* The algorithm consists of the following 4 steps.
	 * Step 1.  Carry out a depth-first search of the problem graph.
	 *    Number the vertices from 1 to N as they are reached during
	 *    the search.  Initialize the variables used in succeeding steps.
	 * Step 2.  Compute the semidominators of all vertices by applying
	 *    theorem 4.   Carry out the computation vertex by vertex in
	 *    decreasing order by number.
	 * Step 3.  Implicitly define the immediate dominator of each vertex
	 *    by applying Corollary 1.
	 * Step 4.  Explicitly define the immediate dominator of each vertex,
	 *    carrying out the computation vertex by vertex in increasing order
	 *    by number.
	 */
	/* Step 1 initialize the basic block information */
	sd = xcmalloc(sizeof(*sd) * (bb->last_vertex + 1), "sdom_state");
	initialize_sdblock(sd, 0, bb->first_block, 0);
#if 0
	sd[1].size  = 0;
	sd[1].label = 0;
	sd[1].sdom  = 0;
#endif
	/* Step 2 compute the semidominators */
	/* Step 3 implicitly define the immediate dominator of each vertex */
	compute_sdom(state, bb, sd);
	/* Step 4 explicitly define the immediate dominator of each vertex */
	compute_idom(state, bb, sd);
	xfree(sd);
}

static void find_post_dominators(struct compile_state *state,
	struct basic_blocks *bb)
{
	struct sdom_block *sd;
	int vertex;
	/* Step 1 initialize the basic block information */
	sd = xcmalloc(sizeof(*sd) * (bb->last_vertex + 1), "sdom_state");

	vertex = setup_spdblocks(state, bb, sd);
	if (vertex != bb->last_vertex) {
		internal_error(state, 0, "missing %d blocks",
			bb->last_vertex - vertex);
	}

	/* Step 2 compute the semidominators */
	/* Step 3 implicitly define the immediate dominator of each vertex */
	compute_spdom(state, bb, sd);
	/* Step 4 explicitly define the immediate dominator of each vertex */
	compute_ipdom(state, bb, sd);
	xfree(sd);
}



static void find_block_domf(struct compile_state *state, struct block *block)
{
	struct block *child;
	struct block_set *user, *edge;
	if (block->domfrontier != 0) {
		internal_error(state, block->first, "domfrontier present?");
	}
	for(user = block->idominates; user; user = user->next) {
		child = user->member;
		if (child->idom != block) {
			internal_error(state, block->first, "bad idom");
		}
		find_block_domf(state, child);
	}
	for(edge = block->edges; edge; edge = edge->next) {
		if (edge->member->idom != block) {
			domf_block(block, edge->member);
		}
	}
	for(user = block->idominates; user; user = user->next) {
		struct block_set *frontier;
		child = user->member;
		for(frontier = child->domfrontier; frontier; frontier = frontier->next) {
			if (frontier->member->idom != block) {
				domf_block(block, frontier->member);
			}
		}
	}
}

static void find_block_ipdomf(struct compile_state *state, struct block *block)
{
	struct block *child;
	struct block_set *user;
	if (block->ipdomfrontier != 0) {
		internal_error(state, block->first, "ipdomfrontier present?");
	}
	for(user = block->ipdominates; user; user = user->next) {
		child = user->member;
		if (child->ipdom != block) {
			internal_error(state, block->first, "bad ipdom");
		}
		find_block_ipdomf(state, child);
	}
	for(user = block->use; user; user = user->next) {
		if (user->member->ipdom != block) {
			ipdomf_block(block, user->member);
		}
	}
	for(user = block->ipdominates; user; user = user->next) {
		struct block_set *frontier;
		child = user->member;
		for(frontier = child->ipdomfrontier; frontier; frontier = frontier->next) {
			if (frontier->member->ipdom != block) {
				ipdomf_block(block, frontier->member);
			}
		}
	}
}

static void print_dominated(
	struct compile_state *state, struct block *block, void *arg)
{
	struct block_set *user;
	FILE *fp = arg;

	fprintf(fp, "%d:", block->vertex);
	for(user = block->idominates; user; user = user->next) {
		fprintf(fp, " %d", user->member->vertex);
		if (user->member->idom != block) {
			internal_error(state, user->member->first, "bad idom");
		}
	}
	fprintf(fp,"\n");
}

static void print_dominated2(
	struct compile_state *state, FILE *fp, int depth, struct block *block)
{
	struct block_set *user;
	struct triple *ins;
	struct occurrence *ptr, *ptr2;
	const char *filename1, *filename2;
	int equal_filenames;
	int i;
	for(i = 0; i < depth; i++) {
		fprintf(fp, "   ");
	}
	fprintf(fp, "%3d: %p (%p - %p) @",
		block->vertex, block, block->first, block->last);
	ins = block->first;
	while(ins != block->last && (ins->occurrence->line == 0)) {
		ins = ins->next;
	}
	ptr = ins->occurrence;
	ptr2 = block->last->occurrence;
	filename1 = ptr->filename? ptr->filename : "";
	filename2 = ptr2->filename? ptr2->filename : "";
	equal_filenames = (strcmp(filename1, filename2) == 0);
	if ((ptr == ptr2) || (equal_filenames && ptr->line == ptr2->line)) {
		fprintf(fp, " %s:%d", ptr->filename, ptr->line);
	} else if (equal_filenames) {
		fprintf(fp, " %s:(%d - %d)",
			ptr->filename, ptr->line, ptr2->line);
	} else {
		fprintf(fp, " (%s:%d - %s:%d)",
			ptr->filename, ptr->line,
			ptr2->filename, ptr2->line);
	}
	fprintf(fp, "\n");
	for(user = block->idominates; user; user = user->next) {
		print_dominated2(state, fp, depth + 1, user->member);
	}
}

static void print_dominators(struct compile_state *state, FILE *fp, struct basic_blocks *bb)
{
	fprintf(fp, "\ndominates\n");
	walk_blocks(state, bb, print_dominated, fp);
	fprintf(fp, "dominates\n");
	print_dominated2(state, fp, 0, bb->first_block);
}


static int print_frontiers(
	struct compile_state *state, FILE *fp, struct block *block, int vertex)
{
	struct block_set *user, *edge;

	if (!block || (block->vertex != vertex + 1)) {
		return vertex;
	}
	vertex += 1;

	fprintf(fp, "%d:", block->vertex);
	for(user = block->domfrontier; user; user = user->next) {
		fprintf(fp, " %d", user->member->vertex);
	}
	fprintf(fp, "\n");

	for(edge = block->edges; edge; edge = edge->next) {
		vertex = print_frontiers(state, fp, edge->member, vertex);
	}
	return vertex;
}
static void print_dominance_frontiers(struct compile_state *state,
	FILE *fp, struct basic_blocks *bb)
{
	fprintf(fp, "\ndominance frontiers\n");
	print_frontiers(state, fp, bb->first_block, 0);

}

static void analyze_idominators(struct compile_state *state, struct basic_blocks *bb)
{
	/* Find the immediate dominators */
	find_immediate_dominators(state, bb);
	/* Find the dominance frontiers */
	find_block_domf(state, bb->first_block);
	/* If debuging print the print what I have just found */
	if (state->compiler->debug & DEBUG_FDOMINATORS) {
		print_dominators(state, state->dbgout, bb);
		print_dominance_frontiers(state, state->dbgout, bb);
		print_control_flow(state, state->dbgout, bb);
	}
}


static void print_ipdominated(
	struct compile_state *state, struct block *block, void *arg)
{
	struct block_set *user;
	FILE *fp = arg;

	fprintf(fp, "%d:", block->vertex);
	for(user = block->ipdominates; user; user = user->next) {
		fprintf(fp, " %d", user->member->vertex);
		if (user->member->ipdom != block) {
			internal_error(state, user->member->first, "bad ipdom");
		}
	}
	fprintf(fp, "\n");
}

static void print_ipdominators(struct compile_state *state, FILE *fp,
	struct basic_blocks *bb)
{
	fprintf(fp, "\nipdominates\n");
	walk_blocks(state, bb, print_ipdominated, fp);
}

static int print_pfrontiers(
	struct compile_state *state, FILE *fp, struct block *block, int vertex)
{
	struct block_set *user;

	if (!block || (block->vertex != vertex + 1)) {
		return vertex;
	}
	vertex += 1;

	fprintf(fp, "%d:", block->vertex);
	for(user = block->ipdomfrontier; user; user = user->next) {
		fprintf(fp, " %d", user->member->vertex);
	}
	fprintf(fp, "\n");
	for(user = block->use; user; user = user->next) {
		vertex = print_pfrontiers(state, fp, user->member, vertex);
	}
	return vertex;
}
static void print_ipdominance_frontiers(struct compile_state *state,
	FILE *fp, struct basic_blocks *bb)
{
	fprintf(fp, "\nipdominance frontiers\n");
	print_pfrontiers(state, fp, bb->last_block, 0);

}

static void analyze_ipdominators(struct compile_state *state,
	struct basic_blocks *bb)
{
	/* Find the post dominators */
	find_post_dominators(state, bb);
	/* Find the control dependencies (post dominance frontiers) */
	find_block_ipdomf(state, bb->last_block);
	/* If debuging print the print what I have just found */
	if (state->compiler->debug & DEBUG_RDOMINATORS) {
		print_ipdominators(state, state->dbgout, bb);
		print_ipdominance_frontiers(state, state->dbgout, bb);
		print_control_flow(state, state->dbgout, bb);
	}
}

static int bdominates(struct compile_state *state,
	struct block *dom, struct block *sub)
{
	while(sub && (sub != dom)) {
		sub = sub->idom;
	}
	return sub == dom;
}

static int tdominates(struct compile_state *state,
	struct triple *dom, struct triple *sub)
{
	struct block *bdom, *bsub;
	int result;
	bdom = block_of_triple(state, dom);
	bsub = block_of_triple(state, sub);
	if (bdom != bsub) {
		result = bdominates(state, bdom, bsub);
	}
	else {
		struct triple *ins;
		if (!bdom || !bsub) {
			internal_error(state, dom, "huh?");
		}
		ins = sub;
		while((ins != bsub->first) && (ins != dom)) {
			ins = ins->prev;
		}
		result = (ins == dom);
	}
	return result;
}

static void analyze_basic_blocks(
	struct compile_state *state, struct basic_blocks *bb)
{
	setup_basic_blocks(state, bb);
	analyze_idominators(state, bb);
	analyze_ipdominators(state, bb);
}

static void insert_phi_operations(struct compile_state *state)
{
	size_t size;
	struct triple *first;
	int *has_already, *work;
	struct block *work_list, **work_list_tail;
	int iter;
	struct triple *var, *vnext;

	size = sizeof(int) * (state->bb.last_vertex + 1);
	has_already = xcmalloc(size, "has_already");
	work =        xcmalloc(size, "work");
	iter = 0;

	first = state->first;
	for(var = first->next; var != first ; var = vnext) {
		struct block *block;
		struct triple_set *user, *unext;
		vnext = var->next;

		if (!triple_is_auto_var(state, var) || !var->use) {
			continue;
		}

		iter += 1;
		work_list = 0;
		work_list_tail = &work_list;
		for(user = var->use; user; user = unext) {
			unext = user->next;
			if (MISC(var, 0) == user->member) {
				continue;
			}
			if (user->member->op == OP_READ) {
				continue;
			}
			if (user->member->op != OP_WRITE) {
				internal_error(state, user->member,
					"bad variable access");
			}
			block = user->member->u.block;
			if (!block) {
				warning(state, user->member, "dead code");
				release_triple(state, user->member);
				continue;
			}
			if (work[block->vertex] >= iter) {
				continue;
			}
			work[block->vertex] = iter;
			*work_list_tail = block;
			block->work_next = 0;
			work_list_tail = &block->work_next;
		}
		for(block = work_list; block; block = block->work_next) {
			struct block_set *df;
			for(df = block->domfrontier; df; df = df->next) {
				struct triple *phi;
				struct block *front;
				int in_edges;
				front = df->member;

				if (has_already[front->vertex] >= iter) {
					continue;
				}
				/* Count how many edges flow into this block */
				in_edges = front->users;
				/* Insert a phi function for this variable */
				get_occurrence(var->occurrence);
				phi = alloc_triple(
					state, OP_PHI, var->type, -1, in_edges,
					var->occurrence);
				phi->u.block = front;
				MISC(phi, 0) = var;
				use_triple(var, phi);
#if 1
				if (phi->rhs != in_edges) {
					internal_error(state, phi, "phi->rhs: %d != in_edges: %d",
						phi->rhs, in_edges);
				}
#endif
				/* Insert the phi functions immediately after the label */
				insert_triple(state, front->first->next, phi);
				if (front->first == front->last) {
					front->last = front->first->next;
				}
				has_already[front->vertex] = iter;
				transform_to_arch_instruction(state, phi);

				/* If necessary plan to visit the basic block */
				if (work[front->vertex] >= iter) {
					continue;
				}
				work[front->vertex] = iter;
				*work_list_tail = front;
				front->work_next = 0;
				work_list_tail = &front->work_next;
			}
		}
	}
	xfree(has_already);
	xfree(work);
}


struct stack {
	struct triple_set *top;
	unsigned orig_id;
};

static int count_auto_vars(struct compile_state *state)
{
	struct triple *first, *ins;
	int auto_vars = 0;
	first = state->first;
	ins = first;
	do {
		if (triple_is_auto_var(state, ins)) {
			auto_vars += 1;
		}
		ins = ins->next;
	} while(ins != first);
	return auto_vars;
}

static void number_auto_vars(struct compile_state *state, struct stack *stacks)
{
	struct triple *first, *ins;
	int auto_vars = 0;
	first = state->first;
	ins = first;
	do {
		if (triple_is_auto_var(state, ins)) {
			auto_vars += 1;
			stacks[auto_vars].orig_id = ins->id;
			ins->id = auto_vars;
		}
		ins = ins->next;
	} while(ins != first);
}

static void restore_auto_vars(struct compile_state *state, struct stack *stacks)
{
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		if (triple_is_auto_var(state, ins)) {
			ins->id = stacks[ins->id].orig_id;
		}
		ins = ins->next;
	} while(ins != first);
}

static struct triple *peek_triple(struct stack *stacks, struct triple *var)
{
	struct triple_set *head;
	struct triple *top_val;
	top_val = 0;
	head = stacks[var->id].top;
	if (head) {
		top_val = head->member;
	}
	return top_val;
}

static void push_triple(struct stack *stacks, struct triple *var, struct triple *val)
{
	struct triple_set *new;
	/* Append new to the head of the list,
	 * it's the only sensible behavoir for a stack.
	 */
	new = xcmalloc(sizeof(*new), "triple_set");
	new->member = val;
	new->next   = stacks[var->id].top;
	stacks[var->id].top = new;
}

static void pop_triple(struct stack *stacks, struct triple *var, struct triple *oldval)
{
	struct triple_set *set, **ptr;
	ptr = &stacks[var->id].top;
	while(*ptr) {
		set = *ptr;
		if (set->member == oldval) {
			*ptr = set->next;
			xfree(set);
			/* Only free one occurrence from the stack */
			return;
		}
		else {
			ptr = &set->next;
		}
	}
}

/*
 * C(V)
 * S(V)
 */
static void fixup_block_phi_variables(
	struct compile_state *state, struct stack *stacks, struct block *parent, struct block *block)
{
	struct block_set *set;
	struct triple *ptr;
	int edge;
	if (!parent || !block)
		return;
	/* Find the edge I am coming in on */
	edge = 0;
	for(set = block->use; set; set = set->next, edge++) {
		if (set->member == parent) {
			break;
		}
	}
	if (!set) {
		internal_error(state, 0, "phi input is not on a control predecessor");
	}
	for(ptr = block->first; ; ptr = ptr->next) {
		if (ptr->op == OP_PHI) {
			struct triple *var, *val, **slot;
			var = MISC(ptr, 0);
			if (!var) {
				internal_error(state, ptr, "no var???");
			}
			/* Find the current value of the variable */
			val = peek_triple(stacks, var);
			if (val && ((val->op == OP_WRITE) || (val->op == OP_READ))) {
				internal_error(state, val, "bad value in phi");
			}
			if (edge >= ptr->rhs) {
				internal_error(state, ptr, "edges > phi rhs");
			}
			slot = &RHS(ptr, edge);
			if ((*slot != 0) && (*slot != val)) {
				internal_error(state, ptr, "phi already bound on this edge");
			}
			*slot = val;
			use_triple(val, ptr);
		}
		if (ptr == block->last) {
			break;
		}
	}
}


static void rename_block_variables(
	struct compile_state *state, struct stack *stacks, struct block *block)
{
	struct block_set *user, *edge;
	struct triple *ptr, *next, *last;
	int done;
	if (!block)
		return;
	last = block->first;
	done = 0;
	for(ptr = block->first; !done; ptr = next) {
		next = ptr->next;
		if (ptr == block->last) {
			done = 1;
		}
		/* RHS(A) */
		if (ptr->op == OP_READ) {
			struct triple *var, *val;
			var = RHS(ptr, 0);
			if (!triple_is_auto_var(state, var)) {
				internal_error(state, ptr, "read of non auto var!");
			}
			unuse_triple(var, ptr);
			/* Find the current value of the variable */
			val = peek_triple(stacks, var);
			if (!val) {
				/* Let the optimizer at variables that are not initially
				 * set.  But give it a bogus value so things seem to
				 * work by accident.  This is useful for bitfields because
				 * setting them always involves a read-modify-write.
				 */
				if (TYPE_ARITHMETIC(ptr->type->type)) {
					val = pre_triple(state, ptr, OP_INTCONST, ptr->type, 0, 0);
					val->u.cval = 0xdeadbeaf;
				} else {
					val = pre_triple(state, ptr, OP_UNKNOWNVAL, ptr->type, 0, 0);
				}
			}
			if (!val) {
				error(state, ptr, "variable used without being set");
			}
			if ((val->op == OP_WRITE) || (val->op == OP_READ)) {
				internal_error(state, val, "bad value in read");
			}
			propagate_use(state, ptr, val);
			release_triple(state, ptr);
			continue;
		}
		/* LHS(A) */
		if (ptr->op == OP_WRITE) {
			struct triple *var, *val, *tval;
			var = MISC(ptr, 0);
			if (!triple_is_auto_var(state, var)) {
				internal_error(state, ptr, "write to non auto var!");
			}
			tval = val = RHS(ptr, 0);
			if ((val->op == OP_WRITE) || (val->op == OP_READ) ||
				triple_is_auto_var(state, val)) {
				internal_error(state, ptr, "bad value in write");
			}
			/* Insert a cast if the types differ */
			if (!is_subset_type(ptr->type, val->type)) {
				if (val->op == OP_INTCONST) {
					tval = pre_triple(state, ptr, OP_INTCONST, ptr->type, 0, 0);
					tval->u.cval = val->u.cval;
				}
				else {
					tval = pre_triple(state, ptr, OP_CONVERT, ptr->type, val, 0);
					use_triple(val, tval);
				}
				transform_to_arch_instruction(state, tval);
				unuse_triple(val, ptr);
				RHS(ptr, 0) = tval;
				use_triple(tval, ptr);
			}
			propagate_use(state, ptr, tval);
			unuse_triple(var, ptr);
			/* Push OP_WRITE ptr->right onto a stack of variable uses */
			push_triple(stacks, var, tval);
		}
		if (ptr->op == OP_PHI) {
			struct triple *var;
			var = MISC(ptr, 0);
			if (!triple_is_auto_var(state, var)) {
				internal_error(state, ptr, "phi references non auto var!");
			}
			/* Push OP_PHI onto a stack of variable uses */
			push_triple(stacks, var, ptr);
		}
		last = ptr;
	}
	block->last = last;

	/* Fixup PHI functions in the cf successors */
	for(edge = block->edges; edge; edge = edge->next) {
		fixup_block_phi_variables(state, stacks, block, edge->member);
	}
	/* rename variables in the dominated nodes */
	for(user = block->idominates; user; user = user->next) {
		rename_block_variables(state, stacks, user->member);
	}
	/* pop the renamed variable stack */
	last = block->first;
	done = 0;
	for(ptr = block->first; !done ; ptr = next) {
		next = ptr->next;
		if (ptr == block->last) {
			done = 1;
		}
		if (ptr->op == OP_WRITE) {
			struct triple *var;
			var = MISC(ptr, 0);
			/* Pop OP_WRITE ptr->right from the stack of variable uses */
			pop_triple(stacks, var, RHS(ptr, 0));
			release_triple(state, ptr);
			continue;
		}
		if (ptr->op == OP_PHI) {
			struct triple *var;
			var = MISC(ptr, 0);
			/* Pop OP_WRITE ptr->right from the stack of variable uses */
			pop_triple(stacks, var, ptr);
		}
		last = ptr;
	}
	block->last = last;
}

static void rename_variables(struct compile_state *state)
{
	struct stack *stacks;
	int auto_vars;

	/* Allocate stacks for the Variables */
	auto_vars = count_auto_vars(state);
	stacks = xcmalloc(sizeof(stacks[0])*(auto_vars + 1), "auto var stacks");

	/* Give each auto_var a stack */
	number_auto_vars(state, stacks);

	/* Rename the variables */
	rename_block_variables(state, stacks, state->bb.first_block);

	/* Remove the stacks from the auto_vars */
	restore_auto_vars(state, stacks);
	xfree(stacks);
}

static void prune_block_variables(struct compile_state *state,
	struct block *block)
{
	struct block_set *user;
	struct triple *next, *ptr;
	int done;

	done = 0;
	for(ptr = block->first; !done; ptr = next) {
		/* Be extremely careful I am deleting the list
		 * as I walk trhough it.
		 */
		next = ptr->next;
		if (ptr == block->last) {
			done = 1;
		}
		if (triple_is_auto_var(state, ptr)) {
			struct triple_set *user, *next;
			for(user = ptr->use; user; user = next) {
				struct triple *use;
				next = user->next;
				use = user->member;
				if (MISC(ptr, 0) == user->member) {
					continue;
				}
				if (use->op != OP_PHI) {
					internal_error(state, use, "decl still used");
				}
				if (MISC(use, 0) != ptr) {
					internal_error(state, use, "bad phi use of decl");
				}
				unuse_triple(ptr, use);
				MISC(use, 0) = 0;
			}
			if ((ptr->u.cval == 0) && (MISC(ptr, 0)->lhs == 1)) {
				/* Delete the adecl */
				release_triple(state, MISC(ptr, 0));
				/* And the piece */
				release_triple(state, ptr);
			}
			continue;
		}
	}
	for(user = block->idominates; user; user = user->next) {
		prune_block_variables(state, user->member);
	}
}

struct phi_triple {
	struct triple *phi;
	unsigned orig_id;
	int alive;
};

static void keep_phi(struct compile_state *state, struct phi_triple *live, struct triple *phi)
{
	struct triple **slot;
	int zrhs, i;
	if (live[phi->id].alive) {
		return;
	}
	live[phi->id].alive = 1;
	zrhs = phi->rhs;
	slot = &RHS(phi, 0);
	for(i = 0; i < zrhs; i++) {
		struct triple *used;
		used = slot[i];
		if (used && (used->op == OP_PHI)) {
			keep_phi(state, live, used);
		}
	}
}

static void prune_unused_phis(struct compile_state *state)
{
	struct triple *first, *phi;
	struct phi_triple *live;
	int phis, i;

	/* Find the first instruction */
	first = state->first;

	/* Count how many phi functions I need to process */
	phis = 0;
	for(phi = first->next; phi != first; phi = phi->next) {
		if (phi->op == OP_PHI) {
			phis += 1;
		}
	}

	/* Mark them all dead */
	live = xcmalloc(sizeof(*live) * (phis + 1), "phi_triple");
	phis = 0;
	for(phi = first->next; phi != first; phi = phi->next) {
		if (phi->op != OP_PHI) {
			continue;
		}
		live[phis].alive   = 0;
		live[phis].orig_id = phi->id;
		live[phis].phi     = phi;
		phi->id = phis;
		phis += 1;
	}

	/* Mark phis alive that are used by non phis */
	for(i = 0; i < phis; i++) {
		struct triple_set *set;
		for(set = live[i].phi->use; !live[i].alive && set; set = set->next) {
			if (set->member->op != OP_PHI) {
				keep_phi(state, live, live[i].phi);
				break;
			}
		}
	}

	/* Delete the extraneous phis */
	for(i = 0; i < phis; i++) {
		struct triple **slot;
		int zrhs, j;
		if (!live[i].alive) {
			release_triple(state, live[i].phi);
			continue;
		}
		phi = live[i].phi;
		slot = &RHS(phi, 0);
		zrhs = phi->rhs;
		for(j = 0; j < zrhs; j++) {
			if(!slot[j]) {
				struct triple *unknown;
				get_occurrence(phi->occurrence);
				unknown = flatten(state, state->global_pool,
					alloc_triple(state, OP_UNKNOWNVAL,
						phi->type, 0, 0, phi->occurrence));
				slot[j] = unknown;
				use_triple(unknown, phi);
				transform_to_arch_instruction(state, unknown);
#if 0
				warning(state, phi, "variable not set at index %d on all paths to use", j);
#endif
			}
		}
	}
	xfree(live);
}

static void transform_to_ssa_form(struct compile_state *state)
{
	insert_phi_operations(state);
	rename_variables(state);

	prune_block_variables(state, state->bb.first_block);
	prune_unused_phis(state);

	print_blocks(state, __func__, state->dbgout);
}


static void clear_vertex(
	struct compile_state *state, struct block *block, void *arg)
{
	/* Clear the current blocks vertex and the vertex of all
	 * of the current blocks neighbors in case there are malformed
	 * blocks with now instructions at this point.
	 */
	struct block_set *user, *edge;
	block->vertex = 0;
	for(edge = block->edges; edge; edge = edge->next) {
		edge->member->vertex = 0;
	}
	for(user = block->use; user; user = user->next) {
		user->member->vertex = 0;
	}
}

static void mark_live_block(
	struct compile_state *state, struct block *block, int *next_vertex)
{
	/* See if this is a block that has not been marked */
	if (block->vertex != 0) {
		return;
	}
	block->vertex = *next_vertex;
	*next_vertex += 1;
	if (triple_is_branch(state, block->last)) {
		struct triple **targ;
		targ = triple_edge_targ(state, block->last, 0);
		for(; targ; targ = triple_edge_targ(state, block->last, targ)) {
			if (!*targ) {
				continue;
			}
			if (!triple_stores_block(state, *targ)) {
				internal_error(state, 0, "bad targ");
			}
			mark_live_block(state, (*targ)->u.block, next_vertex);
		}
		/* Ensure the last block of a function remains alive */
		if (triple_is_call(state, block->last)) {
			mark_live_block(state, MISC(block->last, 0)->u.block, next_vertex);
		}
	}
	else if (block->last->next != state->first) {
		struct triple *ins;
		ins = block->last->next;
		if (!triple_stores_block(state, ins)) {
			internal_error(state, 0, "bad block start");
		}
		mark_live_block(state, ins->u.block, next_vertex);
	}
}

static void transform_from_ssa_form(struct compile_state *state)
{
	/* To get out of ssa form we insert moves on the incoming
	 * edges to blocks containting phi functions.
	 */
	struct triple *first;
	struct triple *phi, *var, *next;
	int next_vertex;

	/* Walk the control flow to see which blocks remain alive */
	walk_blocks(state, &state->bb, clear_vertex, 0);
	next_vertex = 1;
	mark_live_block(state, state->bb.first_block, &next_vertex);

	/* Walk all of the operations to find the phi functions */
	first = state->first;
	for(phi = first->next; phi != first ; phi = next) {
		struct block_set *set;
		struct block *block;
		struct triple **slot;
		struct triple *var;
		struct triple_set *use, *use_next;
		int edge, writers, readers;
		next = phi->next;
		if (phi->op != OP_PHI) {
			continue;
		}

		block = phi->u.block;
		slot  = &RHS(phi, 0);

		/* If this phi is in a dead block just forget it */
		if (block->vertex == 0) {
			release_triple(state, phi);
			continue;
		}

		/* Forget uses from code in dead blocks */
		for(use = phi->use; use; use = use_next) {
			struct block *ublock;
			struct triple **expr;
			use_next = use->next;
			ublock = block_of_triple(state, use->member);
			if ((use->member == phi) || (ublock->vertex != 0)) {
				continue;
			}
			expr = triple_rhs(state, use->member, 0);
			for(; expr; expr = triple_rhs(state, use->member, expr)) {
				if (*expr == phi) {
					*expr = 0;
				}
			}
			unuse_triple(phi, use->member);
		}
		/* A variable to replace the phi function */
		if (registers_of(state, phi->type) != 1) {
			internal_error(state, phi, "phi->type does not fit in a single register!");
		}
		var = post_triple(state, phi, OP_ADECL, phi->type, 0, 0);
		var = var->next; /* point at the var */

		/* Replaces use of phi with var */
		propagate_use(state, phi, var);

		/* Count the readers */
		readers = 0;
		for(use = var->use; use; use = use->next) {
			if (use->member != MISC(var, 0)) {
				readers++;
			}
		}

		/* Walk all of the incoming edges/blocks and insert moves.
		 */
		writers = 0;
		for(edge = 0, set = block->use; set; set = set->next, edge++) {
			struct block *eblock, *vblock;
			struct triple *move;
			struct triple *val, *base;
			eblock = set->member;
			val = slot[edge];
			slot[edge] = 0;
			unuse_triple(val, phi);
			vblock = block_of_triple(state, val);

			/* If we don't have a value that belongs in an OP_WRITE
			 * continue on.
			 */
			if (!val || (val == &unknown_triple) || (val == phi)
				|| (vblock && (vblock->vertex == 0))) {
				continue;
			}
			/* If the value should never occur error */
			if (!vblock) {
				internal_error(state, val, "no vblock?");
				continue;
			}

			/* If the value occurs in a dead block see if a replacement
			 * block can be found.
			 */
			while(eblock && (eblock->vertex == 0)) {
				eblock = eblock->idom;
			}
			/* If not continue on with the next value. */
			if (!eblock || (eblock->vertex == 0)) {
				continue;
			}

			/* If we have an empty incoming block ignore it. */
			if (!eblock->first) {
				internal_error(state, 0, "empty block?");
			}

			/* Make certain the write is placed in the edge block... */
			/* Walk through the edge block backwards to find an
			 * appropriate location for the OP_WRITE.
			 */
			for(base = eblock->last; base != eblock->first; base = base->prev) {
				struct triple **expr;
				if (base->op == OP_PIECE) {
					base = MISC(base, 0);
				}
				if ((base == var) || (base == val)) {
					goto out;
				}
				expr = triple_lhs(state, base, 0);
				for(; expr; expr = triple_lhs(state, base, expr)) {
					if ((*expr) == val) {
						goto out;
					}
				}
				expr = triple_rhs(state, base, 0);
				for(; expr; expr = triple_rhs(state, base, expr)) {
					if ((*expr) == var) {
						goto out;
					}
				}
			}
		out:
			if (triple_is_branch(state, base)) {
				internal_error(state, base,
					"Could not insert write to phi");
			}
			move = post_triple(state, base, OP_WRITE, var->type, val, var);
			use_triple(val, move);
			use_triple(var, move);
			writers++;
		}
		if (!writers && readers) {
			internal_error(state, var, "no value written to in use phi?");
		}
		/* If var is not used free it */
		if (!writers) {
			release_triple(state, MISC(var, 0));
			release_triple(state, var);
		}
		/* Release the phi function */
		release_triple(state, phi);
	}

	/* Walk all of the operations to find the adecls */
	for(var = first->next; var != first ; var = var->next) {
		struct triple_set *use, *use_next;
		if (!triple_is_auto_var(state, var)) {
			continue;
		}

		/* Walk through all of the rhs uses of var and
		 * replace them with read of var.
		 */
		for(use = var->use; use; use = use_next) {
			struct triple *read, *user;
			struct triple **slot;
			int zrhs, i, used;
			use_next = use->next;
			user = use->member;

			/* Generate a read of var */
			read = pre_triple(state, user, OP_READ, var->type, var, 0);
			use_triple(var, read);

			/* Find the rhs uses and see if they need to be replaced */
			used = 0;
			zrhs = user->rhs;
			slot = &RHS(user, 0);
			for(i = 0; i < zrhs; i++) {
				if (slot[i] == var) {
					slot[i] = read;
					used = 1;
				}
			}
			/* If we did use it cleanup the uses */
			if (used) {
				unuse_triple(var, user);
				use_triple(read, user);
			}
			/* If we didn't use it release the extra triple */
			else {
				release_triple(state, read);
			}
		}
	}
}

#define HI() if (state->compiler->debug & DEBUG_REBUILD_SSA_FORM) { \
	FILE *fp = state->dbgout; \
	fprintf(fp, "@ %s:%d\n", __FILE__, __LINE__); romcc_print_blocks(state, fp); \
	}

static void rebuild_ssa_form(struct compile_state *state)
{
HI();
	transform_from_ssa_form(state);
HI();
	state->bb.first = state->first;
	free_basic_blocks(state, &state->bb);
	analyze_basic_blocks(state, &state->bb);
HI();
	insert_phi_operations(state);
HI();
	rename_variables(state);
HI();

	prune_block_variables(state, state->bb.first_block);
HI();
	prune_unused_phis(state);
HI();
}
#undef HI

/*
 * Register conflict resolution
 * =========================================================
 */

static struct reg_info find_def_color(
	struct compile_state *state, struct triple *def)
{
	struct triple_set *set;
	struct reg_info info;
	info.reg = REG_UNSET;
	info.regcm = 0;
	if (!triple_is_def(state, def)) {
		return info;
	}
	info = arch_reg_lhs(state, def, 0);
	if (info.reg >= MAX_REGISTERS) {
		info.reg = REG_UNSET;
	}
	for(set = def->use; set; set = set->next) {
		struct reg_info tinfo;
		int i;
		i = find_rhs_use(state, set->member, def);
		if (i < 0) {
			continue;
		}
		tinfo = arch_reg_rhs(state, set->member, i);
		if (tinfo.reg >= MAX_REGISTERS) {
			tinfo.reg = REG_UNSET;
		}
		if ((tinfo.reg != REG_UNSET) &&
			(info.reg != REG_UNSET) &&
			(tinfo.reg != info.reg)) {
			internal_error(state, def, "register conflict");
		}
		if ((info.regcm & tinfo.regcm) == 0) {
			internal_error(state, def, "regcm conflict %x & %x == 0",
				info.regcm, tinfo.regcm);
		}
		if (info.reg == REG_UNSET) {
			info.reg = tinfo.reg;
		}
		info.regcm &= tinfo.regcm;
	}
	if (info.reg >= MAX_REGISTERS) {
		internal_error(state, def, "register out of range");
	}
	return info;
}

static struct reg_info find_lhs_pre_color(
	struct compile_state *state, struct triple *ins, int index)
{
	struct reg_info info;
	int zlhs, zrhs, i;
	zrhs = ins->rhs;
	zlhs = ins->lhs;
	if (!zlhs && triple_is_def(state, ins)) {
		zlhs = 1;
	}
	if (index >= zlhs) {
		internal_error(state, ins, "Bad lhs %d", index);
	}
	info = arch_reg_lhs(state, ins, index);
	for(i = 0; i < zrhs; i++) {
		struct reg_info rinfo;
		rinfo = arch_reg_rhs(state, ins, i);
		if ((info.reg == rinfo.reg) &&
			(rinfo.reg >= MAX_REGISTERS)) {
			struct reg_info tinfo;
			tinfo = find_lhs_pre_color(state, RHS(ins, index), 0);
			info.reg = tinfo.reg;
			info.regcm &= tinfo.regcm;
			break;
		}
	}
	if (info.reg >= MAX_REGISTERS) {
		info.reg = REG_UNSET;
	}
	return info;
}

static struct reg_info find_rhs_post_color(
	struct compile_state *state, struct triple *ins, int index);

static struct reg_info find_lhs_post_color(
	struct compile_state *state, struct triple *ins, int index)
{
	struct triple_set *set;
	struct reg_info info;
	struct triple *lhs;
#if DEBUG_TRIPLE_COLOR
	fprintf(state->errout, "find_lhs_post_color(%p, %d)\n",
		ins, index);
#endif
	if ((index == 0) && triple_is_def(state, ins)) {
		lhs = ins;
	}
	else if (index < ins->lhs) {
		lhs = LHS(ins, index);
	}
	else {
		internal_error(state, ins, "Bad lhs %d", index);
		lhs = 0;
	}
	info = arch_reg_lhs(state, ins, index);
	if (info.reg >= MAX_REGISTERS) {
		info.reg = REG_UNSET;
	}
	for(set = lhs->use; set; set = set->next) {
		struct reg_info rinfo;
		struct triple *user;
		int zrhs, i;
		user = set->member;
		zrhs = user->rhs;
		for(i = 0; i < zrhs; i++) {
			if (RHS(user, i) != lhs) {
				continue;
			}
			rinfo = find_rhs_post_color(state, user, i);
			if ((info.reg != REG_UNSET) &&
				(rinfo.reg != REG_UNSET) &&
				(info.reg != rinfo.reg)) {
				internal_error(state, ins, "register conflict");
			}
			if ((info.regcm & rinfo.regcm) == 0) {
				internal_error(state, ins, "regcm conflict %x & %x == 0",
					info.regcm, rinfo.regcm);
			}
			if (info.reg == REG_UNSET) {
				info.reg = rinfo.reg;
			}
			info.regcm &= rinfo.regcm;
		}
	}
#if DEBUG_TRIPLE_COLOR
	fprintf(state->errout, "find_lhs_post_color(%p, %d) -> ( %d, %x)\n",
		ins, index, info.reg, info.regcm);
#endif
	return info;
}

static struct reg_info find_rhs_post_color(
	struct compile_state *state, struct triple *ins, int index)
{
	struct reg_info info, rinfo;
	int zlhs, i;
#if DEBUG_TRIPLE_COLOR
	fprintf(state->errout, "find_rhs_post_color(%p, %d)\n",
		ins, index);
#endif
	rinfo = arch_reg_rhs(state, ins, index);
	zlhs = ins->lhs;
	if (!zlhs && triple_is_def(state, ins)) {
		zlhs = 1;
	}
	info = rinfo;
	if (info.reg >= MAX_REGISTERS) {
		info.reg = REG_UNSET;
	}
	for(i = 0; i < zlhs; i++) {
		struct reg_info linfo;
		linfo = arch_reg_lhs(state, ins, i);
		if ((linfo.reg == rinfo.reg) &&
			(linfo.reg >= MAX_REGISTERS)) {
			struct reg_info tinfo;
			tinfo = find_lhs_post_color(state, ins, i);
			if (tinfo.reg >= MAX_REGISTERS) {
				tinfo.reg = REG_UNSET;
			}
			info.regcm &= linfo.regcm;
			info.regcm &= tinfo.regcm;
			if (info.reg != REG_UNSET) {
				internal_error(state, ins, "register conflict");
			}
			if (info.regcm == 0) {
				internal_error(state, ins, "regcm conflict");
			}
			info.reg = tinfo.reg;
		}
	}
#if DEBUG_TRIPLE_COLOR
	fprintf(state->errout, "find_rhs_post_color(%p, %d) -> ( %d, %x)\n",
		ins, index, info.reg, info.regcm);
#endif
	return info;
}

static struct reg_info find_lhs_color(
	struct compile_state *state, struct triple *ins, int index)
{
	struct reg_info pre, post, info;
#if DEBUG_TRIPLE_COLOR
	fprintf(state->errout, "find_lhs_color(%p, %d)\n",
		ins, index);
#endif
	pre = find_lhs_pre_color(state, ins, index);
	post = find_lhs_post_color(state, ins, index);
	if ((pre.reg != post.reg) &&
		(pre.reg != REG_UNSET) &&
		(post.reg != REG_UNSET)) {
		internal_error(state, ins, "register conflict");
	}
	info.regcm = pre.regcm & post.regcm;
	info.reg = pre.reg;
	if (info.reg == REG_UNSET) {
		info.reg = post.reg;
	}
#if DEBUG_TRIPLE_COLOR
	fprintf(state->errout, "find_lhs_color(%p, %d) -> ( %d, %x) ... (%d, %x) (%d, %x)\n",
		ins, index, info.reg, info.regcm,
		pre.reg, pre.regcm, post.reg, post.regcm);
#endif
	return info;
}

static struct triple *post_copy(struct compile_state *state, struct triple *ins)
{
	struct triple_set *entry, *next;
	struct triple *out;
	struct reg_info info, rinfo;

	info = arch_reg_lhs(state, ins, 0);
	out = post_triple(state, ins, OP_COPY, ins->type, ins, 0);
	use_triple(RHS(out, 0), out);
	/* Get the users of ins to use out instead */
	for(entry = ins->use; entry; entry = next) {
		int i;
		next = entry->next;
		if (entry->member == out) {
			continue;
		}
		i = find_rhs_use(state, entry->member, ins);
		if (i < 0) {
			continue;
		}
		rinfo = arch_reg_rhs(state, entry->member, i);
		if ((info.reg == REG_UNNEEDED) && (rinfo.reg == REG_UNNEEDED)) {
			continue;
		}
		replace_rhs_use(state, ins, out, entry->member);
	}
	transform_to_arch_instruction(state, out);
	return out;
}

static struct triple *typed_pre_copy(
	struct compile_state *state, struct type *type, struct triple *ins, int index)
{
	/* Carefully insert enough operations so that I can
	 * enter any operation with a GPR32.
	 */
	struct triple *in;
	struct triple **expr;
	unsigned classes;
	struct reg_info info;
	int op;
	if (ins->op == OP_PHI) {
		internal_error(state, ins, "pre_copy on a phi?");
	}
	classes = arch_type_to_regcm(state, type);
	info = arch_reg_rhs(state, ins, index);
	expr = &RHS(ins, index);
	if ((info.regcm & classes) == 0) {
		FILE *fp = state->errout;
		fprintf(fp, "src_type: ");
		name_of(fp, ins->type);
 		fprintf(fp, "\ndst_type: ");
		name_of(fp, type);
		fprintf(fp, "\n");
		internal_error(state, ins, "pre_copy with no register classes");
	}
	op = OP_COPY;
	if (!equiv_types(type, (*expr)->type)) {
		op = OP_CONVERT;
	}
	in = pre_triple(state, ins, op, type, *expr, 0);
	unuse_triple(*expr, ins);
	*expr = in;
	use_triple(RHS(in, 0), in);
	use_triple(in, ins);
	transform_to_arch_instruction(state, in);
	return in;

}
static struct triple *pre_copy(
	struct compile_state *state, struct triple *ins, int index)
{
	return typed_pre_copy(state, RHS(ins, index)->type, ins, index);
}


static void insert_copies_to_phi(struct compile_state *state)
{
	/* To get out of ssa form we insert moves on the incoming
	 * edges to blocks containting phi functions.
	 */
	struct triple *first;
	struct triple *phi;

	/* Walk all of the operations to find the phi functions */
	first = state->first;
	for(phi = first->next; phi != first ; phi = phi->next) {
		struct block_set *set;
		struct block *block;
		struct triple **slot, *copy;
		int edge;
		if (phi->op != OP_PHI) {
			continue;
		}
		phi->id |= TRIPLE_FLAG_POST_SPLIT;
		block = phi->u.block;
		slot  = &RHS(phi, 0);
		/* Phi's that feed into mandatory live range joins
		 * cause nasty complications.  Insert a copy of
		 * the phi value so I never have to deal with
		 * that in the rest of the code.
		 */
		copy = post_copy(state, phi);
		copy->id |= TRIPLE_FLAG_PRE_SPLIT;
		/* Walk all of the incoming edges/blocks and insert moves.
		 */
		for(edge = 0, set = block->use; set; set = set->next, edge++) {
			struct block *eblock;
			struct triple *move;
			struct triple *val;
			struct triple *ptr;
			eblock = set->member;
			val = slot[edge];

			if (val == phi) {
				continue;
			}

			get_occurrence(val->occurrence);
			move = build_triple(state, OP_COPY, val->type, val, 0,
				val->occurrence);
			move->u.block = eblock;
			move->id |= TRIPLE_FLAG_PRE_SPLIT;
			use_triple(val, move);

			slot[edge] = move;
			unuse_triple(val, phi);
			use_triple(move, phi);

			/* Walk up the dominator tree until I have found the appropriate block */
			while(eblock && !tdominates(state, val, eblock->last)) {
				eblock = eblock->idom;
			}
			if (!eblock) {
				internal_error(state, phi, "Cannot find block dominated by %p",
					val);
			}

			/* Walk through the block backwards to find
			 * an appropriate location for the OP_COPY.
			 */
			for(ptr = eblock->last; ptr != eblock->first; ptr = ptr->prev) {
				struct triple **expr;
				if (ptr->op == OP_PIECE) {
					ptr = MISC(ptr, 0);
				}
				if ((ptr == phi) || (ptr == val)) {
					goto out;
				}
				expr = triple_lhs(state, ptr, 0);
				for(;expr; expr = triple_lhs(state, ptr, expr)) {
					if ((*expr) == val) {
						goto out;
					}
				}
				expr = triple_rhs(state, ptr, 0);
				for(;expr; expr = triple_rhs(state, ptr, expr)) {
					if ((*expr) == phi) {
						goto out;
					}
				}
			}
		out:
			if (triple_is_branch(state, ptr)) {
				internal_error(state, ptr,
					"Could not insert write to phi");
			}
			insert_triple(state, after_lhs(state, ptr), move);
			if (eblock->last == after_lhs(state, ptr)->prev) {
				eblock->last = move;
			}
			transform_to_arch_instruction(state, move);
		}
	}
	print_blocks(state, __func__, state->dbgout);
}

struct triple_reg_set;
struct reg_block;


static int do_triple_set(struct triple_reg_set **head,
	struct triple *member, struct triple *new_member)
{
	struct triple_reg_set **ptr, *new;
	if (!member)
		return 0;
	ptr = head;
	while(*ptr) {
		if ((*ptr)->member == member) {
			return 0;
		}
		ptr = &(*ptr)->next;
	}
	new = xcmalloc(sizeof(*new), "triple_set");
	new->member = member;
	new->new    = new_member;
	new->next   = *head;
	*head       = new;
	return 1;
}

static void do_triple_unset(struct triple_reg_set **head, struct triple *member)
{
	struct triple_reg_set *entry, **ptr;
	ptr = head;
	while(*ptr) {
		entry = *ptr;
		if (entry->member == member) {
			*ptr = entry->next;
			xfree(entry);
			return;
		}
		else {
			ptr = &entry->next;
		}
	}
}

static int in_triple(struct reg_block *rb, struct triple *in)
{
	return do_triple_set(&rb->in, in, 0);
}

#if DEBUG_ROMCC_WARNING
static void unin_triple(struct reg_block *rb, struct triple *unin)
{
	do_triple_unset(&rb->in, unin);
}
#endif

static int out_triple(struct reg_block *rb, struct triple *out)
{
	return do_triple_set(&rb->out, out, 0);
}
#if DEBUG_ROMCC_WARNING
static void unout_triple(struct reg_block *rb, struct triple *unout)
{
	do_triple_unset(&rb->out, unout);
}
#endif

static int initialize_regblock(struct reg_block *blocks,
	struct block *block, int vertex)
{
	struct block_set *user;
	if (!block || (blocks[block->vertex].block == block)) {
		return vertex;
	}
	vertex += 1;
	/* Renumber the blocks in a convenient fashion */
	block->vertex = vertex;
	blocks[vertex].block    = block;
	blocks[vertex].vertex   = vertex;
	for(user = block->use; user; user = user->next) {
		vertex = initialize_regblock(blocks, user->member, vertex);
	}
	return vertex;
}

static struct triple *part_to_piece(struct compile_state *state, struct triple *ins)
{
/* Part to piece is a best attempt and it cannot be correct all by
 * itself.  If various values are read as different sizes in different
 * parts of the code this function cannot work.  Or rather it cannot
 * work in conjunction with compute_variable_liftimes.  As the
 * analysis will get confused.
 */
	struct triple *base;
	unsigned reg;
	if (!is_lvalue(state, ins)) {
		return ins;
	}
	base = 0;
	reg = 0;
	while(ins && triple_is_part(state, ins) && (ins->op != OP_PIECE)) {
		base = MISC(ins, 0);
		switch(ins->op) {
		case OP_INDEX:
			reg += index_reg_offset(state, base->type, ins->u.cval)/REG_SIZEOF_REG;
			break;
		case OP_DOT:
			reg += field_reg_offset(state, base->type, ins->u.field)/REG_SIZEOF_REG;
			break;
		default:
			internal_error(state, ins, "unhandled part");
			break;
		}
		ins = base;
	}
	if (base) {
		if (reg > base->lhs) {
			internal_error(state, base, "part out of range?");
		}
		ins = LHS(base, reg);
	}
	return ins;
}

static int this_def(struct compile_state *state,
	struct triple *ins, struct triple *other)
{
	if (ins == other) {
		return 1;
	}
	if (ins->op == OP_WRITE) {
		ins = part_to_piece(state, MISC(ins, 0));
	}
	return ins == other;
}

static int phi_in(struct compile_state *state, struct reg_block *blocks,
	struct reg_block *rb, struct block *suc)
{
	/* Read the conditional input set of a successor block
	 * (i.e. the input to the phi nodes) and place it in the
	 * current blocks output set.
	 */
	struct block_set *set;
	struct triple *ptr;
	int edge;
	int done, change;
	change = 0;
	/* Find the edge I am coming in on */
	for(edge = 0, set = suc->use; set; set = set->next, edge++) {
		if (set->member == rb->block) {
			break;
		}
	}
	if (!set) {
		internal_error(state, 0, "Not coming on a control edge?");
	}
	for(done = 0, ptr = suc->first; !done; ptr = ptr->next) {
		struct triple **slot, *expr, *ptr2;
		int out_change, done2;
		done = (ptr == suc->last);
		if (ptr->op != OP_PHI) {
			continue;
		}
		slot = &RHS(ptr, 0);
		expr = slot[edge];
		out_change = out_triple(rb, expr);
		if (!out_change) {
			continue;
		}
		/* If we don't define the variable also plast it
		 * in the current blocks input set.
		 */
		ptr2 = rb->block->first;
		for(done2 = 0; !done2; ptr2 = ptr2->next) {
			if (this_def(state, ptr2, expr)) {
				break;
			}
			done2 = (ptr2 == rb->block->last);
		}
		if (!done2) {
			continue;
		}
		change |= in_triple(rb, expr);
	}
	return change;
}

static int reg_in(struct compile_state *state, struct reg_block *blocks,
	struct reg_block *rb, struct block *suc)
{
	struct triple_reg_set *in_set;
	int change;
	change = 0;
	/* Read the input set of a successor block
	 * and place it in the current blocks output set.
	 */
	in_set = blocks[suc->vertex].in;
	for(; in_set; in_set = in_set->next) {
		int out_change, done;
		struct triple *first, *last, *ptr;
		out_change = out_triple(rb, in_set->member);
		if (!out_change) {
			continue;
		}
		/* If we don't define the variable also place it
		 * in the current blocks input set.
		 */
		first = rb->block->first;
		last = rb->block->last;
		done = 0;
		for(ptr = first; !done; ptr = ptr->next) {
			if (this_def(state, ptr, in_set->member)) {
				break;
			}
			done = (ptr == last);
		}
		if (!done) {
			continue;
		}
		change |= in_triple(rb, in_set->member);
	}
	change |= phi_in(state, blocks, rb, suc);
	return change;
}

static int use_in(struct compile_state *state, struct reg_block *rb)
{
	/* Find the variables we use but don't define and add
	 * it to the current blocks input set.
	 */
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME is this O(N^2) algorithm bad?"
#endif
	struct block *block;
	struct triple *ptr;
	int done;
	int change;
	block = rb->block;
	change = 0;
	for(done = 0, ptr = block->last; !done; ptr = ptr->prev) {
		struct triple **expr;
		done = (ptr == block->first);
		/* The variable a phi function uses depends on the
		 * control flow, and is handled in phi_in, not
		 * here.
		 */
		if (ptr->op == OP_PHI) {
			continue;
		}
		expr = triple_rhs(state, ptr, 0);
		for(;expr; expr = triple_rhs(state, ptr, expr)) {
			struct triple *rhs, *test;
			int tdone;
			rhs = part_to_piece(state, *expr);
			if (!rhs) {
				continue;
			}

			/* See if rhs is defined in this block.
			 * A write counts as a definition.
			 */
			for(tdone = 0, test = ptr; !tdone; test = test->prev) {
				tdone = (test == block->first);
				if (this_def(state, test, rhs)) {
					rhs = 0;
					break;
				}
			}
			/* If I still have a valid rhs add it to in */
			change |= in_triple(rb, rhs);
		}
	}
	return change;
}

static struct reg_block *compute_variable_lifetimes(
	struct compile_state *state, struct basic_blocks *bb)
{
	struct reg_block *blocks;
	int change;
	blocks = xcmalloc(
		sizeof(*blocks)*(bb->last_vertex + 1), "reg_block");
	initialize_regblock(blocks, bb->last_block, 0);
	do {
		int i;
		change = 0;
		for(i = 1; i <= bb->last_vertex; i++) {
			struct block_set *edge;
			struct reg_block *rb;
			rb = &blocks[i];
			/* Add the all successor's input set to in */
			for(edge = rb->block->edges; edge; edge = edge->next) {
				change |= reg_in(state, blocks, rb, edge->member);
			}
			/* Add use to in... */
			change |= use_in(state, rb);
		}
	} while(change);
	return blocks;
}

static void free_variable_lifetimes(struct compile_state *state,
	struct basic_blocks *bb, struct reg_block *blocks)
{
	int i;
	/* free in_set && out_set on each block */
	for(i = 1; i <= bb->last_vertex; i++) {
		struct triple_reg_set *entry, *next;
		struct reg_block *rb;
		rb = &blocks[i];
		for(entry = rb->in; entry ; entry = next) {
			next = entry->next;
			do_triple_unset(&rb->in, entry->member);
		}
		for(entry = rb->out; entry; entry = next) {
			next = entry->next;
			do_triple_unset(&rb->out, entry->member);
		}
	}
	xfree(blocks);

}

typedef void (*wvl_cb_t)(
	struct compile_state *state,
	struct reg_block *blocks, struct triple_reg_set *live,
	struct reg_block *rb, struct triple *ins, void *arg);

static void walk_variable_lifetimes(struct compile_state *state,
	struct basic_blocks *bb, struct reg_block *blocks,
	wvl_cb_t cb, void *arg)
{
	int i;

	for(i = 1; i <= state->bb.last_vertex; i++) {
		struct triple_reg_set *live;
		struct triple_reg_set *entry, *next;
		struct triple *ptr, *prev;
		struct reg_block *rb;
		struct block *block;
		int done;

		/* Get the blocks */
		rb = &blocks[i];
		block = rb->block;

		/* Copy out into live */
		live = 0;
		for(entry = rb->out; entry; entry = next) {
			next = entry->next;
			do_triple_set(&live, entry->member, entry->new);
		}
		/* Walk through the basic block calculating live */
		for(done = 0, ptr = block->last; !done; ptr = prev) {
			struct triple **expr;

			prev = ptr->prev;
			done = (ptr == block->first);

			/* Ensure the current definition is in live */
			if (triple_is_def(state, ptr)) {
				do_triple_set(&live, ptr, 0);
			}

			/* Inform the callback function of what is
			 * going on.
			 */
			 cb(state, blocks, live, rb, ptr, arg);

			/* Remove the current definition from live */
			do_triple_unset(&live, ptr);

			/* Add the current uses to live.
			 *
			 * It is safe to skip phi functions because they do
			 * not have any block local uses, and the block
			 * output sets already properly account for what
			 * control flow depedent uses phi functions do have.
			 */
			if (ptr->op == OP_PHI) {
				continue;
			}
			expr = triple_rhs(state, ptr, 0);
			for(;expr; expr = triple_rhs(state, ptr, expr)) {
				/* If the triple is not a definition skip it. */
				if (!*expr || !triple_is_def(state, *expr)) {
					continue;
				}
				do_triple_set(&live, *expr, 0);
			}
		}
		/* Free live */
		for(entry = live; entry; entry = next) {
			next = entry->next;
			do_triple_unset(&live, entry->member);
		}
	}
}

struct print_live_variable_info {
	struct reg_block *rb;
	FILE *fp;
};
#if DEBUG_EXPLICIT_CLOSURES
static void print_live_variables_block(
	struct compile_state *state, struct block *block, void *arg)

{
	struct print_live_variable_info *info = arg;
	struct block_set *edge;
	FILE *fp = info->fp;
	struct reg_block *rb;
	struct triple *ptr;
	int phi_present;
	int done;
	rb = &info->rb[block->vertex];

	fprintf(fp, "\nblock: %p (%d),",
		block, 	block->vertex);
	for(edge = block->edges; edge; edge = edge->next) {
		fprintf(fp, " %p<-%p",
			edge->member,
			edge->member && edge->member->use?edge->member->use->member : 0);
	}
	fprintf(fp, "\n");
	if (rb->in) {
		struct triple_reg_set *in_set;
		fprintf(fp, "        in:");
		for(in_set = rb->in; in_set; in_set = in_set->next) {
			fprintf(fp, " %-10p", in_set->member);
		}
		fprintf(fp, "\n");
	}
	phi_present = 0;
	for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
		done = (ptr == block->last);
		if (ptr->op == OP_PHI) {
			phi_present = 1;
			break;
		}
	}
	if (phi_present) {
		int edge;
		for(edge = 0; edge < block->users; edge++) {
			fprintf(fp, "     in(%d):", edge);
			for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
				struct triple **slot;
				done = (ptr == block->last);
				if (ptr->op != OP_PHI) {
					continue;
				}
				slot = &RHS(ptr, 0);
				fprintf(fp, " %-10p", slot[edge]);
			}
			fprintf(fp, "\n");
		}
	}
	if (block->first->op == OP_LABEL) {
		fprintf(fp, "%p:\n", block->first);
	}
	for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
		done = (ptr == block->last);
		display_triple(fp, ptr);
	}
	if (rb->out) {
		struct triple_reg_set *out_set;
		fprintf(fp, "       out:");
		for(out_set = rb->out; out_set; out_set = out_set->next) {
			fprintf(fp, " %-10p", out_set->member);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

static void print_live_variables(struct compile_state *state,
	struct basic_blocks *bb, struct reg_block *rb, FILE *fp)
{
	struct print_live_variable_info info;
	info.rb = rb;
	info.fp = fp;
	fprintf(fp, "\nlive variables by block\n");
	walk_blocks(state, bb, print_live_variables_block, &info);

}
#endif

static int count_triples(struct compile_state *state)
{
	struct triple *first, *ins;
	int triples = 0;
	first = state->first;
	ins = first;
	do {
		triples++;
		ins = ins->next;
	} while (ins != first);
	return triples;
}


struct dead_triple {
	struct triple *triple;
	struct dead_triple *work_next;
	struct block *block;
	int old_id;
	int flags;
#define TRIPLE_FLAG_ALIVE 1
#define TRIPLE_FLAG_FREE  1
};

static void print_dead_triples(struct compile_state *state,
	struct dead_triple *dtriple)
{
	struct triple *first, *ins;
	struct dead_triple *dt;
	FILE *fp;
	if (!(state->compiler->debug & DEBUG_TRIPLES)) {
		return;
	}
	fp = state->dbgout;
	fprintf(fp, "--------------- dtriples ---------------\n");
	first = state->first;
	ins = first;
	do {
		dt = &dtriple[ins->id];
		if ((ins->op == OP_LABEL) && (ins->use)) {
			fprintf(fp, "\n%p:\n", ins);
		}
		fprintf(fp, "%c",
			(dt->flags & TRIPLE_FLAG_ALIVE)?' ': '-');
		display_triple(fp, ins);
		if (triple_is_branch(state, ins)) {
			fprintf(fp, "\n");
		}
		ins = ins->next;
	} while(ins != first);
	fprintf(fp, "\n");
}


static void awaken(
	struct compile_state *state,
	struct dead_triple *dtriple, struct triple **expr,
	struct dead_triple ***work_list_tail)
{
	struct triple *triple;
	struct dead_triple *dt;
	if (!expr) {
		return;
	}
	triple = *expr;
	if (!triple) {
		return;
	}
	if (triple->id <= 0)  {
		internal_error(state, triple, "bad triple id: %d",
			triple->id);
	}
	if (triple->op == OP_NOOP) {
		internal_error(state, triple, "awakening noop?");
		return;
	}
	dt = &dtriple[triple->id];
	if (!(dt->flags & TRIPLE_FLAG_ALIVE)) {
		dt->flags |= TRIPLE_FLAG_ALIVE;
		if (!dt->work_next) {
			**work_list_tail = dt;
			*work_list_tail = &dt->work_next;
		}
	}
}

static void eliminate_inefectual_code(struct compile_state *state)
{
	struct dead_triple *dtriple, *work_list, **work_list_tail, *dt;
	int triples, i;
	struct triple *first, *ins;

	if (!(state->compiler->flags & COMPILER_ELIMINATE_INEFECTUAL_CODE)) {
		return;
	}

	/* Setup the work list */
	work_list = 0;
	work_list_tail = &work_list;

	first = state->first;

	/* Count how many triples I have */
	triples = count_triples(state);

	/* Now put then in an array and mark all of the triples dead */
	dtriple = xcmalloc(sizeof(*dtriple) * (triples + 1), "dtriples");

	ins = first;
	i = 1;
	do {
		dtriple[i].triple = ins;
		dtriple[i].block  = block_of_triple(state, ins);
		dtriple[i].flags  = 0;
		dtriple[i].old_id = ins->id;
		ins->id = i;
		/* See if it is an operation we always keep */
		if (!triple_is_pure(state, ins, dtriple[i].old_id)) {
			awaken(state, dtriple, &ins, &work_list_tail);
		}
		i++;
		ins = ins->next;
	} while(ins != first);
	while(work_list) {
		struct block *block;
		struct dead_triple *dt;
		struct block_set *user;
		struct triple **expr;
		dt = work_list;
		work_list = dt->work_next;
		if (!work_list) {
			work_list_tail = &work_list;
		}
		/* Make certain the block the current instruction is in lives */
		block = block_of_triple(state, dt->triple);
		awaken(state, dtriple, &block->first, &work_list_tail);
		if (triple_is_branch(state, block->last)) {
			awaken(state, dtriple, &block->last, &work_list_tail);
		} else {
			awaken(state, dtriple, &block->last->next, &work_list_tail);
		}

		/* Wake up the data depencencies of this triple */
		expr = 0;
		do {
			expr = triple_rhs(state, dt->triple, expr);
			awaken(state, dtriple, expr, &work_list_tail);
		} while(expr);
		do {
			expr = triple_lhs(state, dt->triple, expr);
			awaken(state, dtriple, expr, &work_list_tail);
		} while(expr);
		do {
			expr = triple_misc(state, dt->triple, expr);
			awaken(state, dtriple, expr, &work_list_tail);
		} while(expr);
		/* Wake up the forward control dependencies */
		do {
			expr = triple_targ(state, dt->triple, expr);
			awaken(state, dtriple, expr, &work_list_tail);
		} while(expr);
		/* Wake up the reverse control dependencies of this triple */
		for(user = dt->block->ipdomfrontier; user; user = user->next) {
			struct triple *last;
			last = user->member->last;
			while((last->op == OP_NOOP) && (last != user->member->first)) {
#if DEBUG_ROMCC_WARNINGS
#warning "Should we bring the awakening noops back?"
#endif
				// internal_warning(state, last, "awakening noop?");
				last = last->prev;
			}
			awaken(state, dtriple, &last, &work_list_tail);
		}
	}
	print_dead_triples(state, dtriple);
	for(dt = &dtriple[1]; dt <= &dtriple[triples]; dt++) {
		if ((dt->triple->op == OP_NOOP) &&
			(dt->flags & TRIPLE_FLAG_ALIVE)) {
			internal_error(state, dt->triple, "noop effective?");
		}
		dt->triple->id = dt->old_id;	/* Restore the color */
		if (!(dt->flags & TRIPLE_FLAG_ALIVE)) {
			release_triple(state, dt->triple);
		}
	}
	xfree(dtriple);

	rebuild_ssa_form(state);

	print_blocks(state, __func__, state->dbgout);
}


static void insert_mandatory_copies(struct compile_state *state)
{
	struct triple *ins, *first;

	/* The object is with a minimum of inserted copies,
	 * to resolve in fundamental register conflicts between
	 * register value producers and consumers.
	 * Theoretically we may be greater than minimal when we
	 * are inserting copies before instructions but that
	 * case should be rare.
	 */
	first = state->first;
	ins = first;
	do {
		struct triple_set *entry, *next;
		struct triple *tmp;
		struct reg_info info;
		unsigned reg, regcm;
		int do_post_copy, do_pre_copy;
		tmp = 0;
		if (!triple_is_def(state, ins)) {
			goto next;
		}
		/* Find the architecture specific color information */
		info = find_lhs_pre_color(state, ins, 0);
		if (info.reg >= MAX_REGISTERS) {
			info.reg = REG_UNSET;
		}

		reg = REG_UNSET;
		regcm = arch_type_to_regcm(state, ins->type);
		do_pre_copy = 0;

		/* Walk through the uses of ins and check for conflicts */
		for(entry = ins->use; entry; entry = next) {
			struct reg_info rinfo;
			int i;
			next = entry->next;
			i = find_rhs_use(state, entry->member, ins);
			if (i < 0) {
				continue;
			}

			/* Find the users color requirements */
			rinfo = arch_reg_rhs(state, entry->member, i);
			if (rinfo.reg >= MAX_REGISTERS) {
				rinfo.reg = REG_UNSET;
			}

			/* See if I need a pre_copy */
			if (rinfo.reg != REG_UNSET) {
				if ((reg != REG_UNSET) && (reg != rinfo.reg)) {
					do_pre_copy = 1;
				}
				reg = rinfo.reg;
			}
			regcm &= rinfo.regcm;
			regcm = arch_regcm_normalize(state, regcm);
			if (regcm == 0) {
				do_pre_copy = 1;
			}
			/* Always use pre_copies for constants.
			 * They do not take up any registers until a
			 * copy places them in one.
			 */
			if ((info.reg == REG_UNNEEDED) &&
				(rinfo.reg != REG_UNNEEDED)) {
				do_pre_copy = 1;
			}
		}
		do_post_copy =
			!do_pre_copy &&
			(((info.reg != REG_UNSET) &&
				(reg != REG_UNSET) &&
				(info.reg != reg)) ||
			((info.regcm & regcm) == 0));

		reg = info.reg;
		regcm = info.regcm;
		/* Walk through the uses of ins and do a pre_copy or see if a post_copy is warranted */
		for(entry = ins->use; entry; entry = next) {
			struct reg_info rinfo;
			int i;
			next = entry->next;
			i = find_rhs_use(state, entry->member, ins);
			if (i < 0) {
				continue;
			}

			/* Find the users color requirements */
			rinfo = arch_reg_rhs(state, entry->member, i);
			if (rinfo.reg >= MAX_REGISTERS) {
				rinfo.reg = REG_UNSET;
			}

			/* Now see if it is time to do the pre_copy */
			if (rinfo.reg != REG_UNSET) {
				if (((reg != REG_UNSET) && (reg != rinfo.reg)) ||
					((regcm & rinfo.regcm) == 0) ||
					/* Don't let a mandatory coalesce sneak
					 * into a operation that is marked to prevent
					 * coalescing.
					 */
					((reg != REG_UNNEEDED) &&
					((ins->id & TRIPLE_FLAG_POST_SPLIT) ||
					(entry->member->id & TRIPLE_FLAG_PRE_SPLIT)))
					) {
					if (do_pre_copy) {
						struct triple *user;
						user = entry->member;
						if (RHS(user, i) != ins) {
							internal_error(state, user, "bad rhs");
						}
						tmp = pre_copy(state, user, i);
						tmp->id |= TRIPLE_FLAG_PRE_SPLIT;
						continue;
					} else {
						do_post_copy = 1;
					}
				}
				reg = rinfo.reg;
			}
			if ((regcm & rinfo.regcm) == 0) {
				if (do_pre_copy) {
					struct triple *user;
					user = entry->member;
					if (RHS(user, i) != ins) {
						internal_error(state, user, "bad rhs");
					}
					tmp = pre_copy(state, user, i);
					tmp->id |= TRIPLE_FLAG_PRE_SPLIT;
					continue;
				} else {
					do_post_copy = 1;
				}
			}
			regcm &= rinfo.regcm;

		}
		if (do_post_copy) {
			struct reg_info pre, post;
			tmp = post_copy(state, ins);
			tmp->id |= TRIPLE_FLAG_PRE_SPLIT;
			pre = arch_reg_lhs(state, ins, 0);
			post = arch_reg_lhs(state, tmp, 0);
			if ((pre.reg == post.reg) && (pre.regcm == post.regcm)) {
				internal_error(state, tmp, "useless copy");
			}
		}
	next:
		ins = ins->next;
	} while(ins != first);

	print_blocks(state, __func__, state->dbgout);
}


struct live_range_edge;
struct live_range_def;
struct live_range {
	struct live_range_edge *edges;
	struct live_range_def *defs;
/* Note. The list pointed to by defs is kept in order.
 * That is baring splits in the flow control
 * defs dominates defs->next wich dominates defs->next->next
 * etc.
 */
	unsigned color;
	unsigned classes;
	unsigned degree;
	unsigned length;
	struct live_range *group_next, **group_prev;
};

struct live_range_edge {
	struct live_range_edge *next;
	struct live_range *node;
};

struct live_range_def {
	struct live_range_def *next;
	struct live_range_def *prev;
	struct live_range *lr;
	struct triple *def;
	unsigned orig_id;
};

#define LRE_HASH_SIZE 2048
struct lre_hash {
	struct lre_hash *next;
	struct live_range *left;
	struct live_range *right;
};


struct reg_state {
	struct lre_hash *hash[LRE_HASH_SIZE];
	struct reg_block *blocks;
	struct live_range_def *lrd;
	struct live_range *lr;
	struct live_range *low, **low_tail;
	struct live_range *high, **high_tail;
	unsigned defs;
	unsigned ranges;
	int passes, max_passes;
};


struct print_interference_block_info {
	struct reg_state *rstate;
	FILE *fp;
	int need_edges;
};
static void print_interference_block(
	struct compile_state *state, struct block *block, void *arg)

{
	struct print_interference_block_info *info = arg;
	struct reg_state *rstate = info->rstate;
	struct block_set *edge;
	FILE *fp = info->fp;
	struct reg_block *rb;
	struct triple *ptr;
	int phi_present;
	int done;
	rb = &rstate->blocks[block->vertex];

	fprintf(fp, "\nblock: %p (%d),",
		block, 	block->vertex);
	for(edge = block->edges; edge; edge = edge->next) {
		fprintf(fp, " %p<-%p",
			edge->member,
			edge->member && edge->member->use?edge->member->use->member : 0);
	}
	fprintf(fp, "\n");
	if (rb->in) {
		struct triple_reg_set *in_set;
		fprintf(fp, "        in:");
		for(in_set = rb->in; in_set; in_set = in_set->next) {
			fprintf(fp, " %-10p", in_set->member);
		}
		fprintf(fp, "\n");
	}
	phi_present = 0;
	for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
		done = (ptr == block->last);
		if (ptr->op == OP_PHI) {
			phi_present = 1;
			break;
		}
	}
	if (phi_present) {
		int edge;
		for(edge = 0; edge < block->users; edge++) {
			fprintf(fp, "     in(%d):", edge);
			for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
				struct triple **slot;
				done = (ptr == block->last);
				if (ptr->op != OP_PHI) {
					continue;
				}
				slot = &RHS(ptr, 0);
				fprintf(fp, " %-10p", slot[edge]);
			}
			fprintf(fp, "\n");
		}
	}
	if (block->first->op == OP_LABEL) {
		fprintf(fp, "%p:\n", block->first);
	}
	for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
		struct live_range *lr;
		unsigned id;
		done = (ptr == block->last);
		lr = rstate->lrd[ptr->id].lr;

		id = ptr->id;
		ptr->id = rstate->lrd[id].orig_id;
		SET_REG(ptr->id, lr->color);
		display_triple(fp, ptr);
		ptr->id = id;

		if (triple_is_def(state, ptr) && (lr->defs == 0)) {
			internal_error(state, ptr, "lr has no defs!");
		}
		if (info->need_edges) {
			if (lr->defs) {
				struct live_range_def *lrd;
				fprintf(fp, "       range:");
				lrd = lr->defs;
				do {
					fprintf(fp, " %-10p", lrd->def);
					lrd = lrd->next;
				} while(lrd != lr->defs);
				fprintf(fp, "\n");
			}
			if (lr->edges > 0) {
				struct live_range_edge *edge;
				fprintf(fp, "       edges:");
				for(edge = lr->edges; edge; edge = edge->next) {
					struct live_range_def *lrd;
					lrd = edge->node->defs;
					do {
						fprintf(fp, " %-10p", lrd->def);
						lrd = lrd->next;
					} while(lrd != edge->node->defs);
					fprintf(fp, "|");
				}
				fprintf(fp, "\n");
			}
		}
		/* Do a bunch of sanity checks */
		valid_ins(state, ptr);
		if (ptr->id > rstate->defs) {
			internal_error(state, ptr, "Invalid triple id: %d",
				ptr->id);
		}
	}
	if (rb->out) {
		struct triple_reg_set *out_set;
		fprintf(fp, "       out:");
		for(out_set = rb->out; out_set; out_set = out_set->next) {
			fprintf(fp, " %-10p", out_set->member);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

static void print_interference_blocks(
	struct compile_state *state, struct reg_state *rstate, FILE *fp, int need_edges)
{
	struct print_interference_block_info info;
	info.rstate = rstate;
	info.fp = fp;
	info.need_edges = need_edges;
	fprintf(fp, "\nlive variables by block\n");
	walk_blocks(state, &state->bb, print_interference_block, &info);

}

static unsigned regc_max_size(struct compile_state *state, int classes)
{
	unsigned max_size;
	int i;
	max_size = 0;
	for(i = 0; i < MAX_REGC; i++) {
		if (classes & (1 << i)) {
			unsigned size;
			size = arch_regc_size(state, i);
			if (size > max_size) {
				max_size = size;
			}
		}
	}
	return max_size;
}

static int reg_is_reg(struct compile_state *state, int reg1, int reg2)
{
	unsigned equivs[MAX_REG_EQUIVS];
	int i;
	if ((reg1 < 0) || (reg1 >= MAX_REGISTERS)) {
		internal_error(state, 0, "invalid register");
	}
	if ((reg2 < 0) || (reg2 >= MAX_REGISTERS)) {
		internal_error(state, 0, "invalid register");
	}
	arch_reg_equivs(state, equivs, reg1);
	for(i = 0; (i < MAX_REG_EQUIVS) && equivs[i] != REG_UNSET; i++) {
		if (equivs[i] == reg2) {
			return 1;
		}
	}
	return 0;
}

static void reg_fill_used(struct compile_state *state, char *used, int reg)
{
	unsigned equivs[MAX_REG_EQUIVS];
	int i;
	if (reg == REG_UNNEEDED) {
		return;
	}
	arch_reg_equivs(state, equivs, reg);
	for(i = 0; (i < MAX_REG_EQUIVS) && equivs[i] != REG_UNSET; i++) {
		used[equivs[i]] = 1;
	}
	return;
}

static void reg_inc_used(struct compile_state *state, char *used, int reg)
{
	unsigned equivs[MAX_REG_EQUIVS];
	int i;
	if (reg == REG_UNNEEDED) {
		return;
	}
	arch_reg_equivs(state, equivs, reg);
	for(i = 0; (i < MAX_REG_EQUIVS) && equivs[i] != REG_UNSET; i++) {
		used[equivs[i]] += 1;
	}
	return;
}

static unsigned int hash_live_edge(
	struct live_range *left, struct live_range *right)
{
	unsigned int hash, val;
	unsigned long lval, rval;
	lval = ((unsigned long)left)/sizeof(struct live_range);
	rval = ((unsigned long)right)/sizeof(struct live_range);
	hash = 0;
	while(lval) {
		val = lval & 0xff;
		lval >>= 8;
		hash = (hash *263) + val;
	}
	while(rval) {
		val = rval & 0xff;
		rval >>= 8;
		hash = (hash *263) + val;
	}
	hash = hash & (LRE_HASH_SIZE - 1);
	return hash;
}

static struct lre_hash **lre_probe(struct reg_state *rstate,
	struct live_range *left, struct live_range *right)
{
	struct lre_hash **ptr;
	unsigned int index;
	/* Ensure left <= right */
	if (left > right) {
		struct live_range *tmp;
		tmp = left;
		left = right;
		right = tmp;
	}
	index = hash_live_edge(left, right);

	ptr = &rstate->hash[index];
	while(*ptr) {
		if (((*ptr)->left == left) && ((*ptr)->right == right)) {
			break;
		}
		ptr = &(*ptr)->next;
	}
	return ptr;
}

static int interfere(struct reg_state *rstate,
	struct live_range *left, struct live_range *right)
{
	struct lre_hash **ptr;
	ptr = lre_probe(rstate, left, right);
	return ptr && *ptr;
}

static void add_live_edge(struct reg_state *rstate,
	struct live_range *left, struct live_range *right)
{
	/* FIXME the memory allocation overhead is noticeable here... */
	struct lre_hash **ptr, *new_hash;
	struct live_range_edge *edge;

	if (left == right) {
		return;
	}
	if ((left == &rstate->lr[0]) || (right == &rstate->lr[0])) {
		return;
	}
	/* Ensure left <= right */
	if (left > right) {
		struct live_range *tmp;
		tmp = left;
		left = right;
		right = tmp;
	}
	ptr = lre_probe(rstate, left, right);
	if (*ptr) {
		return;
	}
#if 0
	fprintf(state->errout, "new_live_edge(%p, %p)\n",
		left, right);
#endif
	new_hash = xmalloc(sizeof(*new_hash), "lre_hash");
	new_hash->next  = *ptr;
	new_hash->left  = left;
	new_hash->right = right;
	*ptr = new_hash;

	edge = xmalloc(sizeof(*edge), "live_range_edge");
	edge->next   = left->edges;
	edge->node   = right;
	left->edges  = edge;
	left->degree += 1;

	edge = xmalloc(sizeof(*edge), "live_range_edge");
	edge->next    = right->edges;
	edge->node    = left;
	right->edges  = edge;
	right->degree += 1;
}

static void remove_live_edge(struct reg_state *rstate,
	struct live_range *left, struct live_range *right)
{
	struct live_range_edge *edge, **ptr;
	struct lre_hash **hptr, *entry;
	hptr = lre_probe(rstate, left, right);
	if (!hptr || !*hptr) {
		return;
	}
	entry = *hptr;
	*hptr = entry->next;
	xfree(entry);

	for(ptr = &left->edges; *ptr; ptr = &(*ptr)->next) {
		edge = *ptr;
		if (edge->node == right) {
			*ptr = edge->next;
			memset(edge, 0, sizeof(*edge));
			xfree(edge);
			right->degree--;
			break;
		}
	}
	for(ptr = &right->edges; *ptr; ptr = &(*ptr)->next) {
		edge = *ptr;
		if (edge->node == left) {
			*ptr = edge->next;
			memset(edge, 0, sizeof(*edge));
			xfree(edge);
			left->degree--;
			break;
		}
	}
}

static void remove_live_edges(struct reg_state *rstate, struct live_range *range)
{
	struct live_range_edge *edge, *next;
	for(edge = range->edges; edge; edge = next) {
		next = edge->next;
		remove_live_edge(rstate, range, edge->node);
	}
}

static void transfer_live_edges(struct reg_state *rstate,
	struct live_range *dest, struct live_range *src)
{
	struct live_range_edge *edge, *next;
	for(edge = src->edges; edge; edge = next) {
		struct live_range *other;
		next = edge->next;
		other = edge->node;
		remove_live_edge(rstate, src, other);
		add_live_edge(rstate, dest, other);
	}
}


/* Interference graph...
 *
 * new(n) --- Return a graph with n nodes but no edges.
 * add(g,x,y) --- Return a graph including g with an between x and y
 * interfere(g, x, y) --- Return true if there exists an edge between the nodes
 *                x and y in the graph g
 * degree(g, x) --- Return the degree of the node x in the graph g
 * neighbors(g, x, f) --- Apply function f to each neighbor of node x in the graph g
 *
 * Implement with a hash table && a set of adjcency vectors.
 * The hash table supports constant time implementations of add and interfere.
 * The adjacency vectors support an efficient implementation of neighbors.
 */

/*
 *     +---------------------------------------------------+
 *     |         +--------------+                          |
 *     v         v              |                          |
 * renumber -> build graph -> colalesce -> spill_costs -> simplify -> select
 *
 * -- In simplify implment optimistic coloring... (No backtracking)
 * -- Implement Rematerialization it is the only form of spilling we can perform
 *    Essentially this means dropping a constant from a register because
 *    we can regenerate it later.
 *
 * --- Very conservative colalescing (don't colalesce just mark the opportunities)
 *     coalesce at phi points...
 * --- Bias coloring if at all possible do the coalesing a compile time.
 *
 *
 */

#if DEBUG_ROMCC_WARNING
static void different_colored(
	struct compile_state *state, struct reg_state *rstate,
	struct triple *parent, struct triple *ins)
{
	struct live_range *lr;
	struct triple **expr;
	lr = rstate->lrd[ins->id].lr;
	expr = triple_rhs(state, ins, 0);
	for(;expr; expr = triple_rhs(state, ins, expr)) {
		struct live_range *lr2;
		if (!*expr || (*expr == parent) || (*expr == ins)) {
			continue;
		}
		lr2 = rstate->lrd[(*expr)->id].lr;
		if (lr->color == lr2->color) {
			internal_error(state, ins, "live range too big");
		}
	}
}
#endif

static struct live_range *coalesce_ranges(
	struct compile_state *state, struct reg_state *rstate,
	struct live_range *lr1, struct live_range *lr2)
{
	struct live_range_def *head, *mid1, *mid2, *end, *lrd;
	unsigned color;
	unsigned classes;
	if (lr1 == lr2) {
		return lr1;
	}
	if (!lr1->defs || !lr2->defs) {
		internal_error(state, 0,
			"cannot coalese dead live ranges");
	}
	if ((lr1->color == REG_UNNEEDED) ||
		(lr2->color == REG_UNNEEDED)) {
		internal_error(state, 0,
			"cannot coalesce live ranges without a possible color");
	}
	if ((lr1->color != lr2->color) &&
		(lr1->color != REG_UNSET) &&
		(lr2->color != REG_UNSET)) {
		internal_error(state, lr1->defs->def,
			"cannot coalesce live ranges of different colors");
	}
	color = lr1->color;
	if (color == REG_UNSET) {
		color = lr2->color;
	}
	classes = lr1->classes & lr2->classes;
	if (!classes) {
		internal_error(state, lr1->defs->def,
			"cannot coalesce live ranges with dissimilar register classes");
	}
	if (state->compiler->debug & DEBUG_COALESCING) {
		FILE *fp = state->errout;
		fprintf(fp, "coalescing:");
		lrd = lr1->defs;
		do {
			fprintf(fp, " %p", lrd->def);
			lrd = lrd->next;
		} while(lrd != lr1->defs);
		fprintf(fp, " |");
		lrd = lr2->defs;
		do {
			fprintf(fp, " %p", lrd->def);
			lrd = lrd->next;
		} while(lrd != lr2->defs);
		fprintf(fp, "\n");
	}
	/* If there is a clear dominate live range put it in lr1,
	 * For purposes of this test phi functions are
	 * considered dominated by the definitions that feed into
	 * them.
	 */
	if ((lr1->defs->prev->def->op == OP_PHI) ||
		((lr2->defs->prev->def->op != OP_PHI) &&
		tdominates(state, lr2->defs->def, lr1->defs->def))) {
		struct live_range *tmp;
		tmp = lr1;
		lr1 = lr2;
		lr2 = tmp;
	}
#if 0
	if (lr1->defs->orig_id  & TRIPLE_FLAG_POST_SPLIT) {
		fprintf(state->errout, "lr1 post\n");
	}
	if (lr1->defs->orig_id & TRIPLE_FLAG_PRE_SPLIT) {
		fprintf(state->errout, "lr1 pre\n");
	}
	if (lr2->defs->orig_id  & TRIPLE_FLAG_POST_SPLIT) {
		fprintf(state->errout, "lr2 post\n");
	}
	if (lr2->defs->orig_id & TRIPLE_FLAG_PRE_SPLIT) {
		fprintf(state->errout, "lr2 pre\n");
	}
#endif
#if 0
	fprintf(state->errout, "coalesce color1(%p): %3d color2(%p) %3d\n",
		lr1->defs->def,
		lr1->color,
		lr2->defs->def,
		lr2->color);
#endif

	/* Append lr2 onto lr1 */
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME should this be a merge instead of a splice?"
#endif
	/* This FIXME item applies to the correctness of live_range_end
	 * and to the necessity of making multiple passes of coalesce_live_ranges.
	 * A failure to find some coalesce opportunities in coaleace_live_ranges
	 * does not impact the correct of the compiler just the efficiency with
	 * which registers are allocated.
	 */
	head = lr1->defs;
	mid1 = lr1->defs->prev;
	mid2 = lr2->defs;
	end  = lr2->defs->prev;

	head->prev = end;
	end->next  = head;

	mid1->next = mid2;
	mid2->prev = mid1;

	/* Fixup the live range in the added live range defs */
	lrd = head;
	do {
		lrd->lr = lr1;
		lrd = lrd->next;
	} while(lrd != head);

	/* Mark lr2 as free. */
	lr2->defs = 0;
	lr2->color = REG_UNNEEDED;
	lr2->classes = 0;

	if (!lr1->defs) {
		internal_error(state, 0, "lr1->defs == 0 ?");
	}

	lr1->color   = color;
	lr1->classes = classes;

	/* Keep the graph in sync by transferring the edges from lr2 to lr1 */
	transfer_live_edges(rstate, lr1, lr2);

	return lr1;
}

static struct live_range_def *live_range_head(
	struct compile_state *state, struct live_range *lr,
	struct live_range_def *last)
{
	struct live_range_def *result;
	result = 0;
	if (last == 0) {
		result = lr->defs;
	}
	else if (!tdominates(state, lr->defs->def, last->next->def)) {
		result = last->next;
	}
	return result;
}

static struct live_range_def *live_range_end(
	struct compile_state *state, struct live_range *lr,
	struct live_range_def *last)
{
	struct live_range_def *result;
	result = 0;
	if (last == 0) {
		result = lr->defs->prev;
	}
	else if (!tdominates(state, last->prev->def, lr->defs->prev->def)) {
		result = last->prev;
	}
	return result;
}


static void initialize_live_ranges(
	struct compile_state *state, struct reg_state *rstate)
{
	struct triple *ins, *first;
	size_t count, size;
	int i, j;

	first = state->first;
	/* First count how many instructions I have.
	 */
	count = count_triples(state);
	/* Potentially I need one live range definitions for each
	 * instruction.
	 */
	rstate->defs = count;
	/* Potentially I need one live range for each instruction
	 * plus an extra for the dummy live range.
	 */
	rstate->ranges = count + 1;
	size = sizeof(rstate->lrd[0]) * rstate->defs;
	rstate->lrd = xcmalloc(size, "live_range_def");
	size = sizeof(rstate->lr[0]) * rstate->ranges;
	rstate->lr  = xcmalloc(size, "live_range");

	/* Setup the dummy live range */
	rstate->lr[0].classes = 0;
	rstate->lr[0].color = REG_UNSET;
	rstate->lr[0].defs = 0;
	i = j = 0;
	ins = first;
	do {
		/* If the triple is a variable give it a live range */
		if (triple_is_def(state, ins)) {
			struct reg_info info;
			/* Find the architecture specific color information */
			info = find_def_color(state, ins);
			i++;
			rstate->lr[i].defs    = &rstate->lrd[j];
			rstate->lr[i].color   = info.reg;
			rstate->lr[i].classes = info.regcm;
			rstate->lr[i].degree  = 0;
			rstate->lrd[j].lr = &rstate->lr[i];
		}
		/* Otherwise give the triple the dummy live range. */
		else {
			rstate->lrd[j].lr = &rstate->lr[0];
		}

		/* Initialize the live_range_def */
		rstate->lrd[j].next    = &rstate->lrd[j];
		rstate->lrd[j].prev    = &rstate->lrd[j];
		rstate->lrd[j].def     = ins;
		rstate->lrd[j].orig_id = ins->id;
		ins->id = j;

		j++;
		ins = ins->next;
	} while(ins != first);
	rstate->ranges = i;

	/* Make a second pass to handle architecture specific register
	 * constraints.
	 */
	ins = first;
	do {
		int zlhs, zrhs, i, j;
		if (ins->id > rstate->defs) {
			internal_error(state, ins, "bad id");
		}

		/* Walk through the template of ins and coalesce live ranges */
		zlhs = ins->lhs;
		if ((zlhs == 0) && triple_is_def(state, ins)) {
			zlhs = 1;
		}
		zrhs = ins->rhs;

		if (state->compiler->debug & DEBUG_COALESCING2) {
			fprintf(state->errout, "mandatory coalesce: %p %d %d\n",
				ins, zlhs, zrhs);
		}

		for(i = 0; i < zlhs; i++) {
			struct reg_info linfo;
			struct live_range_def *lhs;
			linfo = arch_reg_lhs(state, ins, i);
			if (linfo.reg < MAX_REGISTERS) {
				continue;
			}
			if (triple_is_def(state, ins)) {
				lhs = &rstate->lrd[ins->id];
			} else {
				lhs = &rstate->lrd[LHS(ins, i)->id];
			}

			if (state->compiler->debug & DEBUG_COALESCING2) {
				fprintf(state->errout, "coalesce lhs(%d): %p %d\n",
					i, lhs, linfo.reg);
			}

			for(j = 0; j < zrhs; j++) {
				struct reg_info rinfo;
				struct live_range_def *rhs;
				rinfo = arch_reg_rhs(state, ins, j);
				if (rinfo.reg < MAX_REGISTERS) {
					continue;
				}
				rhs = &rstate->lrd[RHS(ins, j)->id];

				if (state->compiler->debug & DEBUG_COALESCING2) {
					fprintf(state->errout, "coalesce rhs(%d): %p %d\n",
						j, rhs, rinfo.reg);
				}

				if (rinfo.reg == linfo.reg) {
					coalesce_ranges(state, rstate,
						lhs->lr, rhs->lr);
				}
			}
		}
		ins = ins->next;
	} while(ins != first);
}

static void graph_ins(
	struct compile_state *state,
	struct reg_block *blocks, struct triple_reg_set *live,
	struct reg_block *rb, struct triple *ins, void *arg)
{
	struct reg_state *rstate = arg;
	struct live_range *def;
	struct triple_reg_set *entry;

	/* If the triple is not a definition
	 * we do not have a definition to add to
	 * the interference graph.
	 */
	if (!triple_is_def(state, ins)) {
		return;
	}
	def = rstate->lrd[ins->id].lr;

	/* Create an edge between ins and everything that is
	 * alive, unless the live_range cannot share
	 * a physical register with ins.
	 */
	for(entry = live; entry; entry = entry->next) {
		struct live_range *lr;
		if (entry->member->id > rstate->defs) {
			internal_error(state, 0, "bad entry?");
		}
		lr = rstate->lrd[entry->member->id].lr;
		if (def == lr) {
			continue;
		}
		if (!arch_regcm_intersect(def->classes, lr->classes)) {
			continue;
		}
		add_live_edge(rstate, def, lr);
	}
	return;
}

#if DEBUG_CONSISTENCY > 1
static struct live_range *get_verify_live_range(
	struct compile_state *state, struct reg_state *rstate, struct triple *ins)
{
	struct live_range *lr;
	struct live_range_def *lrd;
	int ins_found;
	if ((ins->id < 0) || (ins->id > rstate->defs)) {
		internal_error(state, ins, "bad ins?");
	}
	lr = rstate->lrd[ins->id].lr;
	ins_found = 0;
	lrd = lr->defs;
	do {
		if (lrd->def == ins) {
			ins_found = 1;
		}
		lrd = lrd->next;
	} while(lrd != lr->defs);
	if (!ins_found) {
		internal_error(state, ins, "ins not in live range");
	}
	return lr;
}

static void verify_graph_ins(
	struct compile_state *state,
	struct reg_block *blocks, struct triple_reg_set *live,
	struct reg_block *rb, struct triple *ins, void *arg)
{
	struct reg_state *rstate = arg;
	struct triple_reg_set *entry1, *entry2;


	/* Compare live against edges and make certain the code is working */
	for(entry1 = live; entry1; entry1 = entry1->next) {
		struct live_range *lr1;
		lr1 = get_verify_live_range(state, rstate, entry1->member);
		for(entry2 = live; entry2; entry2 = entry2->next) {
			struct live_range *lr2;
			struct live_range_edge *edge2;
			int lr1_found;
			int lr2_degree;
			if (entry2 == entry1) {
				continue;
			}
			lr2 = get_verify_live_range(state, rstate, entry2->member);
			if (lr1 == lr2) {
				internal_error(state, entry2->member,
					"live range with 2 values simultaneously alive");
			}
			if (!arch_regcm_intersect(lr1->classes, lr2->classes)) {
				continue;
			}
			if (!interfere(rstate, lr1, lr2)) {
				internal_error(state, entry2->member,
					"edges don't interfere?");
			}

			lr1_found = 0;
			lr2_degree = 0;
			for(edge2 = lr2->edges; edge2; edge2 = edge2->next) {
				lr2_degree++;
				if (edge2->node == lr1) {
					lr1_found = 1;
				}
			}
			if (lr2_degree != lr2->degree) {
				internal_error(state, entry2->member,
					"computed degree: %d does not match reported degree: %d\n",
					lr2_degree, lr2->degree);
			}
			if (!lr1_found) {
				internal_error(state, entry2->member, "missing edge");
			}
		}
	}
	return;
}
#endif

static void print_interference_ins(
	struct compile_state *state,
	struct reg_block *blocks, struct triple_reg_set *live,
	struct reg_block *rb, struct triple *ins, void *arg)
{
	struct reg_state *rstate = arg;
	struct live_range *lr;
	unsigned id;
	FILE *fp = state->dbgout;

	lr = rstate->lrd[ins->id].lr;
	id = ins->id;
	ins->id = rstate->lrd[id].orig_id;
	SET_REG(ins->id, lr->color);
	display_triple(state->dbgout, ins);
	ins->id = id;

	if (lr->defs) {
		struct live_range_def *lrd;
		fprintf(fp, "       range:");
		lrd = lr->defs;
		do {
			fprintf(fp, " %-10p", lrd->def);
			lrd = lrd->next;
		} while(lrd != lr->defs);
		fprintf(fp, "\n");
	}
	if (live) {
		struct triple_reg_set *entry;
		fprintf(fp, "        live:");
		for(entry = live; entry; entry = entry->next) {
			fprintf(fp, " %-10p", entry->member);
		}
		fprintf(fp, "\n");
	}
	if (lr->edges) {
		struct live_range_edge *entry;
		fprintf(fp, "       edges:");
		for(entry = lr->edges; entry; entry = entry->next) {
			struct live_range_def *lrd;
			lrd = entry->node->defs;
			do {
				fprintf(fp, " %-10p", lrd->def);
				lrd = lrd->next;
			} while(lrd != entry->node->defs);
			fprintf(fp, "|");
		}
		fprintf(fp, "\n");
	}
	if (triple_is_branch(state, ins)) {
		fprintf(fp, "\n");
	}
	return;
}

static int coalesce_live_ranges(
	struct compile_state *state, struct reg_state *rstate)
{
	/* At the point where a value is moved from one
	 * register to another that value requires two
	 * registers, thus increasing register pressure.
	 * Live range coaleescing reduces the register
	 * pressure by keeping a value in one register
	 * longer.
	 *
	 * In the case of a phi function all paths leading
	 * into it must be allocated to the same register
	 * otherwise the phi function may not be removed.
	 *
	 * Forcing a value to stay in a single register
	 * for an extended period of time does have
	 * limitations when applied to non homogenous
	 * register pool.
	 *
	 * The two cases I have identified are:
	 * 1) Two forced register assignments may
	 *    collide.
	 * 2) Registers may go unused because they
	 *    are only good for storing the value
	 *    and not manipulating it.
	 *
	 * Because of this I need to split live ranges,
	 * even outside of the context of coalesced live
	 * ranges.  The need to split live ranges does
	 * impose some constraints on live range coalescing.
	 *
	 * - Live ranges may not be coalesced across phi
	 *   functions.  This creates a 2 headed live
	 *   range that cannot be sanely split.
	 *
	 * - phi functions (coalesced in initialize_live_ranges)
	 *   are handled as pre split live ranges so we will
	 *   never attempt to split them.
	 */
	int coalesced;
	int i;

	coalesced = 0;
	for(i = 0; i <= rstate->ranges; i++) {
		struct live_range *lr1;
		struct live_range_def *lrd1;
		lr1 = &rstate->lr[i];
		if (!lr1->defs) {
			continue;
		}
		lrd1 = live_range_end(state, lr1, 0);
		for(; lrd1; lrd1 = live_range_end(state, lr1, lrd1)) {
			struct triple_set *set;
			if (lrd1->def->op != OP_COPY) {
				continue;
			}
			/* Skip copies that are the result of a live range split. */
			if (lrd1->orig_id & TRIPLE_FLAG_POST_SPLIT) {
				continue;
			}
			for(set = lrd1->def->use; set; set = set->next) {
				struct live_range_def *lrd2;
				struct live_range *lr2, *res;

				lrd2 = &rstate->lrd[set->member->id];

				/* Don't coalesce with instructions
				 * that are the result of a live range
				 * split.
				 */
				if (lrd2->orig_id & TRIPLE_FLAG_PRE_SPLIT) {
					continue;
				}
				lr2 = rstate->lrd[set->member->id].lr;
				if (lr1 == lr2) {
					continue;
				}
				if ((lr1->color != lr2->color) &&
					(lr1->color != REG_UNSET) &&
					(lr2->color != REG_UNSET)) {
					continue;
				}
				if ((lr1->classes & lr2->classes) == 0) {
					continue;
				}

				if (interfere(rstate, lr1, lr2)) {
					continue;
				}

				res = coalesce_ranges(state, rstate, lr1, lr2);
				coalesced += 1;
				if (res != lr1) {
					goto next;
				}
			}
		}
	next:
		;
	}
	return coalesced;
}


static void fix_coalesce_conflicts(struct compile_state *state,
	struct reg_block *blocks, struct triple_reg_set *live,
	struct reg_block *rb, struct triple *ins, void *arg)
{
	int *conflicts = arg;
	int zlhs, zrhs, i, j;

	/* See if we have a mandatory coalesce operation between
	 * a lhs and a rhs value.  If so and the rhs value is also
	 * alive then this triple needs to be pre copied.  Otherwise
	 * we would have two definitions in the same live range simultaneously
	 * alive.
	 */
	zlhs = ins->lhs;
	if ((zlhs == 0) && triple_is_def(state, ins)) {
		zlhs = 1;
	}
	zrhs = ins->rhs;
	for(i = 0; i < zlhs; i++) {
		struct reg_info linfo;
		linfo = arch_reg_lhs(state, ins, i);
		if (linfo.reg < MAX_REGISTERS) {
			continue;
		}
		for(j = 0; j < zrhs; j++) {
			struct reg_info rinfo;
			struct triple *rhs;
			struct triple_reg_set *set;
			int found;
			found = 0;
			rinfo = arch_reg_rhs(state, ins, j);
			if (rinfo.reg != linfo.reg) {
				continue;
			}
			rhs = RHS(ins, j);
			for(set = live; set && !found; set = set->next) {
				if (set->member == rhs) {
					found = 1;
				}
			}
			if (found) {
				struct triple *copy;
				copy = pre_copy(state, ins, j);
				copy->id |= TRIPLE_FLAG_PRE_SPLIT;
				(*conflicts)++;
			}
		}
	}
	return;
}

static int correct_coalesce_conflicts(
	struct compile_state *state, struct reg_block *blocks)
{
	int conflicts;
	conflicts = 0;
	walk_variable_lifetimes(state, &state->bb, blocks,
		fix_coalesce_conflicts, &conflicts);
	return conflicts;
}

static void replace_set_use(struct compile_state *state,
	struct triple_reg_set *head, struct triple *orig, struct triple *new)
{
	struct triple_reg_set *set;
	for(set = head; set; set = set->next) {
		if (set->member == orig) {
			set->member = new;
		}
	}
}

static void replace_block_use(struct compile_state *state,
	struct reg_block *blocks, struct triple *orig, struct triple *new)
{
	int i;
#if DEBUG_ROMCC_WARNINGS
#warning "WISHLIST visit just those blocks that need it *"
#endif
	for(i = 1; i <= state->bb.last_vertex; i++) {
		struct reg_block *rb;
		rb = &blocks[i];
		replace_set_use(state, rb->in, orig, new);
		replace_set_use(state, rb->out, orig, new);
	}
}

static void color_instructions(struct compile_state *state)
{
	struct triple *ins, *first;
	first = state->first;
	ins = first;
	do {
		if (triple_is_def(state, ins)) {
			struct reg_info info;
			info = find_lhs_color(state, ins, 0);
			if (info.reg >= MAX_REGISTERS) {
				info.reg = REG_UNSET;
			}
			SET_INFO(ins->id, info);
		}
		ins = ins->next;
	} while(ins != first);
}

static struct reg_info read_lhs_color(
	struct compile_state *state, struct triple *ins, int index)
{
	struct reg_info info;
	if ((index == 0) && triple_is_def(state, ins)) {
		info.reg   = ID_REG(ins->id);
		info.regcm = ID_REGCM(ins->id);
	}
	else if (index < ins->lhs) {
		info = read_lhs_color(state, LHS(ins, index), 0);
	}
	else {
		internal_error(state, ins, "Bad lhs %d", index);
		info.reg = REG_UNSET;
		info.regcm = 0;
	}
	return info;
}

static struct triple *resolve_tangle(
	struct compile_state *state, struct triple *tangle)
{
	struct reg_info info, uinfo;
	struct triple_set *set, *next;
	struct triple *copy;

#if DEBUG_ROMCC_WARNINGS
#warning "WISHLIST recalculate all affected instructions colors"
#endif
	info = find_lhs_color(state, tangle, 0);
	for(set = tangle->use; set; set = next) {
		struct triple *user;
		int i, zrhs;
		next = set->next;
		user = set->member;
		zrhs = user->rhs;
		for(i = 0; i < zrhs; i++) {
			if (RHS(user, i) != tangle) {
				continue;
			}
			uinfo = find_rhs_post_color(state, user, i);
			if (uinfo.reg == info.reg) {
				copy = pre_copy(state, user, i);
				copy->id |= TRIPLE_FLAG_PRE_SPLIT;
				SET_INFO(copy->id, uinfo);
			}
		}
	}
	copy = 0;
	uinfo = find_lhs_pre_color(state, tangle, 0);
	if (uinfo.reg == info.reg) {
		struct reg_info linfo;
		copy = post_copy(state, tangle);
		copy->id |= TRIPLE_FLAG_PRE_SPLIT;
		linfo = find_lhs_color(state, copy, 0);
		SET_INFO(copy->id, linfo);
	}
	info = find_lhs_color(state, tangle, 0);
	SET_INFO(tangle->id, info);

	return copy;
}


static void fix_tangles(struct compile_state *state,
	struct reg_block *blocks, struct triple_reg_set *live,
	struct reg_block *rb, struct triple *ins, void *arg)
{
	int *tangles = arg;
	struct triple *tangle;
	do {
		char used[MAX_REGISTERS];
		struct triple_reg_set *set;
		tangle = 0;

		/* Find out which registers have multiple uses at this point */
		memset(used, 0, sizeof(used));
		for(set = live; set; set = set->next) {
			struct reg_info info;
			info = read_lhs_color(state, set->member, 0);
			if (info.reg == REG_UNSET) {
				continue;
			}
			reg_inc_used(state, used, info.reg);
		}

		/* Now find the least dominated definition of a register in
		 * conflict I have seen so far.
		 */
		for(set = live; set; set = set->next) {
			struct reg_info info;
			info = read_lhs_color(state, set->member, 0);
			if (used[info.reg] < 2) {
				continue;
			}
			/* Changing copies that feed into phi functions
			 * is incorrect.
			 */
			if (set->member->use &&
				(set->member->use->member->op == OP_PHI)) {
				continue;
			}
			if (!tangle || tdominates(state, set->member, tangle)) {
				tangle = set->member;
			}
		}
		/* If I have found a tangle resolve it */
		if (tangle) {
			struct triple *post_copy;
			(*tangles)++;
			post_copy = resolve_tangle(state, tangle);
			if (post_copy) {
				replace_block_use(state, blocks, tangle, post_copy);
			}
			if (post_copy && (tangle != ins)) {
				replace_set_use(state, live, tangle, post_copy);
			}
		}
	} while(tangle);
	return;
}

static int correct_tangles(
	struct compile_state *state, struct reg_block *blocks)
{
	int tangles;
	tangles = 0;
	color_instructions(state);
	walk_variable_lifetimes(state, &state->bb, blocks,
		fix_tangles, &tangles);
	return tangles;
}


static void ids_from_rstate(struct compile_state *state, struct reg_state *rstate);
static void cleanup_rstate(struct compile_state *state, struct reg_state *rstate);

struct triple *find_constrained_def(
	struct compile_state *state, struct live_range *range, struct triple *constrained)
{
	struct live_range_def *lrd, *lrd_next;
	lrd_next = range->defs;
	do {
		struct reg_info info;
		unsigned regcm;

		lrd = lrd_next;
		lrd_next = lrd->next;

		regcm = arch_type_to_regcm(state, lrd->def->type);
		info = find_lhs_color(state, lrd->def, 0);
		regcm      = arch_regcm_reg_normalize(state, regcm);
		info.regcm = arch_regcm_reg_normalize(state, info.regcm);
		/* If the 2 register class masks are equal then
		 * the current register class is not constrained.
		 */
		if (regcm == info.regcm) {
			continue;
		}

		/* If there is just one use.
		 * That use cannot accept a larger register class.
		 * There are no intervening definitions except
		 * definitions that feed into that use.
		 * Then a triple is not constrained.
		 * FIXME handle this case!
		 */
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME ignore cases that cannot be fixed (a definition followed by a use)"
#endif


		/* Of the constrained live ranges deal with the
		 * least dominated one first.
		 */
		if (state->compiler->debug & DEBUG_RANGE_CONFLICTS) {
			fprintf(state->errout, "canidate: %p %-8s regcm: %x %x\n",
				lrd->def, tops(lrd->def->op), regcm, info.regcm);
		}
		if (!constrained ||
			tdominates(state, lrd->def, constrained))
		{
			constrained = lrd->def;
		}
	} while(lrd_next != range->defs);
	return constrained;
}

static int split_constrained_ranges(
	struct compile_state *state, struct reg_state *rstate,
	struct live_range *range)
{
	/* Walk through the edges in conflict and our current live
	 * range, and find definitions that are more severly constrained
	 * than they type of data they contain require.
	 *
	 * Then pick one of those ranges and relax the constraints.
	 */
	struct live_range_edge *edge;
	struct triple *constrained;

	constrained = 0;
	for(edge = range->edges; edge; edge = edge->next) {
		constrained = find_constrained_def(state, edge->node, constrained);
	}
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME should I call find_constrained_def here only if no previous constrained def was found?"
#endif
	if (!constrained) {
		constrained = find_constrained_def(state, range, constrained);
	}

	if (state->compiler->debug & DEBUG_RANGE_CONFLICTS) {
		fprintf(state->errout, "constrained: ");
		display_triple(state->errout, constrained);
	}
	if (constrained) {
		ids_from_rstate(state, rstate);
		cleanup_rstate(state, rstate);
		resolve_tangle(state, constrained);
	}
	return !!constrained;
}

static int split_ranges(
	struct compile_state *state, struct reg_state *rstate,
	char *used, struct live_range *range)
{
	int split;
	if (state->compiler->debug & DEBUG_RANGE_CONFLICTS) {
		fprintf(state->errout, "split_ranges %d %s %p\n",
			rstate->passes, tops(range->defs->def->op), range->defs->def);
	}
	if ((range->color == REG_UNNEEDED) ||
		(rstate->passes >= rstate->max_passes)) {
		return 0;
	}
	split = split_constrained_ranges(state, rstate, range);

	/* Ideally I would split the live range that will not be used
	 * for the longest period of time in hopes that this will
	 * (a) allow me to spill a register or
	 * (b) allow me to place a value in another register.
	 *
	 * So far I don't have a test case for this, the resolving
	 * of mandatory constraints has solved all of my
	 * know issues.  So I have chosen not to write any
	 * code until I cat get a better feel for cases where
	 * it would be useful to have.
	 *
	 */
#if DEBUG_ROMCC_WARNINGS
#warning "WISHLIST implement live range splitting..."
#endif

	if (!split && (state->compiler->debug & DEBUG_RANGE_CONFLICTS2)) {
		FILE *fp = state->errout;
		print_interference_blocks(state, rstate, fp, 0);
		print_dominators(state, fp, &state->bb);
	}
	return split;
}

static FILE *cgdebug_fp(struct compile_state *state)
{
	FILE *fp;
	fp = 0;
	if (!fp && (state->compiler->debug & DEBUG_COLOR_GRAPH2)) {
		fp = state->errout;
	}
	if (!fp && (state->compiler->debug & DEBUG_COLOR_GRAPH)) {
		fp = state->dbgout;
	}
	return fp;
}

static void cgdebug_printf(struct compile_state *state, const char *fmt, ...)
{
	FILE *fp;
	fp = cgdebug_fp(state);
	if (fp) {
		va_list args;
		va_start(args, fmt);
		vfprintf(fp, fmt, args);
		va_end(args);
	}
}

static void cgdebug_flush(struct compile_state *state)
{
	FILE *fp;
	fp = cgdebug_fp(state);
	if (fp) {
		fflush(fp);
	}
}

static void cgdebug_loc(struct compile_state *state, struct triple *ins)
{
	FILE *fp;
	fp = cgdebug_fp(state);
	if (fp) {
		loc(fp, state, ins);
	}
}

static int select_free_color(struct compile_state *state,
	struct reg_state *rstate, struct live_range *range)
{
	struct triple_set *entry;
	struct live_range_def *lrd;
	struct live_range_def *phi;
	struct live_range_edge *edge;
	char used[MAX_REGISTERS];
	struct triple **expr;

	/* Instead of doing just the trivial color select here I try
	 * a few extra things because a good color selection will help reduce
	 * copies.
	 */

	/* Find the registers currently in use */
	memset(used, 0, sizeof(used));
	for(edge = range->edges; edge; edge = edge->next) {
		if (edge->node->color == REG_UNSET) {
			continue;
		}
		reg_fill_used(state, used, edge->node->color);
	}

	if (state->compiler->debug & DEBUG_COLOR_GRAPH2) {
		int i;
		i = 0;
		for(edge = range->edges; edge; edge = edge->next) {
			i++;
		}
		cgdebug_printf(state, "\n%s edges: %d",
			tops(range->defs->def->op), i);
		cgdebug_loc(state, range->defs->def);
		cgdebug_printf(state, "\n");
		for(i = 0; i < MAX_REGISTERS; i++) {
			if (used[i]) {
				cgdebug_printf(state, "used: %s\n",
					arch_reg_str(i));
			}
		}
	}

	/* If a color is already assigned see if it will work */
	if (range->color != REG_UNSET) {
		struct live_range_def *lrd;
		if (!used[range->color]) {
			return 1;
		}
		for(edge = range->edges; edge; edge = edge->next) {
			if (edge->node->color != range->color) {
				continue;
			}
			warning(state, edge->node->defs->def, "edge: ");
			lrd = edge->node->defs;
			do {
				warning(state, lrd->def, " %p %s",
					lrd->def, tops(lrd->def->op));
				lrd = lrd->next;
			} while(lrd != edge->node->defs);
		}
		lrd = range->defs;
		warning(state, range->defs->def, "def: ");
		do {
			warning(state, lrd->def, " %p %s",
				lrd->def, tops(lrd->def->op));
			lrd = lrd->next;
		} while(lrd != range->defs);
		internal_error(state, range->defs->def,
			"live range with already used color %s",
			arch_reg_str(range->color));
	}

	/* If I feed into an expression reuse it's color.
	 * This should help remove copies in the case of 2 register instructions
	 * and phi functions.
	 */
	phi = 0;
	lrd = live_range_end(state, range, 0);
	for(; (range->color == REG_UNSET) && lrd ; lrd = live_range_end(state, range, lrd)) {
		entry = lrd->def->use;
		for(;(range->color == REG_UNSET) && entry; entry = entry->next) {
			struct live_range_def *insd;
			unsigned regcm;
			insd = &rstate->lrd[entry->member->id];
			if (insd->lr->defs == 0) {
				continue;
			}
			if (!phi && (insd->def->op == OP_PHI) &&
				!interfere(rstate, range, insd->lr)) {
				phi = insd;
			}
			if (insd->lr->color == REG_UNSET) {
				continue;
			}
			regcm = insd->lr->classes;
			if (((regcm & range->classes) == 0) ||
				(used[insd->lr->color])) {
				continue;
			}
			if (interfere(rstate, range, insd->lr)) {
				continue;
			}
			range->color = insd->lr->color;
		}
	}
	/* If I feed into a phi function reuse it's color or the color
	 * of something else that feeds into the phi function.
	 */
	if (phi) {
		if (phi->lr->color != REG_UNSET) {
			if (used[phi->lr->color]) {
				range->color = phi->lr->color;
			}
		}
		else {
			expr = triple_rhs(state, phi->def, 0);
			for(; expr; expr = triple_rhs(state, phi->def, expr)) {
				struct live_range *lr;
				unsigned regcm;
				if (!*expr) {
					continue;
				}
				lr = rstate->lrd[(*expr)->id].lr;
				if (lr->color == REG_UNSET) {
					continue;
				}
				regcm = lr->classes;
				if (((regcm & range->classes) == 0) ||
					(used[lr->color])) {
					continue;
				}
				if (interfere(rstate, range, lr)) {
					continue;
				}
				range->color = lr->color;
			}
		}
	}
	/* If I don't interfere with a rhs node reuse it's color */
	lrd = live_range_head(state, range, 0);
	for(; (range->color == REG_UNSET) && lrd ; lrd = live_range_head(state, range, lrd)) {
		expr = triple_rhs(state, lrd->def, 0);
		for(; expr; expr = triple_rhs(state, lrd->def, expr)) {
			struct live_range *lr;
			unsigned regcm;
			if (!*expr) {
				continue;
			}
			lr = rstate->lrd[(*expr)->id].lr;
			if (lr->color == REG_UNSET) {
				continue;
			}
			regcm = lr->classes;
			if (((regcm & range->classes) == 0) ||
				(used[lr->color])) {
				continue;
			}
			if (interfere(rstate, range, lr)) {
				continue;
			}
			range->color = lr->color;
			break;
		}
	}
	/* If I have not opportunitically picked a useful color
	 * pick the first color that is free.
	 */
	if (range->color == REG_UNSET) {
		range->color =
			arch_select_free_register(state, used, range->classes);
	}
	if (range->color == REG_UNSET) {
		struct live_range_def *lrd;
		int i;
		if (split_ranges(state, rstate, used, range)) {
			return 0;
		}
		for(edge = range->edges; edge; edge = edge->next) {
			warning(state, edge->node->defs->def, "edge reg %s",
				arch_reg_str(edge->node->color));
			lrd = edge->node->defs;
			do {
				warning(state, lrd->def, " %s %p",
					tops(lrd->def->op), lrd->def);
				lrd = lrd->next;
			} while(lrd != edge->node->defs);
		}
		warning(state, range->defs->def, "range: ");
		lrd = range->defs;
		do {
			warning(state, lrd->def, " %s %p",
				tops(lrd->def->op), lrd->def);
			lrd = lrd->next;
		} while(lrd != range->defs);

		warning(state, range->defs->def, "classes: %x",
			range->classes);
		for(i = 0; i < MAX_REGISTERS; i++) {
			if (used[i]) {
				warning(state, range->defs->def, "used: %s",
					arch_reg_str(i));
			}
		}
		error(state, range->defs->def, "too few registers");
	}
	range->classes &= arch_reg_regcm(state, range->color);
	if ((range->color == REG_UNSET) || (range->classes == 0)) {
		internal_error(state, range->defs->def, "select_free_color did not?");
	}
	return 1;
}

static int color_graph(struct compile_state *state, struct reg_state *rstate)
{
	int colored;
	struct live_range_edge *edge;
	struct live_range *range;
	if (rstate->low) {
		cgdebug_printf(state, "Lo: ");
		range = rstate->low;
		if (*range->group_prev != range) {
			internal_error(state, 0, "lo: *prev != range?");
		}
		*range->group_prev = range->group_next;
		if (range->group_next) {
			range->group_next->group_prev = range->group_prev;
		}
		if (&range->group_next == rstate->low_tail) {
			rstate->low_tail = range->group_prev;
		}
		if (rstate->low == range) {
			internal_error(state, 0, "low: next != prev?");
		}
	}
	else if (rstate->high) {
		cgdebug_printf(state, "Hi: ");
		range = rstate->high;
		if (*range->group_prev != range) {
			internal_error(state, 0, "hi: *prev != range?");
		}
		*range->group_prev = range->group_next;
		if (range->group_next) {
			range->group_next->group_prev = range->group_prev;
		}
		if (&range->group_next == rstate->high_tail) {
			rstate->high_tail = range->group_prev;
		}
		if (rstate->high == range) {
			internal_error(state, 0, "high: next != prev?");
		}
	}
	else {
		return 1;
	}
	cgdebug_printf(state, " %d\n", range - rstate->lr);
	range->group_prev = 0;
	for(edge = range->edges; edge; edge = edge->next) {
		struct live_range *node;
		node = edge->node;
		/* Move nodes from the high to the low list */
		if (node->group_prev && (node->color == REG_UNSET) &&
			(node->degree == regc_max_size(state, node->classes))) {
			if (*node->group_prev != node) {
				internal_error(state, 0, "move: *prev != node?");
			}
			*node->group_prev = node->group_next;
			if (node->group_next) {
				node->group_next->group_prev = node->group_prev;
			}
			if (&node->group_next == rstate->high_tail) {
				rstate->high_tail = node->group_prev;
			}
			cgdebug_printf(state, "Moving...%d to low\n", node - rstate->lr);
			node->group_prev  = rstate->low_tail;
			node->group_next  = 0;
			*rstate->low_tail = node;
			rstate->low_tail  = &node->group_next;
			if (*node->group_prev != node) {
				internal_error(state, 0, "move2: *prev != node?");
			}
		}
		node->degree -= 1;
	}
	colored = color_graph(state, rstate);
	if (colored) {
		cgdebug_printf(state, "Coloring %d @", range - rstate->lr);
		cgdebug_loc(state, range->defs->def);
		cgdebug_flush(state);
		colored = select_free_color(state, rstate, range);
		if (colored) {
			cgdebug_printf(state, " %s\n", arch_reg_str(range->color));
		}
	}
	return colored;
}

static void verify_colors(struct compile_state *state, struct reg_state *rstate)
{
	struct live_range *lr;
	struct live_range_edge *edge;
	struct triple *ins, *first;
	char used[MAX_REGISTERS];
	first = state->first;
	ins = first;
	do {
		if (triple_is_def(state, ins)) {
			if (ins->id > rstate->defs) {
				internal_error(state, ins,
					"triple without a live range def");
			}
			lr = rstate->lrd[ins->id].lr;
			if (lr->color == REG_UNSET) {
				internal_error(state, ins,
					"triple without a color");
			}
			/* Find the registers used by the edges */
			memset(used, 0, sizeof(used));
			for(edge = lr->edges; edge; edge = edge->next) {
				if (edge->node->color == REG_UNSET) {
					internal_error(state, 0,
						"live range without a color");
			}
				reg_fill_used(state, used, edge->node->color);
			}
			if (used[lr->color]) {
				internal_error(state, ins,
					"triple with already used color");
			}
		}
		ins = ins->next;
	} while(ins != first);
}

static void color_triples(struct compile_state *state, struct reg_state *rstate)
{
	struct live_range_def *lrd;
	struct live_range *lr;
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		if (ins->id > rstate->defs) {
			internal_error(state, ins,
				"triple without a live range");
		}
		lrd = &rstate->lrd[ins->id];
		lr = lrd->lr;
		ins->id = lrd->orig_id;
		SET_REG(ins->id, lr->color);
		ins = ins->next;
	} while (ins != first);
}

static struct live_range *merge_sort_lr(
	struct live_range *first, struct live_range *last)
{
	struct live_range *mid, *join, **join_tail, *pick;
	size_t size;
	size = (last - first) + 1;
	if (size >= 2) {
		mid = first + size/2;
		first = merge_sort_lr(first, mid -1);
		mid   = merge_sort_lr(mid, last);

		join = 0;
		join_tail = &join;
		/* merge the two lists */
		while(first && mid) {
			if ((first->degree < mid->degree) ||
				((first->degree == mid->degree) &&
					(first->length < mid->length))) {
				pick = first;
				first = first->group_next;
				if (first) {
					first->group_prev = 0;
				}
			}
			else {
				pick = mid;
				mid = mid->group_next;
				if (mid) {
					mid->group_prev = 0;
				}
			}
			pick->group_next = 0;
			pick->group_prev = join_tail;
			*join_tail = pick;
			join_tail = &pick->group_next;
		}
		/* Splice the remaining list */
		pick = (first)? first : mid;
		*join_tail = pick;
		if (pick) {
			pick->group_prev = join_tail;
		}
	}
	else {
		if (!first->defs) {
			first = 0;
		}
		join = first;
	}
	return join;
}

static void ids_from_rstate(struct compile_state *state,
	struct reg_state *rstate)
{
	struct triple *ins, *first;
	if (!rstate->defs) {
		return;
	}
	/* Display the graph if desired */
	if (state->compiler->debug & DEBUG_INTERFERENCE) {
		FILE *fp = state->dbgout;
		print_interference_blocks(state, rstate, fp, 0);
		print_control_flow(state, fp, &state->bb);
		fflush(fp);
	}
	first = state->first;
	ins = first;
	do {
		if (ins->id) {
			struct live_range_def *lrd;
			lrd = &rstate->lrd[ins->id];
			ins->id = lrd->orig_id;
		}
		ins = ins->next;
	} while(ins != first);
}

static void cleanup_live_edges(struct reg_state *rstate)
{
	int i;
	/* Free the edges on each node */
	for(i = 1; i <= rstate->ranges; i++) {
		remove_live_edges(rstate, &rstate->lr[i]);
	}
}

static void cleanup_rstate(struct compile_state *state, struct reg_state *rstate)
{
	cleanup_live_edges(rstate);
	xfree(rstate->lrd);
	xfree(rstate->lr);

	/* Free the variable lifetime information */
	if (rstate->blocks) {
		free_variable_lifetimes(state, &state->bb, rstate->blocks);
	}
	rstate->defs = 0;
	rstate->ranges = 0;
	rstate->lrd = 0;
	rstate->lr = 0;
	rstate->blocks = 0;
}

static void verify_consistency(struct compile_state *state);
static void allocate_registers(struct compile_state *state)
{
	struct reg_state rstate;
	int colored;

	/* Clear out the reg_state */
	memset(&rstate, 0, sizeof(rstate));
	rstate.max_passes = state->compiler->max_allocation_passes;

	do {
		struct live_range **point, **next;
		int tangles;
		int coalesced;

		if (state->compiler->debug & DEBUG_RANGE_CONFLICTS) {
			FILE *fp = state->errout;
			fprintf(fp, "pass: %d\n", rstate.passes);
			fflush(fp);
		}

		/* Restore ids */
		ids_from_rstate(state, &rstate);

		/* Cleanup the temporary data structures */
		cleanup_rstate(state, &rstate);

		/* Compute the variable lifetimes */
		rstate.blocks = compute_variable_lifetimes(state, &state->bb);

		/* Fix invalid mandatory live range coalesce conflicts */
		correct_coalesce_conflicts(state, rstate.blocks);

		/* Fix two simultaneous uses of the same register.
		 * In a few pathlogical cases a partial untangle moves
		 * the tangle to a part of the graph we won't revisit.
		 * So we keep looping until we have no more tangle fixes
		 * to apply.
		 */
		do {
			tangles = correct_tangles(state, rstate.blocks);
		} while(tangles);


		print_blocks(state, "resolve_tangles", state->dbgout);
		verify_consistency(state);

		/* Allocate and initialize the live ranges */
		initialize_live_ranges(state, &rstate);

		/* Note currently doing coalescing in a loop appears to
		 * buys me nothing.  The code is left this way in case
		 * there is some value in it.  Or if a future bugfix
		 * yields some benefit.
		 */
		do {
			if (state->compiler->debug & DEBUG_COALESCING) {
				fprintf(state->errout, "coalescing\n");
			}

			/* Remove any previous live edge calculations */
			cleanup_live_edges(&rstate);

			/* Compute the interference graph */
			walk_variable_lifetimes(
				state, &state->bb, rstate.blocks,
				graph_ins, &rstate);

			/* Display the interference graph if desired */
			if (state->compiler->debug & DEBUG_INTERFERENCE) {
				print_interference_blocks(state, &rstate, state->dbgout, 1);
				fprintf(state->dbgout, "\nlive variables by instruction\n");
				walk_variable_lifetimes(
					state, &state->bb, rstate.blocks,
					print_interference_ins, &rstate);
			}

			coalesced = coalesce_live_ranges(state, &rstate);

			if (state->compiler->debug & DEBUG_COALESCING) {
				fprintf(state->errout, "coalesced: %d\n", coalesced);
			}
		} while(coalesced);

#if DEBUG_CONSISTENCY > 1
# if 0
		fprintf(state->errout, "verify_graph_ins...\n");
# endif
		/* Verify the interference graph */
		walk_variable_lifetimes(
			state, &state->bb, rstate.blocks,
			verify_graph_ins, &rstate);
# if 0
		fprintf(state->errout, "verify_graph_ins done\n");
#endif
#endif

		/* Build the groups low and high.  But with the nodes
		 * first sorted by degree order.
		 */
		rstate.low_tail  = &rstate.low;
		rstate.high_tail = &rstate.high;
		rstate.high = merge_sort_lr(&rstate.lr[1], &rstate.lr[rstate.ranges]);
		if (rstate.high) {
			rstate.high->group_prev = &rstate.high;
		}
		for(point = &rstate.high; *point; point = &(*point)->group_next)
			;
		rstate.high_tail = point;
		/* Walk through the high list and move everything that needs
		 * to be onto low.
		 */
		for(point = &rstate.high; *point; point = next) {
			struct live_range *range;
			next = &(*point)->group_next;
			range = *point;

			/* If it has a low degree or it already has a color
			 * place the node in low.
			 */
			if ((range->degree < regc_max_size(state, range->classes)) ||
				(range->color != REG_UNSET)) {
				cgdebug_printf(state, "Lo: %5d degree %5d%s\n",
					range - rstate.lr, range->degree,
					(range->color != REG_UNSET) ? " (colored)": "");
				*range->group_prev = range->group_next;
				if (range->group_next) {
					range->group_next->group_prev = range->group_prev;
				}
				if (&range->group_next == rstate.high_tail) {
					rstate.high_tail = range->group_prev;
				}
				range->group_prev  = rstate.low_tail;
				range->group_next  = 0;
				*rstate.low_tail   = range;
				rstate.low_tail    = &range->group_next;
				next = point;
			}
			else {
				cgdebug_printf(state, "hi: %5d degree %5d%s\n",
					range - rstate.lr, range->degree,
					(range->color != REG_UNSET) ? " (colored)": "");
			}
		}
		/* Color the live_ranges */
		colored = color_graph(state, &rstate);
		rstate.passes++;
	} while (!colored);

	/* Verify the graph was properly colored */
	verify_colors(state, &rstate);

	/* Move the colors from the graph to the triples */
	color_triples(state, &rstate);

	/* Cleanup the temporary data structures */
	cleanup_rstate(state, &rstate);

	/* Display the new graph */
	print_blocks(state, __func__, state->dbgout);
}

/* Sparce Conditional Constant Propogation
 * =========================================
 */
struct ssa_edge;
struct flow_block;
struct lattice_node {
	unsigned old_id;
	struct triple *def;
	struct ssa_edge *out;
	struct flow_block *fblock;
	struct triple *val;
	/* lattice high   val == def
	 * lattice const  is_const(val)
	 * lattice low    other
	 */
};
struct ssa_edge {
	struct lattice_node *src;
	struct lattice_node *dst;
	struct ssa_edge *work_next;
	struct ssa_edge *work_prev;
	struct ssa_edge *out_next;
};
struct flow_edge {
	struct flow_block *src;
	struct flow_block *dst;
	struct flow_edge *work_next;
	struct flow_edge *work_prev;
	struct flow_edge *in_next;
	struct flow_edge *out_next;
	int executable;
};
#define MAX_FLOW_BLOCK_EDGES 3
struct flow_block {
	struct block *block;
	struct flow_edge *in;
	struct flow_edge *out;
	struct flow_edge *edges;
};

struct scc_state {
	int ins_count;
	struct lattice_node *lattice;
	struct ssa_edge     *ssa_edges;
	struct flow_block   *flow_blocks;
	struct flow_edge    *flow_work_list;
	struct ssa_edge     *ssa_work_list;
};


static int is_scc_const(struct compile_state *state, struct triple *ins)
{
	return ins && (triple_is_ubranch(state, ins) || is_const(ins));
}

static int is_lattice_hi(struct compile_state *state, struct lattice_node *lnode)
{
	return !is_scc_const(state, lnode->val) && (lnode->val == lnode->def);
}

static int is_lattice_const(struct compile_state *state, struct lattice_node *lnode)
{
	return is_scc_const(state, lnode->val);
}

static int is_lattice_lo(struct compile_state *state, struct lattice_node *lnode)
{
	return (lnode->val != lnode->def) && !is_scc_const(state, lnode->val);
}

static void scc_add_fedge(struct compile_state *state, struct scc_state *scc,
	struct flow_edge *fedge)
{
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
		fprintf(state->errout, "adding fedge: %p (%4d -> %5d)\n",
			fedge,
			fedge->src->block?fedge->src->block->last->id: 0,
			fedge->dst->block?fedge->dst->block->first->id: 0);
	}
	if ((fedge == scc->flow_work_list) ||
		(fedge->work_next != fedge) ||
		(fedge->work_prev != fedge)) {

		if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
			fprintf(state->errout, "dupped fedge: %p\n",
				fedge);
		}
		return;
	}
	if (!scc->flow_work_list) {
		scc->flow_work_list = fedge;
		fedge->work_next = fedge->work_prev = fedge;
	}
	else {
		struct flow_edge *ftail;
		ftail = scc->flow_work_list->work_prev;
		fedge->work_next = ftail->work_next;
		fedge->work_prev = ftail;
		fedge->work_next->work_prev = fedge;
		fedge->work_prev->work_next = fedge;
	}
}

static struct flow_edge *scc_next_fedge(
	struct compile_state *state, struct scc_state *scc)
{
	struct flow_edge *fedge;
	fedge = scc->flow_work_list;
	if (fedge) {
		fedge->work_next->work_prev = fedge->work_prev;
		fedge->work_prev->work_next = fedge->work_next;
		if (fedge->work_next != fedge) {
			scc->flow_work_list = fedge->work_next;
		} else {
			scc->flow_work_list = 0;
		}
		fedge->work_next = fedge->work_prev = fedge;
	}
	return fedge;
}

static void scc_add_sedge(struct compile_state *state, struct scc_state *scc,
	struct ssa_edge *sedge)
{
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
		fprintf(state->errout, "adding sedge: %5ld (%4d -> %5d)\n",
			(long)(sedge - scc->ssa_edges),
			sedge->src->def->id,
			sedge->dst->def->id);
	}
	if ((sedge == scc->ssa_work_list) ||
		(sedge->work_next != sedge) ||
		(sedge->work_prev != sedge)) {

		if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
			fprintf(state->errout, "dupped sedge: %5ld\n",
				(long)(sedge - scc->ssa_edges));
		}
		return;
	}
	if (!scc->ssa_work_list) {
		scc->ssa_work_list = sedge;
		sedge->work_next = sedge->work_prev = sedge;
	}
	else {
		struct ssa_edge *stail;
		stail = scc->ssa_work_list->work_prev;
		sedge->work_next = stail->work_next;
		sedge->work_prev = stail;
		sedge->work_next->work_prev = sedge;
		sedge->work_prev->work_next = sedge;
	}
}

static struct ssa_edge *scc_next_sedge(
	struct compile_state *state, struct scc_state *scc)
{
	struct ssa_edge *sedge;
	sedge = scc->ssa_work_list;
	if (sedge) {
		sedge->work_next->work_prev = sedge->work_prev;
		sedge->work_prev->work_next = sedge->work_next;
		if (sedge->work_next != sedge) {
			scc->ssa_work_list = sedge->work_next;
		} else {
			scc->ssa_work_list = 0;
		}
		sedge->work_next = sedge->work_prev = sedge;
	}
	return sedge;
}

static void initialize_scc_state(
	struct compile_state *state, struct scc_state *scc)
{
	int ins_count, ssa_edge_count;
	int ins_index, ssa_edge_index, fblock_index;
	struct triple *first, *ins;
	struct block *block;
	struct flow_block *fblock;

	memset(scc, 0, sizeof(*scc));

	/* Inialize pass zero find out how much memory we need */
	first = state->first;
	ins = first;
	ins_count = ssa_edge_count = 0;
	do {
		struct triple_set *edge;
		ins_count += 1;
		for(edge = ins->use; edge; edge = edge->next) {
			ssa_edge_count++;
		}
		ins = ins->next;
	} while(ins != first);
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
		fprintf(state->errout, "ins_count: %d ssa_edge_count: %d vertex_count: %d\n",
			ins_count, ssa_edge_count, state->bb.last_vertex);
	}
	scc->ins_count   = ins_count;
	scc->lattice     =
		xcmalloc(sizeof(*scc->lattice)*(ins_count + 1), "lattice");
	scc->ssa_edges   =
		xcmalloc(sizeof(*scc->ssa_edges)*(ssa_edge_count + 1), "ssa_edges");
	scc->flow_blocks =
		xcmalloc(sizeof(*scc->flow_blocks)*(state->bb.last_vertex + 1),
			"flow_blocks");

	/* Initialize pass one collect up the nodes */
	fblock = 0;
	block = 0;
	ins_index = ssa_edge_index = fblock_index = 0;
	ins = first;
	do {
		if ((ins->op == OP_LABEL) && (block != ins->u.block)) {
			block = ins->u.block;
			if (!block) {
				internal_error(state, ins, "label without block");
			}
			fblock_index += 1;
			block->vertex = fblock_index;
			fblock = &scc->flow_blocks[fblock_index];
			fblock->block = block;
			fblock->edges = xcmalloc(sizeof(*fblock->edges)*block->edge_count,
				"flow_edges");
		}
		{
			struct lattice_node *lnode;
			ins_index += 1;
			lnode = &scc->lattice[ins_index];
			lnode->def = ins;
			lnode->out = 0;
			lnode->fblock = fblock;
			lnode->val = ins; /* LATTICE HIGH */
			if (lnode->val->op == OP_UNKNOWNVAL) {
				lnode->val = 0; /* LATTICE LOW by definition */
			}
			lnode->old_id = ins->id;
			ins->id = ins_index;
		}
		ins = ins->next;
	} while(ins != first);
	/* Initialize pass two collect up the edges */
	block = 0;
	fblock = 0;
	ins = first;
	do {
		{
			struct triple_set *edge;
			struct ssa_edge **stail;
			struct lattice_node *lnode;
			lnode = &scc->lattice[ins->id];
			lnode->out = 0;
			stail = &lnode->out;
			for(edge = ins->use; edge; edge = edge->next) {
				struct ssa_edge *sedge;
				ssa_edge_index += 1;
				sedge = &scc->ssa_edges[ssa_edge_index];
				*stail = sedge;
				stail = &sedge->out_next;
				sedge->src = lnode;
				sedge->dst = &scc->lattice[edge->member->id];
				sedge->work_next = sedge->work_prev = sedge;
				sedge->out_next = 0;
			}
		}
		if ((ins->op == OP_LABEL) && (block != ins->u.block)) {
			struct flow_edge *fedge, **ftail;
			struct block_set *bedge;
			block = ins->u.block;
			fblock = &scc->flow_blocks[block->vertex];
			fblock->in = 0;
			fblock->out = 0;
			ftail = &fblock->out;

			fedge = fblock->edges;
			bedge = block->edges;
			for(; bedge; bedge = bedge->next, fedge++) {
				fedge->dst = &scc->flow_blocks[bedge->member->vertex];
				if (fedge->dst->block != bedge->member) {
					internal_error(state, 0, "block mismatch");
				}
				*ftail = fedge;
				ftail = &fedge->out_next;
				fedge->out_next = 0;
			}
			for(fedge = fblock->out; fedge; fedge = fedge->out_next) {
				fedge->src = fblock;
				fedge->work_next = fedge->work_prev = fedge;
				fedge->executable = 0;
			}
		}
		ins = ins->next;
	} while (ins != first);
	block = 0;
	fblock = 0;
	ins = first;
	do {
		if ((ins->op  == OP_LABEL) && (block != ins->u.block)) {
			struct flow_edge **ftail;
			struct block_set *bedge;
			block = ins->u.block;
			fblock = &scc->flow_blocks[block->vertex];
			ftail = &fblock->in;
			for(bedge = block->use; bedge; bedge = bedge->next) {
				struct block *src_block;
				struct flow_block *sfblock;
				struct flow_edge *sfedge;
				src_block = bedge->member;
				sfblock = &scc->flow_blocks[src_block->vertex];
				for(sfedge = sfblock->out; sfedge; sfedge = sfedge->out_next) {
					if (sfedge->dst == fblock) {
						break;
					}
				}
				if (!sfedge) {
					internal_error(state, 0, "edge mismatch");
				}
				*ftail = sfedge;
				ftail = &sfedge->in_next;
				sfedge->in_next = 0;
			}
		}
		ins = ins->next;
	} while(ins != first);
	/* Setup a dummy block 0 as a node above the start node */
	{
		struct flow_block *fblock, *dst;
		struct flow_edge *fedge;
		fblock = &scc->flow_blocks[0];
		fblock->block = 0;
		fblock->edges = xcmalloc(sizeof(*fblock->edges)*1, "flow_edges");
		fblock->in = 0;
		fblock->out = fblock->edges;
		dst = &scc->flow_blocks[state->bb.first_block->vertex];
		fedge = fblock->edges;
		fedge->src        = fblock;
		fedge->dst        = dst;
		fedge->work_next  = fedge;
		fedge->work_prev  = fedge;
		fedge->in_next    = fedge->dst->in;
		fedge->out_next   = 0;
		fedge->executable = 0;
		fedge->dst->in = fedge;

		/* Initialize the work lists */
		scc->flow_work_list = 0;
		scc->ssa_work_list  = 0;
		scc_add_fedge(state, scc, fedge);
	}
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
		fprintf(state->errout, "ins_index: %d ssa_edge_index: %d fblock_index: %d\n",
			ins_index, ssa_edge_index, fblock_index);
	}
}


static void free_scc_state(
	struct compile_state *state, struct scc_state *scc)
{
	int i;
	for(i = 0; i < state->bb.last_vertex + 1; i++) {
		struct flow_block *fblock;
		fblock = &scc->flow_blocks[i];
		if (fblock->edges) {
			xfree(fblock->edges);
			fblock->edges = 0;
		}
	}
	xfree(scc->flow_blocks);
	xfree(scc->ssa_edges);
	xfree(scc->lattice);

}

static struct lattice_node *triple_to_lattice(
	struct compile_state *state, struct scc_state *scc, struct triple *ins)
{
	if (ins->id <= 0) {
		internal_error(state, ins, "bad id");
	}
	return &scc->lattice[ins->id];
}

static struct triple *preserve_lval(
	struct compile_state *state, struct lattice_node *lnode)
{
	struct triple *old;
	/* Preserve the original value */
	if (lnode->val) {
		old = dup_triple(state, lnode->val);
		if (lnode->val != lnode->def) {
			xfree(lnode->val);
		}
		lnode->val = 0;
	} else {
		old = 0;
	}
	return old;
}

static int lval_changed(struct compile_state *state,
	struct triple *old, struct lattice_node *lnode)
{
	int changed;
	/* See if the lattice value has changed */
	changed = 1;
	if (!old && !lnode->val) {
		changed = 0;
	}
	if (changed &&
		lnode->val && old &&
		(memcmp(lnode->val->param, old->param,
			TRIPLE_SIZE(lnode->val) * sizeof(lnode->val->param[0])) == 0) &&
		(memcmp(&lnode->val->u, &old->u, sizeof(old->u)) == 0)) {
		changed = 0;
	}
	if (old) {
		xfree(old);
	}
	return changed;

}

static void scc_debug_lnode(
	struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode, int changed)
{
	if ((state->compiler->debug & DEBUG_SCC_TRANSFORM2) && lnode->val) {
		display_triple_changes(state->errout, lnode->val, lnode->def);
	}
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
		FILE *fp = state->errout;
		struct triple *val, **expr;
		val = lnode->val? lnode->val : lnode->def;
		fprintf(fp, "%p %s %3d %10s (",
			lnode->def,
			((lnode->def->op == OP_PHI)? "phi: ": "expr:"),
			lnode->def->id,
			tops(lnode->def->op));
		expr = triple_rhs(state, lnode->def, 0);
		for(;expr;expr = triple_rhs(state, lnode->def, expr)) {
			if (*expr) {
				fprintf(fp, " %d", (*expr)->id);
			}
		}
		if (val->op == OP_INTCONST) {
			fprintf(fp, " <0x%08lx>", (unsigned long)(val->u.cval));
		}
		fprintf(fp, " ) -> %s %s\n",
			(is_lattice_hi(state, lnode)? "hi":
				is_lattice_const(state, lnode)? "const" : "lo"),
			changed? "changed" : ""
			);
	}
}

static int compute_lnode_val(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	int changed;
	struct triple *old, *scratch;
	struct triple **dexpr, **vexpr;
	int count, i;

	/* Store the original value */
	old = preserve_lval(state, lnode);

	/* Reinitialize the value */
	lnode->val = scratch = dup_triple(state, lnode->def);
	scratch->id = lnode->old_id;
	scratch->next     = scratch;
	scratch->prev     = scratch;
	scratch->use      = 0;

	count = TRIPLE_SIZE(scratch);
	for(i = 0; i < count; i++) {
		dexpr = &lnode->def->param[i];
		vexpr = &scratch->param[i];
		*vexpr = *dexpr;
		if (((i < TRIPLE_MISC_OFF(scratch)) ||
			(i >= TRIPLE_TARG_OFF(scratch))) &&
			*dexpr) {
			struct lattice_node *tmp;
			tmp = triple_to_lattice(state, scc, *dexpr);
			*vexpr = (tmp->val)? tmp->val : tmp->def;
		}
	}
	if (triple_is_branch(state, scratch)) {
		scratch->next = lnode->def->next;
	}
	/* Recompute the value */
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME see if simplify does anything bad"
#endif
	/* So far it looks like only the strength reduction
	 * optimization are things I need to worry about.
	 */
	simplify(state, scratch);
	/* Cleanup my value */
	if (scratch->use) {
		internal_error(state, lnode->def, "scratch used?");
	}
	if ((scratch->prev != scratch) ||
		((scratch->next != scratch) &&
			(!triple_is_branch(state, lnode->def) ||
				(scratch->next != lnode->def->next)))) {
		internal_error(state, lnode->def, "scratch in list?");
	}
	/* undo any uses... */
	count = TRIPLE_SIZE(scratch);
	for(i = 0; i < count; i++) {
		vexpr = &scratch->param[i];
		if (*vexpr) {
			unuse_triple(*vexpr, scratch);
		}
	}
	if (lnode->val->op == OP_UNKNOWNVAL) {
		lnode->val = 0; /* Lattice low by definition */
	}
	/* Find the case when I am lattice high */
	if (lnode->val &&
		(lnode->val->op == lnode->def->op) &&
		(memcmp(lnode->val->param, lnode->def->param,
			count * sizeof(lnode->val->param[0])) == 0) &&
		(memcmp(&lnode->val->u, &lnode->def->u, sizeof(lnode->def->u)) == 0)) {
		lnode->val = lnode->def;
	}
	/* Only allow lattice high when all of my inputs
	 * are also lattice high.  Occasionally I can
	 * have constants with a lattice low input, so
	 * I do not need to check that case.
	 */
	if (is_lattice_hi(state, lnode)) {
		struct lattice_node *tmp;
		int rhs;
		rhs = lnode->val->rhs;
		for(i = 0; i < rhs; i++) {
			tmp = triple_to_lattice(state, scc, RHS(lnode->val, i));
			if (!is_lattice_hi(state, tmp)) {
				lnode->val = 0;
				break;
			}
		}
	}
	/* Find the cases that are always lattice lo */
	if (lnode->val &&
		triple_is_def(state, lnode->val) &&
		!triple_is_pure(state, lnode->val, lnode->old_id)) {
		lnode->val = 0;
	}
	/* See if the lattice value has changed */
	changed = lval_changed(state, old, lnode);
	/* See if this value should not change */
	if ((lnode->val != lnode->def) &&
		((	!triple_is_def(state, lnode->def)  &&
			!triple_is_cbranch(state, lnode->def)) ||
			(lnode->def->op == OP_PIECE))) {
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME constant propagate through expressions with multiple left hand sides"
#endif
		if (changed) {
			internal_warning(state, lnode->def, "non def changes value?");
		}
		lnode->val = 0;
	}

	/* See if we need to free the scratch value */
	if (lnode->val != scratch) {
		xfree(scratch);
	}

	return changed;
}


static void scc_visit_cbranch(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	struct lattice_node *cond;
	struct flow_edge *left, *right;
	int changed;

	/* Update the branch value */
	changed = compute_lnode_val(state, scc, lnode);
	scc_debug_lnode(state, scc, lnode, changed);

	/* This only applies to conditional branches */
	if (!triple_is_cbranch(state, lnode->def)) {
		internal_error(state, lnode->def, "not a conditional branch");
	}

	if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
		struct flow_edge *fedge;
		FILE *fp = state->errout;
		fprintf(fp, "%s: %d (",
			tops(lnode->def->op),
			lnode->def->id);

		for(fedge = lnode->fblock->out; fedge; fedge = fedge->out_next) {
			fprintf(fp, " %d", fedge->dst->block->vertex);
		}
		fprintf(fp, " )");
		if (lnode->def->rhs > 0) {
			fprintf(fp, " <- %d",
				RHS(lnode->def, 0)->id);
		}
		fprintf(fp, "\n");
	}
	cond = triple_to_lattice(state, scc, RHS(lnode->def,0));
	for(left = cond->fblock->out; left; left = left->out_next) {
		if (left->dst->block->first == lnode->def->next) {
			break;
		}
	}
	if (!left) {
		internal_error(state, lnode->def, "Cannot find left branch edge");
	}
	for(right = cond->fblock->out; right; right = right->out_next) {
		if (right->dst->block->first == TARG(lnode->def, 0)) {
			break;
		}
	}
	if (!right) {
		internal_error(state, lnode->def, "Cannot find right branch edge");
	}
	/* I should only come here if the controlling expressions value
	 * has changed, which means it must be either a constant or lo.
	 */
	if (is_lattice_hi(state, cond)) {
		internal_error(state, cond->def, "condition high?");
		return;
	}
	if (is_lattice_lo(state, cond)) {
		scc_add_fedge(state, scc, left);
		scc_add_fedge(state, scc, right);
	}
	else if (cond->val->u.cval) {
		scc_add_fedge(state, scc, right);
	} else {
		scc_add_fedge(state, scc, left);
	}

}


static void scc_add_sedge_dst(struct compile_state *state,
	struct scc_state *scc, struct ssa_edge *sedge)
{
	if (triple_is_cbranch(state, sedge->dst->def)) {
		scc_visit_cbranch(state, scc, sedge->dst);
	}
	else if (triple_is_def(state, sedge->dst->def)) {
		scc_add_sedge(state, scc, sedge);
	}
}

static void scc_visit_phi(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	struct lattice_node *tmp;
	struct triple **slot, *old;
	struct flow_edge *fedge;
	int changed;
	int index;
	if (lnode->def->op != OP_PHI) {
		internal_error(state, lnode->def, "not phi");
	}
	/* Store the original value */
	old = preserve_lval(state, lnode);

	/* default to lattice high */
	lnode->val = lnode->def;
	slot = &RHS(lnode->def, 0);
	index = 0;
	for(fedge = lnode->fblock->in; fedge; index++, fedge = fedge->in_next) {
		if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
			fprintf(state->errout, "Examining edge: %d vertex: %d executable: %d\n",
				index,
				fedge->dst->block->vertex,
				fedge->executable
				);
		}
		if (!fedge->executable) {
			continue;
		}
		if (!slot[index]) {
			internal_error(state, lnode->def, "no phi value");
		}
		tmp = triple_to_lattice(state, scc, slot[index]);
		/* meet(X, lattice low) = lattice low */
		if (is_lattice_lo(state, tmp)) {
			lnode->val = 0;
		}
		/* meet(X, lattice high) = X */
		else if (is_lattice_hi(state, tmp)) {
		}
		/* meet(lattice high, X) = X */
		else if (is_lattice_hi(state, lnode)) {
			lnode->val = dup_triple(state, tmp->val);
			/* Only change the type if necessary */
			if (!is_subset_type(lnode->def->type, tmp->val->type)) {
				lnode->val->type = lnode->def->type;
			}
		}
		/* meet(const, const) = const or lattice low */
		else if (!constants_equal(state, lnode->val, tmp->val)) {
			lnode->val = 0;
		}

		/* meet(lattice low, X) = lattice low */
		if (is_lattice_lo(state, lnode)) {
			lnode->val = 0;
			break;
		}
	}
	changed = lval_changed(state, old, lnode);
	scc_debug_lnode(state, scc, lnode, changed);

	/* If the lattice value has changed update the work lists. */
	if (changed) {
		struct ssa_edge *sedge;
		for(sedge = lnode->out; sedge; sedge = sedge->out_next) {
			scc_add_sedge_dst(state, scc, sedge);
		}
	}
}


static void scc_visit_expr(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	int changed;

	if (!triple_is_def(state, lnode->def)) {
		internal_warning(state, lnode->def, "not visiting an expression?");
	}
	changed = compute_lnode_val(state, scc, lnode);
	scc_debug_lnode(state, scc, lnode, changed);

	if (changed) {
		struct ssa_edge *sedge;
		for(sedge = lnode->out; sedge; sedge = sedge->out_next) {
			scc_add_sedge_dst(state, scc, sedge);
		}
	}
}

static void scc_writeback_values(
	struct compile_state *state, struct scc_state *scc)
{
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		struct lattice_node *lnode;
		lnode = triple_to_lattice(state, scc, ins);
		if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
			if (is_lattice_hi(state, lnode) &&
				(lnode->val->op != OP_NOOP))
			{
				struct flow_edge *fedge;
				int executable;
				executable = 0;
				for(fedge = lnode->fblock->in;
				    !executable && fedge; fedge = fedge->in_next) {
					executable |= fedge->executable;
				}
				if (executable) {
					internal_warning(state, lnode->def,
						"lattice node %d %s->%s still high?",
						ins->id,
						tops(lnode->def->op),
						tops(lnode->val->op));
				}
			}
		}

		/* Restore id */
		ins->id = lnode->old_id;
		if (lnode->val && (lnode->val != ins)) {
			/* See if it something I know how to write back */
			switch(lnode->val->op) {
			case OP_INTCONST:
				mkconst(state, ins, lnode->val->u.cval);
				break;
			case OP_ADDRCONST:
				mkaddr_const(state, ins,
					MISC(lnode->val, 0), lnode->val->u.cval);
				break;
			default:
				/* By default don't copy the changes,
				 * recompute them in place instead.
				 */
				simplify(state, ins);
				break;
			}
			if (is_const(lnode->val) &&
				!constants_equal(state, lnode->val, ins)) {
				internal_error(state, 0, "constants not equal");
			}
			/* Free the lattice nodes */
			xfree(lnode->val);
			lnode->val = 0;
		}
		ins = ins->next;
	} while(ins != first);
}

static void scc_transform(struct compile_state *state)
{
	struct scc_state scc;
	if (!(state->compiler->flags & COMPILER_SCC_TRANSFORM)) {
		return;
	}

	initialize_scc_state(state, &scc);

	while(scc.flow_work_list || scc.ssa_work_list) {
		struct flow_edge *fedge;
		struct ssa_edge *sedge;
		struct flow_edge *fptr;
		while((fedge = scc_next_fedge(state, &scc))) {
			struct block *block;
			struct triple *ptr;
			struct flow_block *fblock;
			int reps;
			int done;
			if (fedge->executable) {
				continue;
			}
			if (!fedge->dst) {
				internal_error(state, 0, "fedge without dst");
			}
			if (!fedge->src) {
				internal_error(state, 0, "fedge without src");
			}
			fedge->executable = 1;
			fblock = fedge->dst;
			block = fblock->block;
			reps = 0;
			for(fptr = fblock->in; fptr; fptr = fptr->in_next) {
				if (fptr->executable) {
					reps++;
				}
			}

			if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
				fprintf(state->errout, "vertex: %d reps: %d\n",
					block->vertex, reps);
			}

			done = 0;
			for(ptr = block->first; !done; ptr = ptr->next) {
				struct lattice_node *lnode;
				done = (ptr == block->last);
				lnode = &scc.lattice[ptr->id];
				if (ptr->op == OP_PHI) {
					scc_visit_phi(state, &scc, lnode);
				}
				else if ((reps == 1) && triple_is_def(state, ptr))
				{
					scc_visit_expr(state, &scc, lnode);
				}
			}
			/* Add unconditional branch edges */
			if (!triple_is_cbranch(state, fblock->block->last)) {
				struct flow_edge *out;
				for(out = fblock->out; out; out = out->out_next) {
					scc_add_fedge(state, &scc, out);
				}
			}
		}
		while((sedge = scc_next_sedge(state, &scc))) {
			struct lattice_node *lnode;
			struct flow_block *fblock;
			lnode = sedge->dst;
			fblock = lnode->fblock;

			if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
				fprintf(state->errout, "sedge: %5ld (%5d -> %5d)\n",
					(unsigned long)sedge - (unsigned long)scc.ssa_edges,
					sedge->src->def->id,
					sedge->dst->def->id);
			}

			if (lnode->def->op == OP_PHI) {
				scc_visit_phi(state, &scc, lnode);
			}
			else {
				for(fptr = fblock->in; fptr; fptr = fptr->in_next) {
					if (fptr->executable) {
						break;
					}
				}
				if (fptr) {
					scc_visit_expr(state, &scc, lnode);
				}
			}
		}
	}

	scc_writeback_values(state, &scc);
	free_scc_state(state, &scc);
	rebuild_ssa_form(state);

	print_blocks(state, __func__, state->dbgout);
}


static void transform_to_arch_instructions(struct compile_state *state)
{
	struct triple *ins, *first;
	first = state->first;
	ins = first;
	do {
		ins = transform_to_arch_instruction(state, ins);
	} while(ins != first);

	print_blocks(state, __func__, state->dbgout);
}

#if DEBUG_CONSISTENCY
static void verify_uses(struct compile_state *state)
{
	struct triple *first, *ins;
	struct triple_set *set;
	first = state->first;
	ins = first;
	do {
		struct triple **expr;
		expr = triple_rhs(state, ins, 0);
		for(; expr; expr = triple_rhs(state, ins, expr)) {
			struct triple *rhs;
			rhs = *expr;
			for(set = rhs?rhs->use:0; set; set = set->next) {
				if (set->member == ins) {
					break;
				}
			}
			if (!set) {
				internal_error(state, ins, "rhs not used");
			}
		}
		expr = triple_lhs(state, ins, 0);
		for(; expr; expr = triple_lhs(state, ins, expr)) {
			struct triple *lhs;
			lhs = *expr;
			for(set =  lhs?lhs->use:0; set; set = set->next) {
				if (set->member == ins) {
					break;
				}
			}
			if (!set) {
				internal_error(state, ins, "lhs not used");
			}
		}
		expr = triple_misc(state, ins, 0);
		if (ins->op != OP_PHI) {
			for(; expr; expr = triple_targ(state, ins, expr)) {
				struct triple *misc;
				misc = *expr;
				for(set = misc?misc->use:0; set; set = set->next) {
					if (set->member == ins) {
						break;
					}
				}
				if (!set) {
					internal_error(state, ins, "misc not used");
				}
			}
		}
		if (!triple_is_ret(state, ins)) {
			expr = triple_targ(state, ins, 0);
			for(; expr; expr = triple_targ(state, ins, expr)) {
				struct triple *targ;
				targ = *expr;
				for(set = targ?targ->use:0; set; set = set->next) {
					if (set->member == ins) {
						break;
					}
				}
				if (!set) {
					internal_error(state, ins, "targ not used");
				}
			}
		}
		ins = ins->next;
	} while(ins != first);

}
static void verify_blocks_present(struct compile_state *state)
{
	struct triple *first, *ins;
	if (!state->bb.first_block) {
		return;
	}
	first = state->first;
	ins = first;
	do {
		valid_ins(state, ins);
		if (triple_stores_block(state, ins)) {
			if (!ins->u.block) {
				internal_error(state, ins,
					"%p not in a block?", ins);
			}
		}
		ins = ins->next;
	} while(ins != first);


}

static int edge_present(struct compile_state *state, struct block *block, struct triple *edge)
{
	struct block_set *bedge;
	struct block *targ;
	targ = block_of_triple(state, edge);
	for(bedge = block->edges; bedge; bedge = bedge->next) {
		if (bedge->member == targ) {
			return 1;
		}
	}
	return 0;
}

static void verify_blocks(struct compile_state *state)
{
	struct triple *ins;
	struct block *block;
	int blocks;
	block = state->bb.first_block;
	if (!block) {
		return;
	}
	blocks = 0;
	do {
		int users;
		struct block_set *user, *edge;
		blocks++;
		for(ins = block->first; ins != block->last->next; ins = ins->next) {
			if (triple_stores_block(state, ins) && (ins->u.block != block)) {
				internal_error(state, ins, "inconsitent block specified");
			}
			valid_ins(state, ins);
		}
		users = 0;
		for(user = block->use; user; user = user->next) {
			users++;
			if (!user->member->first) {
				internal_error(state, block->first, "user is empty");
			}
			if ((block == state->bb.last_block) &&
				(user->member == state->bb.first_block)) {
				continue;
			}
			for(edge = user->member->edges; edge; edge = edge->next) {
				if (edge->member == block) {
					break;
				}
			}
			if (!edge) {
				internal_error(state, user->member->first,
					"user does not use block");
			}
		}
		if (triple_is_branch(state, block->last)) {
			struct triple **expr;
			expr = triple_edge_targ(state, block->last, 0);
			for(;expr; expr = triple_edge_targ(state, block->last, expr)) {
				if (*expr && !edge_present(state, block, *expr)) {
					internal_error(state, block->last, "no edge to targ");
				}
			}
		}
		if (!triple_is_ubranch(state, block->last) &&
			(block != state->bb.last_block) &&
			!edge_present(state, block, block->last->next)) {
			internal_error(state, block->last, "no edge to block->last->next");
		}
		for(edge = block->edges; edge; edge = edge->next) {
			for(user = edge->member->use; user; user = user->next) {
				if (user->member == block) {
					break;
				}
			}
			if (!user || user->member != block) {
				internal_error(state, block->first,
					"block does not use edge");
			}
			if (!edge->member->first) {
				internal_error(state, block->first, "edge block is empty");
			}
		}
		if (block->users != users) {
			internal_error(state, block->first,
				"computed users %d != stored users %d",
				users, block->users);
		}
		if (!(block->last->next) || !(block->last->next->u.block)) {
			internal_error(state, block->last,
				"bad next block");
		}
		if (!triple_stores_block(state, block->last->next)) {
			internal_error(state, block->last->next,
				"cannot find next block");
		}
		block = block->last->next->u.block;
	} while(block != state->bb.first_block);
	if (blocks != state->bb.last_vertex) {
		internal_error(state, 0, "computed blocks: %d != stored blocks %d",
			blocks, state->bb.last_vertex);
	}
}

static void verify_domination(struct compile_state *state)
{
	struct triple *first, *ins;
	struct triple_set *set;
	if (!state->bb.first_block) {
		return;
	}

	first = state->first;
	ins = first;
	do {
		for(set = ins->use; set; set = set->next) {
			struct triple **slot;
			struct triple *use_point;
			int i, zrhs;
			use_point = 0;
			zrhs = set->member->rhs;
			slot = &RHS(set->member, 0);
			/* See if the use is on the right hand side */
			for(i = 0; i < zrhs; i++) {
				if (slot[i] == ins) {
					break;
				}
			}
			if (i < zrhs) {
				use_point = set->member;
				if (set->member->op == OP_PHI) {
					struct block_set *bset;
					int edge;
					bset = set->member->u.block->use;
					for(edge = 0; bset && (edge < i); edge++) {
						bset = bset->next;
					}
					if (!bset) {
						internal_error(state, set->member,
							"no edge for phi rhs %d", i);
					}
					use_point = bset->member->last;
				}
			}
			if (use_point &&
				!tdominates(state, ins, use_point)) {
				if (is_const(ins)) {
					internal_warning(state, ins,
					"non dominated rhs use point %p?", use_point);
				}
				else {
					internal_error(state, ins,
						"non dominated rhs use point %p?", use_point);
				}
			}
		}
		ins = ins->next;
	} while(ins != first);
}

static void verify_rhs(struct compile_state *state)
{
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		struct triple **slot;
		int zrhs, i;
		zrhs = ins->rhs;
		slot = &RHS(ins, 0);
		for(i = 0; i < zrhs; i++) {
			if (slot[i] == 0) {
				internal_error(state, ins,
					"missing rhs %d on %s",
					i, tops(ins->op));
			}
			if ((ins->op != OP_PHI) && (slot[i] == ins)) {
				internal_error(state, ins,
					"ins == rhs[%d] on %s",
					i, tops(ins->op));
			}
		}
		ins = ins->next;
	} while(ins != first);
}

static void verify_piece(struct compile_state *state)
{
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		struct triple *ptr;
		int lhs, i;
		lhs = ins->lhs;
		for(ptr = ins->next, i = 0; i < lhs; i++, ptr = ptr->next) {
			if (ptr != LHS(ins, i)) {
				internal_error(state, ins, "malformed lhs on %s",
					tops(ins->op));
			}
			if (ptr->op != OP_PIECE) {
				internal_error(state, ins, "bad lhs op %s at %d on %s",
					tops(ptr->op), i, tops(ins->op));
			}
			if (ptr->u.cval != i) {
				internal_error(state, ins, "bad u.cval of %d %d expected",
					ptr->u.cval, i);
			}
		}
		ins = ins->next;
	} while(ins != first);
}

static void verify_ins_colors(struct compile_state *state)
{
	struct triple *first, *ins;

	first = state->first;
	ins = first;
	do {
		ins = ins->next;
	} while(ins != first);
}

static void verify_unknown(struct compile_state *state)
{
	struct triple *first, *ins;
	if (	(unknown_triple.next != &unknown_triple) ||
		(unknown_triple.prev != &unknown_triple) ||
#if 0
		(unknown_triple.use != 0) ||
#endif
		(unknown_triple.op != OP_UNKNOWNVAL) ||
		(unknown_triple.lhs != 0) ||
		(unknown_triple.rhs != 0) ||
		(unknown_triple.misc != 0) ||
		(unknown_triple.targ != 0) ||
		(unknown_triple.template_id != 0) ||
		(unknown_triple.id != -1) ||
		(unknown_triple.type != &unknown_type) ||
		(unknown_triple.occurrence != &dummy_occurrence) ||
		(unknown_triple.param[0] != 0) ||
		(unknown_triple.param[1] != 0)) {
		internal_error(state, &unknown_triple, "unknown_triple corrupted!");
	}
	if (	(dummy_occurrence.count != 2) ||
		(strcmp(dummy_occurrence.filename, __FILE__) != 0) ||
		(strcmp(dummy_occurrence.function, "") != 0) ||
		(dummy_occurrence.col != 0) ||
		(dummy_occurrence.parent != 0)) {
		internal_error(state, &unknown_triple, "dummy_occurrence corrupted!");
	}
	if (	(unknown_type.type != TYPE_UNKNOWN)) {
		internal_error(state, &unknown_triple, "unknown_type corrupted!");
	}
	first = state->first;
	ins = first;
	do {
		int params, i;
		if (ins == &unknown_triple) {
			internal_error(state, ins, "unknown triple in list");
		}
		params = TRIPLE_SIZE(ins);
		for(i = 0; i < params; i++) {
			if (ins->param[i] == &unknown_triple) {
				internal_error(state, ins, "unknown triple used!");
			}
		}
		ins = ins->next;
	} while(ins != first);
}

static void verify_types(struct compile_state *state)
{
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		struct type *invalid;
		invalid = invalid_type(state, ins->type);
		if (invalid) {
			FILE *fp = state->errout;
			fprintf(fp, "type: ");
			name_of(fp, ins->type);
			fprintf(fp, "\n");
			fprintf(fp, "invalid type: ");
			name_of(fp, invalid);
			fprintf(fp, "\n");
			internal_error(state, ins, "invalid ins type");
		}
	} while(ins != first);
}

static void verify_copy(struct compile_state *state)
{
	struct triple *first, *ins, *next;
	first = state->first;
	next = ins = first;
	do {
		ins = next;
		next = ins->next;
		if (ins->op != OP_COPY) {
			continue;
		}
		if (!equiv_types(ins->type, RHS(ins, 0)->type)) {
			FILE *fp = state->errout;
			fprintf(fp, "src type: ");
			name_of(fp, RHS(ins, 0)->type);
			fprintf(fp, "\n");
			fprintf(fp, "dst type: ");
			name_of(fp, ins->type);
			fprintf(fp, "\n");
			internal_error(state, ins, "type mismatch in copy");
		}
	} while(next != first);
}

static void verify_consistency(struct compile_state *state)
{
	verify_unknown(state);
	verify_uses(state);
	verify_blocks_present(state);
	verify_blocks(state);
	verify_domination(state);
	verify_rhs(state);
	verify_piece(state);
	verify_ins_colors(state);
	verify_types(state);
	verify_copy(state);
	if (state->compiler->debug & DEBUG_VERIFICATION) {
		fprintf(state->dbgout, "consistency verified\n");
	}
}
#else
static void verify_consistency(struct compile_state *state) {}
#endif /* DEBUG_CONSISTENCY */

static void optimize(struct compile_state *state)
{
	/* Join all of the functions into one giant function */
	join_functions(state);

	/* Dump what the instruction graph intially looks like */
	print_triples(state);

	/* Replace structures with simpler data types */
	decompose_compound_types(state);
	print_triples(state);

	verify_consistency(state);
	/* Analyze the intermediate code */
	state->bb.first = state->first;
	analyze_basic_blocks(state, &state->bb);

	/* Transform the code to ssa form. */
	/*
	 * The transformation to ssa form puts a phi function
	 * on each of edge of a dominance frontier where that
	 * phi function might be needed.  At -O2 if we don't
	 * eleminate the excess phi functions we can get an
	 * exponential code size growth.  So I kill the extra
	 * phi functions early and I kill them often.
	 */
	transform_to_ssa_form(state);
	verify_consistency(state);

	/* Remove dead code */
	eliminate_inefectual_code(state);
	verify_consistency(state);

	/* Do strength reduction and simple constant optimizations */
	simplify_all(state);
	verify_consistency(state);
	/* Propagate constants throughout the code */
	scc_transform(state);
	verify_consistency(state);
#if DEBUG_ROMCC_WARNINGS
#warning "WISHLIST implement single use constants (least possible register pressure)"
#warning "WISHLIST implement induction variable elimination"
#endif
	/* Select architecture instructions and an initial partial
	 * coloring based on architecture constraints.
	 */
	transform_to_arch_instructions(state);
	verify_consistency(state);

	/* Remove dead code */
	eliminate_inefectual_code(state);
	verify_consistency(state);

	/* Color all of the variables to see if they will fit in registers */
	insert_copies_to_phi(state);
	verify_consistency(state);

	insert_mandatory_copies(state);
	verify_consistency(state);

	allocate_registers(state);
	verify_consistency(state);

	/* Remove the optimization information.
	 * This is more to check for memory consistency than to free memory.
	 */
	free_basic_blocks(state, &state->bb);
}

static void print_op_asm(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	struct asm_info *info;
	const char *ptr;
	unsigned lhs, rhs, i;
	info = ins->u.ainfo;
	lhs = ins->lhs;
	rhs = ins->rhs;
	/* Don't count the clobbers in lhs */
	for(i = 0; i < lhs; i++) {
		if (LHS(ins, i)->type == &void_type) {
			break;
		}
	}
	lhs = i;
	fprintf(fp, "#ASM\n");
	fputc('\t', fp);
	for(ptr = info->str; *ptr; ptr++) {
		char *next;
		unsigned long param;
		struct triple *piece;
		if (*ptr != '%') {
			fputc(*ptr, fp);
			continue;
		}
		ptr++;
		if (*ptr == '%') {
			fputc('%', fp);
			continue;
		}
		param = strtoul(ptr, &next, 10);
		if (ptr == next) {
			error(state, ins, "Invalid asm template");
		}
		if (param >= (lhs + rhs)) {
			error(state, ins, "Invalid param %%%u in asm template",
				param);
		}
		piece = (param < lhs)? LHS(ins, param) : RHS(ins, param - lhs);
		fprintf(fp, "%s",
			arch_reg_str(ID_REG(piece->id)));
		ptr = next -1;
	}
	fprintf(fp, "\n#NOT ASM\n");
}


/* Only use the low x86 byte registers.  This allows me
 * allocate the entire register when a byte register is used.
 */
#define X86_4_8BIT_GPRS 1

/* x86 featrues */
#define X86_MMX_REGS  (1<<0)
#define X86_XMM_REGS  (1<<1)
#define X86_NOOP_COPY (1<<2)

/* The x86 register classes */
#define REGC_FLAGS       0
#define REGC_GPR8        1
#define REGC_GPR16       2
#define REGC_GPR32       3
#define REGC_DIVIDEND64  4
#define REGC_DIVIDEND32  5
#define REGC_MMX         6
#define REGC_XMM         7
#define REGC_GPR32_8     8
#define REGC_GPR16_8     9
#define REGC_GPR8_LO    10
#define REGC_IMM32      11
#define REGC_IMM16      12
#define REGC_IMM8       13
#define LAST_REGC  REGC_IMM8
#if LAST_REGC >= MAX_REGC
#error "MAX_REGC is to low"
#endif

/* Register class masks */
#define REGCM_FLAGS      (1 << REGC_FLAGS)
#define REGCM_GPR8       (1 << REGC_GPR8)
#define REGCM_GPR16      (1 << REGC_GPR16)
#define REGCM_GPR32      (1 << REGC_GPR32)
#define REGCM_DIVIDEND64 (1 << REGC_DIVIDEND64)
#define REGCM_DIVIDEND32 (1 << REGC_DIVIDEND32)
#define REGCM_MMX        (1 << REGC_MMX)
#define REGCM_XMM        (1 << REGC_XMM)
#define REGCM_GPR32_8    (1 << REGC_GPR32_8)
#define REGCM_GPR16_8    (1 << REGC_GPR16_8)
#define REGCM_GPR8_LO    (1 << REGC_GPR8_LO)
#define REGCM_IMM32      (1 << REGC_IMM32)
#define REGCM_IMM16      (1 << REGC_IMM16)
#define REGCM_IMM8       (1 << REGC_IMM8)
#define REGCM_ALL        ((1 << (LAST_REGC + 1)) - 1)
#define REGCM_IMMALL	(REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8)

/* The x86 registers */
#define REG_EFLAGS  2
#define REGC_FLAGS_FIRST REG_EFLAGS
#define REGC_FLAGS_LAST  REG_EFLAGS
#define REG_AL      3
#define REG_BL      4
#define REG_CL      5
#define REG_DL      6
#define REG_AH      7
#define REG_BH      8
#define REG_CH      9
#define REG_DH      10
#define REGC_GPR8_LO_FIRST REG_AL
#define REGC_GPR8_LO_LAST  REG_DL
#define REGC_GPR8_FIRST  REG_AL
#define REGC_GPR8_LAST   REG_DH
#define REG_AX     11
#define REG_BX     12
#define REG_CX     13
#define REG_DX     14
#define REG_SI     15
#define REG_DI     16
#define REG_BP     17
#define REG_SP     18
#define REGC_GPR16_FIRST REG_AX
#define REGC_GPR16_LAST  REG_SP
#define REG_EAX    19
#define REG_EBX    20
#define REG_ECX    21
#define REG_EDX    22
#define REG_ESI    23
#define REG_EDI    24
#define REG_EBP    25
#define REG_ESP    26
#define REGC_GPR32_FIRST REG_EAX
#define REGC_GPR32_LAST  REG_ESP
#define REG_EDXEAX 27
#define REGC_DIVIDEND64_FIRST REG_EDXEAX
#define REGC_DIVIDEND64_LAST  REG_EDXEAX
#define REG_DXAX   28
#define REGC_DIVIDEND32_FIRST REG_DXAX
#define REGC_DIVIDEND32_LAST  REG_DXAX
#define REG_MMX0   29
#define REG_MMX1   30
#define REG_MMX2   31
#define REG_MMX3   32
#define REG_MMX4   33
#define REG_MMX5   34
#define REG_MMX6   35
#define REG_MMX7   36
#define REGC_MMX_FIRST REG_MMX0
#define REGC_MMX_LAST  REG_MMX7
#define REG_XMM0   37
#define REG_XMM1   38
#define REG_XMM2   39
#define REG_XMM3   40
#define REG_XMM4   41
#define REG_XMM5   42
#define REG_XMM6   43
#define REG_XMM7   44
#define REGC_XMM_FIRST REG_XMM0
#define REGC_XMM_LAST  REG_XMM7

#if DEBUG_ROMCC_WARNINGS
#warning "WISHLIST figure out how to use pinsrw and pextrw to better use extended regs"
#endif

#define LAST_REG   REG_XMM7

#define REGC_GPR32_8_FIRST REG_EAX
#define REGC_GPR32_8_LAST  REG_EDX
#define REGC_GPR16_8_FIRST REG_AX
#define REGC_GPR16_8_LAST  REG_DX

#define REGC_IMM8_FIRST    -1
#define REGC_IMM8_LAST     -1
#define REGC_IMM16_FIRST   -2
#define REGC_IMM16_LAST    -1
#define REGC_IMM32_FIRST   -4
#define REGC_IMM32_LAST    -1

#if LAST_REG >= MAX_REGISTERS
#error "MAX_REGISTERS to low"
#endif


static unsigned regc_size[LAST_REGC +1] = {
	[REGC_FLAGS]      = REGC_FLAGS_LAST      - REGC_FLAGS_FIRST + 1,
	[REGC_GPR8]       = REGC_GPR8_LAST       - REGC_GPR8_FIRST + 1,
	[REGC_GPR16]      = REGC_GPR16_LAST      - REGC_GPR16_FIRST + 1,
	[REGC_GPR32]      = REGC_GPR32_LAST      - REGC_GPR32_FIRST + 1,
	[REGC_DIVIDEND64] = REGC_DIVIDEND64_LAST - REGC_DIVIDEND64_FIRST + 1,
	[REGC_DIVIDEND32] = REGC_DIVIDEND32_LAST - REGC_DIVIDEND32_FIRST + 1,
	[REGC_MMX]        = REGC_MMX_LAST        - REGC_MMX_FIRST + 1,
	[REGC_XMM]        = REGC_XMM_LAST        - REGC_XMM_FIRST + 1,
	[REGC_GPR32_8]    = REGC_GPR32_8_LAST    - REGC_GPR32_8_FIRST + 1,
	[REGC_GPR16_8]    = REGC_GPR16_8_LAST    - REGC_GPR16_8_FIRST + 1,
	[REGC_GPR8_LO]    = REGC_GPR8_LO_LAST    - REGC_GPR8_LO_FIRST + 1,
	[REGC_IMM32]      = 0,
	[REGC_IMM16]      = 0,
	[REGC_IMM8]       = 0,
};

static const struct {
	int first, last;
} regcm_bound[LAST_REGC + 1] = {
	[REGC_FLAGS]      = { REGC_FLAGS_FIRST,      REGC_FLAGS_LAST },
	[REGC_GPR8]       = { REGC_GPR8_FIRST,       REGC_GPR8_LAST },
	[REGC_GPR16]      = { REGC_GPR16_FIRST,      REGC_GPR16_LAST },
	[REGC_GPR32]      = { REGC_GPR32_FIRST,      REGC_GPR32_LAST },
	[REGC_DIVIDEND64] = { REGC_DIVIDEND64_FIRST, REGC_DIVIDEND64_LAST },
	[REGC_DIVIDEND32] = { REGC_DIVIDEND32_FIRST, REGC_DIVIDEND32_LAST },
	[REGC_MMX]        = { REGC_MMX_FIRST,        REGC_MMX_LAST },
	[REGC_XMM]        = { REGC_XMM_FIRST,        REGC_XMM_LAST },
	[REGC_GPR32_8]    = { REGC_GPR32_8_FIRST,    REGC_GPR32_8_LAST },
	[REGC_GPR16_8]    = { REGC_GPR16_8_FIRST,    REGC_GPR16_8_LAST },
	[REGC_GPR8_LO]    = { REGC_GPR8_LO_FIRST,    REGC_GPR8_LO_LAST },
	[REGC_IMM32]      = { REGC_IMM32_FIRST,      REGC_IMM32_LAST },
	[REGC_IMM16]      = { REGC_IMM16_FIRST,      REGC_IMM16_LAST },
	[REGC_IMM8]       = { REGC_IMM8_FIRST,       REGC_IMM8_LAST },
};

#if ARCH_INPUT_REGS != 4
#error ARCH_INPUT_REGS size mismatch
#endif
static const struct reg_info arch_input_regs[ARCH_INPUT_REGS] = {
	{ .reg = REG_EAX, .regcm = REGCM_GPR32 },
	{ .reg = REG_EBX, .regcm = REGCM_GPR32 },
	{ .reg = REG_ECX, .regcm = REGCM_GPR32 },
	{ .reg = REG_EDX, .regcm = REGCM_GPR32 },
};

#if ARCH_OUTPUT_REGS != 4
#error ARCH_INPUT_REGS size mismatch
#endif
static const struct reg_info arch_output_regs[ARCH_OUTPUT_REGS] = {
	{ .reg = REG_EAX, .regcm = REGCM_GPR32 },
	{ .reg = REG_EBX, .regcm = REGCM_GPR32 },
	{ .reg = REG_ECX, .regcm = REGCM_GPR32 },
	{ .reg = REG_EDX, .regcm = REGCM_GPR32 },
};

static void init_arch_state(struct arch_state *arch)
{
	memset(arch, 0, sizeof(*arch));
	arch->features = 0;
}

static const struct compiler_flag arch_flags[] = {
	{ "mmx",       X86_MMX_REGS },
	{ "sse",       X86_XMM_REGS },
	{ "noop-copy", X86_NOOP_COPY },
	{ 0,     0 },
};
static const struct compiler_flag arch_cpus[] = {
	{ "i386", 0 },
	{ "p2",   X86_MMX_REGS },
	{ "p3",   X86_MMX_REGS | X86_XMM_REGS },
	{ "p4",   X86_MMX_REGS | X86_XMM_REGS },
	{ "k7",   X86_MMX_REGS },
	{ "k8",   X86_MMX_REGS | X86_XMM_REGS },
	{ "c3",   X86_MMX_REGS },
	{ "c3-2", X86_MMX_REGS | X86_XMM_REGS }, /* Nehemiah */
	{  0,     0 }
};
static int arch_encode_flag(struct arch_state *arch, const char *flag)
{
	int result;
	int act;

	act = 1;
	if (strncmp(flag, "no-", 3) == 0) {
		flag += 3;
		act = 0;
	}
	if (act && strncmp(flag, "cpu=", 4) == 0) {
		flag += 4;
		result = set_flag(arch_cpus, &arch->features, 1, flag);
	}
	else {
		result = set_flag(arch_flags, &arch->features, act, flag);
	}
	return result;
}

static void arch_usage(FILE *fp)
{
	flag_usage(fp, arch_flags, "-m", "-mno-");
	flag_usage(fp, arch_cpus, "-mcpu=", 0);
}

static unsigned arch_regc_size(struct compile_state *state, int class)
{
	if ((class < 0) || (class > LAST_REGC)) {
		return 0;
	}
	return regc_size[class];
}

static int arch_regcm_intersect(unsigned regcm1, unsigned regcm2)
{
	/* See if two register classes may have overlapping registers */
	unsigned gpr_mask = REGCM_GPR8 | REGCM_GPR8_LO | REGCM_GPR16_8 | REGCM_GPR16 |
		REGCM_GPR32_8 | REGCM_GPR32 |
		REGCM_DIVIDEND32 | REGCM_DIVIDEND64;

	/* Special case for the immediates */
	if ((regcm1 & (REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8)) &&
		((regcm1 & ~(REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8)) == 0) &&
		(regcm2 & (REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8)) &&
		((regcm2 & ~(REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8)) == 0)) {
		return 0;
	}
	return (regcm1 & regcm2) ||
		((regcm1 & gpr_mask) && (regcm2 & gpr_mask));
}

static void arch_reg_equivs(
	struct compile_state *state, unsigned *equiv, int reg)
{
	if ((reg < 0) || (reg > LAST_REG)) {
		internal_error(state, 0, "invalid register");
	}
	*equiv++ = reg;
	switch(reg) {
	case REG_AL:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_AH;
#endif
		*equiv++ = REG_AX;
		*equiv++ = REG_EAX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_AH:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_AL;
#endif
		*equiv++ = REG_AX;
		*equiv++ = REG_EAX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_BL:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_BH;
#endif
		*equiv++ = REG_BX;
		*equiv++ = REG_EBX;
		break;

	case REG_BH:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_BL;
#endif
		*equiv++ = REG_BX;
		*equiv++ = REG_EBX;
		break;
	case REG_CL:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_CH;
#endif
		*equiv++ = REG_CX;
		*equiv++ = REG_ECX;
		break;

	case REG_CH:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_CL;
#endif
		*equiv++ = REG_CX;
		*equiv++ = REG_ECX;
		break;
	case REG_DL:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_DH;
#endif
		*equiv++ = REG_DX;
		*equiv++ = REG_EDX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_DH:
#if X86_4_8BIT_GPRS
		*equiv++ = REG_DL;
#endif
		*equiv++ = REG_DX;
		*equiv++ = REG_EDX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_AX:
		*equiv++ = REG_AL;
		*equiv++ = REG_AH;
		*equiv++ = REG_EAX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_BX:
		*equiv++ = REG_BL;
		*equiv++ = REG_BH;
		*equiv++ = REG_EBX;
		break;
	case REG_CX:
		*equiv++ = REG_CL;
		*equiv++ = REG_CH;
		*equiv++ = REG_ECX;
		break;
	case REG_DX:
		*equiv++ = REG_DL;
		*equiv++ = REG_DH;
		*equiv++ = REG_EDX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_SI:
		*equiv++ = REG_ESI;
		break;
	case REG_DI:
		*equiv++ = REG_EDI;
		break;
	case REG_BP:
		*equiv++ = REG_EBP;
		break;
	case REG_SP:
		*equiv++ = REG_ESP;
		break;
	case REG_EAX:
		*equiv++ = REG_AL;
		*equiv++ = REG_AH;
		*equiv++ = REG_AX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_EBX:
		*equiv++ = REG_BL;
		*equiv++ = REG_BH;
		*equiv++ = REG_BX;
		break;
	case REG_ECX:
		*equiv++ = REG_CL;
		*equiv++ = REG_CH;
		*equiv++ = REG_CX;
		break;
	case REG_EDX:
		*equiv++ = REG_DL;
		*equiv++ = REG_DH;
		*equiv++ = REG_DX;
		*equiv++ = REG_DXAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_ESI:
		*equiv++ = REG_SI;
		break;
	case REG_EDI:
		*equiv++ = REG_DI;
		break;
	case REG_EBP:
		*equiv++ = REG_BP;
		break;
	case REG_ESP:
		*equiv++ = REG_SP;
		break;
	case REG_DXAX:
		*equiv++ = REG_AL;
		*equiv++ = REG_AH;
		*equiv++ = REG_DL;
		*equiv++ = REG_DH;
		*equiv++ = REG_AX;
		*equiv++ = REG_DX;
		*equiv++ = REG_EAX;
		*equiv++ = REG_EDX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_EDXEAX:
		*equiv++ = REG_AL;
		*equiv++ = REG_AH;
		*equiv++ = REG_DL;
		*equiv++ = REG_DH;
		*equiv++ = REG_AX;
		*equiv++ = REG_DX;
		*equiv++ = REG_EAX;
		*equiv++ = REG_EDX;
		*equiv++ = REG_DXAX;
		break;
	}
	*equiv++ = REG_UNSET;
}

static unsigned arch_avail_mask(struct compile_state *state)
{
	unsigned avail_mask;
	/* REGCM_GPR8 is not available */
	avail_mask = REGCM_GPR8_LO | REGCM_GPR16_8 | REGCM_GPR16 |
		REGCM_GPR32 | REGCM_GPR32_8 |
		REGCM_DIVIDEND32 | REGCM_DIVIDEND64 |
		REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8 | REGCM_FLAGS;
	if (state->arch->features & X86_MMX_REGS) {
		avail_mask |= REGCM_MMX;
	}
	if (state->arch->features & X86_XMM_REGS) {
		avail_mask |= REGCM_XMM;
	}
	return avail_mask;
}

static unsigned arch_regcm_normalize(struct compile_state *state, unsigned regcm)
{
	unsigned mask, result;
	int class, class2;
	result = regcm;

	for(class = 0, mask = 1; mask; mask <<= 1, class++) {
		if ((result & mask) == 0) {
			continue;
		}
		if (class > LAST_REGC) {
			result &= ~mask;
		}
		for(class2 = 0; class2 <= LAST_REGC; class2++) {
			if ((regcm_bound[class2].first >= regcm_bound[class].first) &&
				(regcm_bound[class2].last <= regcm_bound[class].last)) {
				result |= (1 << class2);
			}
		}
	}
	result &= arch_avail_mask(state);
	return result;
}

static unsigned arch_regcm_reg_normalize(struct compile_state *state, unsigned regcm)
{
	/* Like arch_regcm_normalize except immediate register classes are excluded */
	regcm = arch_regcm_normalize(state, regcm);
	/* Remove the immediate register classes */
	regcm &= ~(REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8);
	return regcm;

}

static unsigned arch_reg_regcm(struct compile_state *state, int reg)
{
	unsigned mask;
	int class;
	mask = 0;
	for(class = 0; class <= LAST_REGC; class++) {
		if ((reg >= regcm_bound[class].first) &&
			(reg <= regcm_bound[class].last)) {
			mask |= (1 << class);
		}
	}
	if (!mask) {
		internal_error(state, 0, "reg %d not in any class", reg);
	}
	return mask;
}

static struct reg_info arch_reg_constraint(
	struct compile_state *state, struct type *type, const char *constraint)
{
	static const struct {
		char class;
		unsigned int mask;
		unsigned int reg;
	} constraints[] = {
		{ 'r', REGCM_GPR32,   REG_UNSET },
		{ 'g', REGCM_GPR32,   REG_UNSET },
		{ 'p', REGCM_GPR32,   REG_UNSET },
		{ 'q', REGCM_GPR8_LO, REG_UNSET },
		{ 'Q', REGCM_GPR32_8, REG_UNSET },
		{ 'x', REGCM_XMM,     REG_UNSET },
		{ 'y', REGCM_MMX,     REG_UNSET },
		{ 'a', REGCM_GPR32,   REG_EAX },
		{ 'b', REGCM_GPR32,   REG_EBX },
		{ 'c', REGCM_GPR32,   REG_ECX },
		{ 'd', REGCM_GPR32,   REG_EDX },
		{ 'D', REGCM_GPR32,   REG_EDI },
		{ 'S', REGCM_GPR32,   REG_ESI },
		{ '\0', 0, REG_UNSET },
	};
	unsigned int regcm;
	unsigned int mask, reg;
	struct reg_info result;
	const char *ptr;
	regcm = arch_type_to_regcm(state, type);
	reg = REG_UNSET;
	mask = 0;
	for(ptr = constraint; *ptr; ptr++) {
		int i;
		if (*ptr ==  ' ') {
			continue;
		}
		for(i = 0; constraints[i].class != '\0'; i++) {
			if (constraints[i].class == *ptr) {
				break;
			}
		}
		if (constraints[i].class == '\0') {
			error(state, 0, "invalid register constraint ``%c''", *ptr);
			break;
		}
		if ((constraints[i].mask & regcm) == 0) {
			error(state, 0, "invalid register class %c specified",
				*ptr);
		}
		mask |= constraints[i].mask;
		if (constraints[i].reg != REG_UNSET) {
			if ((reg != REG_UNSET) && (reg != constraints[i].reg)) {
				error(state, 0, "Only one register may be specified");
			}
			reg = constraints[i].reg;
		}
	}
	result.reg = reg;
	result.regcm = mask;
	return result;
}

static struct reg_info arch_reg_clobber(
	struct compile_state *state, const char *clobber)
{
	struct reg_info result;
	if (strcmp(clobber, "memory") == 0) {
		result.reg = REG_UNSET;
		result.regcm = 0;
	}
	else if (strcmp(clobber, "eax") == 0) {
		result.reg = REG_EAX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "ebx") == 0) {
		result.reg = REG_EBX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "ecx") == 0) {
		result.reg = REG_ECX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "edx") == 0) {
		result.reg = REG_EDX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "esi") == 0) {
		result.reg = REG_ESI;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "edi") == 0) {
		result.reg = REG_EDI;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "ebp") == 0) {
		result.reg = REG_EBP;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "esp") == 0) {
		result.reg = REG_ESP;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "cc") == 0) {
		result.reg = REG_EFLAGS;
		result.regcm = REGCM_FLAGS;
	}
	else if ((strncmp(clobber, "xmm", 3) == 0)  &&
		octdigitp(clobber[3]) && (clobber[4] == '\0')) {
		result.reg = REG_XMM0 + octdigval(clobber[3]);
		result.regcm = REGCM_XMM;
	}
	else if ((strncmp(clobber, "mm", 2) == 0) &&
		octdigitp(clobber[3]) && (clobber[4] == '\0')) {
		result.reg = REG_MMX0 + octdigval(clobber[3]);
		result.regcm = REGCM_MMX;
	}
	else {
		error(state, 0, "unknown register name `%s' in asm",
			clobber);
		result.reg = REG_UNSET;
		result.regcm = 0;
	}
	return result;
}

static int do_select_reg(struct compile_state *state,
	char *used, int reg, unsigned classes)
{
	unsigned mask;
	if (used[reg]) {
		return REG_UNSET;
	}
	mask = arch_reg_regcm(state, reg);
	return (classes & mask) ? reg : REG_UNSET;
}

static int arch_select_free_register(
	struct compile_state *state, char *used, int classes)
{
	/* Live ranges with the most neighbors are colored first.
	 *
	 * Generally it does not matter which colors are given
	 * as the register allocator attempts to color live ranges
	 * in an order where you are guaranteed not to run out of colors.
	 *
	 * Occasionally the register allocator cannot find an order
	 * of register selection that will find a free color.  To
	 * increase the odds the register allocator will work when
	 * it guesses first give out registers from register classes
	 * least likely to run out of registers.
	 *
	 */
	int i, reg;
	reg = REG_UNSET;
	for(i = REGC_XMM_FIRST; (reg == REG_UNSET) && (i <= REGC_XMM_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_MMX_FIRST; (reg == REG_UNSET) && (i <= REGC_MMX_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR32_LAST; (reg == REG_UNSET) && (i >= REGC_GPR32_FIRST); i--) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR16_FIRST; (reg == REG_UNSET) && (i <= REGC_GPR16_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR8_FIRST; (reg == REG_UNSET) && (i <= REGC_GPR8_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR8_LO_FIRST; (reg == REG_UNSET) && (i <= REGC_GPR8_LO_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_DIVIDEND32_FIRST; (reg == REG_UNSET) && (i <= REGC_DIVIDEND32_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_DIVIDEND64_FIRST; (reg == REG_UNSET) && (i <= REGC_DIVIDEND64_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_FLAGS_FIRST; (reg == REG_UNSET) && (i <= REGC_FLAGS_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	return reg;
}


static unsigned arch_type_to_regcm(struct compile_state *state, struct type *type)
{

#if DEBUG_ROMCC_WARNINGS
#warning "FIXME force types smaller (if legal) before I get here"
#endif
	unsigned mask;
	switch(type->type & TYPE_MASK) {
	case TYPE_ARRAY:
	case TYPE_VOID:
		mask = 0;
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		mask = REGCM_GPR8 | REGCM_GPR8_LO |
			REGCM_GPR16 | REGCM_GPR16_8 |
			REGCM_GPR32 | REGCM_GPR32_8 |
			REGCM_DIVIDEND32 | REGCM_DIVIDEND64 |
			REGCM_MMX | REGCM_XMM |
			REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8;
		break;
	case TYPE_SHORT:
	case TYPE_USHORT:
		mask = 	REGCM_GPR16 | REGCM_GPR16_8 |
			REGCM_GPR32 | REGCM_GPR32_8 |
			REGCM_DIVIDEND32 | REGCM_DIVIDEND64 |
			REGCM_MMX | REGCM_XMM |
			REGCM_IMM32 | REGCM_IMM16;
		break;
	case TYPE_ENUM:
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_LONG:
	case TYPE_ULONG:
	case TYPE_POINTER:
		mask = 	REGCM_GPR32 | REGCM_GPR32_8 |
			REGCM_DIVIDEND32 | REGCM_DIVIDEND64 |
			REGCM_MMX | REGCM_XMM |
			REGCM_IMM32;
		break;
	case TYPE_JOIN:
	case TYPE_UNION:
		mask = arch_type_to_regcm(state, type->left);
		break;
	case TYPE_OVERLAP:
		mask = arch_type_to_regcm(state, type->left) &
			arch_type_to_regcm(state, type->right);
		break;
	case TYPE_BITFIELD:
		mask = arch_type_to_regcm(state, type->left);
		break;
	default:
		fprintf(state->errout, "type: ");
		name_of(state->errout, type);
		fprintf(state->errout, "\n");
		internal_error(state, 0, "no register class for type");
		break;
	}
	mask = arch_regcm_normalize(state, mask);
	return mask;
}

static int is_imm32(struct triple *imm)
{
	// second condition commented out to prevent compiler warning:
	// imm->u.cval is always 32bit unsigned, so the comparison is
	// always true.
	return ((imm->op == OP_INTCONST) /* && (imm->u.cval <= 0xffffffffUL) */ ) ||
		(imm->op == OP_ADDRCONST);

}
static int is_imm16(struct triple *imm)
{
	return ((imm->op == OP_INTCONST) && (imm->u.cval <= 0xffff));
}
static int is_imm8(struct triple *imm)
{
	return ((imm->op == OP_INTCONST) && (imm->u.cval <= 0xff));
}

static int get_imm32(struct triple *ins, struct triple **expr)
{
	struct triple *imm;
	imm = *expr;
	while(imm->op == OP_COPY) {
		imm = RHS(imm, 0);
	}
	if (!is_imm32(imm)) {
		return 0;
	}
	unuse_triple(*expr, ins);
	use_triple(imm, ins);
	*expr = imm;
	return 1;
}

static int get_imm8(struct triple *ins, struct triple **expr)
{
	struct triple *imm;
	imm = *expr;
	while(imm->op == OP_COPY) {
		imm = RHS(imm, 0);
	}
	if (!is_imm8(imm)) {
		return 0;
	}
	unuse_triple(*expr, ins);
	use_triple(imm, ins);
	*expr = imm;
	return 1;
}

#define TEMPLATE_NOP           0
#define TEMPLATE_INTCONST8     1
#define TEMPLATE_INTCONST32    2
#define TEMPLATE_UNKNOWNVAL    3
#define TEMPLATE_COPY8_REG     5
#define TEMPLATE_COPY16_REG    6
#define TEMPLATE_COPY32_REG    7
#define TEMPLATE_COPY_IMM8     8
#define TEMPLATE_COPY_IMM16    9
#define TEMPLATE_COPY_IMM32   10
#define TEMPLATE_PHI8         11
#define TEMPLATE_PHI16        12
#define TEMPLATE_PHI32        13
#define TEMPLATE_STORE8       14
#define TEMPLATE_STORE16      15
#define TEMPLATE_STORE32      16
#define TEMPLATE_LOAD8        17
#define TEMPLATE_LOAD16       18
#define TEMPLATE_LOAD32       19
#define TEMPLATE_BINARY8_REG  20
#define TEMPLATE_BINARY16_REG 21
#define TEMPLATE_BINARY32_REG 22
#define TEMPLATE_BINARY8_IMM  23
#define TEMPLATE_BINARY16_IMM 24
#define TEMPLATE_BINARY32_IMM 25
#define TEMPLATE_SL8_CL       26
#define TEMPLATE_SL16_CL      27
#define TEMPLATE_SL32_CL      28
#define TEMPLATE_SL8_IMM      29
#define TEMPLATE_SL16_IMM     30
#define TEMPLATE_SL32_IMM     31
#define TEMPLATE_UNARY8       32
#define TEMPLATE_UNARY16      33
#define TEMPLATE_UNARY32      34
#define TEMPLATE_CMP8_REG     35
#define TEMPLATE_CMP16_REG    36
#define TEMPLATE_CMP32_REG    37
#define TEMPLATE_CMP8_IMM     38
#define TEMPLATE_CMP16_IMM    39
#define TEMPLATE_CMP32_IMM    40
#define TEMPLATE_TEST8        41
#define TEMPLATE_TEST16       42
#define TEMPLATE_TEST32       43
#define TEMPLATE_SET          44
#define TEMPLATE_JMP          45
#define TEMPLATE_RET          46
#define TEMPLATE_INB_DX       47
#define TEMPLATE_INB_IMM      48
#define TEMPLATE_INW_DX       49
#define TEMPLATE_INW_IMM      50
#define TEMPLATE_INL_DX       51
#define TEMPLATE_INL_IMM      52
#define TEMPLATE_OUTB_DX      53
#define TEMPLATE_OUTB_IMM     54
#define TEMPLATE_OUTW_DX      55
#define TEMPLATE_OUTW_IMM     56
#define TEMPLATE_OUTL_DX      57
#define TEMPLATE_OUTL_IMM     58
#define TEMPLATE_BSF          59
#define TEMPLATE_RDMSR        60
#define TEMPLATE_WRMSR        61
#define TEMPLATE_UMUL8        62
#define TEMPLATE_UMUL16       63
#define TEMPLATE_UMUL32       64
#define TEMPLATE_DIV8         65
#define TEMPLATE_DIV16        66
#define TEMPLATE_DIV32        67
#define LAST_TEMPLATE       TEMPLATE_DIV32
#if LAST_TEMPLATE >= MAX_TEMPLATES
#error "MAX_TEMPLATES to low"
#endif

#define COPY8_REGCM     (REGCM_DIVIDEND64 | REGCM_DIVIDEND32 | REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO | REGCM_MMX | REGCM_XMM)
#define COPY16_REGCM    (REGCM_DIVIDEND64 | REGCM_DIVIDEND32 | REGCM_GPR32 | REGCM_GPR16 | REGCM_MMX | REGCM_XMM)
#define COPY32_REGCM    (REGCM_DIVIDEND64 | REGCM_DIVIDEND32 | REGCM_GPR32 | REGCM_MMX | REGCM_XMM)


static struct ins_template templates[] = {
	[TEMPLATE_NOP]      = {
		.lhs = {
			[ 0] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 1] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 2] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 3] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 4] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 5] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 6] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 7] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 8] = { REG_UNNEEDED, REGCM_IMMALL },
			[ 9] = { REG_UNNEEDED, REGCM_IMMALL },
			[10] = { REG_UNNEEDED, REGCM_IMMALL },
			[11] = { REG_UNNEEDED, REGCM_IMMALL },
			[12] = { REG_UNNEEDED, REGCM_IMMALL },
			[13] = { REG_UNNEEDED, REGCM_IMMALL },
			[14] = { REG_UNNEEDED, REGCM_IMMALL },
			[15] = { REG_UNNEEDED, REGCM_IMMALL },
			[16] = { REG_UNNEEDED, REGCM_IMMALL },
			[17] = { REG_UNNEEDED, REGCM_IMMALL },
			[18] = { REG_UNNEEDED, REGCM_IMMALL },
			[19] = { REG_UNNEEDED, REGCM_IMMALL },
			[20] = { REG_UNNEEDED, REGCM_IMMALL },
			[21] = { REG_UNNEEDED, REGCM_IMMALL },
			[22] = { REG_UNNEEDED, REGCM_IMMALL },
			[23] = { REG_UNNEEDED, REGCM_IMMALL },
			[24] = { REG_UNNEEDED, REGCM_IMMALL },
			[25] = { REG_UNNEEDED, REGCM_IMMALL },
			[26] = { REG_UNNEEDED, REGCM_IMMALL },
			[27] = { REG_UNNEEDED, REGCM_IMMALL },
			[28] = { REG_UNNEEDED, REGCM_IMMALL },
			[29] = { REG_UNNEEDED, REGCM_IMMALL },
			[30] = { REG_UNNEEDED, REGCM_IMMALL },
			[31] = { REG_UNNEEDED, REGCM_IMMALL },
			[32] = { REG_UNNEEDED, REGCM_IMMALL },
			[33] = { REG_UNNEEDED, REGCM_IMMALL },
			[34] = { REG_UNNEEDED, REGCM_IMMALL },
			[35] = { REG_UNNEEDED, REGCM_IMMALL },
			[36] = { REG_UNNEEDED, REGCM_IMMALL },
			[37] = { REG_UNNEEDED, REGCM_IMMALL },
			[38] = { REG_UNNEEDED, REGCM_IMMALL },
			[39] = { REG_UNNEEDED, REGCM_IMMALL },
			[40] = { REG_UNNEEDED, REGCM_IMMALL },
			[41] = { REG_UNNEEDED, REGCM_IMMALL },
			[42] = { REG_UNNEEDED, REGCM_IMMALL },
			[43] = { REG_UNNEEDED, REGCM_IMMALL },
			[44] = { REG_UNNEEDED, REGCM_IMMALL },
			[45] = { REG_UNNEEDED, REGCM_IMMALL },
			[46] = { REG_UNNEEDED, REGCM_IMMALL },
			[47] = { REG_UNNEEDED, REGCM_IMMALL },
			[48] = { REG_UNNEEDED, REGCM_IMMALL },
			[49] = { REG_UNNEEDED, REGCM_IMMALL },
			[50] = { REG_UNNEEDED, REGCM_IMMALL },
			[51] = { REG_UNNEEDED, REGCM_IMMALL },
			[52] = { REG_UNNEEDED, REGCM_IMMALL },
			[53] = { REG_UNNEEDED, REGCM_IMMALL },
			[54] = { REG_UNNEEDED, REGCM_IMMALL },
			[55] = { REG_UNNEEDED, REGCM_IMMALL },
			[56] = { REG_UNNEEDED, REGCM_IMMALL },
			[57] = { REG_UNNEEDED, REGCM_IMMALL },
			[58] = { REG_UNNEEDED, REGCM_IMMALL },
			[59] = { REG_UNNEEDED, REGCM_IMMALL },
			[60] = { REG_UNNEEDED, REGCM_IMMALL },
			[61] = { REG_UNNEEDED, REGCM_IMMALL },
			[62] = { REG_UNNEEDED, REGCM_IMMALL },
			[63] = { REG_UNNEEDED, REGCM_IMMALL },
		},
	},
	[TEMPLATE_INTCONST8] = {
		.lhs = { [0] = { REG_UNNEEDED, REGCM_IMM8 } },
	},
	[TEMPLATE_INTCONST32] = {
		.lhs = { [0] = { REG_UNNEEDED, REGCM_IMM32 } },
	},
	[TEMPLATE_UNKNOWNVAL] = {
		.lhs = { [0] = { REG_UNSET, COPY32_REGCM } },
	},
	[TEMPLATE_COPY8_REG] = {
		.lhs = { [0] = { REG_UNSET, COPY8_REGCM } },
		.rhs = { [0] = { REG_UNSET, COPY8_REGCM }  },
	},
	[TEMPLATE_COPY16_REG] = {
		.lhs = { [0] = { REG_UNSET, COPY16_REGCM } },
		.rhs = { [0] = { REG_UNSET, COPY16_REGCM }  },
	},
	[TEMPLATE_COPY32_REG] = {
		.lhs = { [0] = { REG_UNSET, COPY32_REGCM } },
		.rhs = { [0] = { REG_UNSET, COPY32_REGCM }  },
	},
	[TEMPLATE_COPY_IMM8] = {
		.lhs = { [0] = { REG_UNSET, COPY8_REGCM } },
		.rhs = { [0] = { REG_UNNEEDED, REGCM_IMM8 } },
	},
	[TEMPLATE_COPY_IMM16] = {
		.lhs = { [0] = { REG_UNSET, COPY16_REGCM } },
		.rhs = { [0] = { REG_UNNEEDED, REGCM_IMM16 | REGCM_IMM8 } },
	},
	[TEMPLATE_COPY_IMM32] = {
		.lhs = { [0] = { REG_UNSET, COPY32_REGCM } },
		.rhs = { [0] = { REG_UNNEEDED, REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8 } },
	},
	[TEMPLATE_PHI8] = {
		.lhs = { [0] = { REG_VIRT0, COPY8_REGCM } },
		.rhs = { [0] = { REG_VIRT0, COPY8_REGCM } },
	},
	[TEMPLATE_PHI16] = {
		.lhs = { [0] = { REG_VIRT0, COPY16_REGCM } },
		.rhs = { [0] = { REG_VIRT0, COPY16_REGCM } },
	},
	[TEMPLATE_PHI32] = {
		.lhs = { [0] = { REG_VIRT0, COPY32_REGCM } },
		.rhs = { [0] = { REG_VIRT0, COPY32_REGCM } },
	},
	[TEMPLATE_STORE8] = {
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR32 },
			[1] = { REG_UNSET, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_STORE16] = {
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR32 },
			[1] = { REG_UNSET, REGCM_GPR16 },
		},
	},
	[TEMPLATE_STORE32] = {
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR32 },
			[1] = { REG_UNSET, REGCM_GPR32 },
		},
	},
	[TEMPLATE_LOAD8] = {
		.lhs = { [0] = { REG_UNSET, REGCM_GPR8_LO } },
		.rhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
	},
	[TEMPLATE_LOAD16] = {
		.lhs = { [0] = { REG_UNSET, REGCM_GPR16 } },
		.rhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
	},
	[TEMPLATE_LOAD32] = {
		.lhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
		.rhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
	},
	[TEMPLATE_BINARY8_REG] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR8_LO } },
		.rhs = {
			[0] = { REG_VIRT0, REGCM_GPR8_LO },
			[1] = { REG_UNSET, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_BINARY16_REG] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR16 } },
		.rhs = {
			[0] = { REG_VIRT0, REGCM_GPR16 },
			[1] = { REG_UNSET, REGCM_GPR16 },
		},
	},
	[TEMPLATE_BINARY32_REG] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR32 } },
		.rhs = {
			[0] = { REG_VIRT0, REGCM_GPR32 },
			[1] = { REG_UNSET, REGCM_GPR32 },
		},
	},
	[TEMPLATE_BINARY8_IMM] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR8_LO } },
		.rhs = {
			[0] = { REG_VIRT0,    REGCM_GPR8_LO },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_BINARY16_IMM] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR16 } },
		.rhs = {
			[0] = { REG_VIRT0,    REGCM_GPR16 },
			[1] = { REG_UNNEEDED, REGCM_IMM16 },
		},
	},
	[TEMPLATE_BINARY32_IMM] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR32 } },
		.rhs = {
			[0] = { REG_VIRT0,    REGCM_GPR32 },
			[1] = { REG_UNNEEDED, REGCM_IMM32 },
		},
	},
	[TEMPLATE_SL8_CL] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR8_LO } },
		.rhs = {
			[0] = { REG_VIRT0, REGCM_GPR8_LO },
			[1] = { REG_CL, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_SL16_CL] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR16 } },
		.rhs = {
			[0] = { REG_VIRT0, REGCM_GPR16 },
			[1] = { REG_CL, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_SL32_CL] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR32 } },
		.rhs = {
			[0] = { REG_VIRT0, REGCM_GPR32 },
			[1] = { REG_CL, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_SL8_IMM] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR8_LO } },
		.rhs = {
			[0] = { REG_VIRT0,    REGCM_GPR8_LO },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_SL16_IMM] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR16 } },
		.rhs = {
			[0] = { REG_VIRT0,    REGCM_GPR16 },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_SL32_IMM] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR32 } },
		.rhs = {
			[0] = { REG_VIRT0,    REGCM_GPR32 },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_UNARY8] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR8_LO } },
		.rhs = { [0] = { REG_VIRT0, REGCM_GPR8_LO } },
	},
	[TEMPLATE_UNARY16] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR16 } },
		.rhs = { [0] = { REG_VIRT0, REGCM_GPR16 } },
	},
	[TEMPLATE_UNARY32] = {
		.lhs = { [0] = { REG_VIRT0, REGCM_GPR32 } },
		.rhs = { [0] = { REG_VIRT0, REGCM_GPR32 } },
	},
	[TEMPLATE_CMP8_REG] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR8_LO },
			[1] = { REG_UNSET, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_CMP16_REG] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR16 },
			[1] = { REG_UNSET, REGCM_GPR16 },
		},
	},
	[TEMPLATE_CMP32_REG] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR32 },
			[1] = { REG_UNSET, REGCM_GPR32 },
		},
	},
	[TEMPLATE_CMP8_IMM] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR8_LO },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_CMP16_IMM] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR16 },
			[1] = { REG_UNNEEDED, REGCM_IMM16 },
		},
	},
	[TEMPLATE_CMP32_IMM] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = {
			[0] = { REG_UNSET, REGCM_GPR32 },
			[1] = { REG_UNNEEDED, REGCM_IMM32 },
		},
	},
	[TEMPLATE_TEST8] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = { [0] = { REG_UNSET, REGCM_GPR8_LO } },
	},
	[TEMPLATE_TEST16] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = { [0] = { REG_UNSET, REGCM_GPR16 } },
	},
	[TEMPLATE_TEST32] = {
		.lhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
		.rhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
	},
	[TEMPLATE_SET] = {
		.lhs = { [0] = { REG_UNSET, REGCM_GPR8_LO } },
		.rhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
	},
	[TEMPLATE_JMP] = {
		.rhs = { [0] = { REG_EFLAGS, REGCM_FLAGS } },
	},
	[TEMPLATE_RET] = {
		.rhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
	},
	[TEMPLATE_INB_DX] = {
		.lhs = { [0] = { REG_AL,  REGCM_GPR8_LO } },
		.rhs = { [0] = { REG_DX, REGCM_GPR16 } },
	},
	[TEMPLATE_INB_IMM] = {
		.lhs = { [0] = { REG_AL,  REGCM_GPR8_LO } },
		.rhs = { [0] = { REG_UNNEEDED, REGCM_IMM8 } },
	},
	[TEMPLATE_INW_DX]  = {
		.lhs = { [0] = { REG_AX,  REGCM_GPR16 } },
		.rhs = { [0] = { REG_DX, REGCM_GPR16 } },
	},
	[TEMPLATE_INW_IMM] = {
		.lhs = { [0] = { REG_AX,  REGCM_GPR16 } },
		.rhs = { [0] = { REG_UNNEEDED, REGCM_IMM8 } },
	},
	[TEMPLATE_INL_DX]  = {
		.lhs = { [0] = { REG_EAX, REGCM_GPR32 } },
		.rhs = { [0] = { REG_DX, REGCM_GPR16 } },
	},
	[TEMPLATE_INL_IMM] = {
		.lhs = { [0] = { REG_EAX, REGCM_GPR32 } },
		.rhs = { [0] = { REG_UNNEEDED, REGCM_IMM8 } },
	},
	[TEMPLATE_OUTB_DX] = {
		.rhs = {
			[0] = { REG_AL,  REGCM_GPR8_LO },
			[1] = { REG_DX, REGCM_GPR16 },
		},
	},
	[TEMPLATE_OUTB_IMM] = {
		.rhs = {
			[0] = { REG_AL,  REGCM_GPR8_LO },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_OUTW_DX] = {
		.rhs = {
			[0] = { REG_AX,  REGCM_GPR16 },
			[1] = { REG_DX, REGCM_GPR16 },
		},
	},
	[TEMPLATE_OUTW_IMM] = {
		.rhs = {
			[0] = { REG_AX,  REGCM_GPR16 },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_OUTL_DX] = {
		.rhs = {
			[0] = { REG_EAX, REGCM_GPR32 },
			[1] = { REG_DX, REGCM_GPR16 },
		},
	},
	[TEMPLATE_OUTL_IMM] = {
		.rhs = {
			[0] = { REG_EAX, REGCM_GPR32 },
			[1] = { REG_UNNEEDED, REGCM_IMM8 },
		},
	},
	[TEMPLATE_BSF] = {
		.lhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
		.rhs = { [0] = { REG_UNSET, REGCM_GPR32 } },
	},
	[TEMPLATE_RDMSR] = {
		.lhs = {
			[0] = { REG_EAX, REGCM_GPR32 },
			[1] = { REG_EDX, REGCM_GPR32 },
		},
		.rhs = { [0] = { REG_ECX, REGCM_GPR32 } },
	},
	[TEMPLATE_WRMSR] = {
		.rhs = {
			[0] = { REG_ECX, REGCM_GPR32 },
			[1] = { REG_EAX, REGCM_GPR32 },
			[2] = { REG_EDX, REGCM_GPR32 },
		},
	},
	[TEMPLATE_UMUL8] = {
		.lhs = { [0] = { REG_AX, REGCM_GPR16 } },
		.rhs = {
			[0] = { REG_AL, REGCM_GPR8_LO },
			[1] = { REG_UNSET, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_UMUL16] = {
		.lhs = { [0] = { REG_DXAX, REGCM_DIVIDEND32 } },
		.rhs = {
			[0] = { REG_AX, REGCM_GPR16 },
			[1] = { REG_UNSET, REGCM_GPR16 },
		},
	},
	[TEMPLATE_UMUL32] = {
		.lhs = { [0] = { REG_EDXEAX, REGCM_DIVIDEND64 } },
		.rhs = {
			[0] = { REG_EAX, REGCM_GPR32 },
			[1] = { REG_UNSET, REGCM_GPR32 },
		},
	},
	[TEMPLATE_DIV8] = {
		.lhs = {
			[0] = { REG_AL, REGCM_GPR8_LO },
			[1] = { REG_AH, REGCM_GPR8 },
		},
		.rhs = {
			[0] = { REG_AX, REGCM_GPR16 },
			[1] = { REG_UNSET, REGCM_GPR8_LO },
		},
	},
	[TEMPLATE_DIV16] = {
		.lhs = {
			[0] = { REG_AX, REGCM_GPR16 },
			[1] = { REG_DX, REGCM_GPR16 },
		},
		.rhs = {
			[0] = { REG_DXAX, REGCM_DIVIDEND32 },
			[1] = { REG_UNSET, REGCM_GPR16 },
		},
	},
	[TEMPLATE_DIV32] = {
		.lhs = {
			[0] = { REG_EAX, REGCM_GPR32 },
			[1] = { REG_EDX, REGCM_GPR32 },
		},
		.rhs = {
			[0] = { REG_EDXEAX, REGCM_DIVIDEND64 },
			[1] = { REG_UNSET, REGCM_GPR32 },
		},
	},
};

static void fixup_branch(struct compile_state *state,
	struct triple *branch, int jmp_op, int cmp_op, struct type *cmp_type,
	struct triple *left, struct triple *right)
{
	struct triple *test;
	if (!left) {
		internal_error(state, branch, "no branch test?");
	}
	test = pre_triple(state, branch,
		cmp_op, cmp_type, left, right);
	test->template_id = TEMPLATE_TEST32;
	if (cmp_op == OP_CMP) {
		test->template_id = TEMPLATE_CMP32_REG;
		if (get_imm32(test, &RHS(test, 1))) {
			test->template_id = TEMPLATE_CMP32_IMM;
		}
	}
	use_triple(RHS(test, 0), test);
	use_triple(RHS(test, 1), test);
	unuse_triple(RHS(branch, 0), branch);
	RHS(branch, 0) = test;
	branch->op = jmp_op;
	branch->template_id = TEMPLATE_JMP;
	use_triple(RHS(branch, 0), branch);
}

static void fixup_branches(struct compile_state *state,
	struct triple *cmp, struct triple *use, int jmp_op)
{
	struct triple_set *entry, *next;
	for(entry = use->use; entry; entry = next) {
		next = entry->next;
		if (entry->member->op == OP_COPY) {
			fixup_branches(state, cmp, entry->member, jmp_op);
		}
		else if (entry->member->op == OP_CBRANCH) {
			struct triple *branch;
			struct triple *left, *right;
			left = right = 0;
			left = RHS(cmp, 0);
			if (cmp->rhs > 1) {
				right = RHS(cmp, 1);
			}
			branch = entry->member;
			fixup_branch(state, branch, jmp_op,
				cmp->op, cmp->type, left, right);
		}
	}
}

static void bool_cmp(struct compile_state *state,
	struct triple *ins, int cmp_op, int jmp_op, int set_op)
{
	struct triple_set *entry, *next;
	struct triple *set, *convert;

	/* Put a barrier up before the cmp which preceeds the
	 * copy instruction.  If a set actually occurs this gives
	 * us a chance to move variables in registers out of the way.
	 */

	/* Modify the comparison operator */
	ins->op = cmp_op;
	ins->template_id = TEMPLATE_TEST32;
	if (cmp_op == OP_CMP) {
		ins->template_id = TEMPLATE_CMP32_REG;
		if (get_imm32(ins, &RHS(ins, 1))) {
			ins->template_id =  TEMPLATE_CMP32_IMM;
		}
	}
	/* Generate the instruction sequence that will transform the
	 * result of the comparison into a logical value.
	 */
	set = post_triple(state, ins, set_op, &uchar_type, ins, 0);
	use_triple(ins, set);
	set->template_id = TEMPLATE_SET;

	convert = set;
	if (!equiv_types(ins->type, set->type)) {
		convert = post_triple(state, set, OP_CONVERT, ins->type, set, 0);
		use_triple(set, convert);
		convert->template_id = TEMPLATE_COPY32_REG;
	}

	for(entry = ins->use; entry; entry = next) {
		next = entry->next;
		if (entry->member == set) {
			continue;
		}
		replace_rhs_use(state, ins, convert, entry->member);
	}
	fixup_branches(state, ins, convert, jmp_op);
}

struct reg_info arch_reg_lhs(struct compile_state *state, struct triple *ins, int index)
{
	struct ins_template *template;
	struct reg_info result;
	int zlhs;
	if (ins->op == OP_PIECE) {
		index = ins->u.cval;
		ins = MISC(ins, 0);
	}
	zlhs = ins->lhs;
	if (triple_is_def(state, ins)) {
		zlhs = 1;
	}
	if (index >= zlhs) {
		internal_error(state, ins, "index %d out of range for %s",
			index, tops(ins->op));
	}
	switch(ins->op) {
	case OP_ASM:
		template = &ins->u.ainfo->tmpl;
		break;
	default:
		if (ins->template_id > LAST_TEMPLATE) {
			internal_error(state, ins, "bad template number %d",
				ins->template_id);
		}
		template = &templates[ins->template_id];
		break;
	}
	result = template->lhs[index];
	result.regcm = arch_regcm_normalize(state, result.regcm);
	if (result.reg != REG_UNNEEDED) {
		result.regcm &= ~(REGCM_IMM32 | REGCM_IMM16 | REGCM_IMM8);
	}
	if (result.regcm == 0) {
		internal_error(state, ins, "lhs %d regcm == 0", index);
	}
	return result;
}

struct reg_info arch_reg_rhs(struct compile_state *state, struct triple *ins, int index)
{
	struct reg_info result;
	struct ins_template *template;
	if ((index > ins->rhs) ||
		(ins->op == OP_PIECE)) {
		internal_error(state, ins, "index %d out of range for %s\n",
			index, tops(ins->op));
	}
	switch(ins->op) {
	case OP_ASM:
		template = &ins->u.ainfo->tmpl;
		break;
	case OP_PHI:
		index = 0;
		/* Fall through */
	default:
		if (ins->template_id > LAST_TEMPLATE) {
			internal_error(state, ins, "bad template number %d",
				ins->template_id);
		}
		template = &templates[ins->template_id];
		break;
	}
	result = template->rhs[index];
	result.regcm = arch_regcm_normalize(state, result.regcm);
	if (result.regcm == 0) {
		internal_error(state, ins, "rhs %d regcm == 0", index);
	}
	return result;
}

static struct triple *mod_div(struct compile_state *state,
	struct triple *ins, int div_op, int index)
{
	struct triple *div, *piece1;

	/* Generate the appropriate division instruction */
	div = post_triple(state, ins, div_op, ins->type, 0, 0);
	RHS(div, 0) = RHS(ins, 0);
	RHS(div, 1) = RHS(ins, 1);
	piece1 = LHS(div, 1);
	div->template_id  = TEMPLATE_DIV32;
	use_triple(RHS(div, 0), div);
	use_triple(RHS(div, 1), div);
	use_triple(LHS(div, 0), div);
	use_triple(LHS(div, 1), div);

	/* Replate uses of ins with the appropriate piece of the div */
	propagate_use(state, ins, LHS(div, index));
	release_triple(state, ins);

	/* Return the address of the next instruction */
	return piece1->next;
}

static int noop_adecl(struct triple *adecl)
{
	struct triple_set *use;
	/* It's a noop if it doesn't specify stoorage */
	if (adecl->lhs == 0) {
		return 1;
	}
	/* Is the adecl used? If not it's a noop */
	for(use = adecl->use; use ; use = use->next) {
		if ((use->member->op != OP_PIECE) ||
			(MISC(use->member, 0) != adecl)) {
			return 0;
		}
	}
	return 1;
}

static struct triple *x86_deposit(struct compile_state *state, struct triple *ins)
{
	struct triple *mask, *nmask, *shift;
	struct triple *val, *val_mask, *val_shift;
	struct triple *targ, *targ_mask;
	struct triple *new;
	ulong_t the_mask, the_nmask;

	targ = RHS(ins, 0);
	val = RHS(ins, 1);

	/* Get constant for the mask value */
	the_mask = 1;
	the_mask <<= ins->u.bitfield.size;
	the_mask -= 1;
	the_mask <<= ins->u.bitfield.offset;
	mask = pre_triple(state, ins, OP_INTCONST, &uint_type, 0, 0);
	mask->u.cval = the_mask;

	/* Get the inverted mask value */
	the_nmask = ~the_mask;
	nmask = pre_triple(state, ins, OP_INTCONST, &uint_type, 0, 0);
	nmask->u.cval = the_nmask;

	/* Get constant for the shift value */
	shift = pre_triple(state, ins, OP_INTCONST, &uint_type, 0, 0);
	shift->u.cval = ins->u.bitfield.offset;

	/* Shift and mask the source value */
	val_shift = val;
	if (shift->u.cval != 0) {
		val_shift = pre_triple(state, ins, OP_SL, val->type, val, shift);
		use_triple(val, val_shift);
		use_triple(shift, val_shift);
	}
	val_mask = val_shift;
	if (is_signed(val->type)) {
		val_mask = pre_triple(state, ins, OP_AND, val->type, val_shift, mask);
		use_triple(val_shift, val_mask);
		use_triple(mask, val_mask);
	}

	/* Mask the target value */
	targ_mask = pre_triple(state, ins, OP_AND, targ->type, targ, nmask);
	use_triple(targ, targ_mask);
	use_triple(nmask, targ_mask);

	/* Now combined them together */
	new = pre_triple(state, ins, OP_OR, targ->type, targ_mask, val_mask);
	use_triple(targ_mask, new);
	use_triple(val_mask, new);

	/* Move all of the users over to the new expression */
	propagate_use(state, ins, new);

	/* Delete the original triple */
	release_triple(state, ins);

	/* Restart the transformation at mask */
	return mask;
}

static struct triple *x86_extract(struct compile_state *state, struct triple *ins)
{
	struct triple *mask, *shift;
	struct triple *val, *val_mask, *val_shift;
	ulong_t the_mask;

	val = RHS(ins, 0);

	/* Get constant for the mask value */
	the_mask = 1;
	the_mask <<= ins->u.bitfield.size;
	the_mask -= 1;
	mask = pre_triple(state, ins, OP_INTCONST, &int_type, 0, 0);
	mask->u.cval = the_mask;

	/* Get constant for the right shift value */
	shift = pre_triple(state, ins, OP_INTCONST, &int_type, 0, 0);
	shift->u.cval = ins->u.bitfield.offset;

	/* Shift arithmetic right, to correct the sign */
	val_shift = val;
	if (shift->u.cval != 0) {
		int op;
		if (ins->op == OP_SEXTRACT) {
			op = OP_SSR;
		} else {
			op = OP_USR;
		}
		val_shift = pre_triple(state, ins, op, val->type, val, shift);
		use_triple(val, val_shift);
		use_triple(shift, val_shift);
	}

	/* Finally mask the value */
	val_mask = pre_triple(state, ins, OP_AND, ins->type, val_shift, mask);
	use_triple(val_shift, val_mask);
	use_triple(mask,      val_mask);

	/* Move all of the users over to the new expression */
	propagate_use(state, ins, val_mask);

	/* Release the original instruction */
	release_triple(state, ins);

	return mask;

}

static struct triple *transform_to_arch_instruction(
	struct compile_state *state, struct triple *ins)
{
	/* Transform from generic 3 address instructions
	 * to archtecture specific instructions.
	 * And apply architecture specific constraints to instructions.
	 * Copies are inserted to preserve the register flexibility
	 * of 3 address instructions.
	 */
	struct triple *next, *value;
	size_t size;
	next = ins->next;
	switch(ins->op) {
	case OP_INTCONST:
		ins->template_id = TEMPLATE_INTCONST32;
		if (ins->u.cval < 256) {
			ins->template_id = TEMPLATE_INTCONST8;
		}
		break;
	case OP_ADDRCONST:
		ins->template_id = TEMPLATE_INTCONST32;
		break;
	case OP_UNKNOWNVAL:
		ins->template_id = TEMPLATE_UNKNOWNVAL;
		break;
	case OP_NOOP:
	case OP_SDECL:
	case OP_BLOBCONST:
	case OP_LABEL:
		ins->template_id = TEMPLATE_NOP;
		break;
	case OP_COPY:
	case OP_CONVERT:
		size = size_of(state, ins->type);
		value = RHS(ins, 0);
		if (is_imm8(value) && (size <= SIZEOF_I8)) {
			ins->template_id = TEMPLATE_COPY_IMM8;
		}
		else if (is_imm16(value) && (size <= SIZEOF_I16)) {
			ins->template_id = TEMPLATE_COPY_IMM16;
		}
		else if (is_imm32(value) && (size <= SIZEOF_I32)) {
			ins->template_id = TEMPLATE_COPY_IMM32;
		}
		else if (is_const(value)) {
			internal_error(state, ins, "bad constant passed to copy");
		}
		else if (size <= SIZEOF_I8) {
			ins->template_id = TEMPLATE_COPY8_REG;
		}
		else if (size <= SIZEOF_I16) {
			ins->template_id = TEMPLATE_COPY16_REG;
		}
		else if (size <= SIZEOF_I32) {
			ins->template_id = TEMPLATE_COPY32_REG;
		}
		else {
			internal_error(state, ins, "bad type passed to copy");
		}
		break;
	case OP_PHI:
		size = size_of(state, ins->type);
		if (size <= SIZEOF_I8) {
			ins->template_id = TEMPLATE_PHI8;
		}
		else if (size <= SIZEOF_I16) {
			ins->template_id = TEMPLATE_PHI16;
		}
		else if (size <= SIZEOF_I32) {
			ins->template_id = TEMPLATE_PHI32;
		}
		else {
			internal_error(state, ins, "bad type passed to phi");
		}
		break;
	case OP_ADECL:
		/* Adecls should always be treated as dead code and
		 * removed.  If we are not optimizing they may linger.
		 */
		if (!noop_adecl(ins)) {
			internal_error(state, ins, "adecl remains?");
		}
		ins->template_id = TEMPLATE_NOP;
		next = after_lhs(state, ins);
		break;
	case OP_STORE:
		switch(ins->type->type & TYPE_MASK) {
		case TYPE_CHAR:    case TYPE_UCHAR:
			ins->template_id = TEMPLATE_STORE8;
			break;
		case TYPE_SHORT:   case TYPE_USHORT:
			ins->template_id = TEMPLATE_STORE16;
			break;
		case TYPE_INT:     case TYPE_UINT:
		case TYPE_LONG:    case TYPE_ULONG:
		case TYPE_POINTER:
			ins->template_id = TEMPLATE_STORE32;
			break;
		default:
			internal_error(state, ins, "unknown type in store");
			break;
		}
		break;
	case OP_LOAD:
		switch(ins->type->type & TYPE_MASK) {
		case TYPE_CHAR:   case TYPE_UCHAR:
		case TYPE_SHORT:  case TYPE_USHORT:
		case TYPE_INT:    case TYPE_UINT:
		case TYPE_LONG:   case TYPE_ULONG:
		case TYPE_POINTER:
			break;
		default:
			internal_error(state, ins, "unknown type in load");
			break;
		}
		ins->template_id = TEMPLATE_LOAD32;
		break;
	case OP_ADD:
	case OP_SUB:
	case OP_AND:
	case OP_XOR:
	case OP_OR:
	case OP_SMUL:
		ins->template_id = TEMPLATE_BINARY32_REG;
		if (get_imm32(ins, &RHS(ins, 1))) {
			ins->template_id = TEMPLATE_BINARY32_IMM;
		}
		break;
	case OP_SDIVT:
	case OP_UDIVT:
		ins->template_id = TEMPLATE_DIV32;
		next = after_lhs(state, ins);
		break;
	case OP_UMUL:
		ins->template_id = TEMPLATE_UMUL32;
		break;
	case OP_UDIV:
		next = mod_div(state, ins, OP_UDIVT, 0);
		break;
	case OP_SDIV:
		next = mod_div(state, ins, OP_SDIVT, 0);
		break;
	case OP_UMOD:
		next = mod_div(state, ins, OP_UDIVT, 1);
		break;
	case OP_SMOD:
		next = mod_div(state, ins, OP_SDIVT, 1);
		break;
	case OP_SL:
	case OP_SSR:
	case OP_USR:
		ins->template_id = TEMPLATE_SL32_CL;
		if (get_imm8(ins, &RHS(ins, 1))) {
			ins->template_id = TEMPLATE_SL32_IMM;
		} else if (size_of(state, RHS(ins, 1)->type) > SIZEOF_CHAR) {
			typed_pre_copy(state, &uchar_type, ins, 1);
		}
		break;
	case OP_INVERT:
	case OP_NEG:
		ins->template_id = TEMPLATE_UNARY32;
		break;
	case OP_EQ:
		bool_cmp(state, ins, OP_CMP, OP_JMP_EQ, OP_SET_EQ);
		break;
	case OP_NOTEQ:
		bool_cmp(state, ins, OP_CMP, OP_JMP_NOTEQ, OP_SET_NOTEQ);
		break;
	case OP_SLESS:
		bool_cmp(state, ins, OP_CMP, OP_JMP_SLESS, OP_SET_SLESS);
		break;
	case OP_ULESS:
		bool_cmp(state, ins, OP_CMP, OP_JMP_ULESS, OP_SET_ULESS);
		break;
	case OP_SMORE:
		bool_cmp(state, ins, OP_CMP, OP_JMP_SMORE, OP_SET_SMORE);
		break;
	case OP_UMORE:
		bool_cmp(state, ins, OP_CMP, OP_JMP_UMORE, OP_SET_UMORE);
		break;
	case OP_SLESSEQ:
		bool_cmp(state, ins, OP_CMP, OP_JMP_SLESSEQ, OP_SET_SLESSEQ);
		break;
	case OP_ULESSEQ:
		bool_cmp(state, ins, OP_CMP, OP_JMP_ULESSEQ, OP_SET_ULESSEQ);
		break;
	case OP_SMOREEQ:
		bool_cmp(state, ins, OP_CMP, OP_JMP_SMOREEQ, OP_SET_SMOREEQ);
		break;
	case OP_UMOREEQ:
		bool_cmp(state, ins, OP_CMP, OP_JMP_UMOREEQ, OP_SET_UMOREEQ);
		break;
	case OP_LTRUE:
		bool_cmp(state, ins, OP_TEST, OP_JMP_NOTEQ, OP_SET_NOTEQ);
		break;
	case OP_LFALSE:
		bool_cmp(state, ins, OP_TEST, OP_JMP_EQ, OP_SET_EQ);
		break;
	case OP_BRANCH:
		ins->op = OP_JMP;
		ins->template_id = TEMPLATE_NOP;
		break;
	case OP_CBRANCH:
		fixup_branch(state, ins, OP_JMP_NOTEQ, OP_TEST,
			RHS(ins, 0)->type, RHS(ins, 0), 0);
		break;
	case OP_CALL:
		ins->template_id = TEMPLATE_NOP;
		break;
	case OP_RET:
		ins->template_id = TEMPLATE_RET;
		break;
	case OP_INB:
	case OP_INW:
	case OP_INL:
		switch(ins->op) {
		case OP_INB: ins->template_id = TEMPLATE_INB_DX; break;
		case OP_INW: ins->template_id = TEMPLATE_INW_DX; break;
		case OP_INL: ins->template_id = TEMPLATE_INL_DX; break;
		}
		if (get_imm8(ins, &RHS(ins, 0))) {
			ins->template_id += 1;
		}
		break;
	case OP_OUTB:
	case OP_OUTW:
	case OP_OUTL:
		switch(ins->op) {
		case OP_OUTB: ins->template_id = TEMPLATE_OUTB_DX; break;
		case OP_OUTW: ins->template_id = TEMPLATE_OUTW_DX; break;
		case OP_OUTL: ins->template_id = TEMPLATE_OUTL_DX; break;
		}
		if (get_imm8(ins, &RHS(ins, 1))) {
			ins->template_id += 1;
		}
		break;
	case OP_BSF:
	case OP_BSR:
		ins->template_id = TEMPLATE_BSF;
		break;
	case OP_RDMSR:
		ins->template_id = TEMPLATE_RDMSR;
		next = after_lhs(state, ins);
		break;
	case OP_WRMSR:
		ins->template_id = TEMPLATE_WRMSR;
		break;
	case OP_HLT:
		ins->template_id = TEMPLATE_NOP;
		break;
	case OP_ASM:
		ins->template_id = TEMPLATE_NOP;
		next = after_lhs(state, ins);
		break;
		/* Already transformed instructions */
	case OP_TEST:
		ins->template_id = TEMPLATE_TEST32;
		break;
	case OP_CMP:
		ins->template_id = TEMPLATE_CMP32_REG;
		if (get_imm32(ins, &RHS(ins, 1))) {
			ins->template_id = TEMPLATE_CMP32_IMM;
		}
		break;
	case OP_JMP:
		ins->template_id = TEMPLATE_NOP;
		break;
	case OP_JMP_EQ:      case OP_JMP_NOTEQ:
	case OP_JMP_SLESS:   case OP_JMP_ULESS:
	case OP_JMP_SMORE:   case OP_JMP_UMORE:
	case OP_JMP_SLESSEQ: case OP_JMP_ULESSEQ:
	case OP_JMP_SMOREEQ: case OP_JMP_UMOREEQ:
		ins->template_id = TEMPLATE_JMP;
		break;
	case OP_SET_EQ:      case OP_SET_NOTEQ:
	case OP_SET_SLESS:   case OP_SET_ULESS:
	case OP_SET_SMORE:   case OP_SET_UMORE:
	case OP_SET_SLESSEQ: case OP_SET_ULESSEQ:
	case OP_SET_SMOREEQ: case OP_SET_UMOREEQ:
		ins->template_id = TEMPLATE_SET;
		break;
	case OP_DEPOSIT:
		next = x86_deposit(state, ins);
		break;
	case OP_SEXTRACT:
	case OP_UEXTRACT:
		next = x86_extract(state, ins);
		break;
		/* Unhandled instructions */
	case OP_PIECE:
	default:
		internal_error(state, ins, "unhandled ins: %d %s",
			ins->op, tops(ins->op));
		break;
	}
	return next;
}

static long next_label(struct compile_state *state)
{
	static long label_counter = 1000;
	return ++label_counter;
}
static void generate_local_labels(struct compile_state *state)
{
	struct triple *first, *label;
	first = state->first;
	label = first;
	do {
		if ((label->op == OP_LABEL) ||
			(label->op == OP_SDECL)) {
			if (label->use) {
				label->u.cval = next_label(state);
			} else {
				label->u.cval = 0;
			}

		}
		label = label->next;
	} while(label != first);
}

static int check_reg(struct compile_state *state,
	struct triple *triple, int classes)
{
	unsigned mask;
	int reg;
	reg = ID_REG(triple->id);
	if (reg == REG_UNSET) {
		internal_error(state, triple, "register not set");
	}
	mask = arch_reg_regcm(state, reg);
	if (!(classes & mask)) {
		internal_error(state, triple, "reg %d in wrong class",
			reg);
	}
	return reg;
}


#if REG_XMM7 != 44
#error "Registers have renumberd fix arch_reg_str"
#endif
static const char *arch_regs[] = {
	"%unset",
	"%unneeded",
	"%eflags",
	"%al", "%bl", "%cl", "%dl", "%ah", "%bh", "%ch", "%dh",
	"%ax", "%bx", "%cx", "%dx", "%si", "%di", "%bp", "%sp",
	"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi", "%ebp", "%esp",
	"%edx:%eax",
	"%dx:%ax",
	"%mm0", "%mm1", "%mm2", "%mm3", "%mm4", "%mm5", "%mm6", "%mm7",
	"%xmm0", "%xmm1", "%xmm2", "%xmm3",
	"%xmm4", "%xmm5", "%xmm6", "%xmm7",
};
static const char *arch_reg_str(int reg)
{
	if (!((reg >= REG_EFLAGS) && (reg <= REG_XMM7))) {
		reg = 0;
	}
	return arch_regs[reg];
}

static const char *reg(struct compile_state *state, struct triple *triple,
	int classes)
{
	int reg;
	reg = check_reg(state, triple, classes);
	return arch_reg_str(reg);
}

static int arch_reg_size(int reg)
{
	int size;
	size = 0;
	if (reg == REG_EFLAGS) {
		size = 32;
	}
	else if ((reg >= REG_AL) && (reg <= REG_DH)) {
		size = 8;
	}
	else if ((reg >= REG_AX) && (reg <= REG_SP)) {
		size = 16;
	}
	else if ((reg >= REG_EAX) && (reg <= REG_ESP)) {
		size = 32;
	}
	else if (reg == REG_EDXEAX) {
		size = 64;
	}
	else if (reg == REG_DXAX) {
		size = 32;
	}
	else if ((reg >= REG_MMX0) && (reg <= REG_MMX7)) {
		size = 64;
	}
	else if ((reg >= REG_XMM0) && (reg <= REG_XMM7)) {
		size = 128;
	}
	return size;
}

static int reg_size(struct compile_state *state, struct triple *ins)
{
	int reg;
	reg = ID_REG(ins->id);
	if (reg == REG_UNSET) {
		internal_error(state, ins, "register not set");
	}
	return arch_reg_size(reg);
}



const char *type_suffix(struct compile_state *state, struct type *type)
{
	const char *suffix;
	switch(size_of(state, type)) {
	case SIZEOF_I8:  suffix = "b"; break;
	case SIZEOF_I16: suffix = "w"; break;
	case SIZEOF_I32: suffix = "l"; break;
	default:
		internal_error(state, 0, "unknown suffix");
		suffix = 0;
		break;
	}
	return suffix;
}

static void print_const_val(
	struct compile_state *state, struct triple *ins, FILE *fp)
{
	switch(ins->op) {
	case OP_INTCONST:
		fprintf(fp, " $%ld ",
			(long)(ins->u.cval));
		break;
	case OP_ADDRCONST:
		if ((MISC(ins, 0)->op != OP_SDECL) &&
			(MISC(ins, 0)->op != OP_LABEL))
		{
			internal_error(state, ins, "bad base for addrconst");
		}
		if (MISC(ins, 0)->u.cval <= 0) {
			internal_error(state, ins, "unlabeled constant");
		}
		fprintf(fp, " $L%s%lu+%lu ",
			state->compiler->label_prefix,
			(unsigned long)(MISC(ins, 0)->u.cval),
			(unsigned long)(ins->u.cval));
		break;
	default:
		internal_error(state, ins, "unknown constant type");
		break;
	}
}

static void print_const(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	switch(ins->op) {
	case OP_INTCONST:
		switch(ins->type->type & TYPE_MASK) {
		case TYPE_CHAR:
		case TYPE_UCHAR:
			fprintf(fp, ".byte 0x%02lx\n",
				(unsigned long)(ins->u.cval));
			break;
		case TYPE_SHORT:
		case TYPE_USHORT:
			fprintf(fp, ".short 0x%04lx\n",
				(unsigned long)(ins->u.cval));
			break;
		case TYPE_INT:
		case TYPE_UINT:
		case TYPE_LONG:
		case TYPE_ULONG:
		case TYPE_POINTER:
			fprintf(fp, ".int %lu\n",
				(unsigned long)(ins->u.cval));
			break;
		default:
			fprintf(state->errout, "type: ");
			name_of(state->errout, ins->type);
			fprintf(state->errout, "\n");
			internal_error(state, ins, "Unknown constant type. Val: %lu",
				(unsigned long)(ins->u.cval));
		}

		break;
	case OP_ADDRCONST:
		if ((MISC(ins, 0)->op != OP_SDECL) &&
			(MISC(ins, 0)->op != OP_LABEL)) {
			internal_error(state, ins, "bad base for addrconst");
		}
		if (MISC(ins, 0)->u.cval <= 0) {
			internal_error(state, ins, "unlabeled constant");
		}
		fprintf(fp, ".int L%s%lu+%lu\n",
			state->compiler->label_prefix,
			(unsigned long)(MISC(ins, 0)->u.cval),
			(unsigned long)(ins->u.cval));
		break;
	case OP_BLOBCONST:
	{
		unsigned char *blob;
		size_t size, i;
		size = size_of_in_bytes(state, ins->type);
		blob = ins->u.blob;
		for(i = 0; i < size; i++) {
			fprintf(fp, ".byte 0x%02x\n",
				blob[i]);
		}
		break;
	}
	default:
		internal_error(state, ins, "Unknown constant type");
		break;
	}
}

#define TEXT_SECTION ".rom.text"
#define DATA_SECTION ".rom.data"

static long get_const_pool_ref(
	struct compile_state *state, struct triple *ins, size_t size, FILE *fp)
{
	size_t fill_bytes;
	long ref;
	ref = next_label(state);
	fprintf(fp, ".section \"" DATA_SECTION "\"\n");
	fprintf(fp, ".balign %ld\n", (long int)align_of_in_bytes(state, ins->type));
	fprintf(fp, "L%s%lu:\n", state->compiler->label_prefix, ref);
	print_const(state, ins, fp);
	fill_bytes = bits_to_bytes(size - size_of(state, ins->type));
	if (fill_bytes) {
		fprintf(fp, ".fill %ld, 1, 0\n", (long int)fill_bytes);
	}
	fprintf(fp, ".section \"" TEXT_SECTION "\"\n");
	return ref;
}

static long get_mask_pool_ref(
	struct compile_state *state, struct triple *ins, unsigned long mask, FILE *fp)
{
	long ref;
	if (mask == 0xff) {
		ref = 1;
	}
	else if (mask == 0xffff) {
		ref = 2;
	}
	else {
		internal_error(state, ins, "unhandled mask value");
	}
	return ref;
}

static void print_binary_op(struct compile_state *state,
	const char *op, struct triple *ins, FILE *fp)
{
	unsigned mask;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO;
	if (ID_REG(RHS(ins, 0)->id) != ID_REG(ins->id)) {
		internal_error(state, ins, "invalid register assignment");
	}
	if (is_const(RHS(ins, 1))) {
		fprintf(fp, "\t%s ", op);
		print_const_val(state, RHS(ins, 1), fp);
		fprintf(fp, ", %s\n",
			reg(state, RHS(ins, 0), mask));
	}
	else {
		unsigned lmask, rmask;
		int lreg, rreg;
		lreg = check_reg(state, RHS(ins, 0), mask);
		rreg = check_reg(state, RHS(ins, 1), mask);
		lmask = arch_reg_regcm(state, lreg);
		rmask = arch_reg_regcm(state, rreg);
		mask = lmask & rmask;
		fprintf(fp, "\t%s %s, %s\n",
			op,
			reg(state, RHS(ins, 1), mask),
			reg(state, RHS(ins, 0), mask));
	}
}
static void print_unary_op(struct compile_state *state,
	const char *op, struct triple *ins, FILE *fp)
{
	unsigned mask;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO;
	fprintf(fp, "\t%s %s\n",
		op,
		reg(state, RHS(ins, 0), mask));
}

static void print_op_shift(struct compile_state *state,
	const char *op, struct triple *ins, FILE *fp)
{
	unsigned mask;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO;
	if (ID_REG(RHS(ins, 0)->id) != ID_REG(ins->id)) {
		internal_error(state, ins, "invalid register assignment");
	}
	if (is_const(RHS(ins, 1))) {
		fprintf(fp, "\t%s ", op);
		print_const_val(state, RHS(ins, 1), fp);
		fprintf(fp, ", %s\n",
			reg(state, RHS(ins, 0), mask));
	}
	else {
		fprintf(fp, "\t%s %s, %s\n",
			op,
			reg(state, RHS(ins, 1), REGCM_GPR8_LO),
			reg(state, RHS(ins, 0), mask));
	}
}

static void print_op_in(struct compile_state *state, struct triple *ins, FILE *fp)
{
	const char *op;
	int mask;
	int dreg;
	switch(ins->op) {
	case OP_INB: op = "inb", mask = REGCM_GPR8_LO; break;
	case OP_INW: op = "inw", mask = REGCM_GPR16; break;
	case OP_INL: op = "inl", mask = REGCM_GPR32; break;
	default:
		internal_error(state, ins, "not an in operation");
		op = 0;
		break;
	}
	dreg = check_reg(state, ins, mask);
	if (!reg_is_reg(state, dreg, REG_EAX)) {
		internal_error(state, ins, "dst != %%eax");
	}
	if (is_const(RHS(ins, 0))) {
		fprintf(fp, "\t%s ", op);
		print_const_val(state, RHS(ins, 0), fp);
		fprintf(fp, ", %s\n",
			reg(state, ins, mask));
	}
	else {
		int addr_reg;
		addr_reg = check_reg(state, RHS(ins, 0), REGCM_GPR16);
		if (!reg_is_reg(state, addr_reg, REG_DX)) {
			internal_error(state, ins, "src != %%dx");
		}
		fprintf(fp, "\t%s %s, %s\n",
			op,
			reg(state, RHS(ins, 0), REGCM_GPR16),
			reg(state, ins, mask));
	}
}

static void print_op_out(struct compile_state *state, struct triple *ins, FILE *fp)
{
	const char *op;
	int mask;
	int lreg;
	switch(ins->op) {
	case OP_OUTB: op = "outb", mask = REGCM_GPR8_LO; break;
	case OP_OUTW: op = "outw", mask = REGCM_GPR16; break;
	case OP_OUTL: op = "outl", mask = REGCM_GPR32; break;
	default:
		internal_error(state, ins, "not an out operation");
		op = 0;
		break;
	}
	lreg = check_reg(state, RHS(ins, 0), mask);
	if (!reg_is_reg(state, lreg, REG_EAX)) {
		internal_error(state, ins, "src != %%eax");
	}
	if (is_const(RHS(ins, 1))) {
		fprintf(fp, "\t%s %s,",
			op, reg(state, RHS(ins, 0), mask));
		print_const_val(state, RHS(ins, 1), fp);
		fprintf(fp, "\n");
	}
	else {
		int addr_reg;
		addr_reg = check_reg(state, RHS(ins, 1), REGCM_GPR16);
		if (!reg_is_reg(state, addr_reg, REG_DX)) {
			internal_error(state, ins, "dst != %%dx");
		}
		fprintf(fp, "\t%s %s, %s\n",
			op,
			reg(state, RHS(ins, 0), mask),
			reg(state, RHS(ins, 1), REGCM_GPR16));
	}
}

static void print_op_move(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	/* op_move is complex because there are many types
	 * of registers we can move between.
	 * Because OP_COPY will be introduced in arbitrary locations
	 * OP_COPY must not affect flags.
	 * OP_CONVERT can change the flags and it is the only operation
	 * where it is expected the types in the registers can change.
	 */
	int omit_copy = 1; /* Is it o.k. to omit a noop copy? */
	struct triple *dst, *src;
	if (state->arch->features & X86_NOOP_COPY) {
		omit_copy = 0;
	}
	if ((ins->op == OP_COPY) || (ins->op == OP_CONVERT)) {
		src = RHS(ins, 0);
		dst = ins;
	}
	else {
		internal_error(state, ins, "unknown move operation");
		src = dst = 0;
	}
	if (reg_size(state, dst) < size_of(state, dst->type)) {
		internal_error(state, ins, "Invalid destination register");
	}
	if (!equiv_types(src->type, dst->type) && (dst->op == OP_COPY)) {
		fprintf(state->errout, "src type: ");
		name_of(state->errout, src->type);
		fprintf(state->errout, "\n");
		fprintf(state->errout, "dst type: ");
		name_of(state->errout, dst->type);
		fprintf(state->errout, "\n");
		internal_error(state, ins, "Type mismatch for OP_COPY");
	}

	if (!is_const(src)) {
		int src_reg, dst_reg;
		int src_regcm, dst_regcm;
		src_reg   = ID_REG(src->id);
		dst_reg   = ID_REG(dst->id);
		src_regcm = arch_reg_regcm(state, src_reg);
		dst_regcm = arch_reg_regcm(state, dst_reg);
		/* If the class is the same just move the register */
		if (src_regcm & dst_regcm &
			(REGCM_GPR8_LO | REGCM_GPR16 | REGCM_GPR32)) {
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmov %s, %s\n",
					reg(state, src, src_regcm),
					reg(state, dst, dst_regcm));
			}
		}
		/* Move 32bit to 16bit */
		else if ((src_regcm & REGCM_GPR32) &&
			(dst_regcm & REGCM_GPR16)) {
			src_reg = (src_reg - REGC_GPR32_FIRST) + REGC_GPR16_FIRST;
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmovw %s, %s\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
		/* Move from 32bit gprs to 16bit gprs */
		else if ((src_regcm & REGCM_GPR32) &&
			(dst_regcm & REGCM_GPR16)) {
			dst_reg = (dst_reg - REGC_GPR16_FIRST) + REGC_GPR32_FIRST;
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmov %s, %s\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
		/* Move 32bit to 8bit */
		else if ((src_regcm & REGCM_GPR32_8) &&
			(dst_regcm & REGCM_GPR8_LO))
		{
			src_reg = (src_reg - REGC_GPR32_8_FIRST) + REGC_GPR8_FIRST;
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmovb %s, %s\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
		/* Move 16bit to 8bit */
		else if ((src_regcm & REGCM_GPR16_8) &&
			(dst_regcm & REGCM_GPR8_LO))
		{
			src_reg = (src_reg - REGC_GPR16_8_FIRST) + REGC_GPR8_FIRST;
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmovb %s, %s\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
		/* Move 8/16bit to 16/32bit */
		else if ((src_regcm & (REGCM_GPR8_LO | REGCM_GPR16)) &&
			(dst_regcm & (REGCM_GPR16 | REGCM_GPR32))) {
			const char *op;
			op = is_signed(src->type)? "movsx": "movzx";
			fprintf(fp, "\t%s %s, %s\n",
				op,
				reg(state, src, src_regcm),
				reg(state, dst, dst_regcm));
		}
		/* Move between sse registers */
		else if ((src_regcm & dst_regcm & REGCM_XMM)) {
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmovdqa %s, %s\n",
					reg(state, src, src_regcm),
					reg(state, dst, dst_regcm));
			}
		}
		/* Move between mmx registers */
		else if ((src_regcm & dst_regcm & REGCM_MMX)) {
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmovq %s, %s\n",
					reg(state, src, src_regcm),
					reg(state, dst, dst_regcm));
			}
		}
		/* Move from sse to mmx registers */
		else if ((src_regcm & REGCM_XMM) && (dst_regcm & REGCM_MMX)) {
			fprintf(fp, "\tmovdq2q %s, %s\n",
				reg(state, src, src_regcm),
				reg(state, dst, dst_regcm));
		}
		/* Move from mmx to sse registers */
		else if ((src_regcm & REGCM_MMX) && (dst_regcm & REGCM_XMM)) {
			fprintf(fp, "\tmovq2dq %s, %s\n",
				reg(state, src, src_regcm),
				reg(state, dst, dst_regcm));
		}
		/* Move between 32bit gprs & mmx/sse registers */
		else if ((src_regcm & (REGCM_GPR32 | REGCM_MMX | REGCM_XMM)) &&
			(dst_regcm & (REGCM_GPR32 | REGCM_MMX | REGCM_XMM))) {
			fprintf(fp, "\tmovd %s, %s\n",
				reg(state, src, src_regcm),
				reg(state, dst, dst_regcm));
		}
		/* Move from 16bit gprs &  mmx/sse registers */
		else if ((src_regcm & REGCM_GPR16) &&
			(dst_regcm & (REGCM_MMX | REGCM_XMM))) {
			const char *op;
			int mid_reg;
			op = is_signed(src->type)? "movsx":"movzx";
			mid_reg = (src_reg - REGC_GPR16_FIRST) + REGC_GPR32_FIRST;
			fprintf(fp, "\t%s %s, %s\n\tmovd %s, %s\n",
				op,
				arch_reg_str(src_reg),
				arch_reg_str(mid_reg),
				arch_reg_str(mid_reg),
				arch_reg_str(dst_reg));
		}
		/* Move from mmx/sse registers to 16bit gprs */
		else if ((src_regcm & (REGCM_MMX | REGCM_XMM)) &&
			(dst_regcm & REGCM_GPR16)) {
			dst_reg = (dst_reg - REGC_GPR16_FIRST) + REGC_GPR32_FIRST;
			fprintf(fp, "\tmovd %s, %s\n",
				arch_reg_str(src_reg),
				arch_reg_str(dst_reg));
		}
		/* Move from gpr to 64bit dividend */
		else if ((src_regcm & (REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO))  &&
			(dst_regcm & REGCM_DIVIDEND64)) {
			const char *extend;
			extend = is_signed(src->type)? "cltd":"movl $0, %edx";
			fprintf(fp, "\tmov %s, %%eax\n\t%s\n",
				arch_reg_str(src_reg),
				extend);
		}
		/* Move from 64bit gpr to gpr */
		else if ((src_regcm & REGCM_DIVIDEND64) &&
			(dst_regcm & (REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO))) {
			if (dst_regcm & REGCM_GPR32) {
				src_reg = REG_EAX;
			}
			else if (dst_regcm & REGCM_GPR16) {
				src_reg = REG_AX;
			}
			else if (dst_regcm & REGCM_GPR8_LO) {
				src_reg = REG_AL;
			}
			fprintf(fp, "\tmov %s, %s\n",
				arch_reg_str(src_reg),
				arch_reg_str(dst_reg));
		}
		/* Move from mmx/sse registers to 64bit gpr */
		else if ((src_regcm & (REGCM_MMX | REGCM_XMM)) &&
			(dst_regcm & REGCM_DIVIDEND64)) {
			const char *extend;
			extend = is_signed(src->type)? "cltd": "movl $0, %edx";
			fprintf(fp, "\tmovd %s, %%eax\n\t%s\n",
				arch_reg_str(src_reg),
				extend);
		}
		/* Move from 64bit gpr to mmx/sse register */
		else if ((src_regcm & REGCM_DIVIDEND64) &&
			(dst_regcm & (REGCM_XMM | REGCM_MMX))) {
			fprintf(fp, "\tmovd %%eax, %s\n",
				arch_reg_str(dst_reg));
		}
#if X86_4_8BIT_GPRS
		/* Move from 8bit gprs to  mmx/sse registers */
		else if ((src_regcm & REGCM_GPR8_LO) && (src_reg <= REG_DL) &&
			(dst_regcm & (REGCM_MMX | REGCM_XMM))) {
			const char *op;
			int mid_reg;
			op = is_signed(src->type)? "movsx":"movzx";
			mid_reg = (src_reg - REGC_GPR8_FIRST) + REGC_GPR32_FIRST;
			fprintf(fp, "\t%s %s, %s\n\tmovd %s, %s\n",
				op,
				reg(state, src, src_regcm),
				arch_reg_str(mid_reg),
				arch_reg_str(mid_reg),
				reg(state, dst, dst_regcm));
		}
		/* Move from mmx/sse registers and 8bit gprs */
		else if ((src_regcm & (REGCM_MMX | REGCM_XMM)) &&
			(dst_regcm & REGCM_GPR8_LO) && (dst_reg <= REG_DL)) {
			int mid_reg;
			mid_reg = (dst_reg - REGC_GPR8_FIRST) + REGC_GPR32_FIRST;
			fprintf(fp, "\tmovd %s, %s\n",
				reg(state, src, src_regcm),
				arch_reg_str(mid_reg));
		}
		/* Move from 32bit gprs to 8bit gprs */
		else if ((src_regcm & REGCM_GPR32) &&
			(dst_regcm & REGCM_GPR8_LO)) {
			dst_reg = (dst_reg - REGC_GPR8_FIRST) + REGC_GPR32_FIRST;
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmov %s, %s\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
		/* Move from 16bit gprs to 8bit gprs */
		else if ((src_regcm & REGCM_GPR16) &&
			(dst_regcm & REGCM_GPR8_LO)) {
			dst_reg = (dst_reg - REGC_GPR8_FIRST) + REGC_GPR16_FIRST;
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmov %s, %s\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
#endif /* X86_4_8BIT_GPRS */
		/* Move from %eax:%edx to %eax:%edx */
		else if ((src_regcm & REGCM_DIVIDEND64) &&
			(dst_regcm & REGCM_DIVIDEND64) &&
			(src_reg == dst_reg)) {
			if (!omit_copy) {
				fprintf(fp, "\t/*mov %s, %s*/\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
		else {
			if ((src_regcm & ~REGCM_FLAGS) == 0) {
				internal_error(state, ins, "attempt to copy from %%eflags!");
			}
			internal_error(state, ins, "unknown copy type");
		}
	}
	else {
		size_t dst_size;
		int dst_reg;
		int dst_regcm;
		dst_size = size_of(state, dst->type);
		dst_reg = ID_REG(dst->id);
		dst_regcm = arch_reg_regcm(state, dst_reg);
		if (dst_regcm & (REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO)) {
			fprintf(fp, "\tmov ");
			print_const_val(state, src, fp);
			fprintf(fp, ", %s\n",
				reg(state, dst, REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO));
		}
		else if (dst_regcm & REGCM_DIVIDEND64) {
			if (dst_size > SIZEOF_I32) {
				internal_error(state, ins, "%dbit constant...", dst_size);
			}
			fprintf(fp, "\tmov $0, %%edx\n");
			fprintf(fp, "\tmov ");
			print_const_val(state, src, fp);
			fprintf(fp, ", %%eax\n");
		}
		else if (dst_regcm & REGCM_DIVIDEND32) {
			if (dst_size > SIZEOF_I16) {
				internal_error(state, ins, "%dbit constant...", dst_size);
			}
			fprintf(fp, "\tmov $0, %%dx\n");
			fprintf(fp, "\tmov ");
			print_const_val(state, src, fp);
			fprintf(fp, ", %%ax");
		}
		else if (dst_regcm & (REGCM_XMM | REGCM_MMX)) {
			long ref;
			if (dst_size > SIZEOF_I32) {
				internal_error(state, ins, "%d bit constant...", dst_size);
			}
			ref = get_const_pool_ref(state, src, SIZEOF_I32, fp);
			fprintf(fp, "\tmovd L%s%lu, %s\n",
				state->compiler->label_prefix, ref,
				reg(state, dst, (REGCM_XMM | REGCM_MMX)));
		}
		else {
			internal_error(state, ins, "unknown copy immediate type");
		}
	}
	/* Leave now if this is not a type conversion */
	if (ins->op != OP_CONVERT) {
		return;
	}
	/* Now make certain I have not logically overflowed the destination */
	if ((size_of(state, src->type) > size_of(state, dst->type)) &&
		(size_of(state, dst->type) < reg_size(state, dst)))
	{
		unsigned long mask;
		int dst_reg;
		int dst_regcm;
		if (size_of(state, dst->type) >= 32) {
			fprintf(state->errout, "dst type: ");
			name_of(state->errout, dst->type);
			fprintf(state->errout, "\n");
			internal_error(state, dst, "unhandled dst type size");
		}
		mask = 1;
		mask <<= size_of(state, dst->type);
		mask -= 1;

		dst_reg = ID_REG(dst->id);
		dst_regcm = arch_reg_regcm(state, dst_reg);

		if (dst_regcm & (REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO)) {
			fprintf(fp, "\tand $0x%lx, %s\n",
				mask, reg(state, dst, REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO));
		}
		else if (dst_regcm & REGCM_MMX) {
			long ref;
			ref = get_mask_pool_ref(state, dst, mask, fp);
			fprintf(fp, "\tpand L%s%lu, %s\n",
				state->compiler->label_prefix, ref,
				reg(state, dst, REGCM_MMX));
		}
		else if (dst_regcm & REGCM_XMM) {
			long ref;
			ref = get_mask_pool_ref(state, dst, mask, fp);
			fprintf(fp, "\tpand L%s%lu, %s\n",
				state->compiler->label_prefix, ref,
				reg(state, dst, REGCM_XMM));
		}
		else {
			fprintf(state->errout, "dst type: ");
			name_of(state->errout, dst->type);
			fprintf(state->errout, "\n");
			fprintf(state->errout, "dst: %s\n", reg(state, dst, REGCM_ALL));
			internal_error(state, dst, "failed to trunc value: mask %lx", mask);
		}
	}
	/* Make certain I am properly sign extended */
	if ((size_of(state, src->type) < size_of(state, dst->type)) &&
		(is_signed(src->type)))
	{
		int reg_bits, shift_bits;
		int dst_reg;
		int dst_regcm;

		reg_bits = reg_size(state, dst);
		if (reg_bits > 32) {
			reg_bits = 32;
		}
		shift_bits = reg_bits - size_of(state, src->type);
		dst_reg = ID_REG(dst->id);
		dst_regcm = arch_reg_regcm(state, dst_reg);

		if (shift_bits < 0) {
			internal_error(state, dst, "negative shift?");
		}

		if (dst_regcm & (REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO)) {
			fprintf(fp, "\tshl $%d, %s\n",
				shift_bits,
				reg(state, dst, REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO));
			fprintf(fp, "\tsar $%d, %s\n",
				shift_bits,
				reg(state, dst, REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO));
		}
		else if (dst_regcm & (REGCM_MMX | REGCM_XMM)) {
			fprintf(fp, "\tpslld $%d, %s\n",
				shift_bits,
				reg(state, dst, REGCM_MMX | REGCM_XMM));
			fprintf(fp, "\tpsrad $%d, %s\n",
				shift_bits,
				reg(state, dst, REGCM_MMX | REGCM_XMM));
		}
		else {
			fprintf(state->errout, "dst type: ");
			name_of(state->errout, dst->type);
			fprintf(state->errout, "\n");
			fprintf(state->errout, "dst: %s\n", reg(state, dst, REGCM_ALL));
			internal_error(state, dst, "failed to signed extend value");
		}
	}
}

static void print_op_load(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	struct triple *dst, *src;
	const char *op;
	dst = ins;
	src = RHS(ins, 0);
	if (is_const(src) || is_const(dst)) {
		internal_error(state, ins, "unknown load operation");
	}
	switch(ins->type->type & TYPE_MASK) {
	case TYPE_CHAR:   op = "movsbl"; break;
	case TYPE_UCHAR:  op = "movzbl"; break;
	case TYPE_SHORT:  op = "movswl"; break;
	case TYPE_USHORT: op = "movzwl"; break;
	case TYPE_INT:    case TYPE_UINT:
	case TYPE_LONG:   case TYPE_ULONG:
	case TYPE_POINTER:
		op = "movl";
		break;
	default:
		internal_error(state, ins, "unknown type in load");
		op = "<invalid opcode>";
		break;
	}
	fprintf(fp, "\t%s (%s), %s\n",
		op,
		reg(state, src, REGCM_GPR32),
		reg(state, dst, REGCM_GPR32));
}


static void print_op_store(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	struct triple *dst, *src;
	dst = RHS(ins, 0);
	src = RHS(ins, 1);
	if (is_const(src) && (src->op == OP_INTCONST)) {
		long_t value;
		value = (long_t)(src->u.cval);
		fprintf(fp, "\tmov%s $%ld, (%s)\n",
			type_suffix(state, src->type),
			(long)(value),
			reg(state, dst, REGCM_GPR32));
	}
	else if (is_const(dst) && (dst->op == OP_INTCONST)) {
		fprintf(fp, "\tmov%s %s, 0x%08lx\n",
			type_suffix(state, src->type),
			reg(state, src, REGCM_GPR8_LO | REGCM_GPR16 | REGCM_GPR32),
			(unsigned long)(dst->u.cval));
	}
	else {
		if (is_const(src) || is_const(dst)) {
			internal_error(state, ins, "unknown store operation");
		}
		fprintf(fp, "\tmov%s %s, (%s)\n",
			type_suffix(state, src->type),
			reg(state, src, REGCM_GPR8_LO | REGCM_GPR16 | REGCM_GPR32),
			reg(state, dst, REGCM_GPR32));
	}


}

static void print_op_smul(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	if (!is_const(RHS(ins, 1))) {
		fprintf(fp, "\timul %s, %s\n",
			reg(state, RHS(ins, 1), REGCM_GPR32),
			reg(state, RHS(ins, 0), REGCM_GPR32));
	}
	else {
		fprintf(fp, "\timul ");
		print_const_val(state, RHS(ins, 1), fp);
		fprintf(fp, ", %s\n", reg(state, RHS(ins, 0), REGCM_GPR32));
	}
}

static void print_op_cmp(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	unsigned mask;
	int dreg;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO;
	dreg = check_reg(state, ins, REGCM_FLAGS);
	if (!reg_is_reg(state, dreg, REG_EFLAGS)) {
		internal_error(state, ins, "bad dest register for cmp");
	}
	if (is_const(RHS(ins, 1))) {
		fprintf(fp, "\tcmp ");
		print_const_val(state, RHS(ins, 1), fp);
		fprintf(fp, ", %s\n", reg(state, RHS(ins, 0), mask));
	}
	else {
		unsigned lmask, rmask;
		int lreg, rreg;
		lreg = check_reg(state, RHS(ins, 0), mask);
		rreg = check_reg(state, RHS(ins, 1), mask);
		lmask = arch_reg_regcm(state, lreg);
		rmask = arch_reg_regcm(state, rreg);
		mask = lmask & rmask;
		fprintf(fp, "\tcmp %s, %s\n",
			reg(state, RHS(ins, 1), mask),
			reg(state, RHS(ins, 0), mask));
	}
}

static void print_op_test(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	unsigned mask;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO;
	fprintf(fp, "\ttest %s, %s\n",
		reg(state, RHS(ins, 0), mask),
		reg(state, RHS(ins, 0), mask));
}

static void print_op_branch(struct compile_state *state,
	struct triple *branch, FILE *fp)
{
	const char *bop = "j";
	if ((branch->op == OP_JMP) || (branch->op == OP_CALL)) {
		if (branch->rhs != 0) {
			internal_error(state, branch, "jmp with condition?");
		}
		bop = "jmp";
	}
	else {
		struct triple *ptr;
		if (branch->rhs != 1) {
			internal_error(state, branch, "jmpcc without condition?");
		}
		check_reg(state, RHS(branch, 0), REGCM_FLAGS);
		if ((RHS(branch, 0)->op != OP_CMP) &&
			(RHS(branch, 0)->op != OP_TEST)) {
			internal_error(state, branch, "bad branch test");
		}
#if DEBUG_ROMCC_WARNINGS
#warning "FIXME I have observed instructions between the test and branch instructions"
#endif
		for(ptr = RHS(branch, 0)->next; ptr != branch; ptr = ptr->next) {
			if (ptr->op != OP_COPY) {
				internal_error(state, branch, "branch does not follow test");
			}
		}
		switch(branch->op) {
		case OP_JMP_EQ:       bop = "jz";  break;
		case OP_JMP_NOTEQ:    bop = "jnz"; break;
		case OP_JMP_SLESS:    bop = "jl";  break;
		case OP_JMP_ULESS:    bop = "jb";  break;
		case OP_JMP_SMORE:    bop = "jg";  break;
		case OP_JMP_UMORE:    bop = "ja";  break;
		case OP_JMP_SLESSEQ:  bop = "jle"; break;
		case OP_JMP_ULESSEQ:  bop = "jbe"; break;
		case OP_JMP_SMOREEQ:  bop = "jge"; break;
		case OP_JMP_UMOREEQ:  bop = "jae"; break;
		default:
			internal_error(state, branch, "Invalid branch op");
			break;
		}

	}
#if 1
	if (branch->op == OP_CALL) {
		fprintf(fp, "\t/* call */\n");
	}
#endif
	fprintf(fp, "\t%s L%s%lu\n",
		bop,
		state->compiler->label_prefix,
		(unsigned long)(TARG(branch, 0)->u.cval));
}

static void print_op_ret(struct compile_state *state,
	struct triple *branch, FILE *fp)
{
	fprintf(fp, "\tjmp *%s\n",
		reg(state, RHS(branch, 0), REGCM_GPR32));
}

static void print_op_set(struct compile_state *state,
	struct triple *set, FILE *fp)
{
	const char *sop = "set";
	if (set->rhs != 1) {
		internal_error(state, set, "setcc without condition?");
	}
	check_reg(state, RHS(set, 0), REGCM_FLAGS);
	if ((RHS(set, 0)->op != OP_CMP) &&
		(RHS(set, 0)->op != OP_TEST)) {
		internal_error(state, set, "bad set test");
	}
	if (RHS(set, 0)->next != set) {
		internal_error(state, set, "set does not follow test");
	}
	switch(set->op) {
	case OP_SET_EQ:       sop = "setz";  break;
	case OP_SET_NOTEQ:    sop = "setnz"; break;
	case OP_SET_SLESS:    sop = "setl";  break;
	case OP_SET_ULESS:    sop = "setb";  break;
	case OP_SET_SMORE:    sop = "setg";  break;
	case OP_SET_UMORE:    sop = "seta";  break;
	case OP_SET_SLESSEQ:  sop = "setle"; break;
	case OP_SET_ULESSEQ:  sop = "setbe"; break;
	case OP_SET_SMOREEQ:  sop = "setge"; break;
	case OP_SET_UMOREEQ:  sop = "setae"; break;
	default:
		internal_error(state, set, "Invalid set op");
		break;
	}
	fprintf(fp, "\t%s %s\n",
		sop, reg(state, set, REGCM_GPR8_LO));
}

static void print_op_bit_scan(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	const char *op;
	switch(ins->op) {
	case OP_BSF: op = "bsf"; break;
	case OP_BSR: op = "bsr"; break;
	default:
		internal_error(state, ins, "unknown bit scan");
		op = 0;
		break;
	}
	fprintf(fp,
		"\t%s %s, %s\n"
		"\tjnz 1f\n"
		"\tmovl $-1, %s\n"
		"1:\n",
		op,
		reg(state, RHS(ins, 0), REGCM_GPR32),
		reg(state, ins, REGCM_GPR32),
		reg(state, ins, REGCM_GPR32));
}


static void print_sdecl(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	fprintf(fp, ".section \"" DATA_SECTION "\"\n");
	fprintf(fp, ".balign %ld\n", (long int)align_of_in_bytes(state, ins->type));
	fprintf(fp, "L%s%lu:\n",
		state->compiler->label_prefix, (unsigned long)(ins->u.cval));
	print_const(state, MISC(ins, 0), fp);
	fprintf(fp, ".section \"" TEXT_SECTION "\"\n");

}

static void print_instruction(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	/* Assumption: after I have exted the register allocator
	 * everything is in a valid register.
	 */
	switch(ins->op) {
	case OP_ASM:
		print_op_asm(state, ins, fp);
		break;
	case OP_ADD:	print_binary_op(state, "add", ins, fp); break;
	case OP_SUB:	print_binary_op(state, "sub", ins, fp); break;
	case OP_AND:	print_binary_op(state, "and", ins, fp); break;
	case OP_XOR:	print_binary_op(state, "xor", ins, fp); break;
	case OP_OR:	print_binary_op(state, "or",  ins, fp); break;
	case OP_SL:	print_op_shift(state, "shl", ins, fp); break;
	case OP_USR:	print_op_shift(state, "shr", ins, fp); break;
	case OP_SSR:	print_op_shift(state, "sar", ins, fp); break;
	case OP_POS:	break;
	case OP_NEG:	print_unary_op(state, "neg", ins, fp); break;
	case OP_INVERT:	print_unary_op(state, "not", ins, fp); break;
	case OP_NOOP:
	case OP_INTCONST:
	case OP_ADDRCONST:
	case OP_BLOBCONST:
		/* Don't generate anything here for constants */
	case OP_PHI:
		/* Don't generate anything for variable declarations. */
		break;
	case OP_UNKNOWNVAL:
		fprintf(fp, " /* unknown %s */\n",
			reg(state, ins, REGCM_ALL));
		break;
	case OP_SDECL:
		print_sdecl(state, ins, fp);
		break;
	case OP_COPY:
	case OP_CONVERT:
		print_op_move(state, ins, fp);
		break;
	case OP_LOAD:
		print_op_load(state, ins, fp);
		break;
	case OP_STORE:
		print_op_store(state, ins, fp);
		break;
	case OP_SMUL:
		print_op_smul(state, ins, fp);
		break;
	case OP_CMP:    print_op_cmp(state, ins, fp); break;
	case OP_TEST:   print_op_test(state, ins, fp); break;
	case OP_JMP:
	case OP_JMP_EQ:      case OP_JMP_NOTEQ:
	case OP_JMP_SLESS:   case OP_JMP_ULESS:
	case OP_JMP_SMORE:   case OP_JMP_UMORE:
	case OP_JMP_SLESSEQ: case OP_JMP_ULESSEQ:
	case OP_JMP_SMOREEQ: case OP_JMP_UMOREEQ:
	case OP_CALL:
		print_op_branch(state, ins, fp);
		break;
	case OP_RET:
		print_op_ret(state, ins, fp);
		break;
	case OP_SET_EQ:      case OP_SET_NOTEQ:
	case OP_SET_SLESS:   case OP_SET_ULESS:
	case OP_SET_SMORE:   case OP_SET_UMORE:
	case OP_SET_SLESSEQ: case OP_SET_ULESSEQ:
	case OP_SET_SMOREEQ: case OP_SET_UMOREEQ:
		print_op_set(state, ins, fp);
		break;
	case OP_INB:  case OP_INW:  case OP_INL:
		print_op_in(state, ins, fp);
		break;
	case OP_OUTB: case OP_OUTW: case OP_OUTL:
		print_op_out(state, ins, fp);
		break;
	case OP_BSF:
	case OP_BSR:
		print_op_bit_scan(state, ins, fp);
		break;
	case OP_RDMSR:
		after_lhs(state, ins);
		fprintf(fp, "\trdmsr\n");
		break;
	case OP_WRMSR:
		fprintf(fp, "\twrmsr\n");
		break;
	case OP_HLT:
		fprintf(fp, "\thlt\n");
		break;
	case OP_SDIVT:
		fprintf(fp, "\tidiv %s\n", reg(state, RHS(ins, 1), REGCM_GPR32));
		break;
	case OP_UDIVT:
		fprintf(fp, "\tdiv %s\n", reg(state, RHS(ins, 1), REGCM_GPR32));
		break;
	case OP_UMUL:
		fprintf(fp, "\tmul %s\n", reg(state, RHS(ins, 1), REGCM_GPR32));
		break;
	case OP_LABEL:
		if (!ins->use) {
			return;
		}
		fprintf(fp, "L%s%lu:\n",
			state->compiler->label_prefix, (unsigned long)(ins->u.cval));
		break;
	case OP_ADECL:
		/* Ignore adecls with no registers error otherwise */
		if (!noop_adecl(ins)) {
			internal_error(state, ins, "adecl remains?");
		}
		break;
		/* Ignore OP_PIECE */
	case OP_PIECE:
		break;
		/* Operations that should never get here */
	case OP_SDIV: case OP_UDIV:
	case OP_SMOD: case OP_UMOD:
	case OP_LTRUE:   case OP_LFALSE:  case OP_EQ:      case OP_NOTEQ:
	case OP_SLESS:   case OP_ULESS:   case OP_SMORE:   case OP_UMORE:
	case OP_SLESSEQ: case OP_ULESSEQ: case OP_SMOREEQ: case OP_UMOREEQ:
	default:
		internal_error(state, ins, "unknown op: %d %s",
			ins->op, tops(ins->op));
		break;
	}
}

static void print_instructions(struct compile_state *state)
{
	struct triple *first, *ins;
	int print_location;
	struct occurrence *last_occurrence;
	FILE *fp;
	int max_inline_depth;
	max_inline_depth = 0;
	print_location = 1;
	last_occurrence = 0;
	fp = state->output;
	/* Masks for common sizes */
	fprintf(fp, ".section \"" DATA_SECTION "\"\n");
	fprintf(fp, ".balign 16\n");
	fprintf(fp, "L%s1:\n", state->compiler->label_prefix);
	fprintf(fp, ".int 0xff, 0, 0, 0\n");
	fprintf(fp, "L%s2:\n", state->compiler->label_prefix);
	fprintf(fp, ".int 0xffff, 0, 0, 0\n");
	fprintf(fp, ".section \"" TEXT_SECTION "\"\n");
	first = state->first;
	ins = first;
	do {
		if (print_location &&
			last_occurrence != ins->occurrence) {
			if (!ins->occurrence->parent) {
				fprintf(fp, "\t/* %s,%s:%d.%d */\n",
					ins->occurrence->function?ins->occurrence->function:"(null)",
					ins->occurrence->filename?ins->occurrence->filename:"(null)",
					ins->occurrence->line,
					ins->occurrence->col);
			}
			else {
				struct occurrence *ptr;
				int inline_depth;
				fprintf(fp, "\t/*\n");
				inline_depth = 0;
				for(ptr = ins->occurrence; ptr; ptr = ptr->parent) {
					inline_depth++;
					fprintf(fp, "\t * %s,%s:%d.%d\n",
						ptr->function,
						ptr->filename,
						ptr->line,
						ptr->col);
				}
				fprintf(fp, "\t */\n");
				if (inline_depth > max_inline_depth) {
					max_inline_depth = inline_depth;
				}
			}
			if (last_occurrence) {
				put_occurrence(last_occurrence);
			}
			get_occurrence(ins->occurrence);
			last_occurrence = ins->occurrence;
		}

		print_instruction(state, ins, fp);
		ins = ins->next;
	} while(ins != first);
	if (print_location) {
		fprintf(fp, "/* max inline depth %d */\n",
			max_inline_depth);
	}
}

static void generate_code(struct compile_state *state)
{
	generate_local_labels(state);
	print_instructions(state);

}

static void print_preprocessed_tokens(struct compile_state *state)
{
	int tok;
	FILE *fp;
	int line;
	const char *filename;
	fp = state->output;
	filename = 0;
	line = 0;
	for(;;) {
		struct file_state *file;
		struct token *tk;
		const char *token_str;
		tok = peek(state);
		if (tok == TOK_EOF) {
			break;
		}
		tk = eat(state, tok);
		token_str =
			tk->ident ? tk->ident->name :
			tk->str_len ? tk->val.str :
			tokens[tk->tok];

		file = state->file;
		while(file->macro && file->prev) {
			file = file->prev;
		}
		if (!file->macro &&
			((file->line != line) || (file->basename != filename)))
		{
			int i, col;
			if ((file->basename == filename) &&
				(line < file->line)) {
				while(line < file->line) {
					fprintf(fp, "\n");
					line++;
				}
			}
			else {
				fprintf(fp, "\n#line %d \"%s\"\n",
					file->line, file->basename);
			}
			line = file->line;
			filename = file->basename;
			col = get_col(file) - strlen(token_str);
			for(i = 0; i < col; i++) {
				fprintf(fp, " ");
			}
		}

		fprintf(fp, "%s ", token_str);

		if (state->compiler->debug & DEBUG_TOKENS) {
			loc(state->dbgout, state, 0);
			fprintf(state->dbgout, "%s <- `%s'\n",
				tokens[tok], token_str);
		}
	}
}

static void compile(const char *filename,
	struct compiler_state *compiler, struct arch_state *arch)
{
	int i;
	struct compile_state state;
	struct triple *ptr;
	struct filelist *includes = include_filelist;
	memset(&state, 0, sizeof(state));
	state.compiler = compiler;
	state.arch     = arch;
	state.file = 0;
	for(i = 0; i < sizeof(state.token)/sizeof(state.token[0]); i++) {
		memset(&state.token[i], 0, sizeof(state.token[i]));
		state.token[i].tok = -1;
	}
	/* Remember the output descriptors */
	state.errout = stderr;
	state.dbgout = stdout;
	/* Remember the output filename */
	if ((state.compiler->flags & COMPILER_PP_ONLY) && (strcmp("auto.inc",state.compiler->ofilename) == 0)) {
		state.output    = stdout;
	} else {
		state.output    = fopen(state.compiler->ofilename, "w");
		if (!state.output) {
			error(&state, 0, "Cannot open output file %s\n",
				state.compiler->ofilename);
		}
	}
	/* Make certain a good cleanup happens */
	exit_state = &state;
	atexit(exit_cleanup);

	/* Prep the preprocessor */
	state.if_depth = 0;
	memset(state.if_bytes, 0, sizeof(state.if_bytes));
	/* register the C keywords */
	register_keywords(&state);
	/* register the keywords the macro preprocessor knows */
	register_macro_keywords(&state);
	/* generate some builtin macros */
	register_builtin_macros(&state);
	/* Memorize where some special keywords are. */
	state.i_switch        = lookup(&state, "switch", 6);
	state.i_case          = lookup(&state, "case", 4);
	state.i_continue      = lookup(&state, "continue", 8);
	state.i_break         = lookup(&state, "break", 5);
	state.i_default       = lookup(&state, "default", 7);
	state.i_return        = lookup(&state, "return", 6);
	/* Memorize where predefined macros are. */
	state.i___VA_ARGS__   = lookup(&state, "__VA_ARGS__", 11);
	state.i___FILE__      = lookup(&state, "__FILE__", 8);
	state.i___LINE__      = lookup(&state, "__LINE__", 8);
	/* Memorize where predefined identifiers are. */
	state.i___func__      = lookup(&state, "__func__", 8);
	/* Memorize where some attribute keywords are. */
	state.i_noinline      = lookup(&state, "noinline", 8);
	state.i_always_inline = lookup(&state, "always_inline", 13);
	state.i_noreturn      = lookup(&state, "noreturn", 8);
	state.i_unused        = lookup(&state, "unused", 6);
	state.i_packed        = lookup(&state, "packed", 6);

	/* Process the command line macros */
	process_cmdline_macros(&state);

	/* Allocate beginning bounding labels for the function list */
	state.first = label(&state);
	state.first->id |= TRIPLE_FLAG_VOLATILE;
	use_triple(state.first, state.first);
	ptr = label(&state);
	ptr->id |= TRIPLE_FLAG_VOLATILE;
	use_triple(ptr, ptr);
	flatten(&state, state.first, ptr);

	/* Allocate a label for the pool of global variables */
	state.global_pool = label(&state);
	state.global_pool->id |= TRIPLE_FLAG_VOLATILE;
	flatten(&state, state.first, state.global_pool);

	/* Enter the globl definition scope */
	start_scope(&state);
	register_builtins(&state);

	compile_file(&state, filename, 1);

	while (includes) {
		compile_file(&state, includes->filename, 1);
		includes=includes->next;
	}

	/* Stop if all we want is preprocessor output */
	if (state.compiler->flags & COMPILER_PP_ONLY) {
		print_preprocessed_tokens(&state);
		return;
	}

	decls(&state);

	/* Exit the global definition scope */
	end_scope(&state);

	/* Now that basic compilation has happened
	 * optimize the intermediate code
	 */
	optimize(&state);

	generate_code(&state);
	if (state.compiler->debug) {
		fprintf(state.errout, "done\n");
	}
	exit_state = 0;
}

static void version(FILE *fp)
{
	fprintf(fp, "romcc " VERSION " released " RELEASE_DATE "\n");
}

static void usage(void)
{
	FILE *fp = stdout;
	version(fp);
	fprintf(fp,
		"\nUsage: romcc [options] <source>.c\n"
		"Compile a C source file generating a binary that does not implicilty use RAM\n"
		"Options:\n"
		"-o <output file name>\n"
		"-f<option>            Specify a generic compiler option\n"
		"-m<option>            Specify a arch dependent option\n"
		"--                    Specify this is the last option\n"
		"\nGeneric compiler options:\n"
	);
	compiler_usage(fp);
	fprintf(fp,
		"\nArchitecture compiler options:\n"
	);
	arch_usage(fp);
	fprintf(fp,
		"\n"
	);
}

static void arg_error(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	usage();
	exit(1);
}

static void arg_warning(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

int main(int argc, char **argv)
{
	const char *filename;
	struct compiler_state compiler;
	struct arch_state arch;
	int all_opts;


	/* I don't want any surprises */
	setlocale(LC_ALL, "C");

	init_compiler_state(&compiler);
	init_arch_state(&arch);
	filename = 0;
	all_opts = 0;
	while(argc > 1) {
		if (!all_opts && (strcmp(argv[1], "-o") == 0) && (argc > 2)) {
			compiler.ofilename = argv[2];
			argv += 2;
			argc -= 2;
		}
		else if (!all_opts && argv[1][0] == '-') {
			int result;
			result = -1;
			if (strcmp(argv[1], "--") == 0) {
				result = 0;
				all_opts = 1;
			}
			else if (strncmp(argv[1], "-E", 2) == 0) {
				result = compiler_encode_flag(&compiler, argv[1]);
			}
			else if (strncmp(argv[1], "-O", 2) == 0) {
				result = compiler_encode_flag(&compiler, argv[1]);
			}
			else if (strncmp(argv[1], "-I", 2) == 0) {
				result = compiler_encode_flag(&compiler, argv[1]);
			}
			else if (strncmp(argv[1], "-D", 2) == 0) {
				result = compiler_encode_flag(&compiler, argv[1]);
			}
			else if (strncmp(argv[1], "-U", 2) == 0) {
				result = compiler_encode_flag(&compiler, argv[1]);
			}
			else if (strncmp(argv[1], "--label-prefix=", 15) == 0) {
				result = compiler_encode_flag(&compiler, argv[1]+2);
			}
			else if (strncmp(argv[1], "-f", 2) == 0) {
				result = compiler_encode_flag(&compiler, argv[1]+2);
			}
			else if (strncmp(argv[1], "-m", 2) == 0) {
				result = arch_encode_flag(&arch, argv[1]+2);
			}
			else if (strncmp(argv[1], "-c", 2) == 0) {
				result = 0;
			}
			else if (strncmp(argv[1], "-S", 2) == 0) {
				result = 0;
			}
			else if (strncmp(argv[1], "-include", 10) == 0) {
				struct filelist *old_head = include_filelist;
				include_filelist = malloc(sizeof(struct filelist));
				if (!include_filelist) {
					die("Out of memory.\n");
				}
				argv++;
				argc--;
				include_filelist->filename = strdup(argv[1]);
				include_filelist->next = old_head;
				result = 0;
			}
			if (result < 0) {
				arg_error("Invalid option specified: %s\n",
					argv[1]);
			}
			argv++;
			argc--;
		}
		else {
			if (filename) {
				arg_error("Only one filename may be specified\n");
			}
			filename = argv[1];
			argv++;
			argc--;
		}
	}
	if (!filename) {
		arg_error("No filename specified\n");
	}
	compile(filename, &compiler, &arch);

	return 0;
}
