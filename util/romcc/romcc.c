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

#define MAX_ALLOCATION_PASSES 100

#define DEBUG_CONSISTENCY 1
#define DEBUG_SDP_BLOCKS 0
#define DEBUG_TRIPLE_COLOR 0

#warning "FIXME boundary cases with small types in larger registers"
#warning "FIXME give clear error messages about unused variables"
#warning "FIXME properly handle multi dimensional arrays"

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
static void die(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stdout);
	fflush(stderr);
	exit(1);
}

#define MALLOC_STRONG_DEBUG
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
		die("chdir to %s failed: %s\n",
			path, strerror(errno));
	}
}

static int exists(const char *dirname, const char *filename)
{
	int does_exist = 1;
	xchdir(dirname);
	if (access(filename, O_RDONLY) < 0) {
		if ((errno != EACCES) && (errno != EROFS)) {
			does_exist = 0;
		}
	}
	return does_exist;
}


static char *slurp_file(const char *dirname, const char *filename, off_t *r_size)
{
	int fd;
	char *buf;
	off_t size, progress;
	ssize_t result;
	struct stat stats;
	
	if (!filename) {
		*r_size = 0;
		return 0;
	}
	xchdir(dirname);
	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		die("Cannot open '%s' : %s\n",
			filename, strerror(errno));
	}
	result = fstat(fd, &stats);
	if (result < 0) {
		die("Cannot stat: %s: %s\n",
			filename, strerror(errno));
	}
	size = stats.st_size;
	*r_size = size +1;
	buf = xmalloc(size +2, filename);
	buf[size] = '\n'; /* Make certain the file is newline terminated */
	buf[size+1] = '\0'; /* Null terminate the file for good measure */
	progress = 0;
	while(progress < size) {
		result = read(fd, buf + progress, size - progress);
		if (result < 0) {
			if ((errno == EINTR) ||	(errno == EAGAIN))
				continue;
			die("read on %s of %ld bytes failed: %s\n",
				filename, (size - progress)+ 0UL, strerror(errno));
		}
		progress += result;
	}
	result = close(fd);
	if (result < 0) {
		die("Close of %s failed: %s\n",
			filename, strerror(errno));
	}
	return buf;
}

/* Types on the destination platform */
#warning "FIXME this assumes 32bit x86 is the destination"
typedef int8_t   schar_t;
typedef uint8_t  uchar_t;
typedef int8_t   char_t;
typedef int16_t  short_t;
typedef uint16_t ushort_t;
typedef int32_t  int_t;
typedef uint32_t uint_t;
typedef int32_t  long_t;
typedef uint32_t ulong_t;

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

struct file_state {
	struct file_state *prev;
	const char *basename;
	char *dirname;
	char *buf;
	off_t size;
	char *pos;
	int line;
	char *line_start;
	int report_line;
	const char *report_name;
	const char *report_dir;
};
struct hash_entry;
struct token {
	int tok;
	struct hash_entry *ident;
	int str_len;
	union {
		ulong_t integer;
		const char *str;
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

#define OP_NOOP      34

#define OP_MIN_CONST 50
#define OP_MAX_CONST 59
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

#define OP_WRITE     60 
/* OP_WRITE moves one pseudo register to another.
 * RHS(0) holds the destination pseudo register, which must be an OP_DECL.
 * RHS(1) holds the psuedo to move.
 */

#define OP_READ      61
/* OP_READ reads the value of a variable and makes
 * it available for the pseudo operation.
 * Useful for things like def-use chains.
 * RHS(0) holds points to the triple to read from.
 */
#define OP_COPY      62
/* OP_COPY makes a copy of the psedo register or constant in RHS(0).
 */
#define OP_PIECE     63
/* OP_PIECE returns one piece of a instruction that returns a structure.
 * MISC(0) is the instruction
 * u.cval is the LHS piece of the instruction to return.
 */
#define OP_ASM       64
/* OP_ASM holds a sequence of assembly instructions, the result
 * of a C asm directive.
 * RHS(x) holds input value x to the assembly sequence.
 * LHS(x) holds the output value x from the assembly sequence.
 * u.blob holds the string of assembly instructions.
 */

#define OP_DEREF     65
/* OP_DEREF generates an lvalue from a pointer.
 * RHS(0) holds the pointer value.
 * OP_DEREF serves as a place holder to indicate all necessary
 * checks have been done to indicate a value is an lvalue.
 */
#define OP_DOT       66
/* OP_DOT references a submember of a structure lvalue.
 * RHS(0) holds the lvalue.
 * ->u.field holds the name of the field we want.
 *
 * Not seen outside of expressions.
 */
#define OP_VAL       67
/* OP_VAL returns the value of a subexpression of the current expression.
 * Useful for operators that have side effects.
 * RHS(0) holds the expression.
 * MISC(0) holds the subexpression of RHS(0) that is the
 * value of the expression.
 *
 * Not seen outside of expressions.
 */
#define OP_LAND      68
/* OP_LAND performs a C logical and between RHS(0) and RHS(1).
 * Not seen outside of expressions.
 */
#define OP_LOR       69
/* OP_LOR performs a C logical or between RHS(0) and RHS(1).
 * Not seen outside of expressions.
 */
#define OP_COND      70
/* OP_CODE performas a C ? : operation. 
 * RHS(0) holds the test.
 * RHS(1) holds the expression to evaluate if the test returns true.
 * RHS(2) holds the expression to evaluate if the test returns false.
 * Not seen outside of expressions.
 */
#define OP_COMMA     71
/* OP_COMMA performacs a C comma operation.
 * That is RHS(0) is evaluated, then RHS(1)
 * and the value of RHS(1) is returned.
 * Not seen outside of expressions.
 */

#define OP_FCALL      72
/* OP_FCALL performs a procedure call. 
 * MISC(0) holds a pointer to the OP_LIST of a function
 * RHS(x) holds argument x of a function
 * 
 * Currently not seen outside of expressions.
 */
#define OP_VAL_VEC   74
/* OP_VAL_VEC is an array of triples that are either variable
 * or values for a structure or an array.
 * RHS(x) holds element x of the vector.
 * triple->type->elements holds the size of the vector.
 */

/* statements */
#define OP_LIST      80
/* OP_LIST Holds a list of statements that compose a function, and a result value.
 * RHS(0) holds the list of statements.
 * MISC(0) holds the value of the statements.
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
 * TARG(1) holds the branch target.
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
 * ->use is a list of statements that use the variable.
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
#define PURE   1 /* Triple has no side effects */
#define IMPURE 2 /* Triple has side effects */
#define PURE_BITS(FLAGS) ((FLAGS) & 0x3)
#define DEF    4 /* Triple is a variable definition */
#define BLOCK  8 /* Triple stores the current block */
#define STRUCTURAL 16 /* Triple does not generate a machine instruction */
#define BRANCH     32 /* Triple is a branch instruction */
#define CBRANCH    64 /* Triple is a conditional branch instruction */
	unsigned char lhs, rhs, misc, targ;
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

[OP_LOAD       ] = OP( 0,  1, 0, 0, IMPURE | DEF | BLOCK, "load"),
[OP_STORE      ] = OP( 0,  2, 0, 0, IMPURE | BLOCK , "store"),

[OP_NOOP       ] = OP( 0,  0, 0, 0, PURE | BLOCK | STRUCTURAL, "noop"),

[OP_INTCONST   ] = OP( 0,  0, 0, 0, PURE | DEF, "intconst"),
[OP_BLOBCONST  ] = OP( 0,  0, 0, 0, PURE , "blobconst"),
[OP_ADDRCONST  ] = OP( 0,  0, 1, 0, PURE | DEF, "addrconst"),

[OP_WRITE      ] = OP( 0,  2, 0, 0, PURE | BLOCK, "write"),
[OP_READ       ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "read"),
[OP_COPY       ] = OP( 0,  1, 0, 0, PURE | DEF | BLOCK, "copy"),
[OP_PIECE      ] = OP( 0,  0, 1, 0, PURE | DEF | STRUCTURAL, "piece"),
[OP_ASM        ] = OP(-1, -1, 0, 0, IMPURE, "asm"),
[OP_DEREF      ] = OP( 0,  1, 0, 0, 0 | DEF | BLOCK, "deref"), 
[OP_DOT        ] = OP( 0,  1, 0, 0, 0 | DEF | BLOCK, "dot"),

[OP_VAL        ] = OP( 0,  1, 1, 0, 0 | DEF | BLOCK, "val"),
[OP_LAND       ] = OP( 0,  2, 0, 0, 0 | DEF | BLOCK, "land"),
[OP_LOR        ] = OP( 0,  2, 0, 0, 0 | DEF | BLOCK, "lor"),
[OP_COND       ] = OP( 0,  3, 0, 0, 0 | DEF | BLOCK, "cond"),
[OP_COMMA      ] = OP( 0,  2, 0, 0, 0 | DEF | BLOCK, "comma"),
/* Call is special most it can stand in for anything so it depends on context */
[OP_FCALL       ] = OP(-1, -1, 1, 0, 0 | BLOCK, "fcall"),
/* The sizes of OP_FCALL and OP_VAL_VEC depend upon context */
[OP_VAL_VEC    ] = OP( 0, -1, 0, 0, 0 | BLOCK | STRUCTURAL, "valvec"),

[OP_LIST       ] = OP( 0,  1, 1, 0, 0 | DEF | STRUCTURAL, "list"),
[OP_BRANCH     ] = OP( 0,  0, 0, 1, PURE | BLOCK | BRANCH, "branch"),
[OP_CBRANCH    ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "cbranch"),
[OP_CALL       ] = OP( 0,  0, 1, 1, PURE | BLOCK | BRANCH, "call"),
[OP_RET        ] = OP( 0,  1, 0, 0, PURE | BLOCK | BRANCH, "ret"),
[OP_LABEL      ] = OP( 0,  0, 0, 0, PURE | BLOCK | STRUCTURAL, "label"),
[OP_ADECL      ] = OP( 0,  0, 0, 0, PURE | BLOCK | STRUCTURAL, "adecl"),
[OP_SDECL      ] = OP( 0,  0, 1, 0, PURE | BLOCK | STRUCTURAL, "sdecl"),
/* The number of RHS elements of OP_PHI depend upon context */
[OP_PHI        ] = OP( 0, -1, 1, 0, PURE | DEF | BLOCK, "phi"),

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
[OP_JMP        ] = OP( 0,  0, 0, 1, PURE | BLOCK | BRANCH, "jmp"),
[OP_JMP_EQ     ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_eq"),
[OP_JMP_NOTEQ  ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_noteq"),
[OP_JMP_SLESS  ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_sless"),
[OP_JMP_ULESS  ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_uless"),
[OP_JMP_SMORE  ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_smore"),
[OP_JMP_UMORE  ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_umore"),
[OP_JMP_SLESSEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_slesseq"),
[OP_JMP_ULESSEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_ulesseq"),
[OP_JMP_SMOREEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_smoreq"),
[OP_JMP_UMOREEQ] = OP( 0,  1, 0, 1, PURE | BLOCK | BRANCH | CBRANCH, "jmp_umoreq"),

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
	if (index > OP_MAX) {
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

#define MAX_LHS  15
#define MAX_RHS  250
#define MAX_MISC 3
#define MAX_TARG 3

struct occurance {
	int count;
	const char *filename;
	const char *function;
	int line;
	int col;
	struct occurance *parent;
};
struct triple {
	struct triple *next, *prev;
	struct triple_set *use;
	struct type *type;
	unsigned char op;
	unsigned char template_id;
	unsigned short sizes;
#define TRIPLE_LHS(SIZES)  (((SIZES) >>  0) & 0x0f)
#define TRIPLE_RHS(SIZES)  (((SIZES) >>  4) & 0xff)
#define TRIPLE_MISC(SIZES) (((SIZES) >> 12) & 0x03)
#define TRIPLE_TARG(SIZES) (((SIZES) >> 14) & 0x03)
#define TRIPLE_SIZE(SIZES) \
	(TRIPLE_LHS(SIZES)  + \
	 TRIPLE_RHS(SIZES)  + \
	 TRIPLE_MISC(SIZES) + \
	 TRIPLE_TARG(SIZES))
#define TRIPLE_SIZES(LHS, RHS, MISC, TARG) \
	((((LHS) & 0x0f) <<  0) | \
	(((RHS)  & 0xff) <<  4) | \
	(((MISC) & 0x03) << 12) | \
	(((TARG) & 0x03) << 14))
#define TRIPLE_LHS_OFF(SIZES)  (0)
#define TRIPLE_RHS_OFF(SIZES)  (TRIPLE_LHS_OFF(SIZES) + TRIPLE_LHS(SIZES))
#define TRIPLE_MISC_OFF(SIZES) (TRIPLE_RHS_OFF(SIZES) + TRIPLE_RHS(SIZES))
#define TRIPLE_TARG_OFF(SIZES) (TRIPLE_MISC_OFF(SIZES) + TRIPLE_MISC(SIZES))
#define LHS(PTR,INDEX) ((PTR)->param[TRIPLE_LHS_OFF((PTR)->sizes) + (INDEX)])
#define RHS(PTR,INDEX) ((PTR)->param[TRIPLE_RHS_OFF((PTR)->sizes) + (INDEX)])
#define TARG(PTR,INDEX) ((PTR)->param[TRIPLE_TARG_OFF((PTR)->sizes) + (INDEX)])
#define MISC(PTR,INDEX) ((PTR)->param[TRIPLE_MISC_OFF((PTR)->sizes) + (INDEX)])
	unsigned id; /* A scratch value and finally the register */
#define TRIPLE_FLAG_FLATTENED   (1 << 31)
#define TRIPLE_FLAG_PRE_SPLIT   (1 << 30)
#define TRIPLE_FLAG_POST_SPLIT  (1 << 29)
#define TRIPLE_FLAG_VOLATILE    (1 << 28)
#define TRIPLE_FLAG_LOCAL	(1 << 27)
	struct occurance *occurance;
	union {
		ulong_t cval;
		struct block  *block;
		void *blob;
		struct hash_entry *field;
		struct asm_info *ainfo;
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

struct macro {
	struct hash_entry *ident;
	char *buf;
	int buf_len;
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
};
struct arch_state {
	unsigned long features;
};
struct compile_state {
	struct compiler_state *compiler;
	struct arch_state *arch;
	FILE *output;
	struct file_state *file;
	struct occurance *last_occurance;
	const char *function;
	struct token token[4];
	struct hash_entry *hash_table[HASH_TABLE_SIZE];
	struct hash_entry *i_switch;
	struct hash_entry *i_case;
	struct hash_entry *i_continue;
	struct hash_entry *i_break;
	struct hash_entry *i_default;
	struct hash_entry *i_return;
	int scope_depth;
	int if_depth, if_value;
	int macro_line;
	struct file_state *macro_file;
	struct triple *functions;
	struct triple *main_function;
	struct triple *first;
	struct triple *global_pool;
	struct block *first_block, *last_block;
	int last_vertex;
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
#define TYPE_INTEGER(TYPE)    ((((TYPE) >= TYPE_CHAR) && ((TYPE) <= TYPE_ULLONG)) || ((TYPE) == TYPE_ENUM))
#define TYPE_ARITHMETIC(TYPE) ((((TYPE) >= TYPE_CHAR) && ((TYPE) <= TYPE_LDOUBLE)) || ((TYPE) == TYPE_ENUM))
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
#define TYPE_UNION    0x1100
#define TYPE_POINTER  0x1200 
/* For TYPE_POINTER:
 * type->left holds the type pointed to.
 */
#define TYPE_FUNCTION 0x1300 
/* For TYPE_FUNCTION:
 * type->left holds the return type.
 * type->right holds the...
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
 * type-> holds the number of elements.
 */

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
#define REG_UNSET          0
#define REG_UNNEEDED       1
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
#if (MAX_REGC + REGISTER_BITS) > 27
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
static struct triple *transform_to_arch_instruction(
	struct compile_state *state, struct triple *ins);



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

#define DEBUG_DEFAULT ( \
	DEBUG_ABORT_ON_ERROR | \
	DEBUG_BASIC_BLOCKS | \
	DEBUG_FDOMINATORS | \
	DEBUG_RDOMINATORS | \
	DEBUG_TRIPLES | \
	0 )

#define COMPILER_ELIMINATE_INEFECTUAL_CODE 0x00000001
#define COMPILER_SIMPLIFY                  0x00000002
#define COMPILER_SCC_TRANSFORM             0x00000004
#define COMPILER_INLINE                    0x00000008
#define COMPILER_ALWAYS_INLINE             0x00000010
#define COMPILER_SIMPLIFY_OP               0x00000020
#define COMPILER_SIMPLIFY_PHI              0x00000040
#define COMPILER_SIMPLIFY_LABEL            0x00000080
#define COMPILER_SIMPLIFY_BRANCH           0x00000100
#define COMPILER_SIMPLIFY_COPY             0x00000200
#define COMPILER_SIMPLIFY_ARITH            0x00000400
#define COMPILER_SIMPLIFY_SHIFT            0x00000800
#define COMPILER_SIMPLIFY_BITWISE          0x00001000
#define COMPILER_SIMPLIFY_LOGICAL          0x00002000

#define COMPILER_DEFAULT_FLAGS ( \
	COMPILER_ELIMINATE_INEFECTUAL_CODE | \
	COMPILER_INLINE | \
	COMPILER_ALWAYS_INLINE | \
	COMPILER_SIMPLIFY_OP | \
	COMPILER_SIMPLIFY_PHI | \
	COMPILER_SIMPLIFY_LABEL | \
	COMPILER_SIMPLIFY_BRANCH | \
	COMPILER_SIMPLIFY_COPY | \
	COMPILER_SIMPLIFY_ARITH | \
	COMPILER_SIMPLIFY_SHIFT | \
	COMPILER_SIMPLIFY_BITWISE | \
	COMPILER_SIMPLIFY_LOGICAL | \
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

}

struct compiler_flag {
	const char *name;
	unsigned long flag;
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

static int compiler_encode_flag(
	struct compiler_state *compiler, const char *flag)
{
	static const struct compiler_flag flags[] = {
		{ "eliminate-inefectual-code", COMPILER_ELIMINATE_INEFECTUAL_CODE },
		{ "simplify",                  COMPILER_SIMPLIFY },
		{ "scc-transform",             COMPILER_SCC_TRANSFORM },
		{ "inline",                    COMPILER_INLINE },
		{ "always-inline",             COMPILER_ALWAYS_INLINE },
		{ "simplify-op",               COMPILER_SIMPLIFY_OP },
		{ "simplify-phi",              COMPILER_SIMPLIFY_PHI },
		{ "simplify-label",            COMPILER_SIMPLIFY_LABEL },
		{ "simplify-branch",           COMPILER_SIMPLIFY_BRANCH },
		{ "simplify-copy",             COMPILER_SIMPLIFY_COPY },
		{ "simplify-arith",            COMPILER_SIMPLIFY_ARITH },
		{ "simplify-shift",            COMPILER_SIMPLIFY_SHIFT },
		{ "simplify-bitwise",          COMPILER_SIMPLIFY_BITWISE },
		{ "simplify-logical",          COMPILER_SIMPLIFY_LOGICAL },
		{ 0, 0 },
	};
	static const struct compiler_flag opt_flags[] = {
		{ "-O",  COMPILER_SIMPLIFY },
		{ "-O2", COMPILER_SIMPLIFY | COMPILER_SCC_TRANSFORM },
		{ 0, 0, },
	};
	static const struct compiler_flag debug_flags[] = {
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
		{ 0, 0 },
	};
	int act;
	int result;

	act = 1;
	result = -1;
	if (strncmp(flag, "no-", 3) == 0) {
		flag += 3;
		act = 0;
	}
	if (strncmp(flag, "-O", 2) == 0) {
		result = set_flag(opt_flags, &compiler->flags, act, flag);
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
		result = set_flag(debug_flags, &compiler->debug, act, flag);
	}
	else {
		result = set_flag(flags, &compiler->flags, act, flag);
	}
	return result;
}

static void do_cleanup(struct compile_state *state)
{
	if (state->output) {
		fclose(state->output);
		unlink(state->compiler->ofilename);
	}
}

static int get_col(struct file_state *file)
{
	int col;
	char *ptr, *end;
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
	if (triple && triple->occurance) {
		struct occurance *spot;
		for(spot = triple->occurance; spot; spot = spot->parent) {
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

static void internal_error(struct compile_state *state, struct triple *ptr, 
	char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	loc(stderr, state, ptr);
	fputc('\n', stderr);
	if (ptr) {
		fprintf(stderr, "%p %s ", ptr, tops(ptr->op));
	}
	fprintf(stderr, "Internal compiler error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	do_cleanup(state);
	abort();
}


static void internal_warning(struct compile_state *state, struct triple *ptr, 
	char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	loc(stderr, state, ptr);
	if (ptr) {
		fprintf(stderr, "%p %s ", ptr, tops(ptr->op));
	}
	fprintf(stderr, "Internal compiler warning: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}



static void error(struct compile_state *state, struct triple *ptr, 
	char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	loc(stderr, state, ptr);
	fputc('\n', stderr);
	if (ptr && (state->compiler->debug & DEBUG_ABORT_ON_ERROR)) {
		fprintf(stderr, "%p %s ", ptr, tops(ptr->op));
	}
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	do_cleanup(state);
	if (state->compiler->debug & DEBUG_ABORT_ON_ERROR) {
		abort();
	}
	exit(1);
}

static void warning(struct compile_state *state, struct triple *ptr, 
	char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	loc(stderr, state, ptr);
	fprintf(stderr, "warning: "); 
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
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

static void process_trigraphs(struct compile_state *state)
{
	char *src, *dest, *end;
	struct file_state *file;
	file = state->file;
	src = dest = file->buf;
	end = file->buf + file->size;
	while((end - src) >= 3) {
		if ((src[0] == '?') && (src[1] == '?')) {
			int c = -1;
			switch(src[2]) {
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
			if (c != -1) {
				*dest++ = c;
				src += 3;
			}
			else {
				*dest++ = *src++;
			}
		}
		else {
			*dest++ = *src++;
		}
	}
	while(src != end) {
		*dest++ = *src++;
	}
	file->size = dest - file->buf;
}

static void splice_lines(struct compile_state *state)
{
	char *src, *dest, *end;
	struct file_state *file;
	file = state->file;
	src = dest = file->buf;
	end = file->buf + file->size;
	while((end - src) >= 2) {
		if ((src[0] == '\\') && (src[1] == '\n')) {
			src += 2;
		}
		else {
			*dest++ = *src++;
		}
	}
	while(src != end) {
		*dest++ = *src++;
	}
	file->size = dest - file->buf;
}

static struct type void_type;
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

static void put_occurance(struct occurance *occurance)
{
	if (occurance) {
		occurance->count -= 1;
		if (occurance->count <= 0) {
			if (occurance->parent) {
				put_occurance(occurance->parent);
			}
			xfree(occurance);
		}
	}
}

static void get_occurance(struct occurance *occurance)
{
	if (occurance) {
		occurance->count += 1;
	}
}


static struct occurance *new_occurance(struct compile_state *state)
{
	struct occurance *result, *last;
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
	last = state->last_occurance;
	if (last &&
		(last->col == col) &&
		(last->line == line) &&
		(last->function == function) &&
		((last->filename == filename) ||
			(strcmp(last->filename, filename) == 0))) 
	{
		get_occurance(last);
		return last;
	}
	if (last) {
		state->last_occurance = 0;
		put_occurance(last);
	}
	result = xmalloc(sizeof(*result), "occurance");
	result->count    = 2;
	result->filename = filename;
	result->function = function;
	result->line     = line;
	result->col      = col;
	result->parent   = 0;
	state->last_occurance = result;
	return result;
}

static struct occurance *inline_occurance(struct compile_state *state,
	struct occurance *base, struct occurance *top)
{
	struct occurance *result, *last;
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
	/* See if I can reuse the last occurance I had */
	last = state->last_occurance;
	if (last &&
		(last->parent   == base) &&
		(last->col      == top->col) &&
		(last->line     == top->line) &&
		(last->function == top->function) &&
		(last->filename == top->filename)) {
		get_occurance(last);
		return last;
	}
	/* I can't reuse the last occurance so free it */
	if (last) {
		state->last_occurance = 0;
		put_occurance(last);
	}
	/* Generate a new occurance structure */
	get_occurance(base);
	result = xmalloc(sizeof(*result), "occurance");
	result->count    = 2;
	result->filename = top->filename;
	result->function = top->function;
	result->line     = top->line;
	result->col      = top->col;
	result->parent   = base;
	state->last_occurance = result;
	return result;
}

static struct occurance dummy_occurance = {
	.count    = 2,
	.filename = __FILE__,
	.function = "",
	.line     = __LINE__,
	.col      = 0,
	.parent   = 0,
};

/* The zero triple is used as a place holder when we are removing pointers
 * from a triple.  Having allows certain sanity checks to pass even
 * when the original triple that was pointed to is gone.
 */
static struct triple zero_triple = {
	.next      = &zero_triple,
	.prev      = &zero_triple,
	.use       = 0,
	.op        = OP_INTCONST,
	.sizes     = TRIPLE_SIZES(0, 0, 0, 0),
	.id        = -1, /* An invalid id */
	.u = { .cval = 0, },
	.occurance = &dummy_occurance,
	.param = { [0] = 0, [1] = 0, },
};


static unsigned short triple_sizes(struct compile_state *state,
	int op, struct type *type, int lhs_wanted, int rhs_wanted,
	struct occurance *occurance)
{
	int lhs, rhs, misc, targ;
	struct triple dummy;
	dummy.op = op;
	dummy.occurance = occurance;
	valid_op(state, op);
	lhs = table_ops[op].lhs;
	rhs = table_ops[op].rhs;
	misc = table_ops[op].misc;
	targ = table_ops[op].targ;
	
	
	if (op == OP_FCALL) {
		rhs = rhs_wanted;
		lhs = 0;
		if ((type->type & TYPE_MASK) == TYPE_STRUCT) {
			lhs = type->elements;
		}
	}
	else if (op == OP_VAL_VEC) {
		rhs = type->elements;
	}
	else if (op == OP_PHI) {
		rhs = rhs_wanted;
	}
	else if (op == OP_ASM) {
		rhs = rhs_wanted;
		lhs = lhs_wanted;
	}
	if ((rhs < 0) || (rhs > MAX_RHS)) {
		internal_error(state, &dummy, "bad rhs %d", rhs);
	}
	if ((lhs < 0) || (lhs > MAX_LHS)) {
		internal_error(state, &dummy, "bad lhs");
	}
	if ((misc < 0) || (misc > MAX_MISC)) {
		internal_error(state, &dummy, "bad misc");
	}
	if ((targ < 0) || (targ > MAX_TARG)) {
		internal_error(state, &dummy, "bad targs");
	}
	return TRIPLE_SIZES(lhs, rhs, misc, targ);
}

static struct triple *alloc_triple(struct compile_state *state, 
	int op, struct type *type, int lhs, int rhs,
	struct occurance *occurance)
{
	size_t size, sizes, extra_count, min_count;
	struct triple *ret;
	sizes = triple_sizes(state, op, type, lhs, rhs, occurance);

	min_count = sizeof(ret->param)/sizeof(ret->param[0]);
	extra_count = TRIPLE_SIZE(sizes);
	extra_count = (extra_count < min_count)? 0 : extra_count - min_count;

	size = sizeof(*ret) + sizeof(ret->param[0]) * extra_count;
	ret = xcmalloc(size, "tripple");
	ret->op        = op;
	ret->sizes     = sizes;
	ret->type      = type;
	ret->next      = ret;
	ret->prev      = ret;
	ret->occurance = occurance;
	return ret;
}

struct triple *dup_triple(struct compile_state *state, struct triple *src)
{
	struct triple *dup;
	int src_lhs, src_rhs, src_size;
	src_lhs = TRIPLE_LHS(src->sizes);
	src_rhs = TRIPLE_RHS(src->sizes);
	src_size = TRIPLE_SIZE(src->sizes);
	get_occurance(src->occurance);
	dup = alloc_triple(state, src->op, src->type, src_lhs, src_rhs,
		src->occurance);
	memcpy(dup, src, sizeof(*src));
	memcpy(dup->param, src->param, src_size * sizeof(src->param[0]));
	return dup;
}

static struct triple *new_triple(struct compile_state *state, 
	int op, struct type *type, int lhs, int rhs)
{
	struct triple *ret;
	struct occurance *occurance;
	occurance = new_occurance(state);
	ret = alloc_triple(state, op, type, lhs, rhs, occurance);
	return ret;
}

static struct triple *build_triple(struct compile_state *state, 
	int op, struct type *type, struct triple *left, struct triple *right,
	struct occurance *occurance)
{
	struct triple *ret;
	size_t count;
	ret = alloc_triple(state, op, type, -1, -1, occurance);
	count = TRIPLE_SIZE(ret->sizes);
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
	count = TRIPLE_SIZE(ret->sizes);
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
		
		if ((ptr->prev->op == OP_CBRANCH) || (ptr->prev->op == OP_CALL)) {
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

static struct block *block_of_triple(struct compile_state *state, 
	struct triple *ins)
{
	struct triple *first;
	if (!ins || ins == &zero_triple) {
		return 0;
	}
	first = state->first;
	while(ins != first && !triple_stores_block(state, ins)) {
		if (ins == ins->prev) {
			internal_error(state, ins, "ins == ins->prev?");
		}
		ins = ins->prev;
	}
	if (!triple_stores_block(state, ins)) {
		internal_error(state, ins, "Cannot find block");
	}
	return ins->u.block;
}

static struct triple *pre_triple(struct compile_state *state,
	struct triple *base,
	int op, struct type *type, struct triple *left, struct triple *right)
{
	struct block *block;
	struct triple *ret;
	/* If I am an OP_PIECE jump to the real instruction */
	if (base->op == OP_PIECE) {
		base = MISC(base, 0);
	}
	block = block_of_triple(state, base);
	get_occurance(base->occurance);
	ret = build_triple(state, op, type, left, right, base->occurance);
	if (triple_stores_block(state, ret)) {
		ret->u.block = block;
	}
	insert_triple(state, base, ret);
	if (block->first == base) {
		block->first = ret;
	}
	return ret;
}

static struct triple *post_triple(struct compile_state *state,
	struct triple *base,
	int op, struct type *type, struct triple *left, struct triple *right)
{
	struct block *block;
	struct triple *ret;
	int zlhs;
	/* If I am an OP_PIECE jump to the real instruction */
	if (base->op == OP_PIECE) {
		base = MISC(base, 0);
	}
	/* If I have a left hand side skip over it */
	zlhs = TRIPLE_LHS(base->sizes);
	if (zlhs) {
		base = LHS(base, zlhs - 1);
	}

	block = block_of_triple(state, base);
	get_occurance(base->occurance);
	ret = build_triple(state, op, type, left, right, base->occurance);
	if (triple_stores_block(state, ret)) {
		ret->u.block = block;
	}
	insert_triple(state, base->next, ret);
	if (block->last == base) {
		block->last = ret;
	}
	return ret;
}

static struct triple *label(struct compile_state *state)
{
	/* Labels don't get a type */
	struct triple *result;
	result = triple(state, OP_LABEL, &void_type, 0, 0);
	return result;
}

static void display_triple(FILE *fp, struct triple *ins)
{
	struct occurance *ptr;
	const char *reg;
	char pre, post;
	pre = post = ' ';
	if (ins->id & TRIPLE_FLAG_PRE_SPLIT) {
		pre = '^';
	}
	if (ins->id & TRIPLE_FLAG_POST_SPLIT) {
		post = 'v';
	}
	reg = arch_reg_str(ID_REG(ins->id));
	if (ins->op == OP_INTCONST) {
		fprintf(fp, "(%p) %c%c %-7s %-2d %-10s <0x%08lx>         ",
			ins, pre, post, reg, ins->template_id, tops(ins->op), 
			(unsigned long)(ins->u.cval));
	}
	else if (ins->op == OP_ADDRCONST) {
		fprintf(fp, "(%p) %c%c %-7s %-2d %-10s %-10p <0x%08lx>",
			ins, pre, post, reg, ins->template_id, tops(ins->op), 
			MISC(ins, 0), (unsigned long)(ins->u.cval));
	}
	else {
		int i, count;
		fprintf(fp, "(%p) %c%c %-7s %-2d %-10s", 
			ins, pre, post, reg, ins->template_id, tops(ins->op));
		count = TRIPLE_SIZE(ins->sizes);
		for(i = 0; i < count; i++) {
			fprintf(fp, " %-10p", ins->param[i]);
		}
		for(; i < 2; i++) {
			fprintf(fp, "           ");
		}
	}
	fprintf(fp, " @");
	for(ptr = ins->occurance; ptr; ptr = ptr->parent) {
		fprintf(fp, " %s,%s:%d.%d",
			ptr->function, 
			ptr->filename,
			ptr->line, 
			ptr->col);
	}
	fprintf(fp, "\n");
#if 0
	{
		struct triple_set *user;
		for(user = ptr->use; user; user = user->next) {
			fprintf(fp, "use: %p\n", user->member);
		}
	}
#endif
	fflush(fp);
}

static void display_triple_changes(
	FILE *fp, const struct triple *new, const struct triple *orig)
{

	int new_count, orig_count;
	new_count = TRIPLE_SIZE(new->sizes);
	orig_count = TRIPLE_SIZE(orig->sizes);
	if ((new->op != orig->op) ||
		(new_count != orig_count) ||
		(memcmp(orig->param, new->param,	
			orig_count * sizeof(orig->param[0])) != 0) ||
		(memcmp(&orig->u, &new->u, sizeof(orig->u)) != 0)) 
	{
		struct occurance *ptr;
		int i, min_count, indent;
		fprintf(fp, "(%p)", orig);
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
		fprintf(fp, " @");
		for(ptr = orig->occurance; ptr; ptr = ptr->parent) {
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

static void display_func(FILE *fp, struct triple *func)
{
	struct triple *first, *ins;
	fprintf(fp, "display_func %s\n", func->type->type_ident->name);
	first = ins = RHS(func, 0);
	do {
		display_triple(fp, ins);
		ins = ins->next;
	} while(ins != first);
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
		internal_error(state, 0, "Purity of %s not known\n",
			tops(ins->op));
	}
	return (pure == PURE) && !(id & TRIPLE_FLAG_VOLATILE);
}

static int triple_is_branch(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a branch instruction? */
	valid_ins(state, ins);
	return (table_ops[ins->op].flags & BRANCH) != 0;
}

static int triple_is_cond_branch(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a conditional branch instruction? */
	valid_ins(state, ins);
	return (table_ops[ins->op].flags & CBRANCH) != 0;
}

static int triple_is_uncond_branch(struct compile_state *state, struct triple *ins)
{
	/* Is this triple a unconditional branch instruction? */
	valid_ins(state, ins);
	return (table_ops[ins->op].flags & CBRANCH) == 0;
}

static int triple_is_def(struct compile_state *state, struct triple *ins)
{
	/* This function is used to determine which triples need
	 * a register.
	 */
	int is_def;
	valid_ins(state, ins);
	is_def = (table_ops[ins->op].flags & DEF) == DEF;
	return is_def;
}

static int triple_is_structural(struct compile_state *state, struct triple *ins)
{
	int is_structural;
	valid_ins(state, ins);
	is_structural = (table_ops[ins->op].flags & STRUCTURAL) == STRUCTURAL;
	return is_structural;
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
	return triple_iter(state, TRIPLE_LHS(ins->sizes), &LHS(ins,0), 
		ins, last);
}

static struct triple **triple_rhs(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	return triple_iter(state, TRIPLE_RHS(ins->sizes), &RHS(ins,0), 
		ins, last);
}

static struct triple **triple_misc(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	return triple_iter(state, TRIPLE_MISC(ins->sizes), &MISC(ins,0), 
		ins, last);
}

static struct triple **triple_targ(struct compile_state *state,
	struct triple *ins, struct triple **last)
{
	size_t count;
	struct triple **ret, **vector;
	ret = 0;
	count = TRIPLE_TARG(ins->sizes);
	vector = &TARG(ins, 0);
	if (!ret && 
		((ins->op == OP_CALL) || (table_ops[ins->op].flags & CBRANCH))) {
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
	if (!ret && (ins->op == OP_RET)) {
		struct triple_set *use;
		for(use = ins->use; use; use = use->next) {
			if (use->member->op != OP_CALL) {
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

static void verify_use(struct compile_state *state,
	struct triple *user, struct triple *used)
{
	int size, i;
	size = TRIPLE_SIZE(user->sizes);
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
	size = TRIPLE_RHS(user->sizes);
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
		(sizeof(ptr->param[0])*TRIPLE_SIZE(ptr->sizes));
	ptr->prev->next = ptr->next;
	ptr->next->prev = ptr->prev;
	if (ptr->use) {
		internal_error(state, ptr, "ptr->use != 0");
	}
	put_occurance(ptr->occurance);
	memset(ptr, -1, size);
	xfree(ptr);
}

static void release_triple(struct compile_state *state, struct triple *ptr)
{
	struct triple_set *set, *next;
	struct triple **expr;
	struct block *block;
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
				*expr = &zero_triple;
			}
		}
		expr = triple_lhs(state, set->member, 0);
		for(; expr; expr = triple_lhs(state, set->member, expr)) {
			if (*expr == ptr) {
				*expr = &zero_triple;
			}
		}
		expr = triple_misc(state, set->member, 0);
		for(; expr; expr = triple_misc(state, set->member, expr)) {
			if (*expr == ptr) {
				*expr = &zero_triple;
			}
		}
		expr = triple_targ(state, set->member, 0);
		for(; expr; expr = triple_targ(state, set->member, expr)) {
			if (*expr == ptr) {
				*expr = &zero_triple;
			}
		}
		unuse_triple(ptr, set->member);
	}
	free_triple(state, ptr);
}

static void print_triples(struct compile_state *state);
static void print_blocks(struct compile_state *state, const char *func, FILE *fp);

#define TOK_UNKNOWN     0
#define TOK_SPACE       1
#define TOK_SEMI        2
#define TOK_LBRACE      3
#define TOK_RBRACE      4
#define TOK_COMMA       5
#define TOK_EQ          6
#define TOK_COLON       7
#define TOK_LBRACKET    8
#define TOK_RBRACKET    9
#define TOK_LPAREN      10
#define TOK_RPAREN      11
#define TOK_STAR        12
#define TOK_DOTS        13
#define TOK_MORE        14
#define TOK_LESS        15
#define TOK_TIMESEQ     16
#define TOK_DIVEQ       17
#define TOK_MODEQ       18
#define TOK_PLUSEQ      19
#define TOK_MINUSEQ     20
#define TOK_SLEQ        21
#define TOK_SREQ        22
#define TOK_ANDEQ       23
#define TOK_XOREQ       24
#define TOK_OREQ        25
#define TOK_EQEQ        26
#define TOK_NOTEQ       27
#define TOK_QUEST       28
#define TOK_LOGOR       29
#define TOK_LOGAND      30
#define TOK_OR          31
#define TOK_AND         32
#define TOK_XOR         33
#define TOK_LESSEQ      34
#define TOK_MOREEQ      35
#define TOK_SL          36
#define TOK_SR          37
#define TOK_PLUS        38
#define TOK_MINUS       39
#define TOK_DIV         40
#define TOK_MOD         41
#define TOK_PLUSPLUS    42
#define TOK_MINUSMINUS  43
#define TOK_BANG        44
#define TOK_ARROW       45
#define TOK_DOT         46
#define TOK_TILDE       47
#define TOK_LIT_STRING  48
#define TOK_LIT_CHAR    49
#define TOK_LIT_INT     50
#define TOK_LIT_FLOAT   51
#define TOK_MACRO       52
#define TOK_CONCATENATE 53

#define TOK_IDENT       54
#define TOK_STRUCT_NAME 55
#define TOK_ENUM_CONST  56
#define TOK_TYPE_NAME   57

#define TOK_AUTO        58
#define TOK_BREAK       59
#define TOK_CASE        60
#define TOK_CHAR        61
#define TOK_CONST       62
#define TOK_CONTINUE    63
#define TOK_DEFAULT     64
#define TOK_DO          65
#define TOK_DOUBLE      66
#define TOK_ELSE        67
#define TOK_ENUM        68
#define TOK_EXTERN      69
#define TOK_FLOAT       70
#define TOK_FOR         71
#define TOK_GOTO        72
#define TOK_IF          73
#define TOK_INLINE      74
#define TOK_INT         75
#define TOK_LONG        76
#define TOK_REGISTER    77
#define TOK_RESTRICT    78
#define TOK_RETURN      79
#define TOK_SHORT       80
#define TOK_SIGNED      81
#define TOK_SIZEOF      82
#define TOK_STATIC      83
#define TOK_STRUCT      84
#define TOK_SWITCH      85
#define TOK_TYPEDEF     86
#define TOK_UNION       87
#define TOK_UNSIGNED    88
#define TOK_VOID        89
#define TOK_VOLATILE    90
#define TOK_WHILE       91
#define TOK_ASM         92
#define TOK_ATTRIBUTE   93
#define TOK_ALIGNOF     94
#define TOK_FIRST_KEYWORD TOK_AUTO
#define TOK_LAST_KEYWORD  TOK_ALIGNOF

#define TOK_DEFINE      100
#define TOK_UNDEF       101
#define TOK_INCLUDE     102
#define TOK_LINE        103
#define TOK_ERROR       104
#define TOK_WARNING     105
#define TOK_PRAGMA      106
#define TOK_IFDEF       107
#define TOK_IFNDEF      108
#define TOK_ELIF        109
#define TOK_ENDIF       110

#define TOK_FIRST_MACRO TOK_DEFINE
#define TOK_LAST_MACRO  TOK_ENDIF
         
#define TOK_EOF         111

static const char *tokens[] = {
[TOK_UNKNOWN     ] = "unknown",
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

[TOK_DEFINE      ] = "define",
[TOK_UNDEF       ] = "undef",
[TOK_INCLUDE     ] = "include",
[TOK_LINE        ] = "line",
[TOK_ERROR       ] = "error",
[TOK_WARNING     ] = "warning",
[TOK_PRAGMA      ] = "pragma",
[TOK_IFDEF       ] = "ifdef",
[TOK_IFNDEF      ] = "ifndef",
[TOK_ELIF        ] = "elif",
[TOK_ENDIF       ] = "endif",

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
	if (entry && 
		(entry->tok >= TOK_FIRST_MACRO) &&
		(entry->tok <= TOK_LAST_MACRO)) {
		tk->tok = entry->tok;
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

static void symbol(
	struct compile_state *state, struct hash_entry *ident,
	struct symbol **chain, struct triple *def, struct type *type)
{
	struct symbol *sym;
	if (*chain && ((*chain)->scope_depth == state->scope_depth)) {
		error(state, 0, "%s already defined", ident->name);
	}
	sym = xcmalloc(sizeof(*sym), "symbol");
	sym->ident = ident;
	sym->def   = def;
	sym->type  = type;
	sym->scope_depth = state->scope_depth;
	sym->next = *chain;
	*chain    = sym;
}

static void label_symbol(struct compile_state *state, 
	struct hash_entry *ident, struct triple *label)
{
	struct symbol *sym;
	if (ident->sym_label) {
		error(state, 0, "label %s already defined", ident->name);
	}
	sym = xcmalloc(sizeof(*sym), "label");
	sym->ident = ident;
	sym->def   = label;
	sym->type  = &void_type;
	sym->scope_depth = FUNCTION_SCOPE_DEPTH;
	sym->next  = 0;
	ident->sym_label = sym;
}

static void start_scope(struct compile_state *state)
{
	state->scope_depth++;
}

static void end_scope_syms(struct symbol **chain, int depth)
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
			end_scope_syms(&entry->sym_label, depth);
			end_scope_syms(&entry->sym_tag,   depth);
			end_scope_syms(&entry->sym_ident, depth);
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
	hash_keyword(state, "define",        TOK_DEFINE);
	hash_keyword(state, "undef",         TOK_UNDEF);
	hash_keyword(state, "include",       TOK_INCLUDE);
	hash_keyword(state, "line",          TOK_LINE);
	hash_keyword(state, "error",         TOK_ERROR);
	hash_keyword(state, "warning",       TOK_WARNING);
	hash_keyword(state, "pragma",        TOK_PRAGMA);
	hash_keyword(state, "ifdef",         TOK_IFDEF);
	hash_keyword(state, "ifndef",        TOK_IFNDEF);
	hash_keyword(state, "elif",          TOK_ELIF);
	hash_keyword(state, "endif",         TOK_ENDIF);
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
	case '\n':
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
		case '"':  c = '"';  break;
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

static char *after_digits(char *ptr, char *end)
{
	while((ptr < end) && digitp(*ptr)) {
		ptr++;
	}
	return ptr;
}

static char *after_octdigits(char *ptr, char *end)
{
	while((ptr < end) && octdigitp(*ptr)) {
		ptr++;
	}
	return ptr;
}

static char *after_hexdigits(char *ptr, char *end)
{
	while((ptr < end) && hexdigitp(*ptr)) {
		ptr++;
	}
	return ptr;
}

static void save_string(struct compile_state *state, 
	struct token *tk, char *start, char *end, const char *id)
{
	char *str;
	int str_len;
	/* Create a private copy of the string */
	str_len = end - start + 1;
	str = xmalloc(str_len + 1, id);
	memcpy(str, start, str_len);
	str[str_len] = '\0';

	/* Store the copy in the token */
	tk->val.str = str;
	tk->str_len = str_len;
}
static void next_token(struct compile_state *state, int index)
{
	struct file_state *file;
	struct token *tk;
	char *token;
	int c, c1, c2, c3;
	char *tokp, *end;
	int tok;
next_token:
	file = state->file;
	tk = &state->token[index];
	tk->str_len = 0;
	tk->ident = 0;
	token = tokp = file->pos;
	end = file->buf + file->size;
	tok = TOK_UNKNOWN;
	c = -1;
	if (tokp < end) {
		c = *tokp;
	}
	c1 = -1;
	if ((tokp + 1) < end) {
		c1 = tokp[1];
	}
	c2 = -1;
	if ((tokp + 2) < end) {
		c2 = tokp[2];
	}
	c3 = -1;
	if ((tokp + 3) < end) {
		c3 = tokp[3];
	}
	if (tokp >= end) {
		tok = TOK_EOF;
		tokp = end;
	}
	/* Whitespace */
	else if (spacep(c)) {
		tok = TOK_SPACE;
		while ((tokp < end) && spacep(c)) {
			if (c == '\n') {
				file->line++;
				file->report_line++;
				file->line_start = tokp + 1;
			}
			c = *(++tokp);
		}
		if (!spacep(c)) {
			tokp--;
		}
	}
	/* EOL Comments */
	else if ((c == '/') && (c1 == '/')) {
		tok = TOK_SPACE;
		for(tokp += 2; tokp < end; tokp++) {
			c = *tokp;
			if (c == '\n') {
				file->line++;
				file->report_line++;
				file->line_start = tokp +1;
				break;
			}
		}
	}
	/* Comments */
	else if ((c == '/') && (c1 == '*')) {
		int line;
		char *line_start;
		line = file->line;
		line_start = file->line_start;
		for(tokp += 2; (end - tokp) >= 2; tokp++) {
			c = *tokp;
			if (c == '\n') {
				line++;
				line_start = tokp +1;
			}
			else if ((c == '*') && (tokp[1] == '/')) {
				tok = TOK_SPACE;
				tokp += 1;
				break;
			}
		}
		if (tok == TOK_UNKNOWN) {
			error(state, 0, "unterminated comment");
		}
		file->report_line += line - file->line;
		file->line = line;
		file->line_start = line_start;
	}
	/* string constants */
	else if ((c == '"') ||
		((c == 'L') && (c1 == '"'))) {
		int line;
		char *line_start;
		int wchar;
		line = file->line;
		line_start = file->line_start;
		wchar = 0;
		if (c == 'L') {
			wchar = 1;
			tokp++;
		}
		for(tokp += 1; tokp < end; tokp++) {
			c = *tokp;
			if (c == '\n') {
				line++;
				line_start = tokp + 1;
			}
			else if ((c == '\\') && (tokp +1 < end)) {
				tokp++;
			}
			else if (c == '"') {
				tok = TOK_LIT_STRING;
				break;
			}
		}
		if (tok == TOK_UNKNOWN) {
			error(state, 0, "unterminated string constant");
		}
		if (line != file->line) {
			warning(state, 0, "multiline string constant");
		}
		file->report_line += line - file->line;
		file->line = line;
		file->line_start = line_start;

		/* Save the string value */
		save_string(state, tk, token, tokp, "literal string");
	}
	/* character constants */
	else if ((c == '\'') ||
		((c == 'L') && (c1 == '\''))) {
		int line;
		char *line_start;
		int wchar;
		line = file->line;
		line_start = file->line_start;
		wchar = 0;
		if (c == 'L') {
			wchar = 1;
			tokp++;
		}
		for(tokp += 1; tokp < end; tokp++) {
			c = *tokp;
			if (c == '\n') {
				line++;
				line_start = tokp + 1;
			}
			else if ((c == '\\') && (tokp +1 < end)) {
				tokp++;
			}
			else if (c == '\'') {
				tok = TOK_LIT_CHAR;
				break;
			}
		}
		if (tok == TOK_UNKNOWN) {
			error(state, 0, "unterminated character constant");
		}
		if (line != file->line) {
			warning(state, 0, "multiline character constant");
		}
		file->report_line += line - file->line;
		file->line = line;
		file->line_start = line_start;

		/* Save the character value */
		save_string(state, tk, token, tokp, "literal character");
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
		char *next, *new;
		int is_float;
		is_float = 0;
		if (c != '.') {
			next = after_digits(tokp, end);
		}
		else {
			next = tokp;
		}
		if (next[0] == '.') {
			new = after_digits(next, end);
			is_float = (new != next);
			next = new;
		}
		if ((next[0] == 'e') || (next[0] == 'E')) {
			if (((next + 1) < end) && 
				((next[1] == '+') || (next[1] == '-'))) {
				next++;
			}
			new = after_digits(next, end);
			is_float = (new != next);
			next = new;
		}
		if (is_float) {
			tok = TOK_LIT_FLOAT;
			if ((next < end) && (
				(next[0] == 'f') ||
				(next[0] == 'F') ||
				(next[0] == 'l') ||
				(next[0] == 'L'))
				) {
				next++;
			}
		}
		if (!is_float && digitp(c)) {
			tok = TOK_LIT_INT;
			if ((c == '0') && ((c1 == 'x') || (c1 == 'X'))) {
				next = after_hexdigits(tokp + 2, end);
			}
			else if (c == '0') {
				next = after_octdigits(tokp, end);
			}
			else {
				next = after_digits(tokp, end);
			}
			/* crazy integer suffixes */
			if ((next < end) && 
				((next[0] == 'u') || (next[0] == 'U'))) { 
				next++;
				if ((next < end) &&
					((next[0] == 'l') || (next[0] == 'L'))) {
					next++;
				}
			}
			else if ((next < end) &&
				((next[0] == 'l') || (next[0] == 'L'))) {
				next++;
				if ((next < end) && 
					((next[0] == 'u') || (next[0] == 'U'))) { 
					next++;
				}
			}
		}
		tokp = next - 1;

		/* Save the integer/floating point value */
		save_string(state, tk, token, tokp, "literal number");
	}
	/* identifiers */
	else if (letterp(c)) {
		tok = TOK_IDENT;
		for(tokp += 1; tokp < end; tokp++) {
			c = *tokp;
			if (!letterp(c) && !digitp(c)) {
				break;
			}
		}
		tokp -= 1;
		tk->ident = lookup(state, token, tokp +1 - token);
	}
	/* C99 alternate macro characters */
	else if ((c == '%') && (c1 == ':') && (c2 == '%') && (c3 == ':')) { 
		tokp += 3; 
		tok = TOK_CONCATENATE; 
	}
	else if ((c == '.') && (c1 == '.') && (c2 == '.')) { tokp += 2; tok = TOK_DOTS; }
	else if ((c == '<') && (c1 == '<') && (c2 == '=')) { tokp += 2; tok = TOK_SLEQ; }
	else if ((c == '>') && (c1 == '>') && (c2 == '=')) { tokp += 2; tok = TOK_SREQ; }
	else if ((c == '*') && (c1 == '=')) { tokp += 1; tok = TOK_TIMESEQ; }
	else if ((c == '/') && (c1 == '=')) { tokp += 1; tok = TOK_DIVEQ; }
	else if ((c == '%') && (c1 == '=')) { tokp += 1; tok = TOK_MODEQ; }
	else if ((c == '+') && (c1 == '=')) { tokp += 1; tok = TOK_PLUSEQ; }
	else if ((c == '-') && (c1 == '=')) { tokp += 1; tok = TOK_MINUSEQ; }
	else if ((c == '&') && (c1 == '=')) { tokp += 1; tok = TOK_ANDEQ; }
	else if ((c == '^') && (c1 == '=')) { tokp += 1; tok = TOK_XOREQ; }
	else if ((c == '|') && (c1 == '=')) { tokp += 1; tok = TOK_OREQ; }
	else if ((c == '=') && (c1 == '=')) { tokp += 1; tok = TOK_EQEQ; }
	else if ((c == '!') && (c1 == '=')) { tokp += 1; tok = TOK_NOTEQ; }
	else if ((c == '|') && (c1 == '|')) { tokp += 1; tok = TOK_LOGOR; }
	else if ((c == '&') && (c1 == '&')) { tokp += 1; tok = TOK_LOGAND; }
	else if ((c == '<') && (c1 == '=')) { tokp += 1; tok = TOK_LESSEQ; }
	else if ((c == '>') && (c1 == '=')) { tokp += 1; tok = TOK_MOREEQ; }
	else if ((c == '<') && (c1 == '<')) { tokp += 1; tok = TOK_SL; }
	else if ((c == '>') && (c1 == '>')) { tokp += 1; tok = TOK_SR; }
	else if ((c == '+') && (c1 == '+')) { tokp += 1; tok = TOK_PLUSPLUS; }
	else if ((c == '-') && (c1 == '-')) { tokp += 1; tok = TOK_MINUSMINUS; }
	else if ((c == '-') && (c1 == '>')) { tokp += 1; tok = TOK_ARROW; }
	else if ((c == '<') && (c1 == ':')) { tokp += 1; tok = TOK_LBRACKET; }
	else if ((c == ':') && (c1 == '>')) { tokp += 1; tok = TOK_RBRACKET; }
	else if ((c == '<') && (c1 == '%')) { tokp += 1; tok = TOK_LBRACE; }
	else if ((c == '%') && (c1 == '>')) { tokp += 1; tok = TOK_RBRACE; }
	else if ((c == '%') && (c1 == ':')) { tokp += 1; tok = TOK_MACRO; }
	else if ((c == '#') && (c1 == '#')) { tokp += 1; tok = TOK_CONCATENATE; }
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
	if (tok == TOK_MACRO) {
		/* Only match preprocessor directives at the start of a line */
		char *ptr;
		for(ptr = file->line_start; spacep(*ptr); ptr++)
			;
		if (ptr != tokp) {
			tok = TOK_UNKNOWN;
		}
	}
	if (tok == TOK_UNKNOWN) {
		error(state, 0, "unknown token");
	}

	file->pos = tokp + 1;
	tk->tok = tok;
	if (tok == TOK_IDENT) {
		ident_to_keyword(state, tk);
	}
	/* Don't return space tokens. */
	if (tok == TOK_SPACE) {
		goto next_token;
	}
}

static void compile_macro(struct compile_state *state, struct token *tk)
{
	struct file_state *file;
	struct hash_entry *ident;
	ident = tk->ident;
	file = xmalloc(sizeof(*file), "file_state");
	file->basename = xstrdup(tk->ident->name);
	file->dirname = xstrdup("");
	file->size = ident->sym_define->buf_len;
	file->buf = xmalloc(file->size +2,  file->basename);
	memcpy(file->buf, ident->sym_define->buf, file->size);
	file->buf[file->size] = '\n';
	file->buf[file->size + 1] = '\0';
	file->pos = file->buf;
	file->line_start = file->pos;
	file->line = 1;
	file->report_line = 1;
	file->report_name = file->basename;
	file->report_dir  = file->dirname;
	file->prev = state->file;
	state->file = file;
}


static int mpeek(struct compile_state *state, int index)
{
	struct token *tk;
	int rescan;
	tk = &state->token[index + 1];
	if (tk->tok == -1) {
		next_token(state, index + 1);
	}
	do {
		rescan = 0;
		if ((tk->tok == TOK_EOF) && 
			(state->file != state->macro_file) &&
			(state->file->prev)) {
			struct file_state *file = state->file;
			state->file = file->prev;
			/* file->basename is used keep it */
			if (file->report_dir != file->dirname) {
				xfree(file->report_dir);
			}
			xfree(file->dirname);
			xfree(file->buf);
			xfree(file);
			next_token(state, index + 1);
			rescan = 1;
		}
		else if (tk->ident && tk->ident->sym_define) {
			compile_macro(state, tk);
			next_token(state, index + 1);
			rescan = 1;
		}
	} while(rescan);
	/* Don't show the token on the next line */
	if (state->macro_line < state->macro_file->line) {
		return TOK_EOF;
	}
	return state->token[index +1].tok;
}

static void meat(struct compile_state *state, int index, int tok)
{
	int next_tok;
	int i;
	next_tok = mpeek(state, index);
	if (next_tok != tok) {
		const char *name1, *name2;
		name1 = tokens[next_tok];
		name2 = "";
		if (next_tok == TOK_IDENT) {
			name2 = state->token[index + 1].ident->name;
		}
		error(state, 0, "found %s %s expected %s", 
			name1, name2, tokens[tok]);
	}
	/* Free the old token value */
	if (state->token[index].str_len) {
		memset((void *)(state->token[index].val.str), -1, 
			state->token[index].str_len);
		xfree(state->token[index].val.str);
	}
	for(i = index; i < sizeof(state->token)/sizeof(state->token[0]) - 1; i++) {
		state->token[i] = state->token[i + 1];
	}
	memset(&state->token[i], 0, sizeof(state->token[i]));
	state->token[i].tok = -1;
}

static long_t mcexpr(struct compile_state *state, int index);

static long_t mprimary_expr(struct compile_state *state, int index)
{
	long_t val;
	int tok;
	tok = mpeek(state, index);
	while(state->token[index + 1].ident && 
		state->token[index + 1].ident->sym_define) {
		meat(state, index, tok);
		compile_macro(state, &state->token[index]);
		tok = mpeek(state, index);
	}
	switch(tok) {
	case TOK_LPAREN:
		meat(state, index, TOK_LPAREN);
		val = mcexpr(state, index);
		meat(state, index, TOK_RPAREN);
		break;
	case TOK_LIT_INT:
	{
		long lval;
		char *end;
		meat(state, index, TOK_LIT_INT);
		errno = 0;
		lval = strtol(state->token[index].val.str, &end, 0);
		if ((lval > LONG_T_MAX) || (lval < LONG_T_MIN) ||
			(((lval == LONG_MIN) || (lval == LONG_MAX)) &&
				(errno == ERANGE))) {
			error(state, 0, "Integer constant to large");
		}
		val = lval;
		break;
	}
	default:
		meat(state, index, TOK_LIT_INT);
		val = 0;
	}
	return val;
}
static long_t munary_expr(struct compile_state *state, int index)
{
	long_t val;
	switch(mpeek(state, index)) {
	case TOK_PLUS:
		meat(state, index, TOK_PLUS);
		val = munary_expr(state, index);
		val = + val;
		break;
	case TOK_MINUS:
		meat(state, index, TOK_MINUS);
		val = munary_expr(state, index);
		val = - val;
		break;
	case TOK_TILDE:
		meat(state, index, TOK_BANG);
		val = munary_expr(state, index);
		val = ~ val;
		break;
	case TOK_BANG:
		meat(state, index, TOK_BANG);
		val = munary_expr(state, index);
		val = ! val;
		break;
	default:
		val = mprimary_expr(state, index);
		break;
	}
	return val;
	
}
static long_t mmul_expr(struct compile_state *state, int index)
{
	long_t val;
	int done;
	val = munary_expr(state, index);
	do {
		long_t right;
		done = 0;
		switch(mpeek(state, index)) {
		case TOK_STAR:
			meat(state, index, TOK_STAR);
			right = munary_expr(state, index);
			val = val * right;
			break;
		case TOK_DIV:
			meat(state, index, TOK_DIV);
			right = munary_expr(state, index);
			val = val / right;
			break;
		case TOK_MOD:
			meat(state, index, TOK_MOD);
			right = munary_expr(state, index);
			val = val % right;
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);

	return val;
}

static long_t madd_expr(struct compile_state *state, int index)
{
	long_t val;
	int done;
	val = mmul_expr(state, index);
	do {
		long_t right;
		done = 0;
		switch(mpeek(state, index)) {
		case TOK_PLUS:
			meat(state, index, TOK_PLUS);
			right = mmul_expr(state, index);
			val = val + right;
			break;
		case TOK_MINUS:
			meat(state, index, TOK_MINUS);
			right = mmul_expr(state, index);
			val = val - right;
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);

	return val;
}

static long_t mshift_expr(struct compile_state *state, int index)
{
	long_t val;
	int done;
	val = madd_expr(state, index);
	do {
		long_t right;
		done = 0;
		switch(mpeek(state, index)) {
		case TOK_SL:
			meat(state, index, TOK_SL);
			right = madd_expr(state, index);
			val = val << right;
			break;
		case TOK_SR:
			meat(state, index, TOK_SR);
			right = madd_expr(state, index);
			val = val >> right;
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);

	return val;
}

static long_t mrel_expr(struct compile_state *state, int index)
{
	long_t val;
	int done;
	val = mshift_expr(state, index);
	do {
		long_t right;
		done = 0;
		switch(mpeek(state, index)) {
		case TOK_LESS:
			meat(state, index, TOK_LESS);
			right = mshift_expr(state, index);
			val = val < right;
			break;
		case TOK_MORE:
			meat(state, index, TOK_MORE);
			right = mshift_expr(state, index);
			val = val > right;
			break;
		case TOK_LESSEQ:
			meat(state, index, TOK_LESSEQ);
			right = mshift_expr(state, index);
			val = val <= right;
			break;
		case TOK_MOREEQ:
			meat(state, index, TOK_MOREEQ);
			right = mshift_expr(state, index);
			val = val >= right;
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return val;
}

static long_t meq_expr(struct compile_state *state, int index)
{
	long_t val;
	int done;
	val = mrel_expr(state, index);
	do {
		long_t right;
		done = 0;
		switch(mpeek(state, index)) {
		case TOK_EQEQ:
			meat(state, index, TOK_EQEQ);
			right = mrel_expr(state, index);
			val = val == right;
			break;
		case TOK_NOTEQ:
			meat(state, index, TOK_NOTEQ);
			right = mrel_expr(state, index);
			val = val != right;
			break;
		default:
			done = 1;
			break;
		}
	} while(!done);
	return val;
}

static long_t mand_expr(struct compile_state *state, int index)
{
	long_t val;
	val = meq_expr(state, index);
	if (mpeek(state, index) == TOK_AND) {
		long_t right;
		meat(state, index, TOK_AND);
		right = meq_expr(state, index);
		val = val & right;
	}
	return val;
}

static long_t mxor_expr(struct compile_state *state, int index)
{
	long_t val;
	val = mand_expr(state, index);
	if (mpeek(state, index) == TOK_XOR) {
		long_t right;
		meat(state, index, TOK_XOR);
		right = mand_expr(state, index);
		val = val ^ right;
	}
	return val;
}

static long_t mor_expr(struct compile_state *state, int index)
{
	long_t val;
	val = mxor_expr(state, index);
	if (mpeek(state, index) == TOK_OR) {
		long_t right;
		meat(state, index, TOK_OR);
		right = mxor_expr(state, index);
		val = val | right;
	}
	return val;
}

static long_t mland_expr(struct compile_state *state, int index)
{
	long_t val;
	val = mor_expr(state, index);
	if (mpeek(state, index) == TOK_LOGAND) {
		long_t right;
		meat(state, index, TOK_LOGAND);
		right = mor_expr(state, index);
		val = val && right;
	}
	return val;
}
static long_t mlor_expr(struct compile_state *state, int index)
{
	long_t val;
	val = mland_expr(state, index);
	if (mpeek(state, index) == TOK_LOGOR) {
		long_t right;
		meat(state, index, TOK_LOGOR);
		right = mland_expr(state, index);
		val = val || right;
	}
	return val;
}

static long_t mcexpr(struct compile_state *state, int index)
{
	return mlor_expr(state, index);
}
static void preprocess(struct compile_state *state, int index)
{
	/* Doing much more with the preprocessor would require
	 * a parser and a major restructuring.
	 * Postpone that for later.
	 */
	struct file_state *file;
	struct token *tk;
	int line;
	int tok;
	
	file = state->file;
	tk = &state->token[index];
	state->macro_line = line = file->line;
	state->macro_file = file;

	next_token(state, index);
	ident_to_macro(state, tk);
	if (tk->tok == TOK_IDENT) {
		error(state, 0, "undefined preprocessing directive `%s'",
			tk->ident->name);
	}
	switch(tk->tok) {
	case TOK_LIT_INT:
	{
		int override_line;
		override_line = strtoul(tk->val.str, 0, 10);
		next_token(state, index);
		/* I have a cpp line marker parse it */
		if (tk->tok == TOK_LIT_STRING) {
			const char *token, *base;
			char *name, *dir;
			int name_len, dir_len;
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
			file->report_line = override_line - 1;
			file->report_name = name;
			file->report_dir = dir;
		}
	}
		break;
	case TOK_LINE:
		meat(state, index, TOK_LINE);
		meat(state, index, TOK_LIT_INT);
		file->report_line = strtoul(tk->val.str, 0, 10) -1;
		if (mpeek(state, index) == TOK_LIT_STRING) {
			const char *token, *base;
			char *name, *dir;
			int name_len, dir_len;
			meat(state, index, TOK_LIT_STRING);
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
			file->report_name = name;
			file->report_dir = dir;
		}
		break;
	case TOK_UNDEF:
	case TOK_PRAGMA:
		if (state->if_value < 0) {
			break;
		}
		warning(state, 0, "Ignoring preprocessor directive: %s", 
			tk->ident->name);
		break;
	case TOK_ELIF:
		error(state, 0, "#elif not supported");
#warning "FIXME multiple #elif and #else in an #if do not work properly"
		if (state->if_depth == 0) {
			error(state, 0, "#elif without #if");
		}
		/* If the #if was taken the #elif just disables the following code */
		if (state->if_value >= 0) {
			state->if_value = - state->if_value;
		}
		/* If the previous #if was not taken see if the #elif enables the 
		 * trailing code.
		 */
		else if ((state->if_value < 0) && 
			(state->if_depth == - state->if_value))
		{
			if (mcexpr(state, index) != 0) {
				state->if_value = state->if_depth;
			}
			else {
				state->if_value = - state->if_depth;
			}
		}
		break;
	case TOK_IF:
		state->if_depth++;
		if (state->if_value < 0) {
			break;
		}
		if (mcexpr(state, index) != 0) {
			state->if_value = state->if_depth;
		}
		else {
			state->if_value = - state->if_depth;
		}
		break;
	case TOK_IFNDEF:
		state->if_depth++;
		if (state->if_value < 0) {
			break;
		}
		next_token(state, index);
		if ((line != file->line) || (tk->tok != TOK_IDENT)) {
			error(state, 0, "Invalid macro name");
		}
		if (tk->ident->sym_define == 0) {
			state->if_value = state->if_depth;
		} 
		else {
			state->if_value = - state->if_depth;
		}
		break;
	case TOK_IFDEF:
		state->if_depth++;
		if (state->if_value < 0) {
			break;
		}
		next_token(state, index);
		if ((line != file->line) || (tk->tok != TOK_IDENT)) {
			error(state, 0, "Invalid macro name");
		}
		if (tk->ident->sym_define != 0) {
			state->if_value = state->if_depth;
		}
		else {
			state->if_value = - state->if_depth;
		}
		break;
	case TOK_ELSE:
		if (state->if_depth == 0) {
			error(state, 0, "#else without #if");
		}
		if ((state->if_value >= 0) ||
			((state->if_value < 0) && 
				(state->if_depth == -state->if_value)))
		{
			state->if_value = - state->if_value;
		}
		break;
	case TOK_ENDIF:
		if (state->if_depth == 0) {
			error(state, 0, "#endif without #if");
		}
		if ((state->if_value >= 0) ||
			((state->if_value < 0) &&
				(state->if_depth == -state->if_value))) 
		{
			state->if_value = state->if_depth - 1;
		}
		state->if_depth--;
		break;
	case TOK_DEFINE:
	{
		struct hash_entry *ident;
		struct macro *macro;
		char *ptr;
		
		if (state->if_value < 0) /* quit early when #if'd out */
			break;

		meat(state, index, TOK_IDENT);
		ident = tk->ident;
		

		if (*file->pos == '(') {
#warning "FIXME macros with arguments not supported"
			error(state, 0, "Macros with arguments not supported");
		}

		/* Find the end of the line to get an estimate of
		 * the macro's length.
		 */
		for(ptr = file->pos; *ptr != '\n'; ptr++)  
			;

		if (ident->sym_define != 0) {
			error(state, 0, "macro %s already defined\n", ident->name);
		}
		macro = xmalloc(sizeof(*macro), "macro");
		macro->ident = ident;
		macro->buf_len = ptr - file->pos +1;
		macro->buf = xmalloc(macro->buf_len +2, "macro buf");

		memcpy(macro->buf, file->pos, macro->buf_len);
		macro->buf[macro->buf_len] = '\n';
		macro->buf[macro->buf_len +1] = '\0';

		ident->sym_define = macro;
		break;
	}
	case TOK_ERROR:
	{
		char *end;
		int len;
		/* Find the end of the line */
		for(end = file->pos; *end != '\n'; end++)
			;
		len = (end - file->pos);
		if (state->if_value >= 0) {
			error(state, 0, "%*.*s", len, len, file->pos);
		}
		file->pos = end;
		break;
	}
	case TOK_WARNING:
	{
		char *end;
		int len;
		/* Find the end of the line */
		for(end = file->pos; *end != '\n'; end++)
			;
		len = (end - file->pos);
		if (state->if_value >= 0) {
			warning(state, 0, "%*.*s", len, len, file->pos);
		}
		file->pos = end;
		break;
	}
	case TOK_INCLUDE:
	{
		char *name;
		char *ptr;
		int local;
		local = 0;
		name = 0;
		next_token(state, index);
		if (tk->tok == TOK_LIT_STRING) {
			const char *token;
			int name_len;
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
		else if (tk->tok == TOK_LESS) {
			char *start, *end;
			start = file->pos;
			for(end = start; *end != '\n'; end++) {
				if (*end == '>') {
					break;
				}
			}
			if (*end == '\n') {
				error(state, 0, "Unterminated included directive");
			}
			name = xmalloc(end - start + 1, "include");
			memcpy(name, start, end - start);
			name[end - start] = '\0';
			file->pos = end +1;
			local = 0;
		}
		else {
			error(state, 0, "Invalid include directive");
		}
		/* Error if there are any characters after the include */
		for(ptr = file->pos; *ptr != '\n'; ptr++) {
			switch(*ptr) {
			case ' ':
			case '\t':
			case '\v':
				break;
			default:
				error(state, 0, "garbage after include directive");
			}
		}
		if (state->if_value >= 0) {
			compile_file(state, name, local);
		}
		xfree(name);
		next_token(state, index);
		return;
	}
	default:
		/* Ignore # without a following ident */
		if (tk->tok == TOK_IDENT) {
			error(state, 0, "Invalid preprocessor directive: %s", 
				tk->ident->name);
		}
		break;
	}
	/* Consume the rest of the macro line */
	do {
		tok = mpeek(state, index);
		meat(state, index, tok);
	} while(tok != TOK_EOF);
	return;
}

static void token(struct compile_state *state, int index)
{
	struct file_state *file;
	struct token *tk;
	int rescan;

	tk = &state->token[index];
	next_token(state, index);
	do {
		rescan = 0;
		file = state->file;
		if (tk->tok == TOK_EOF && file->prev) {
			state->file = file->prev;
			/* file->basename is used keep it */
			xfree(file->dirname);
			xfree(file->buf);
			xfree(file);
			next_token(state, index);
			rescan = 1;
		}
		else if (tk->tok == TOK_MACRO) {
			preprocess(state, index);
			rescan = 1;
		}
		else if (tk->ident && tk->ident->sym_define) {
			compile_macro(state, tk);
			next_token(state, index);
			rescan = 1;
		}
		else if (state->if_value < 0) {
			next_token(state, index);
			rescan = 1;
		}
	} while(rescan);
}

static int peek(struct compile_state *state)
{
	if (state->token[1].tok == -1) {
		token(state, 1);
	}
	return state->token[1].tok;
}

static int peek2(struct compile_state *state)
{
	if (state->token[1].tok == -1) {
		token(state, 1);
	}
	if (state->token[2].tok == -1) {
		token(state, 2);
	}
	return state->token[2].tok;
}

static void eat(struct compile_state *state, int tok)
{
	int next_tok;
	int i;
	next_tok = peek(state);
	if (next_tok != tok) {
		const char *name1, *name2;
		name1 = tokens[next_tok];
		name2 = "";
		if (next_tok == TOK_IDENT) {
			name2 = state->token[1].ident->name;
		}
		error(state, 0, "\tfound %s %s expected %s",
			name1, name2 ,tokens[tok]);
	}
	/* Free the old token value */
	if (state->token[0].str_len) {
		xfree((void *)(state->token[0].val.str));
	}
	for(i = 0; i < sizeof(state->token)/sizeof(state->token[0]) - 1; i++) {
		state->token[i] = state->token[i + 1];
	}
	memset(&state->token[i], 0, sizeof(state->token[i]));
	state->token[i].tok = -1;
}

#warning "FIXME do not hardcode the include paths"
static char *include_paths[] = {
	"/home/eric/projects/linuxbios/checkin/solo/freebios2/src/include",
	"/home/eric/projects/linuxbios/checkin/solo/freebios2/src/arch/i386/include",
	"/home/eric/projects/linuxbios/checkin/solo/freebios2/src",
	0
};

static void compile_file(struct compile_state *state, const char *filename, int local)
{
	char cwd[4096];
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
	
	if (subdir[0] == '/') {
		file->dirname = xmalloc(subdir_len + 1, "dirname");
		memcpy(file->dirname, subdir, subdir_len);
		file->dirname[subdir_len] = '\0';
	}
	else {
		char *dir;
		int dirlen;
		char **path;
		/* Find the appropriate directory... */
		dir = 0;
		if (!state->file && exists(cwd, filename)) {
			dir = cwd;
		}
		if (local && state->file && exists(state->file->dirname, filename)) {
			dir = state->file->dirname;
		}
		for(path = include_paths; !dir && *path; path++) {
			if (exists(*path, filename)) {
				dir = *path;
			}
		}
		if (!dir) {
			error(state, 0, "Cannot find `%s'\n", filename);
		}
		dirlen = strlen(dir);
		file->dirname = xmalloc(dirlen + 1 + subdir_len + 1, "dirname");
		memcpy(file->dirname, dir, dirlen);
		file->dirname[dirlen] = '/';
		memcpy(file->dirname + dirlen + 1, subdir, subdir_len);
		file->dirname[dirlen + 1 + subdir_len] = '\0';
	}
	file->buf = slurp_file(file->dirname, file->basename, &file->size);
	xchdir(cwd);

	file->pos = file->buf;
	file->line_start = file->pos;
	file->line = 1;

	file->report_line = 1;
	file->report_name = file->basename;
	file->report_dir  = file->dirname;

	file->prev = state->file;
	state->file = file;
	
	process_trigraphs(state);
	splice_lines(state);
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

#define SIZEOF_SHORT 2
#define SIZEOF_INT   4
#define SIZEOF_LONG  (sizeof(long_t))

#define ALIGNOF_SHORT 2
#define ALIGNOF_INT   4
#define ALIGNOF_LONG  (sizeof(long_t))

#define MASK_UCHAR(X)    ((X) & ((ulong_t)0xff))
#define MASK_USHORT(X)   ((X) & (((ulong_t)1 << (SIZEOF_SHORT*8)) - 1))
static inline ulong_t mask_uint(ulong_t x)
{
	if (SIZEOF_INT < SIZEOF_LONG) {
		ulong_t mask = (((ulong_t)1) << ((ulong_t)(SIZEOF_INT*8))) -1;
		x &= mask;
	}
	return x;
}
#define MASK_UINT(X)      (mask_uint(X))
#define MASK_ULONG(X)    (X)

static struct type void_type   = { .type  = TYPE_VOID };
static struct type char_type   = { .type  = TYPE_CHAR };
static struct type uchar_type  = { .type  = TYPE_UCHAR };
static struct type short_type  = { .type  = TYPE_SHORT };
static struct type ushort_type = { .type  = TYPE_USHORT };
static struct type int_type    = { .type  = TYPE_INT };
static struct type uint_type   = { .type  = TYPE_UINT };
static struct type long_type   = { .type  = TYPE_LONG };
static struct type ulong_type  = { .type  = TYPE_ULONG };

static struct type void_ptr_type  = {
	.type = TYPE_POINTER,
	.left = &void_type,
};

static struct type void_func_type = { 
	.type  = TYPE_FUNCTION,
	.left  = &void_type,
	.right = &void_type,
};

static struct triple *variable(struct compile_state *state, struct type *type)
{
	struct triple *result;
	if ((type->type & STOR_MASK) != STOR_PERM) {
		if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
			result = triple(state, OP_ADECL, type, 0, 0);
		} else {
			struct type *field;
			struct triple **vector;
			ulong_t index;
			result = new_triple(state, OP_VAL_VEC, type, -1, -1);
			vector = &result->param[0];

			field = type->left;
			index = 0;
			while((field->type & TYPE_MASK) == TYPE_PRODUCT) {
				vector[index] = variable(state, field->left);
				field = field->right;
				index++;
			}
			vector[index] = variable(state, field);
		}
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
	stor_of(fp, type);
	switch(type->type & TYPE_MASK) {
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
	case TYPE_OVERLAP:
		name_of(fp, type->left);
		fprintf(fp, ", ");
		name_of(fp, type->right);
		break;
	case TYPE_ENUM:
		fprintf(fp, "enum %s", type->type_ident->name);
		qual_of(fp, type);
		break;
	case TYPE_STRUCT:
		fprintf(fp, "struct %s", type->type_ident->name);
		qual_of(fp, type);
		break;
	case TYPE_FUNCTION:
	{
		name_of(fp, type->left);
		fprintf(fp, " (*)(");
		name_of(fp, type->right);
		fprintf(fp, ")");
		break;
	}
	case TYPE_ARRAY:
		name_of(fp, type->left);
		fprintf(fp, " [%ld]", (long)(type->elements));
		break;
	default:
		fprintf(fp, "????: %x", type->type & TYPE_MASK);
		break;
	}
}

static size_t align_of(struct compile_state *state, struct type *type)
{
	size_t align;
	align = 0;
	switch(type->type & TYPE_MASK) {
	case TYPE_VOID:
		align = 1;
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		align = 1;
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
	case TYPE_POINTER:
		align = ALIGNOF_LONG;
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
		align = align_of(state, type->left);
		break;
	default:
		error(state, 0, "alignof not yet defined for type\n");
		break;
	}
	return align;
}

static size_t needed_padding(size_t offset, size_t align)
{
        size_t padding;
	padding = 0;
	if (offset % align) {
		padding = align - (offset % align);
	}
	return padding;
}
static size_t size_of(struct compile_state *state, struct type *type)
{
	size_t size;
	size = 0;
	switch(type->type & TYPE_MASK) {
	case TYPE_VOID:
		size = 0;
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		size = 1;
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
	case TYPE_POINTER:
		size = SIZEOF_LONG;
		break;
	case TYPE_PRODUCT:
	{
		size_t align, pad;
		size = 0;
		while((type->type & TYPE_MASK) == TYPE_PRODUCT) {
			align = align_of(state, type->left);
			pad = needed_padding(size, align);
			size = size + pad + size_of(state, type->left);
			type = type->right;
		}
		align = align_of(state, type);
		pad = needed_padding(size, align);
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
	{
		size_t align, pad;
		size = size_of(state, type->left);
		/* Pad structures so their size is a multiples of their alignment */
		align = align_of(state, type);
		pad = needed_padding(size, align);
		size = size + pad;
		break;
	}
	default:
		internal_error(state, 0, "sizeof not yet defined for type\n");
		break;
	}
	return size;
}

static size_t field_offset(struct compile_state *state, 
	struct type *type, struct hash_entry *field)
{
	struct type *member;
	size_t size, align;
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		internal_error(state, 0, "field_offset only works on structures");
	}
	size = 0;
	member = type->left;
	while((member->type & TYPE_MASK) == TYPE_PRODUCT) {
		align = align_of(state, member->left);
		size += needed_padding(size, align);
		if (member->left->field_ident == field) {
			member = member->left;
			break;
		}
		size += size_of(state, member->left);
		member = member->right;
	}
	align = align_of(state, member);
	size += needed_padding(size, align);
	if (member->field_ident != field) {
		error(state, 0, "member %s not present", field->name);
	}
	return size;
}

static struct type *field_type(struct compile_state *state, 
	struct type *type, struct hash_entry *field)
{
	struct type *member;
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		internal_error(state, 0, "field_type only works on structures");
	}
	member = type->left;
	while((member->type & TYPE_MASK) == TYPE_PRODUCT) {
		if (member->left->field_ident == field) {
			member = member->left;
			break;
		}
		member = member->right;
	}
	if (member->field_ident != field) {
		error(state, 0, "member %s not present", field->name);
	}
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

static struct triple *struct_field(struct compile_state *state,
	struct triple *decl, struct hash_entry *field)
{
	struct triple **vector;
	struct type *type;
	ulong_t index;
	type = decl->type;
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		return decl;
	}
	if (decl->op != OP_VAL_VEC) {
		internal_error(state, 0, "Invalid struct variable");
	}
	if (!field) {
		internal_error(state, 0, "Missing structure field");
	}
	type = type->left;
	vector = &RHS(decl, 0);
	index = 0;
	while((type->type & TYPE_MASK) == TYPE_PRODUCT) {
		if (type->left->field_ident == field) {
			type = type->left;
			break;
		}
		index += 1;
		type = type->right;
	}
	if (type->field_ident != field) {
		internal_error(state, 0, "field %s not found?", field->name);
	}
	return vector[index];
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

static unsigned int do_integral_promotion(unsigned int type)
{
	type &= TYPE_MASK;
	if (type == TYPE_ENUM) type = TYPE_INT;
	if (TYPE_INTEGER(type) && (TYPE_RANK(type) < TYPE_RANK(TYPE_INT))) {
		type = TYPE_INT;
	}
	return type;
}

static unsigned int do_arithmetic_conversion(
	unsigned int left, unsigned int right)
{
	left &= TYPE_MASK;
	right &= TYPE_MASK;
	/* Convert enums to ints */
	if (left == TYPE_ENUM) left = TYPE_INT;
	if (right == TYPE_ENUM) right = TYPE_INT;
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
	/* if the basic types match and it is an arithmetic type we are done */
	if (TYPE_ARITHMETIC(type)) {
		return 1;
	}
	/* If it is a pointer type recurse and keep testing */
	if (type == TYPE_POINTER) {
		return equiv_types(left->left, right->left);
	}
	else if (type == TYPE_ARRAY) {
		return (left->elements == right->elements) &&
			equiv_types(left->left, right->left);
	}
	/* test for struct/union equality */
	else if (type == TYPE_STRUCT) {
		return left->type_ident == right->type_ident;
	}
	/* Test for equivalent functions */
	else if (type == TYPE_FUNCTION) {
		return equiv_types(left->left, right->left) &&
			equiv_types(left->right, right->right);
	}
	/* We only see TYPE_PRODUCT as part of function equivalence matching */
	else if (type == TYPE_PRODUCT) {
		return equiv_types(left->left, right->left) &&
			equiv_types(left->right, right->right);
	}
	/* We should see TYPE_OVERLAP */
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
	/* test for struct/union equality */
	else if (type == TYPE_STRUCT) {
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
		int_type |= do_integral_promotion(type->type);
		if (int_type != type->type) {
			if (def->op != OP_LOAD) {
				def->type = new_type(int_type, 0, 0);
			}
			else {
#warning "FIXME can I just cast all operands like this?"
				def = triple(state, OP_COPY, 
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
	return !!TYPE_SIGNED(type->type);
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
	else if (def->op == OP_VAL_VEC) {
		in_reg = is_in_reg(state, RHS(def, 0));
	}
	else if (def->op == OP_DOT) {
		in_reg = is_in_reg(state, RHS(def, 0));
	}
	else {
		internal_error(state, 0, "unknown expr storage location");
		in_reg = -1;
	}
	return in_reg;
}

/* Is this a stable variable location otherwise it must be a temporary */
static int is_stable(struct compile_state *state, struct triple *def)
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
	else if (def->op == OP_DOT) {
		ret = is_stable(state, RHS(def, 0));
	}
	else if (def->op == OP_VAL_VEC) {
		struct triple **vector;
		ulong_t i;
		ret = 1;
		vector = &RHS(def, 0);
		for(i = 0; i < def->type->elements; i++) {
			if (!is_stable(state, vector[i])) {
				ret = 0;
				break;
			}
		}
	}
	return ret;
}

static int is_lvalue(struct compile_state *state, struct triple *def)
{
	int ret;
	ret = 1;
	if (!def) {
		return 0;
	}
	if (!is_stable(state, def)) {
		return 0;
	}
	if (def->op == OP_DOT) {
		ret = is_lvalue(state, RHS(def, 0));
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
		internal_error(state, 0, "constant for unkown type");
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

	type = new_type(TYPE_POINTER | (type->type & QUAL_MASK), type, 0);

	result = 0;
	if (expr->op == OP_ADECL) {
		error(state, expr, "address of auto variables not supported");
	}
	else if (expr->op == OP_SDECL) {
		result = triple(state, OP_ADDRCONST, type, 0, 0);
		MISC(result, 0) = expr;
		result->u.cval = offset;
	}
	else if (expr->op == OP_DEREF) {
		result = triple(state, OP_ADD, type,
			RHS(expr, 0),
			int_const(state, &ulong_type, offset));
	}
	else if (expr->op == OP_LIST) {
		error(state, 0, "Function addresses not supported");
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
			def = triple(state, OP_COPY, type, def, 0);
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
	if (!field) {
		internal_error(state, 0, "No field passed to deref_field");
	}
	result = 0;
	type = expr->type;
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		error(state, 0, "request for member %s in something not a struct or union",
			field->name);
	}
	member = field_type(state, type, field);
	if ((type->type & STOR_MASK) == STOR_PERM) {
		/* Do the pointer arithmetic to get a deref the field */
		ulong_t offset;
		offset = field_offset(state, type, field);
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

static struct triple *read_expr(struct compile_state *state, struct triple *def)
{
	int op;
	if  (!def) {
		return 0;
	}
#warning "CHECK_ME is this the only place I need to do lvalue conversions?"
	/* Transform lvalues into something we can read */
	def = lvalue_conversion(state, def);
	if (!is_stable(state, def)) {
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
	return triple(state, op, def->type, def, 0);
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
	else if (((dest->type & TYPE_MASK) == TYPE_STRUCT) &&
		((rval->type & TYPE_MASK) == TYPE_STRUCT) &&
		(dest->type_ident == rval->type_ident)) {
		compatible = 1;
	}
	return compatible;
}


static void write_compatible(struct compile_state *state,
	struct type *dest, struct type *rval)
{
	if (!is_write_compatible(state, dest, rval)) {
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
	int op;

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

	/* Now figure out which assignment operator to use */
	op = -1;
	if (is_in_reg(state, dest)) {
		op = OP_WRITE;
	} else {
		op = OP_STORE;
	}
	def = triple(state, op, dest->type, dest, rval);
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
			left->type->type, 
			right->type->type), 0, 0);
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

static struct triple *cond_expr(
	struct compile_state *state, 
	struct triple *test, struct triple *left, struct triple *right)
{
	struct triple *def;
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
	/* Cleanup and invert the test */
	test = lfalse_expr(state, read_expr(state, test));
	def = new_triple(state, OP_COND, result_type, 0, 3);
	def->param[0] = test;
	def->param[1] = left;
	def->param[2] = right;
	return def;
}


static int expr_depth(struct compile_state *state, struct triple *ins)
{
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
	else if (ins->op == OP_COMMA) {
		int ldepth, rdepth;
		ldepth = expr_depth(state, RHS(ins, 0));
		rdepth = expr_depth(state, RHS(ins, 1));
		count = (ldepth >= rdepth)? ldepth : rdepth;
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

static struct triple *flatten(
	struct compile_state *state, struct triple *first, struct triple *ptr);

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
	rhs = TRIPLE_RHS(ptr->sizes);
	lhs = TRIPLE_LHS(ptr->sizes);
	if (TRIPLE_SIZE(ptr->sizes) != lhs + rhs + ignored) {
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
	
	/* Now flatten the lhs elements */
	for(i = 0; i < lhs; i++) {
		struct triple **ins = &LHS(ptr, i);
		*ins = flatten(state, first, *ins);
		use_triple(*ins, ptr);
	}
	return ptr;
}

static struct triple *flatten_land(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	struct triple *left, *right;
	struct triple *val, *test, *jmp, *label1, *end;

	/* Find the triples */
	left = RHS(ptr, 0);
	right = RHS(ptr, 1);

	/* Generate the needed triples */
	end = label(state);

	/* Thread the triples together */
	val          = flatten(state, first, variable(state, ptr->type));
	left         = flatten(state, first, write_expr(state, val, left));
	test         = flatten(state, first, 
		lfalse_expr(state, read_expr(state, val)));
	jmp          = flatten(state, first, branch(state, end, test));
	label1       = flatten(state, first, label(state));
	right        = flatten(state, first, write_expr(state, val, right));
	TARG(jmp, 0) = flatten(state, first, end); 
	
	/* Now give the caller something to chew on */
	return read_expr(state, val);
}

static struct triple *flatten_lor(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	struct triple *left, *right;
	struct triple *val, *jmp, *label1, *end;

	/* Find the triples */
	left = RHS(ptr, 0);
	right = RHS(ptr, 1);

	/* Generate the needed triples */
	end = label(state);

	/* Thread the triples together */
	val          = flatten(state, first, variable(state, ptr->type));
	left         = flatten(state, first, write_expr(state, val, left));
	jmp          = flatten(state, first, branch(state, end, left));
	label1       = flatten(state, first, label(state));
	right        = flatten(state, first, write_expr(state, val, right));
	TARG(jmp, 0) = flatten(state, first, end);
       
	
	/* Now give the caller something to chew on */
	return read_expr(state, val);
}

static struct triple *flatten_cond(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	struct triple *test, *left, *right;
	struct triple *val, *mv1, *jmp1, *label1, *mv2, *middle, *jmp2, *end;

	/* Find the triples */
	test = RHS(ptr, 0);
	left = RHS(ptr, 1);
	right = RHS(ptr, 2);

	/* Generate the needed triples */
	end = label(state);
	middle = label(state);

	/* Thread the triples together */
	val           = flatten(state, first, variable(state, ptr->type));
	test          = flatten(state, first, test);
	jmp1          = flatten(state, first, branch(state, middle, test));
	label1        = flatten(state, first, label(state));
	left          = flatten(state, first, left);
	mv1           = flatten(state, first, write_expr(state, val, left));
	jmp2          = flatten(state, first, branch(state, end, 0));
	TARG(jmp1, 0) = flatten(state, first, middle);
	right         = flatten(state, first, right);
	mv2           = flatten(state, first, write_expr(state, val, right));
	TARG(jmp2, 0) = flatten(state, first, end);
	
	/* Now give the caller something to chew on */
	return read_expr(state, val);
}

static struct triple *flatten_fcall(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
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
		case OP_COMMA:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			ptr = RHS(ptr, 1);
			break;
		case OP_VAL:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			return MISC(ptr, 0);
			break;
		case OP_LAND:
			ptr = flatten_land(state, first, ptr);
			break;
		case OP_LOR:
			ptr = flatten_lor(state, first, ptr);
			break;
		case OP_COND:
			ptr = flatten_cond(state, first, ptr);
			break;
		case OP_FCALL:
			ptr = flatten_fcall(state, first, ptr);
			break;
		case OP_READ:
		case OP_LOAD:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			use_triple(RHS(ptr, 0), ptr);
			break;
		case OP_BRANCH:
			use_triple(TARG(ptr, 0), ptr);
			break;
		case OP_CBRANCH:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			use_triple(RHS(ptr, 0), ptr);
			use_triple(TARG(ptr, 0), ptr);
			if (ptr->next != ptr) {
				use_triple(ptr->next, ptr);
			}
			break;
		case OP_CALL:
			MISC(ptr, 0) = flatten(state, first, MISC(ptr, 0));
			use_triple(MISC(ptr, 0), ptr);
			use_triple(TARG(ptr, 0), ptr);
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
		{
			struct triple *base;
			base = RHS(ptr, 0);
			if (base->op == OP_DEREF) {
				struct triple *left;
				ulong_t offset;
				offset = field_offset(state, base->type, ptr->u.field);
				left = RHS(base, 0);
				ptr = triple(state, OP_ADD, left->type, 
					read_expr(state, left),
					int_const(state, &ulong_type, offset));
				free_triple(state, base);
			}
			else if (base->op == OP_VAL_VEC) {
				base = flatten(state, first, base);
				ptr = struct_field(state, base, ptr->u.field);
			}
			break;
		}
		case OP_PIECE:
			MISC(ptr, 0) = flatten(state, first, MISC(ptr, 0));
			use_triple(MISC(ptr, 0), ptr);
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
			break;
		default:
			/* Flatten the easy cases we don't override */
			ptr = flatten_generic(state, first, ptr, 0);
			break;
		}
	} while(ptr && (ptr != orig_ptr));
	if (ptr) {
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

static void propogate_use(struct compile_state *state,
	struct triple *orig, struct triple *new)
{
	struct triple_set *user, *next;
	for(user = orig->use; user; user = next) {
		struct triple *use;
		int found;
		next = user->next;
		use = user->member;
		found = 0;
		found |= replace_rhs_use(state, orig, new, use);
		found |= replace_lhs_use(state, orig, new, use);
		if (!found) {
			internal_error(state, use, "use without use");
		}
	}
	if (orig->use) {
		internal_error(state, orig, "used after propogate_use");
	}
}

/*
 * Code generators
 * ===========================
 */

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
		right = triple(state, 
			is_signed(right->type)? OP_SMUL : OP_UMUL, 
			&ulong_type, 
			right, 
			int_const(state, &ulong_type, 
				size_of(state, left->type->left)));
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
		right = triple(state, 
			is_signed(right->type)? OP_SMUL : OP_UMUL, 
			&ulong_type, 
			right, 
			int_const(state, &ulong_type, 
				size_of(state, left->type->left)));
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

static struct triple *mk_cast_expr(
	struct compile_state *state, struct type *type, struct triple *expr)
{
	struct triple *def;
	def = read_expr(state, expr);
	def = triple(state, OP_COPY, type, def, 0);
	return def;
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
	return IS_CONST_OP(ins->op) && (ins->op != OP_ADDRCONST);
}

static int constants_equal(struct compile_state *state, 
	struct triple *left, struct triple *right)
{
	int equal;
	if (!is_const(left) || !is_const(right)) {
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
			size_t lsize, rsize;
			lsize = size_of(state, left->type);
			rsize = size_of(state, right->type);
			if (lsize != rsize) {
				break;
			}
			if (memcmp(left->u.blob, right->u.blob, lsize) == 0) {
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

static long_t log2(ulong_t value)
{
	return bsr(value);
}

static long_t tlog2(struct triple *ins)
{
	return log2(ins->u.cval);
}

static int is_pow2(struct triple *ins)
{
	ulong_t value, mask;
	long_t log;
	if (!is_const(ins)) {
		return 0;
	}
	value = ins->u.cval;
	log = log2(value);
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
		break;
	default:
		internal_error(state, rhs, "bad type to read_const\n");
		break;
	}
	if (!is_simple_const(rhs)) {
		internal_error(state, rhs, "bad op to read_const\n");
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
		internal_error(state, 0, "non const passed to const_true\n");
	}
	return !is_zero(rhs);
}

int const_eq(struct compile_state *state, struct triple *ins,
	struct triple *left, struct triple *right)
{
	int result;
	if (!is_const(left) || !is_const(right)) {
		internal_error(state, ins, "non const passed to const_eq\n");
		result = 0;
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
		internal_error(state, ins, "incomparable constants passed to const_eq\n");
		result = 0;
	}
	return result;
	
}

int const_ucmp(struct compile_state *state, struct triple *ins,
	struct triple *left, struct triple *right)
{
	int result;
	if (!is_const(left) || !is_const(right)) {
		internal_error(state, ins, "non const past to ucmp_const\n");
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
		internal_error(state, ins, "incomparable constants passed to const_ucmp\n");
		result = -2;
	}
	return result;
}

int const_scmp(struct compile_state *state, struct triple *ins,
	struct triple *left, struct triple *right)
{
	int result;
	if (!is_const(left) || !is_const(right)) {
		internal_error(state, ins, "non const past to ucmp_const\n");
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
		internal_error(state, ins, "incomparable constants passed to const_scmp\n");
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

static void check_lhs(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	expr = triple_lhs(state, ins, 0);
	for(;expr;expr = triple_lhs(state, ins, expr)) {
		internal_error(state, ins, "unexpected lhs");
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
	unuse_rhs(state, ins);
	unuse_lhs(state, ins);
}

static void mkcopy(struct compile_state *state, 
	struct triple *ins, struct triple *rhs)
{
	struct block *block;
	block = block_of_triple(state, ins);
	wipe_ins(state, ins);
	ins->op = OP_COPY;
	ins->sizes = TRIPLE_SIZES(0, 1, 0, 0);
	ins->u.block = block;
	RHS(ins, 0) = rhs;
	use_triple(RHS(ins, 0), ins);
}

static void mkconst(struct compile_state *state, 
	struct triple *ins, ulong_t value)
{
	if (!is_integral(ins) && !is_pointer(ins)) {
		internal_error(state, ins, "unknown type to make constant\n");
	}
	wipe_ins(state, ins);
	ins->op = OP_INTCONST;
	ins->sizes = TRIPLE_SIZES(0, 0, 0, 0);
	ins->u.cval = value;
}

static void mkaddr_const(struct compile_state *state,
	struct triple *ins, struct triple *sdecl, ulong_t value)
{
	if (sdecl->op != OP_SDECL) {
		internal_error(state, ins, "bad base for addrconst");
	}
	wipe_ins(state, ins);
	ins->op = OP_ADDRCONST;
	ins->sizes = TRIPLE_SIZES(0, 0, 1, 0);
	MISC(ins, 0) = sdecl;
	ins->u.cval = value;
	use_triple(sdecl, ins);
}

/* Transform multicomponent variables into simple register variables */
static void flatten_structures(struct compile_state *state)
{
	struct triple *ins, *first;
	first = state->first;
	ins = first;
	/* Pass one expand structure values into valvecs.
	 */
	ins = first;
	do {
		struct triple *next;
		next = ins->next;
		valid_ins(state, ins);
		if ((ins->type->type & TYPE_MASK) == TYPE_STRUCT) {
			if (ins->op == OP_VAL_VEC) {
				/* Do nothing */
			}
			else if ((ins->op == OP_LOAD) || (ins->op == OP_READ)) {
				struct triple *def, **vector;
				struct type *tptr;
				int op;
				ulong_t i;

				op = ins->op;
				def = RHS(ins, 0);
				get_occurance(ins->occurance);
				next = alloc_triple(state, OP_VAL_VEC, ins->type, -1, -1,
					ins->occurance);

				vector = &RHS(next, 0);
				tptr = next->type->left;
				for(i = 0; i < next->type->elements; i++) {
					struct triple *sfield;
					struct type *mtype;
					mtype = tptr;
					if ((mtype->type & TYPE_MASK) == TYPE_PRODUCT) {
						mtype = mtype->left;
					}
					sfield = deref_field(state, def, mtype->field_ident);
					
					vector[i] = triple(
						state, op, mtype, sfield, 0);
					put_occurance(vector[i]->occurance);
					get_occurance(next->occurance);
					vector[i]->occurance = next->occurance;
					tptr = tptr->right;
				}
				propogate_use(state, ins, next);
				flatten(state, ins, next);
				release_triple(state, ins);
			}
			else if ((ins->op == OP_STORE) || (ins->op == OP_WRITE)) {
				struct triple *src, *dst, **vector;
				struct type *tptr;
				int op;
				ulong_t i;

				op = ins->op;
				src = RHS(ins, 1);
				dst = RHS(ins, 0);
				get_occurance(ins->occurance);
				next = alloc_triple(state, OP_VAL_VEC, ins->type, -1, -1,
					ins->occurance);
				
				vector = &RHS(next, 0);
				tptr = next->type->left;
				for(i = 0; i < ins->type->elements; i++) {
					struct triple *dfield, *sfield;
					struct type *mtype;
					mtype = tptr;
					if ((mtype->type & TYPE_MASK) == TYPE_PRODUCT) {
						mtype = mtype->left;
					}
					sfield = deref_field(state, src, mtype->field_ident);
					dfield = deref_field(state, dst, mtype->field_ident);
					vector[i] = triple(
						state, op, mtype, dfield, sfield);
					put_occurance(vector[i]->occurance);
					get_occurance(next->occurance);
					vector[i]->occurance = next->occurance;
					tptr = tptr->right;
				}
				propogate_use(state, ins, next);
				flatten(state, ins, next);
				release_triple(state, ins);
			}
		}
		ins = next;
	} while(ins != first);
	/* Pass two flatten the valvecs.
	 */
	ins = first;
	do {
		struct triple *next;
		next = ins->next;
		if (ins->op == OP_VAL_VEC) {
			if (ins->use) {
				internal_error(state, ins, "valvec used\n");
			}
			release_triple(state, ins);
		} 
		ins = next;
	} while(ins != first);
	/* Pass three verify the state and set ->id to 0.
	 */
	ins = first;
	do {
		ins->id &= ~TRIPLE_FLAG_FLATTENED;
		if ((ins->op != OP_BLOBCONST) && (ins->op != OP_SDECL) &&
			((ins->type->type & TYPE_MASK) == TYPE_STRUCT)) {
			internal_error(state, ins, "STRUCT_TYPE remains?");
		}
		if (ins->op == OP_DOT) {
			internal_error(state, ins, "OP_DOT remains?");
		}
		if (ins->op == OP_VAL_VEC) {
			internal_error(state, ins, "OP_VAL_VEC remains?");
		}
		ins = ins->next;
	} while(ins != first);
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
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
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
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
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
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
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
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
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
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
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
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
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
	if (is_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, RHS(ins, 1));
		if (right >= (size_of(state, ins->type)*8)) {
			warning(state, ins, "left shift count >= width of type");
		}
	}
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins,  left << right);
	}
}

static void simplify_usr(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, RHS(ins, 1));
		if (right >= (size_of(state, ins->type)*8)) {
			warning(state, ins, "right shift count >= width of type");
		}
	}
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left >> right);
	}
}

static void simplify_ssr(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, RHS(ins, 1));
		if (right >= (size_of(state, ins->type)*8)) {
			warning(state, ins, "right shift count >= width of type");
		}
	}
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(state, ins, RHS(ins, 0));
		right = read_sconst(state, ins, RHS(ins, 1));
		mkconst(state, ins, left >> right);
	}
}

static void simplify_and(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left & right);
	}
}

static void simplify_or(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, RHS(ins, 0));
		right = read_const(state, ins, RHS(ins, 1));
		mkconst(state, ins, left | right);
	}
}

static void simplify_xor(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
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
	if (is_const(RHS(ins, 0))) {
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
	if (is_const(RHS(ins, 0))) {
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
		mkconst(state, ins, const_eq(state, ins, left, right) == 1);
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
		mkconst(state, ins, const_eq(state, ins, left, right) != 1);
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
		mkconst(state, ins, const_scmp(state, ins, left, right) < 0);
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
		mkconst(state, ins, const_ucmp(state, ins, left, right) < 0);
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
		mkconst(state, ins, const_scmp(state, ins, left, right) > 0);
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
		mkconst(state, ins, const_ucmp(state, ins, left, right) > 0);
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
		mkconst(state, ins, const_scmp(state, ins, left, right) <= 0);
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
		mkconst(state, ins, const_ucmp(state, ins, left, right) <= 0);
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
		mkconst(state, ins, const_scmp(state, ins, left, right) >= 0);
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
		mkconst(state, ins, const_ucmp(state, ins, left, right) >= 0);
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

static void simplify_copy(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		switch(RHS(ins, 0)->op) {
		case OP_INTCONST:
		{
			ulong_t left;
			left = read_const(state, ins, RHS(ins, 0));
			mkconst(state, ins, left);
			break;
		}
		case OP_ADDRCONST:
		{
			struct triple *sdecl;
			ulong_t offset;
			sdecl  = MISC(RHS(ins, 0), 0);
			offset = RHS(ins, 0)->u.cval;
			mkaddr_const(state, ins, sdecl, offset);
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
	int simplified;
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
	 */
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
	} while(simplified);

	/* If we have a conditional branch with a constant condition
	 * make it an unconditional branch.
	 */
	if ((ins->op == OP_CBRANCH) && is_const(RHS(ins, 0))) {
		struct triple *targ;
		ulong_t value;
		value = read_const(state, ins, RHS(ins, 0));
		unuse_triple(RHS(ins, 0), ins);
		targ = TARG(ins, 0);
		ins->sizes = TRIPLE_SIZES(0, 0, 0, 1);
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
	
	/* If we have a branch to the next instruction
	 * make it a noop.
	 */
	if (TARG(ins, 0) == ins->next) {
		unuse_triple(ins->next, ins);
		if (ins->op == OP_CBRANCH) {
			unuse_triple(RHS(ins, 0), ins);
			unuse_triple(ins->next, ins);
		}
		ins->sizes = TRIPLE_SIZES(0, 0, 0, 0);
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
	zrhs = TRIPLE_RHS(ins->sizes);
	if (zrhs == 0) {
		return;
	}
	/* See if all of the rhs members of a phi have the same value */
	if (slot[0] && is_simple_const(slot[0])) {
		cvalue = read_const(state, ins, slot[0]);
		for(i = 1; i < zrhs; i++) {
			if (	!slot[i] ||
				!is_simple_const(slot[i]) ||
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
		mkcopy(state, ins, value);
		return;
	}
}


static void simplify_bsf(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, RHS(ins, 0));
		mkconst(state, ins, bsf(left));
	}
}

static void simplify_bsr(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
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

[OP_LOAD       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_STORE      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_NOOP       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_INTCONST   ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_BLOBCONST  ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_ADDRCONST  ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_WRITE      ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_READ       ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_COPY       ] = { simplify_copy,	COMPILER_SIMPLIFY_COPY },
[OP_PIECE      ] = { simplify_piece,	COMPILER_SIMPLIFY_OP },
[OP_ASM        ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

[OP_DOT        ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },
[OP_VAL_VEC    ] = { simplify_noop,	COMPILER_SIMPLIFY_OP },

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

static void simplify(struct compile_state *state, struct triple *ins)
{
	int op;
	simplify_t do_simplify;
	do {
		op = ins->op;
		do_simplify = 0;
		if ((op < 0) || (op > sizeof(table_simplify)/sizeof(table_simplify[0]))) {
			do_simplify = 0;
		}
		else if (state->compiler->flags & table_simplify[op].flag) {
			do_simplify = table_simplify[op].func;
		}
		else {
			do_simplify = simplify_noop;
		}
	
		if (!do_simplify) {
			internal_error(state, ins, "cannot simplify op: %d %s\n",
				op, tops(op));
			return;
		}
		do_simplify(state, ins);
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

	print_blocks(state, __func__, stdout);
}

/*
 * Builtins....
 * ============================
 */

static void register_builtin_function(struct compile_state *state,
	const char *name, int op, struct type *rtype, ...)
{
	struct type *ftype, *atype, *param, **next;
	struct triple *def, *arg, *result, *work, *last, *first, *retvar, *ret;
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

	/* Generate the needed triples */
	def = triple(state, OP_LIST, ftype, 0, 0);
	first = label(state);
	RHS(def, 0) = first;
	retvar = variable(state, &void_ptr_type);
	retvar = flatten(state, first, retvar);
	ret = triple(state, OP_RET, &void_type, read_expr(state, retvar), 0);

	/* Now string them together */
	param = ftype->right;
	for(i = 0; i < parameters; i++) {
		if ((param->type & TYPE_MASK) == TYPE_PRODUCT) {
			atype = param->left;
		} else {
			atype = param;
		}
		arg = flatten(state, first, variable(state, atype));
		param = param->right;
	}
	result = 0;
	if ((rtype->type & TYPE_MASK) != TYPE_VOID) {
		result = flatten(state, first, variable(state, rtype));
	}
	MISC(def, 0) = result;
	work = new_triple(state, op, rtype, -1, parameters);
	for(i = 0, arg = first->next->next; i < parameters; i++, arg = arg->next) {
		RHS(work, i) = read_expr(state, arg);
	}
	if (result && ((rtype->type & TYPE_MASK) == TYPE_STRUCT)) {
		struct triple *val;
		/* Populate the LHS with the target registers */
		work = flatten(state, first, work);
		work->type = &void_type;
		param = rtype->left;
		if (rtype->elements != TRIPLE_LHS(work->sizes)) {
			internal_error(state, 0, "Invalid result type");
		}
		val = new_triple(state, OP_VAL_VEC, rtype, -1, -1);
		for(i = 0; i < rtype->elements; i++) {
			struct triple *piece;
			atype = param;
			if ((param->type & TYPE_MASK) == TYPE_PRODUCT) {
				atype = param->left;
			}
			if (!TYPE_ARITHMETIC(atype->type) &&
				!TYPE_PTR(atype->type)) {
				internal_error(state, 0, "Invalid lhs type");
			}
			piece = triple(state, OP_PIECE, atype, work, 0);
			piece->u.cval = i;
			LHS(work, i) = piece;
			RHS(val, i) = piece;
		}
		work = val;
	}
	if (result) {
		work = write_expr(state, result, work);
	}
	work = flatten(state, first, work);
	last = flatten(state, first, label(state));
	ret  = flatten(state, first, ret);
	name_len = strlen(name);
	ident = lookup(state, name, name_len);
	ftype->type_ident = ident;
	symbol(state, ident, &ident->sym_ident, def, ftype);
	
	state->file = file.prev;
	state->function = 0;
	
	if (!state->functions) {
		state->functions = def;
	} else {
		insert_triple(state, state->functions, def);
	}
	if (state->compiler->debug & DEBUG_INLINE) {
		fprintf(stdout, "\n");
		loc(stdout, state, 0);
		fprintf(stdout, "\n__________ %s _________\n", __FUNCTION__);
		display_func(stdout, def);
		fprintf(stdout, "__________ %s _________ done\n\n", __FUNCTION__);
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
static int isdecl_specifier(int tok);
static struct type *decl_specifiers(struct compile_state *state);
static int istype(int tok);
static struct triple *expr(struct compile_state *state);
static struct triple *assignment_expr(struct compile_state *state);
static struct type *type_name(struct compile_state *state);
static void statement(struct compile_state *state, struct triple *fist);

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
	eat(state, TOK_LIT_CHAR);
	tk = &state->token[0];
	str = tk->val.str + 1;
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
		eat(state, TOK_LIT_STRING);
		tk = &state->token[0];
		str = tk->val.str + 1;
		str_len = tk->str_len - 2;
		if (str_len < 0) {
			error(state, 0, "negative string constant length");
		}
		end = str + str_len;
		ptr = buf;
		buf = xmalloc(type->elements + str_len + 1, "string_constant");
		memcpy(buf, ptr, type->elements);
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

	eat(state, TOK_LIT_INT);
	tk = &state->token[0];
	errno = 0;
	decimal = (tk->val.str[0] != '0');
	val = strtoul(tk->val.str, &end, 0);
	if ((val > ULONG_T_MAX) || ((val == ULONG_MAX) && (errno == ERANGE))) {
		error(state, 0, "Integer constant to large");
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
		eat(state, TOK_IDENT);
		ident = state->token[0].ident;
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
		eat(state, TOK_ENUM_CONST);
		ident = state->token[0].ident;
		if (!ident->sym_ident) {
			error(state, 0, "%s undeclared", ident->name);
		}
		def = ident->sym_ident->def;
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
			eat(state, TOK_IDENT);
			field = state->token[0].ident;
			def = deref_field(state, def, field);
			break;
		}
		case TOK_ARROW:
		{
			struct hash_entry *field;
			eat(state, TOK_ARROW);
			eat(state, TOK_IDENT);
			field = state->token[0].ident;
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
		def = int_const(state, &ulong_type, size_of(state, type));
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
		def = int_const(state, &ulong_type, align_of(state, type));
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
		switch(tok = (peek(state))) {
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
#warning "Extend relational exprs to work on more than arithmetic types"
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
#warning "Extend equality exprs to work on more than arithmetic types"
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

		def = triple(state, OP_LAND, &int_type,
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
		
		def = triple(state, OP_LOR, &int_type,
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

		def = cond_expr(state, test, left, right);
	}
	return def;
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
		head = label(state); /* dummy initial triple */
		flatten(state, head, expr);
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			simplify(state, ptr);
		}
		/* Remove the constant value the tail of the list */
		def = head->prev;
		def->prev->next = def->next;
		def->next->prev = def->prev;
		def->next = def->prev = def;
		if (!is_const(def)) {
			error(state, 0, "Not a constant expression");
		}
		/* Free the intermediate expressions */
		while(head->next != head) {
			release_triple(state, head->next);
		}
		free_triple(state, head);
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
		struct triple *left, *right;
		left = def;
		eat(state, TOK_COMMA);
		right = assignment_expr(state);
		def = triple(state, OP_COMMA, right->type, left, right);
	}
	return def;
}

static void expr_statement(struct compile_state *state, struct triple *first)
{
	if (peek(state) != TOK_SEMI) {
		/* lvalue conversions always apply except when certaion operators
		 * are applied so the values so apply them here as I know no more
		 * operators will be applied.
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

#warning "FIXME implement a more general excess branch elimination"
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
	var = MISC(state->main_function, 0);
	/* Find the return destination */
	dest = state->i_return->sym_ident->def;
	mv = jmp = 0;
	/* If needed generate a jump instruction */
	if (!last) {
		jmp = branch(state, dest, 0);
	}
	/* If needed generate an assignment instruction */
	if (val) {
		mv = write_expr(state, var, val);
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
	eat(state, TOK_IDENT);
	ident = state->token[0].ident;
	if (!ident->sym_label) {
		/* If this is a forward branch allocate the label now,
		 * it will be flattend in the appropriate location later.
		 */
		struct triple *ins;
		ins = label(state);
		label_symbol(state, ident, ins);
 	}
	eat(state, TOK_SEMI);

	flatten(state, first, branch(state, ident->sym_label->def, 0));
}

static void labeled_statement(struct compile_state *state, struct triple *first)
{
	struct triple *ins;
	struct hash_entry *ident;
	eat(state, TOK_IDENT);

	ident = state->token[0].ident;
	if (ident->sym_label && ident->sym_label->def) {
		ins = ident->sym_label->def;
		put_occurance(ins->occurance);
		ins->occurance = new_occurance(state);
	}
	else {
		ins = label(state);
		label_symbol(state, ident, ins);
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

	/* Thread the pieces together */
	TARG(dbranch, 0) = dest;
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

	eat(state, TOK_ASM);
	/* For now ignore the qualifiers */
	switch(peek(state)) {
	case TOK_CONST:
		eat(state, TOK_CONST);
		break;
	case TOK_VOLATILE:
		eat(state, TOK_VOLATILE);
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
		RHS(def, i) = read_expr(state,in_param[i].expr);
	}
	flatten(state, first, def);
	for(i = 0; i < (out + clobbers); i++) {
		struct type *type;
		struct triple *piece;
		type = (i < out)? out_param[i].expr->type : &void_type;
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
	while(peek(state) == TOK_COMMA) {
		eat(state, TOK_COMMA);
		if (peek(state) == TOK_DOTS) {
			eat(state, TOK_DOTS);
			error(state, 0, "variadic functions not supported");
		}
		else {
			*next = new_type(TYPE_PRODUCT, *next, param_decl(state));
			next = &((*next)->right);
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
	struct hash_entry **ident, int need_ident)
{
	struct type *outer;
	int op;
	outer = 0;
	arrays_complete(state, type);
	switch(peek(state)) {
	case TOK_IDENT:
		eat(state, TOK_IDENT);
		if (!ident) {
			error(state, 0, "Unexpected identifier found");
		}
		/* The name of what we are declaring */
		*ident = state->token[0].ident;
		break;
	case TOK_LPAREN:
		eat(state, TOK_LPAREN);
		outer = declarator(state, type, ident, need_ident);
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
	struct hash_entry **ident, int need_ident)
{
	while(peek(state) == TOK_STAR) {
		eat(state, TOK_STAR);
		type = new_type(TYPE_POINTER | (type->type & STOR_MASK), type, 0);
	}
	type = direct_declarator(state, type, ident, need_ident);
	return type;
}


static struct type *typedef_name(
	struct compile_state *state, unsigned int specifiers)
{
	struct hash_entry *ident;
	struct type *type;
	eat(state, TOK_TYPE_NAME);
	ident = state->token[0].ident;
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
		eat(state, tok);
		ident = state->token[0].ident;
		
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
			eat(state, TOK_IDENT);
			eident = state->token[0].ident;
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
	int tok;
	tok = peek(state);
	if (tok != TOK_COLON) {
		type = declarator(state, type, ident, 1);
	}
	if ((tok == TOK_COLON) || (peek(state) == TOK_COLON)) {
		struct triple *value;
		eat(state, TOK_COLON);
		value = constant_expr(state);
#warning "FIXME implement bitfields to reduce register usage"
		error(state, 0, "bitfields not yet implemented");
	}
	return type;
}

static struct type *struct_or_union_specifier(
	struct compile_state *state, unsigned int spec)
{
	struct type *struct_type;
	struct hash_entry *ident;
	unsigned int type_join;
	int tok;
	struct_type = 0;
	ident = 0;
	switch(peek(state)) {
	case TOK_STRUCT:
		eat(state, TOK_STRUCT);
		type_join = TYPE_PRODUCT;
		break;
	case TOK_UNION:
		eat(state, TOK_UNION);
		type_join = TYPE_OVERLAP;
		error(state, 0, "unions not yet supported\n");
		break;
	default:
		eat(state, TOK_STRUCT);
		type_join = TYPE_PRODUCT;
		break;
	}
	tok = peek(state);
	if ((tok == TOK_IDENT) || (tok == TOK_ENUM_CONST) || (tok == TOK_TYPE_NAME)) {
		eat(state, tok);
		ident = state->token[0].ident;
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
		struct_type = new_type(TYPE_STRUCT | spec, struct_type, 0);
		struct_type->type_ident = ident;
		struct_type->elements = elements;
		if (ident) {
			symbol(state, ident, &ident->sym_tag, 0, struct_type);
		}
	}
	if (ident && ident->sym_tag && 
		ident->sym_tag->type && 
		((ident->sym_tag->type->type & TYPE_MASK) == TYPE_STRUCT)) {
		struct_type = clone_type(spec, ident->sym_tag->type);
	}
	else if (ident && !struct_type) {
		error(state, 0, "struct %s undeclared", ident->name);
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
			specifiers = QUAL_CONST;
			break;
		case TOK_VOLATILE:
			eat(state, TOK_VOLATILE);
			specifiers = QUAL_VOLATILE;
			break;
		case TOK_RESTRICT:
			eat(state, TOK_RESTRICT);
			specifiers = QUAL_RESTRICT;
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
	type = 0;
	switch(peek(state)) {
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
			tokens[peek(state)]);
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
			if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
				error(state, 0, "Struct designator not in struct initializer");
			}
			eat(state, TOK_DOT);
			eat(state, TOK_IDENT);
			field = state->token[0].ident;
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
#warning "FIXME more consistent initializer handling (where should eval_const_expr go?"
	if (peek(state) != TOK_LBRACE) {
		result = assignment_expr(state);
		if (((type->type & TYPE_MASK) == TYPE_ARRAY) &&
			(type->elements == ELEMENT_COUNT_UNSPECIFIED) &&
			((result->type->type & TYPE_MASK) == TYPE_ARRAY) &&
			(result->type->elements != ELEMENT_COUNT_UNSPECIFIED) &&
			(equiv_types(type->left, result->type->left))) {
			type->elements = result->type->elements;
		}
		if (is_stable(state, result) && 
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
		buf = xcmalloc(max_offset, "initializer");
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
				buf = xmalloc(max_offset, "initializer");
				memcpy(buf, old_buf, old_size);
				xfree(old_buf);
			}
			dest = ((char *)buf) + info.offset;
			if (value->op == OP_BLOBCONST) {
				memcpy(dest, value->u.blob, value_size);
			}
			else if ((value->op == OP_INTCONST) && (value_size == 1)) {
				*((uint8_t *)dest) = value->u.cval & 0xff;
			}
			else if ((value->op == OP_INTCONST) && (value_size == 2)) {
				*((uint16_t *)dest) = value->u.cval & 0xffff;
			}
			else if ((value->op == OP_INTCONST) && (value_size == 4)) {
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

static void resolve_branches(struct compile_state *state)
{
	/* Make a second pass and finish anything outstanding
	 * with respect to branches.  The only outstanding item
	 * is to see if there are goto to labels that have not
	 * been defined and to error about them.
	 */
	int i;
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
	struct hash_entry *ident;
	struct type *param;
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
		error(state, 0, "No identifier for paramter %d\n", i);
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

	/* Allocate a variable for the return address */
	retvar = variable(state, &void_ptr_type);
	retvar = flatten(state, end, retvar);

	/* Add in the return instruction */
	ret = triple(state, OP_RET, &void_type, read_expr(state, retvar), 0);
	ret = flatten(state, first, ret);

	/* Walk through the parameters and create symbol table entries
	 * for them.
	 */
	param = type->right;
	while((param->type & TYPE_MASK) == TYPE_PRODUCT) {
		ident = param->left->field_ident;
		tmp = variable(state, param->left);
		symbol(state, ident, &ident->sym_ident, tmp, tmp->type);
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
	/* Add a variable for the return value */
	MISC(def, 0) = 0;
	if ((type->left->type & TYPE_MASK) != TYPE_VOID) {
		/* Remove all type qualifiers from the return type */
		tmp = variable(state, clone_type(0, type->left));
		flatten(state, end, tmp);
		/* Remember where the return value is */
		MISC(def, 0) = tmp;
	}

	/* Remember which function I am compiling.
	 * Also assume the last defined function is the main function.
	 */
	state->main_function = def;

	/* Now get the actual function definition */
	compound_statement(state, end);

	/* Finish anything unfinished with branches */
	resolve_branches(state);

	/* Remove the parameter scope */
	end_scope(state);


	/* Remember I have defined a function */
	if (!state->functions) {
		state->functions = def;
	} else {
		insert_triple(state, state->functions, def);
	}
	if (state->compiler->debug & DEBUG_INLINE) {
		fprintf(stdout, "\n");
		loc(stdout, state, 0);
		fprintf(stdout, "\n__________ %s _________\n", __FUNCTION__);
		display_func(stdout, def);
		fprintf(stdout, "__________ %s _________ done\n\n", __FUNCTION__);
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
		error(state, 0, "Function prototypes not supported");
	}
	if (ident && 
		((type->type & STOR_MASK) == STOR_STATIC) &&
		((type->type & QUAL_CONST) == 0)) {
		error(state, 0, "non const static variables not supported");
	}
	if (ident) {
		def = variable(state, type);
		symbol(state, ident, &ident->sym_ident, def, type);
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

static void mark_live_functions(struct compile_state *state, struct triple *first)
{
	struct triple *ptr;
	ptr = first;
	do {
		if (ptr->op == OP_FCALL) {
			struct triple *func;
			func = MISC(ptr, 0);
			if (func->u.cval++ == 0) {
				mark_live_functions(state, RHS(func, 0));
			}
		}
		ptr = ptr->next;
	} while(ptr != first);
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


static int local_triple(struct compile_state *state, 
	struct triple *func, struct triple *ins)
{
	int local = (ins->id & TRIPLE_FLAG_LOCAL);
#if 0
	if (!local) {
		fprintf(stderr, "global: ");
		display_triple(stderr, ins);
	}
#endif
	return local;
}

struct triple *copy_func(struct compile_state *state, struct triple *ofunc, 
	struct occurance *base_occurance)
{
	struct triple *nfunc;
	struct triple *nfirst, *ofirst;
	struct triple *new, *old;

	if (state->compiler->debug & DEBUG_INLINE) {
		fprintf(stdout, "\n");
		loc(stdout, state, 0);
		fprintf(stdout, "\n__________ %s _________\n", __FUNCTION__);
		display_func(stdout, ofunc);
		fprintf(stdout, "__________ %s _________ done\n\n", __FUNCTION__);
	}

	/* Make a new copy of the old function */
	nfunc = triple(state, OP_LIST, ofunc->type, 0, 0);
	nfirst = 0;
	ofirst = old = RHS(ofunc, 0);
	do {
		struct triple *new;
		struct occurance *occurance;
		int old_lhs, old_rhs;
		old_lhs = TRIPLE_LHS(old->sizes);
		old_rhs = TRIPLE_RHS(old->sizes);
		occurance = inline_occurance(state, base_occurance, old->occurance);
		if (ofunc->u.cval && (old->op == OP_FCALL)) {
			MISC(old, 0)->u.cval += 1;
		}
		new = alloc_triple(state, old->op, old->type, old_lhs, old_rhs,
			occurance);
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
		
		/* During the copy remember new as user of old */
		use_triple(old, new);

		/* Populate the return type if present */
		if (old == MISC(ofunc, 0)) {
			MISC(nfunc, 0) = new;
		}
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
		count = TRIPLE_SIZE(old->sizes);
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
				internal_error(state, 0, "Could not copy %d\n", i);
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

static struct triple *flatten_inline_call(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	/* Inline the function call */
	struct type *ptype;
	struct triple *ofunc, *nfunc, *nfirst, *param, *result;
	struct triple *end, *nend;
	int pvals, i;

	/* Find the triples */
	ofunc = MISC(ptr, 0);
	if (ofunc->op != OP_LIST) {
		internal_error(state, 0, "improper function");
	}
	nfunc = copy_func(state, ofunc, ptr->occurance);
	nfirst = RHS(nfunc, 0)->next->next;
	/* Prepend the parameter reading into the new function list */
	ptype = nfunc->type->right;
	param = RHS(nfunc, 0)->next->next;
	pvals = TRIPLE_RHS(ptr->sizes);
	for(i = 0; i < pvals; i++) {
		struct type *atype;
		struct triple *arg;
		atype = ptype;
		if ((ptype->type & TYPE_MASK) == TYPE_PRODUCT) {
			atype = ptype->left;
		}
		while((param->type->type & TYPE_MASK) != (atype->type & TYPE_MASK)) {
			param = param->next;
		}
		arg = RHS(ptr, i);
		flatten(state, nfirst, write_expr(state, param, arg));
		ptype = ptype->right;
		param = param->next;
	}
	result = 0;
	if ((nfunc->type->left->type & TYPE_MASK) != TYPE_VOID) {
		result = read_expr(state, MISC(nfunc,0));
	}
	if (state->compiler->debug & DEBUG_INLINE) {
		fprintf(stdout, "\n");
		loc(stdout, state, 0);
		fprintf(stdout, "\n__________ %s _________\n", __FUNCTION__);
		display_func(stdout, nfunc);
		fprintf(stdout, "__________ %s _________ done\n\n", __FUNCTION__);
	}

	/* Get rid of the extra triples */
	nfirst = RHS(nfunc, 0)->next->next;
	release_triple(state, RHS(nfunc, 0)->prev->prev);
	release_triple(state, RHS(nfunc, 0)->prev);
	release_triple(state, RHS(nfunc, 0)->next);
	free_triple(state, RHS(nfunc, 0));
	RHS(nfunc, 0) = 0;
	free_triple(state, nfunc);

	/* Append the new function list onto the return list */
	end = first->prev;
	nend = nfirst->prev;
	end->next    = nfirst;
	nfirst->prev = end;
	nend->next   = first;
	first->prev  = nend;

	return result;
}

static struct triple *flatten_function_call(
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	/* Generate an ordinary function call */
	struct triple *func, *func_first, *func_last, *retvar;
	struct type *ptype;
	struct triple *param;
	struct triple *jmp;
	struct triple *ret_addr, *ret_loc, *ret_set;
	struct triple *result;
	int pvals, i;

	FINISHME();
	/* Find the triples */
	func = MISC(ptr, 0);
	func_first = RHS(func, 0);
	retvar = func_first->next;
	func_last  = func_first->prev;

	/* Generate some needed triples */
	ret_loc = label(state);
	ret_addr = triple(state, OP_ADDRCONST, &void_ptr_type, ret_loc, 0);

	/* Pass the parameters to the new function */
	ptype = func->type->right;
	param = func_first->next->next;
	pvals = TRIPLE_RHS(ptr->sizes);
	for(i = 0; i < pvals; i++) {
		struct type *atype;
		struct triple *arg;
		atype = ptype;
		if ((ptype->type & TYPE_MASK) == TYPE_PRODUCT) {
			atype = ptype->left;
		}
		while((param->type->type & TYPE_MASK) != (atype->type & TYPE_MASK)) {
			param = param->next;
		}
		arg = RHS(ptr, i);
		flatten(state, first, write_expr(state, param, arg));
		ptype = ptype->right;
		param = param->next;
	}
       
	/* Thread the triples together */
	ret_loc       = flatten(state, first, ret_loc);
	ret_addr      = flatten(state, ret_loc, ret_addr);
	ret_set       = flatten(state, ret_loc, write_expr(state, retvar, ret_addr));
	jmp           = flatten(state, ret_loc, 
		call(state, retvar, ret_addr, func_first, func_last));

	/* Find the result */
	result = 0;
	if ((func->type->left->type & TYPE_MASK) != TYPE_VOID) {
		result = read_expr(state, MISC(func, 0));
	}

	if (state->compiler->debug & DEBUG_INLINE) {
		fprintf(stdout, "\n");
		loc(stdout, state, 0);
		fprintf(stdout, "\n__________ %s _________\n", __FUNCTION__);
		display_func(stdout, func);
		fprintf(stdout, "__________ %s _________ done\n\n", __FUNCTION__);
	}

	return result;
}

static void inline_functions(struct compile_state *state, struct triple *first)
{
	struct triple *ptr, *next;
	ptr = next = first;
	do {
		int do_inline;
		struct triple *func, *prev, *new;
		ptr = next;
		prev = ptr->prev;
		next = ptr->next;
		if (ptr->op != OP_FCALL) {
			continue;
		}
		func = MISC(ptr, 0);
		/* See if the function should be inlined */
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
		if (state->compiler->flags & COMPILER_ALWAYS_INLINE) {
			do_inline = 1;
		}
		if (!(state->compiler->flags & COMPILER_INLINE)) {
			do_inline = 0;
		}
		if (!do_inline) {
			continue;
		}
		if (state->compiler->debug & DEBUG_INLINE) {
			fprintf(stderr, "inlining %s\n",
				func->type->type_ident->name);
		}

		/* Update the function use counts */
		func->u.cval -= 1;
		/* Unhook the call and really inline it */
		next->prev = prev;
		prev->next = next;
		ptr->next = ptr->prev = ptr;
		
		new = flatten(state, next, 
			flatten_inline_call(state, next, ptr));
		if (new) {
			propogate_use(state, ptr, new);
		}
		release_triple(state, ptr);
		next = prev->next;
	} while (next != first);
	ptr = next = first;
	do {
		struct triple *func, *prev, *new;
		ptr = next;
		prev = ptr->prev;
		next = ptr->next;
		if (ptr->op != OP_FCALL) {
			continue;
		}
		func = MISC(ptr, 0);
		inline_functions(state, RHS(func, 0));
		/* Unhook the call and really flatten it */
		next->prev = prev;
		prev->next = next;
		ptr->next = ptr->prev = ptr;
		new = flatten(state, next, 
			flatten_function_call(state, next, ptr));
		if (new) {
			propogate_use(state, ptr, new);
		}
		release_triple(state, ptr);
		next = prev->next;
	} while(next != first);
}
	
static void insert_function(struct compile_state *state,
	struct triple *func, void *arg)
{
	struct triple *first, *end, *ffirst, *fend;

	if (state->compiler->debug & DEBUG_INLINE) {
		fprintf(stderr, "%s func count: %d\n", 
			func->type->type_ident->name, func->u.cval);
	}
	if (func->u.cval == 0) {
		return;
	}
	if (state->compiler->flags & COMPILER_ALWAYS_INLINE) {
		internal_error(state, func, "always inline failed\n");
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

static void join_functions(struct compile_state *state)
{
	struct triple *jmp, *start, *end, *call;
	struct file_state file;

	/* Dummy file state to get debug handing right */
	memset(&file, 0, sizeof(file));
	file.basename = "";
	file.line = 0;
	file.report_line = 0;
	file.report_name = file.basename;
	file.prev = state->file;
	state->file = &file;
	state->function = "";
	
	/* Lay down the basic program structure */
	end = label(state);
	start = label(state);
	start = flatten(state, state->first, start);
	end = flatten(state, state->first, end);
	call = new_triple(state, OP_FCALL, &void_type, -1, 0);
	MISC(call, 0) = state->main_function;
	flatten(state, state->first, call);
	
	/* See which functions are called, and how often */
	mark_live_functions(state, state->first);
	inline_functions(state, state->first);
	walk_functions(state, insert_function, end);

	if (start->next != end) {
		jmp = flatten(state, start, branch(state, end, 0));
	}

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
	int (*cb)(struct compile_state *state, struct triple *ptr))
{
	struct triple *ptr;
	int result;
	ptr = state->first;
	do {
		result = cb(state, ptr);
		if (ptr->next->prev != ptr) {
			internal_error(state, ptr->next, "bad prev");
		}
		ptr = ptr->next;
	} while((result == 0) && (ptr != state->first));
	return result;
}

#define PRINT_LIST 1
static int do_print_triple(struct compile_state *state, struct triple *ins)
{
	int op;
	op = ins->op;
	if (op == OP_LIST) {
#if !PRINT_LIST
		return 0;
#endif
	}
	if ((op == OP_LABEL) && (ins->use)) {
		printf("\n%p:\n", ins);
	}
	display_triple(stdout, ins);

	if (triple_is_branch(state, ins) && ins->use && (ins->op != OP_RET)) {
		internal_error(state, ins, "branch used?");
	}
	if (triple_is_branch(state, ins)) {
		printf("\n");
	}
	return 0;
}

static void print_triples(struct compile_state *state)
{
	if (state->compiler->debug & DEBUG_TRIPLES) {
		walk_triples(state, do_print_triple);
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

static void print_control_flow(struct compile_state *state)
{
	struct cf_block *cf;
	int i;
	printf("\ncontrol flow\n");
	cf = xcmalloc(sizeof(*cf) * (state->last_vertex + 1), "cf_block");
	find_cf_blocks(cf, state->first_block);

	for(i = 1; i <= state->last_vertex; i++) {
		struct block *block;
		struct block_set *edge;
		block = cf[i].block;
		if (!block)
			continue;
		printf("(%p) %d:", block, block->vertex);
		for(edge = block->edges; edge; edge = edge->next) {
			printf(" %d", edge->member->vertex);
		}
		printf("\n");
	}

	xfree(cf);
}


static struct block *basic_block(struct compile_state *state, struct triple *first)
{
	struct block *block;
	struct triple *ptr;
	if (first->op != OP_LABEL) {
		internal_error(state, 0, "block does not start with a label");
	}
	/* See if this basic block has already been setup */
	if (first->u.block != 0) {
		return first->u.block;
	}
	/* Allocate another basic block structure */
	state->last_vertex += 1;
	block = xcmalloc(sizeof(*block), "block");
	block->first = block->last = first;
	block->vertex = state->last_vertex;
	ptr = first;
	do {
		if ((ptr != first) && (ptr->op == OP_LABEL) && (ptr->use)) { 
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
	} while (ptr != state->first);
	if (ptr == state->first) {
		/* The block has no outflowing edges */
	}
	else if (ptr->op == OP_LABEL) {
		struct block *next;
		next = basic_block(state, ptr);
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
		expr = triple_targ(state, ptr, 0);
		if (!expr) {
			internal_error(state, ptr, "branch without targets");
		}
		first = *expr;
		expr = triple_targ(state, ptr, expr);
		for(; expr; expr = triple_targ(state, ptr, expr)) {
			if (!*expr) continue;
			child = basic_block(state, *expr);
			use_block(child, block);
			add_block_edge(block, child, 0);
		}
		if (first) {
			child = basic_block(state, first);
			use_block(child, block);
			add_block_edge(block, child, 1);
		}
	}
	else {
		internal_error(state, 0, "Bad basic block split");
	}
#if 0
{
	struct block_set *edge;
	fprintf(stderr, "basic_block: %10p [%2d] ( %10p - %10p )",
		block, block->vertex, 
		block->first, block->last);
	for(edge = block->edges; edge; edge = edge->next) {
		fprintf(stderr, " %10p [%2d]",
			edge->member ? edge->member->first : 0,
			edge->member ? edge->member->vertex : -1);
	}
	fprintf(stderr, "\n");
}
#endif
	return block;
}


static void walk_blocks(struct compile_state *state,
	void (*cb)(struct compile_state *state, struct block *block, void *arg),
	void *arg)
{
	struct triple *ptr, *first;
	struct block *last_block;
	last_block = 0;
	first = state->first;
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
	for(ptr = block->first; ; ptr = ptr->next) {
		display_triple(fp, ptr);
		if (ptr == block->last)
			break;
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
	walk_blocks(state, print_block, fp);
}
static void print_blocks(struct compile_state *state, const char *func, FILE *fp)
{
	if (state->compiler->debug & DEBUG_BASIC_BLOCKS) {
		fprintf(fp, "After %s\n", func);
		romcc_print_blocks(state, fp);
		print_control_flow(state);
	}
}

static void prune_nonblock_triples(struct compile_state *state)
{
	struct block *block;
	struct triple *first, *ins, *next;
	/* Delete the triples not in a basic block */
	first = state->first;
	block = 0;
	ins = first;
	do {
		next = ins->next;
		if (ins->op == OP_LABEL) {
			block = ins->u.block;
		}
		if (!block) {
			release_triple(state, ins);
		}
		if (block && block->last == ins) {
			block = 0;
		}
		ins = next;
	} while(ins != first);
}

static void setup_basic_blocks(struct compile_state *state)
{
	if (!triple_stores_block(state, state->first)) {
		internal_error(state, 0, "ins will not store block?");
	}
	/* Find the basic blocks */
	state->last_vertex = 0;
	state->first_block = basic_block(state, state->first);
	/* Delete the triples not in a basic block */
	prune_nonblock_triples(state);

	/* Find the last basic block.
	 *
	 * For purposes of reverse flow computation it is
	 * important that the last basic block is empty.
	 * This allows the control flow graph to be modified to
	 * have one unique starting block and one unique final block.
	 * With the insertion of a few extra edges.
	 *
	 * If the final block contained instructions it could contain
	 * phi functions from edges that would never contribute a
	 * value.  Which for now at least I consider a compile error.
	 */
	state->last_block = block_of_triple(state, state->first->prev);
	if ((state->last_block->first != state->last_block->last) ||
		(state->last_block->last->op != OP_LABEL))
	{
		struct block *block, *prev_block;
		struct triple *final;

		prev_block = state->last_block;
		
		final = label(state);
		flatten(state, state->first, final);
		final->id |= TRIPLE_FLAG_VOLATILE;
		use_triple(final, final);
		block = basic_block(state, final);

		state->last_block = block;

		add_block_edge(prev_block, block, 0);
		use_block(block, prev_block);
	}

#if 0
	/* If we are debugging print what I have just done */
	if (state->compiler->debug & DEBUG_BASIC_BLOCKS) {
		print_blocks(state, stdout);
		print_control_flow(state);
	}
#endif
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
	xfree(block);
}

static void free_basic_blocks(struct compile_state *state)
{
	struct triple *first, *ins;
	free_basic_block(state, state->first_block);
	state->last_vertex = 0;
	state->first_block = state->last_block = 0;
	first = state->first;
	ins = first;
	do {
		if (triple_stores_block(state, ins)) {
			ins->u.block = 0;
		}
		ins = ins->next;
	} while(ins != first);
	
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
	/* Renumber the blocks in a convinient fashion */
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
	/* Renumber the blocks in a convinient fashion */
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

static int setup_spdblocks(struct compile_state *state, struct sdom_block *sd)
{
	struct block *block;
	int vertex;
	/* Setup as many sdpblocks as possible without using fake edges */
	vertex = initialize_spdblock(state, sd, 0, state->last_block, 0);

	/* Walk through the graph and find unconnected blocks.  Add a
	 * fake edge from the unconnected blocks to the end of the
	 * graph. 
	 */
	block = state->first_block->last->next->u.block;
	for(; block && block != state->first_block; block = block->last->next->u.block) {
		if (sd[block->vertex].block == block) {
			continue;
		}
#if DEBUG_SDP_BLOCKS
		fprintf(stderr, "Adding %d\n", vertex +1);
#endif
		add_block_edge(block, state->last_block, 0);
		use_block(state->last_block, block);

		vertex = initialize_spdblock(state, sd, state->last_block, block, vertex);
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

static void compute_sdom(struct compile_state *state, struct sdom_block *sd)
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
	for(i = state->last_vertex; i >= 2; i--) {
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

static void compute_spdom(struct compile_state *state, struct sdom_block *sd)
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
	for(i = state->last_vertex; i >= 2; i--) {
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

static void compute_idom(struct compile_state *state, struct sdom_block *sd)
{
	int i;
	for(i = 2; i <= state->last_vertex; i++) {
		struct block *block;
		block = sd[i].block;
		if (block->idom->vertex != sd[i].sdom->vertex) {
			block->idom = block->idom->idom;
		}
		idom_block(block->idom, block);
	}
	sd[1].block->idom = 0;
}

static void compute_ipdom(struct compile_state *state, struct sdom_block *sd)
{
	int i;
	for(i = 2; i <= state->last_vertex; i++) {
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
	 *   minimum amoung vertices u satisfying sdom(w) -> u -> w.
	 *   Then sdom(u) <= sdom(w) and idom(u) = idom(w).
	 */
	/* Lemma 5:  Let vertices v,w satisfy v -> w.
	 *           Then v -> idom(w) or idom(w) -> idom(v)
	 */

static void find_immediate_dominators(struct compile_state *state)
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
	 *   minimum amoung vertices u satisfying sdom(w) -> u -> w.
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
	sd = xcmalloc(sizeof(*sd) * (state->last_vertex + 1), "sdom_state");
	initialize_sdblock(sd, 0, state->first_block, 0);
#if 0
	sd[1].size  = 0;
	sd[1].label = 0;
	sd[1].sdom  = 0;
#endif
	/* Step 2 compute the semidominators */
	/* Step 3 implicitly define the immediate dominator of each vertex */
	compute_sdom(state, sd);
	/* Step 4 explicitly define the immediate dominator of each vertex */
	compute_idom(state, sd);
	xfree(sd);
}

static void find_post_dominators(struct compile_state *state)
{
	struct sdom_block *sd;
	int vertex;
	/* Step 1 initialize the basic block information */
	sd = xcmalloc(sizeof(*sd) * (state->last_vertex + 1), "sdom_state");

	vertex = setup_spdblocks(state, sd);
	if (vertex != state->last_vertex) {
		internal_error(state, 0, "missing %d blocks\n",
			state->last_vertex - vertex);
	}

	/* Step 2 compute the semidominators */
	/* Step 3 implicitly define the immediate dominator of each vertex */
	compute_spdom(state, sd);
	/* Step 4 explicitly define the immediate dominator of each vertex */
	compute_ipdom(state, sd);
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
	struct occurance *ptr, *ptr2;
	const char *filename1, *filename2;
	int equal_filenames;
	int i;
	for(i = 0; i < depth; i++) {
		fprintf(fp, "   ");
	}
	fprintf(fp, "%3d: %p (%p - %p) @", 
		block->vertex, block, block->first, block->last);
	ins = block->first;
	while(ins != block->last && (ins->occurance->line == 0)) {
		ins = ins->next;
	}
	ptr = ins->occurance;
	ptr2 = block->last->occurance;
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

static void print_dominators(struct compile_state *state, FILE *fp)
{
	fprintf(fp, "\ndominates\n");
	walk_blocks(state, print_dominated, fp);
	fprintf(fp, "dominates\n");
	print_dominated2(state, fp, 0, state->first_block);
}


static int print_frontiers(
	struct compile_state *state, struct block *block, int vertex)
{
	struct block_set *user, *edge;

	if (!block || (block->vertex != vertex + 1)) {
		return vertex;
	}
	vertex += 1;

	printf("%d:", block->vertex);
	for(user = block->domfrontier; user; user = user->next) {
		printf(" %d", user->member->vertex);
	}
	printf("\n");
	
	for(edge = block->edges; edge; edge = edge->next) {
		vertex = print_frontiers(state, edge->member, vertex);
	}
	return vertex;
}
static void print_dominance_frontiers(struct compile_state *state)
{
	printf("\ndominance frontiers\n");
	print_frontiers(state, state->first_block, 0);
	
}

static void analyze_idominators(struct compile_state *state)
{
	/* Find the immediate dominators */
	find_immediate_dominators(state);
	/* Find the dominance frontiers */
	find_block_domf(state, state->first_block);
	/* If debuging print the print what I have just found */
	if (state->compiler->debug & DEBUG_FDOMINATORS) {
		print_dominators(state, stdout);
		print_dominance_frontiers(state);
		print_control_flow(state);
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

static void print_ipdominators(struct compile_state *state, FILE *fp)
{
	fprintf(fp, "\nipdominates\n");
	walk_blocks(state, print_ipdominated, fp);
}

static int print_pfrontiers(
	struct compile_state *state, struct block *block, int vertex)
{
	struct block_set *user;

	if (!block || (block->vertex != vertex + 1)) {
		return vertex;
	}
	vertex += 1;

	printf("%d:", block->vertex);
	for(user = block->ipdomfrontier; user; user = user->next) {
		printf(" %d", user->member->vertex);
	}
	printf("\n");
	for(user = block->use; user; user = user->next) {
		vertex = print_pfrontiers(state, user->member, vertex);
	}
	return vertex;
}
static void print_ipdominance_frontiers(struct compile_state *state)
{
	printf("\nipdominance frontiers\n");
	print_pfrontiers(state, state->last_block, 0);
	
}

static void analyze_ipdominators(struct compile_state *state)
{
	/* Find the post dominators */
	find_post_dominators(state);
	/* Find the control dependencies (post dominance frontiers) */
	find_block_ipdomf(state, state->last_block);
	/* If debuging print the print what I have just found */
	if (state->compiler->debug & DEBUG_RDOMINATORS) {
		print_ipdominators(state, stdout);
		print_ipdominance_frontiers(state);
		print_control_flow(state);
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
		ins = sub;
		while((ins != bsub->first) && (ins != dom)) {
			ins = ins->prev;
		}
		result = (ins == dom);
	}
	return result;
}

static void analyze_basic_blocks(struct compile_state *state)
{
	setup_basic_blocks(state);
	analyze_idominators(state);
	analyze_ipdominators(state);
}

static void insert_phi_operations(struct compile_state *state)
{
	size_t size;
	struct triple *first;
	int *has_already, *work;
	struct block *work_list, **work_list_tail;
	int iter;
	struct triple *var, *vnext;

	size = sizeof(int) * (state->last_vertex + 1);
	has_already = xcmalloc(size, "has_already");
	work =        xcmalloc(size, "work");
	iter = 0;

	first = state->first;
	for(var = first->next; var != first ; var = vnext) {
		struct block *block;
		struct triple_set *user, *unext;
		vnext = var->next;
		if ((var->op != OP_ADECL) || !var->use) {
			continue;
		}
		iter += 1;
		work_list = 0;
		work_list_tail = &work_list;
		for(user = var->use; user; user = unext) {
			unext = user->next;
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
				get_occurance(var->occurance);
				phi = alloc_triple(
					state, OP_PHI, var->type, -1, in_edges, 
					var->occurance);
				phi->u.block = front;
				MISC(phi, 0) = var;
				use_triple(var, phi);
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

static int count_adecls(struct compile_state *state)
{
	struct triple *first, *ins;
	int adecls = 0;
	first = state->first;
	ins = first;
	do {
		if (ins->op == OP_ADECL) {
			adecls += 1;
		}
		ins = ins->next;
	} while(ins != first);
	return adecls;
}

static void number_adecls(struct compile_state *state, struct stack *stacks)
{
	struct triple *first, *ins;
	int adecls = 0;
	first = state->first;
	ins = first;
	do {
		if (ins->op == OP_ADECL) {
			adecls += 1;
			stacks[adecls].orig_id = ins->id;
			ins->id = adecls;
		}
		ins = ins->next;
	} while(ins != first);
}

static void restore_adecls(struct compile_state *state, struct stack *stacks)
{
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		if (ins->op == OP_ADECL) {
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
			/* Only free one occurance from the stack */
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
			if (edge >= TRIPLE_RHS(ptr->sizes)) {
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
			unuse_triple(var, ptr);
			/* Find the current value of the variable */
			val = peek_triple(stacks, var);
			if (!val) {
				error(state, ptr, "variable used without being set");
			}
			if ((val->op == OP_WRITE) || (val->op == OP_READ)) {
				internal_error(state, val, "bad value in read");
			}
			propogate_use(state, ptr, val);
			release_triple(state, ptr);
			continue;
		}
		/* LHS(A) */
		if (ptr->op == OP_WRITE) {
			struct triple *var, *val, *tval;
			var = RHS(ptr, 0);
			tval = val = RHS(ptr, 1);
			if ((val->op == OP_WRITE) || (val->op == OP_READ)) {
				internal_error(state, ptr, "bad value in write");
			}
			/* Insert a copy if the types differ */
			if (!equiv_types(ptr->type, val->type)) {
				if (val->op == OP_INTCONST) {
					tval = pre_triple(state, ptr, OP_INTCONST, ptr->type, 0, 0);
					tval->u.cval = val->u.cval;
				}
				else {
					tval = pre_triple(state, ptr, OP_COPY, ptr->type, val, 0);
					use_triple(val, tval);
				}
				transform_to_arch_instruction(state, tval);
				unuse_triple(val, ptr);
				RHS(ptr, 1) = tval;
				use_triple(tval, ptr);
			}
			propogate_use(state, ptr, tval);
			unuse_triple(var, ptr);
			/* Push OP_WRITE ptr->right onto a stack of variable uses */
			push_triple(stacks, var, tval);
		}
		if (ptr->op == OP_PHI) {
			struct triple *var;
			var = MISC(ptr, 0);
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
			var = RHS(ptr, 0);
			/* Pop OP_WRITE ptr->right from the stack of variable uses */
			pop_triple(stacks, var, RHS(ptr, 1));
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
	int adecls;

	/* Allocate stacks for the Variables */
	adecls = count_adecls(state);
	stacks = xcmalloc(sizeof(stacks[0])*(adecls + 1), "adecl stacks");

	/* Give each adecl a stack */
	number_adecls(state, stacks);

	/* Rename the variables */
	rename_block_variables(state, stacks, state->first_block);

	/* Remove the stacks from the adecls */
	restore_adecls(state, stacks);
	xfree(stacks);
}

static void prune_block_variables(struct compile_state *state,
	struct block *block)
{
	struct block_set *user;
	struct triple *next, *last, *ptr;
	int done;
	last = block->first;
	done = 0;
	for(ptr = block->first; !done; ptr = next) {
		next = ptr->next;
		if (ptr == block->last) {
			done = 1;
		}
		if (ptr->op == OP_ADECL) {
			struct triple_set *user, *next;
			for(user = ptr->use; user; user = next) {
				struct triple *use;
				next = user->next;
				use = user->member;
				if (use->op != OP_PHI) {
					internal_error(state, use, "decl still used");
				}
				if (MISC(use, 0) != ptr) {
					internal_error(state, use, "bad phi use of decl");
				}
				unuse_triple(ptr, use);
				MISC(use, 0) = 0;
			}
			release_triple(state, ptr);
			continue;
		}
		last = ptr;
	}
	block->last = last;
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
	zrhs = TRIPLE_RHS(phi->sizes);
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
		zrhs = TRIPLE_RHS(phi->sizes);
		for(j = 0; j < zrhs; j++) {
			if(!slot[j]) {
				error(state, phi, "variable not set on all paths to use");
			}
		}
	}
	xfree(live);
}

static void transform_to_ssa_form(struct compile_state *state)
{
	insert_phi_operations(state);
	rename_variables(state);

	prune_block_variables(state, state->first_block);
	prune_unused_phis(state);

	print_blocks(state, __func__, stdout);
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
		targ = triple_targ(state, block->last, 0);
		for(; targ; targ = triple_targ(state, block->last, targ)) {
			if (!*targ) {
				continue;
			}
			if (!triple_stores_block(state, *targ)) {
				internal_error(state, 0, "bad targ");
			}
			mark_live_block(state, (*targ)->u.block, next_vertex);
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
	walk_blocks(state, clear_vertex, 0);
	next_vertex = 1;
	mark_live_block(state, state->first_block, &next_vertex);

	/* Walk all of the operations to find the phi functions */
	first = state->first;
	for(phi = first->next; phi != first ; phi = next) {
		struct block_set *set;
		struct block *block;
		struct triple **slot;
		struct triple *var;
		struct triple_set *use, *use_next;
		int edge, used;
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
		var = post_triple(state, phi, OP_ADECL, phi->type, 0,0);

		/* Replaces use of phi with var */
		propogate_use(state, phi, var);

		/* Walk all of the incoming edges/blocks and insert moves.
		 */
		used = 0;
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
			if (!val || (val == &zero_triple) || (val == phi) || 
				(!vblock) || (vblock->vertex == 0)) {
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
			base = eblock->first;
			if (block_of_triple(state, val) == eblock) {
				base = val;
			}
			move = post_triple(state, base, OP_WRITE, var->type, var, val);
			use_triple(val, move);
			use_triple(var, move);
			used = 1;
		}		
		/* If var is not used free it */
		if (!used) {
			free_triple(state, var);
		}

		/* Release the phi function */
		release_triple(state, phi);
	}
	
	/* Walk all of the operations to find the adecls */
	for(var = first->next; var != first ; var = var->next) {
		struct triple_set *use, *use_next;
		if (var->op != OP_ADECL) {
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
			zrhs = TRIPLE_RHS(user->sizes);
			slot = &RHS(user, 0);
			for(i = 0; i < zrhs; i++) {
				if ((slot[i] == var) &&
					((i != 0) || (user->op != OP_WRITE))) 
				{
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
	fprintf(stderr, "@ %s:%d\n", __FILE__, __LINE__); romcc_print_blocks(state, stderr); \
	} 

static void rebuild_ssa_form(struct compile_state *state)
{
HI();
	transform_from_ssa_form(state);
HI();
	free_basic_blocks(state);
	analyze_basic_blocks(state);
HI();
	insert_phi_operations(state);
HI();
	rename_variables(state);
HI();
	
	prune_block_variables(state, state->first_block);
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
	zrhs = TRIPLE_RHS(ins->sizes);
	zlhs = TRIPLE_LHS(ins->sizes);
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
	fprintf(stderr, "find_lhs_post_color(%p, %d)\n",
		ins, index);
#endif
	if ((index == 0) && triple_is_def(state, ins)) {
		lhs = ins;
	}
	else if (index < TRIPLE_LHS(ins->sizes)) {
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
		zrhs = TRIPLE_RHS(user->sizes);
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
	fprintf(stderr, "find_lhs_post_color(%p, %d) -> ( %d, %x)\n",
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
	fprintf(stderr, "find_rhs_post_color(%p, %d)\n",
		ins, index);
#endif
	rinfo = arch_reg_rhs(state, ins, index);
	zlhs = TRIPLE_LHS(ins->sizes);
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
	fprintf(stderr, "find_rhs_post_color(%p, %d) -> ( %d, %x)\n",
		ins, index, info.reg, info.regcm);
#endif
	return info;
}

static struct reg_info find_lhs_color(
	struct compile_state *state, struct triple *ins, int index)
{
	struct reg_info pre, post, info;
#if DEBUG_TRIPLE_COLOR
	fprintf(stderr, "find_lhs_color(%p, %d)\n",
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
	fprintf(stderr, "find_lhs_color(%p, %d) -> ( %d, %x) ... (%d, %x) (%d, %x)\n",
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
	if (ins->op == OP_PHI) {
		internal_error(state, ins, "pre_copy on a phi?");
	}
	classes = arch_type_to_regcm(state, type);
	info = arch_reg_rhs(state, ins, index);
	expr = &RHS(ins, index);
	if ((info.regcm & classes) == 0) {
		internal_error(state, ins, "pre_copy with no register classes");
	}
	in = pre_triple(state, ins, OP_COPY, type, *expr, 0);
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

			get_occurance(val->occurance);
			move = build_triple(state, OP_COPY, phi->type, val, 0,
				val->occurance);
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
				if ((ptr == phi) || (ptr == val)) {
					goto out;
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
			insert_triple(state, ptr->next, move);
			if (eblock->last == ptr) {
				eblock->last = move;
			}
			transform_to_arch_instruction(state, move);
		}
	}
	print_blocks(state, __func__, stdout);
}

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
static void unin_triple(struct reg_block *rb, struct triple *unin)
{
	do_triple_unset(&rb->in, unin);
}

static int out_triple(struct reg_block *rb, struct triple *out)
{
	return do_triple_set(&rb->out, out, 0);
}
static void unout_triple(struct reg_block *rb, struct triple *unout)
{
	do_triple_unset(&rb->out, unout);
}

static int initialize_regblock(struct reg_block *blocks,
	struct block *block, int vertex)
{
	struct block_set *user;
	if (!block || (blocks[block->vertex].block == block)) {
		return vertex;
	}
	vertex += 1;
	/* Renumber the blocks in a convinient fashion */
	block->vertex = vertex;
	blocks[vertex].block    = block;
	blocks[vertex].vertex   = vertex;
	for(user = block->use; user; user = user->next) {
		vertex = initialize_regblock(blocks, user->member, vertex);
	}
	return vertex;
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
			if (ptr2 == expr) {
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
			if (ptr == in_set->member) {
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
#warning "FIXME is this O(N^2) algorithm bad?"
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
			rhs = *expr;
			if (!rhs) {
				continue;
			}
			/* See if rhs is defined in this block */
			for(tdone = 0, test = ptr; !tdone; test = test->prev) {
				tdone = (test == block->first);
				if (test == rhs) {
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
	struct compile_state *state)
{
	struct reg_block *blocks;
	int change;
	blocks = xcmalloc(
		sizeof(*blocks)*(state->last_vertex + 1), "reg_block");
	initialize_regblock(blocks, state->last_block, 0);
	do {
		int i;
		change = 0;
		for(i = 1; i <= state->last_vertex; i++) {
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

static void free_variable_lifetimes(
	struct compile_state *state, struct reg_block *blocks)
{
	int i;
	/* free in_set && out_set on each block */
	for(i = 1; i <= state->last_vertex; i++) {
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
	struct reg_block *blocks, wvl_cb_t cb, void *arg)
{
	int i;
	
	for(i = 1; i <= state->last_vertex; i++) {
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
};


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
	struct block *block;
	struct dead_triple *dtriple, *work_list, **work_list_tail, *dt;
	int triples, i;
	struct triple *first, *final, *ins;

	if (!(state->compiler->flags & COMPILER_ELIMINATE_INEFECTUAL_CODE)) {
		return;
	}

	/* Setup the work list */
	work_list = 0;
	work_list_tail = &work_list;

	first = state->first;
	final = state->first->prev;

	/* Count how many triples I have */
	triples = count_triples(state);

	/* Now put then in an array and mark all of the triples dead */
	dtriple = xcmalloc(sizeof(*dtriple) * (triples + 1), "dtriples");
	
	ins = first;
	i = 1;
	block = 0;
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
				internal_warning(state, last, "awakening noop?");
				last = last->prev;
			}
			awaken(state, dtriple, &last, &work_list_tail);
		}
	}
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

	print_blocks(state, __func__, stdout);
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
		info = arch_reg_lhs(state, ins, 0);
		if (info.reg >= MAX_REGISTERS) {
			info.reg = REG_UNSET;
		}
		
		reg = REG_UNSET;
		regcm = arch_type_to_regcm(state, ins->type);
		do_post_copy = do_pre_copy = 0;

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

	print_blocks(state, __func__, stdout);
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
		int op;
		op = ptr->op;
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
		if ((ptr->id < 0) || (ptr->id > rstate->defs)) {
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
	walk_blocks(state, print_interference_block, &info);

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
	fprintf(stderr, "new_live_edge(%p, %p)\n",
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
		fprintf(stderr, "coalescing:");
		lrd = lr1->defs;
		do {
			fprintf(stderr, " %p", lrd->def);
			lrd = lrd->next;
		} while(lrd != lr1->defs);
		fprintf(stderr, " |");
		lrd = lr2->defs;
		do {
			fprintf(stderr, " %p", lrd->def);
			lrd = lrd->next;
		} while(lrd != lr2->defs);
		fprintf(stderr, "\n");
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
		fprintf(stderr, "lr1 post\n");
	}
	if (lr1->defs->orig_id & TRIPLE_FLAG_PRE_SPLIT) {
		fprintf(stderr, "lr1 pre\n");
	}
	if (lr2->defs->orig_id  & TRIPLE_FLAG_POST_SPLIT) {
		fprintf(stderr, "lr2 post\n");
	}
	if (lr2->defs->orig_id & TRIPLE_FLAG_PRE_SPLIT) {
		fprintf(stderr, "lr2 pre\n");
	}
#endif
#if 0
	fprintf(stderr, "coalesce color1(%p): %3d color2(%p) %3d\n",
		lr1->defs->def,
		lr1->color,
		lr2->defs->def,
		lr2->color);
#endif
	
	/* Append lr2 onto lr1 */
#warning "FIXME should this be a merge instead of a splice?"
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

	/* Keep the graph in sync by transfering the edges from lr2 to lr1 */
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

		/* Initalize the live_range_def */
		rstate->lrd[j].next    = &rstate->lrd[j];
		rstate->lrd[j].prev    = &rstate->lrd[j];
		rstate->lrd[j].def     = ins;
		rstate->lrd[j].orig_id = ins->id;
		ins->id = j;

		j++;
		ins = ins->next;
	} while(ins != first);
	rstate->ranges = i;

	/* Make a second pass to handle achitecture specific register
	 * constraints.
	 */
	ins = first;
	do {
		int zlhs, zrhs, i, j;
		if (ins->id > rstate->defs) {
			internal_error(state, ins, "bad id");
		}
		
		/* Walk through the template of ins and coalesce live ranges */
		zlhs = TRIPLE_LHS(ins->sizes);
		if ((zlhs == 0) && triple_is_def(state, ins)) {
			zlhs = 1;
		}
		zrhs = TRIPLE_RHS(ins->sizes);

		if (state->compiler->debug & DEBUG_COALESCING2) {
			fprintf(stderr, "mandatory coalesce: %p %d %d\n",
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
				fprintf(stderr, "coalesce lhs(%d): %p %d\n",
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
					fprintf(stderr, "coalesce rhs(%d): %p %d\n",
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
		if ((entry->member->id < 0) || (entry->member->id > rstate->defs)) {
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


static void print_interference_ins(
	struct compile_state *state, 
	struct reg_block *blocks, struct triple_reg_set *live, 
	struct reg_block *rb, struct triple *ins, void *arg)
{
	struct reg_state *rstate = arg;
	struct live_range *lr;
	unsigned id;

	lr = rstate->lrd[ins->id].lr;
	id = ins->id;
	ins->id = rstate->lrd[id].orig_id;
	SET_REG(ins->id, lr->color);
	display_triple(stdout, ins);
	ins->id = id;

	if (lr->defs) {
		struct live_range_def *lrd;
		printf("       range:");
		lrd = lr->defs;
		do {
			printf(" %-10p", lrd->def);
			lrd = lrd->next;
		} while(lrd != lr->defs);
		printf("\n");
	}
	if (live) {
		struct triple_reg_set *entry;
		printf("        live:");
		for(entry = live; entry; entry = entry->next) {
			printf(" %-10p", entry->member);
		}
		printf("\n");
	}
	if (lr->edges) {
		struct live_range_edge *entry;
		printf("       edges:");
		for(entry = lr->edges; entry; entry = entry->next) {
			struct live_range_def *lrd;
			lrd = entry->node->defs;
			do {
				printf(" %-10p", lrd->def);
				lrd = lrd->next;
			} while(lrd != entry->node->defs);
			printf("|");
		}
		printf("\n");
	}
	if (triple_is_branch(state, ins)) {
		printf("\n");
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
	zlhs = TRIPLE_LHS(ins->sizes);
	if ((zlhs == 0) && triple_is_def(state, ins)) {
		zlhs = 1;
	}
	zrhs = TRIPLE_RHS(ins->sizes);
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
	walk_variable_lifetimes(state, blocks, fix_coalesce_conflicts, &conflicts);
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
#warning "WISHLIST visit just those blocks that need it *"
	for(i = 1; i <= state->last_vertex; i++) {
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
	else if (index < TRIPLE_LHS(ins->sizes)) {
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

#warning "WISHLIST recalculate all affected instructions colors"
	info = find_lhs_color(state, tangle, 0);
	for(set = tangle->use; set; set = next) {
		struct triple *user;
		int i, zrhs;
		next = set->next;
		user = set->member;
		zrhs = TRIPLE_RHS(user->sizes);
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
	walk_variable_lifetimes(state, blocks, fix_tangles, &tangles);
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
#warning "FIXME ignore cases that cannot be fixed (a definition followed by a use)"
		

		/* Of the constrained live ranges deal with the
		 * least dominated one first.
		 */
		if (state->compiler->debug & DEBUG_RANGE_CONFLICTS) {
			fprintf(stderr, "canidate: %p %-8s regcm: %x %x\n",
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
#warning "FIXME should I call find_constrained_def here only if no previous constrained def was found?"
	if (!constrained) {
		constrained = find_constrained_def(state, range, constrained);
	}

	if (state->compiler->debug & DEBUG_RANGE_CONFLICTS) {
		fprintf(stderr, "constrained: %p %-8s\n",
			constrained, tops(constrained->op));
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
		fprintf(stderr, "split_ranges %d %s %p\n", 
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
	 * know issues.  So I have choosen not to write any
	 * code until I cat get a better feel for cases where
	 * it would be useful to have.
	 *
	 */
#warning "WISHLIST implement live range splitting..."
	
	if (!split && (state->compiler->debug & DEBUG_RANGE_CONFLICTS2)) {
		print_interference_blocks(state, rstate, stderr, 0);
		print_dominators(state, stderr);
	}
	return split;
}

static FILE *cgdebug_fp(struct compile_state *state)
{
	FILE *fp;
	fp = 0;
	if (!fp && (state->compiler->debug & DEBUG_COLOR_GRAPH2)) {
		fp = stderr;
	}
	if (!fp && (state->compiler->debug & DEBUG_COLOR_GRAPH)) {
		fp = stdout;
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
		cgdebug_printf(state, " %s\n", arch_reg_str(range->color));
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
			if ((ins->id < 0) || (ins->id > rstate->defs)) {
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
	struct live_range *lr;
	struct triple *first, *ins;
	first = state->first;
	ins = first;
	do {
		if ((ins->id < 0) || (ins->id > rstate->defs)) {
			internal_error(state, ins, 
				"triple without a live range");
		}
		lr = rstate->lrd[ins->id].lr;
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
		print_interference_blocks(state, rstate, stdout, 0);
		print_control_flow(state);
		fflush(stdout);
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
		free_variable_lifetimes(state, rstate->blocks);
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
		int conflicts;
		int tangles;
		int coalesced;

		if (state->compiler->debug & DEBUG_RANGE_CONFLICTS) {
			fprintf(stderr, "pass: %d\n", rstate.passes);
			fflush(stderr);
		}

		/* Restore ids */
		ids_from_rstate(state, &rstate);

		/* Cleanup the temporary data structures */
		cleanup_rstate(state, &rstate);

		/* Compute the variable lifetimes */
		rstate.blocks = compute_variable_lifetimes(state);

		/* Fix invalid mandatory live range coalesce conflicts */
		conflicts = correct_coalesce_conflicts(state, rstate.blocks);

		/* Fix two simultaneous uses of the same register.
		 * In a few pathlogical cases a partial untangle moves
		 * the tangle to a part of the graph we won't revisit.
		 * So we keep looping until we have no more tangle fixes
		 * to apply.
		 */
		do {
			tangles = correct_tangles(state, rstate.blocks);
		} while(tangles);

		
		print_blocks(state, "resolve_tangles", stdout);
		verify_consistency(state);
		
		/* Allocate and initialize the live ranges */
		initialize_live_ranges(state, &rstate);

		/* Note current doing coalescing in a loop appears to 
		 * buys me nothing.  The code is left this way in case
		 * there is some value in it.  Or if a future bugfix
		 *  yields some benefit.
		 */
		do {
			if (state->compiler->debug & DEBUG_COALESCING) {
				fprintf(stderr, "coalescing\n");
			}

			/* Remove any previous live edge calculations */
			cleanup_live_edges(&rstate);

			/* Compute the interference graph */
			walk_variable_lifetimes(
				state, rstate.blocks, graph_ins, &rstate);
			
			/* Display the interference graph if desired */
			if (state->compiler->debug & DEBUG_INTERFERENCE) {
				print_interference_blocks(state, &rstate, stdout, 1);
				printf("\nlive variables by instruction\n");
				walk_variable_lifetimes(
					state, rstate.blocks, 
					print_interference_ins, &rstate);
			}
			
			coalesced = coalesce_live_ranges(state, &rstate);

			if (state->compiler->debug & DEBUG_COALESCING) {
				fprintf(stderr, "coalesced: %d\n", coalesced);
			}
		} while(coalesced);

#if DEBUG_CONSISTENCY > 1
# if 0
		fprintf(stderr, "verify_graph_ins...\n");
# endif
		/* Verify the interference graph */
		walk_variable_lifetimes(
			state, rstate.blocks, verify_graph_ins, &rstate);
# if 0
		fprintf(stderr, "verify_graph_ins done\n");
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
	print_blocks(state, __func__, stdout);
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
	/* lattice high   val && !is_const(val) 
	 * lattice const  is_const(val)
	 * lattice low    val == 0
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


static void scc_add_fedge(struct compile_state *state, struct scc_state *scc, 
	struct flow_edge *fedge)
{
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
		fprintf(stderr, "adding fedge: %p (%4d -> %5d)\n",
			fedge,
			fedge->src->block?fedge->src->block->last->id: 0,
			fedge->dst->block?fedge->dst->block->first->id: 0);
	}
	if ((fedge == scc->flow_work_list) ||
		(fedge->work_next != fedge) ||
		(fedge->work_prev != fedge)) {

		if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
			fprintf(stderr, "dupped fedge: %p\n",
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
		fprintf(stderr, "adding sedge: %5d (%4d -> %5d)\n",
			sedge - scc->ssa_edges,
			sedge->src->def->id,
			sedge->dst->def->id);
	}
	if ((sedge == scc->ssa_work_list) ||
		(sedge->work_next != sedge) ||
		(sedge->work_prev != sedge)) {

		if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
			fprintf(stderr, "dupped sedge: %5d\n",
				sedge - scc->ssa_edges);
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
		fprintf(stderr, "ins_count: %d ssa_edge_count: %d vertex_count: %d\n",
			ins_count, ssa_edge_count, state->last_vertex);
	}
	scc->ins_count   = ins_count;
	scc->lattice     = 
		xcmalloc(sizeof(*scc->lattice)*(ins_count + 1), "lattice");
	scc->ssa_edges   = 
		xcmalloc(sizeof(*scc->ssa_edges)*(ssa_edge_count + 1), "ssa_edges");
	scc->flow_blocks = 
		xcmalloc(sizeof(*scc->flow_blocks)*(state->last_vertex + 1), 
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
		dst = &scc->flow_blocks[state->first_block->vertex];
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
		fprintf(stderr, "ins_index: %d ssa_edge_index: %d fblock_index: %d\n",
			ins_index, ssa_edge_index, fblock_index);
	}
}

	
static void free_scc_state(
	struct compile_state *state, struct scc_state *scc)
{
	int i;
	for(i = 0; i < state->last_vertex + 1; i++) {
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
	if (changed && lnode->val && !is_const(lnode->val)) {
		changed = 0;
	}
	if (changed &&
		lnode->val && old &&
		(memcmp(lnode->val->param, old->param,
			TRIPLE_SIZE(lnode->val->sizes) * sizeof(lnode->val->param[0])) == 0) &&
		(memcmp(&lnode->val->u, &old->u, sizeof(old->u)) == 0)) {
		changed = 0;
	}
	if (old) {
		xfree(old);
	}
	return changed;

}

static void scc_debug_lnode(
	struct compile_state *state, struct lattice_node *lnode, int changed)
{
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
		FILE *fp = stderr;
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
			((!lnode->val)? "lo": is_const(lnode->val)? "const": "hi"),
			changed? "changed" : ""
			);
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
			fprintf(stderr, "Examining edge: %d vertex: %d executable: %d\n", 
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
		if (!tmp->val) {
			lnode->val = 0;
		}
		/* meet(X, lattice high) = X */
		else if (!tmp->val) {
			lnode->val = lnode->val;
		}
		/* meet(lattice high, X) = X */
		else if (!is_const(lnode->val)) {
			lnode->val = dup_triple(state, tmp->val);
			lnode->val->type = lnode->def->type;
		}
		/* meet(const, const) = const or lattice low */
		else if (!constants_equal(state, lnode->val, tmp->val)) {
			lnode->val = 0;
		}
		if (!lnode->val) {
			break;
		}
	}
	changed = lval_changed(state, old, lnode);
	scc_debug_lnode(state, lnode, changed);

	/* If the lattice value has changed update the work lists. */
	if (changed) {
		struct ssa_edge *sedge;
		for(sedge = lnode->out; sedge; sedge = sedge->out_next) {
			scc_add_sedge(state, scc, sedge);
		}
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

	count = TRIPLE_SIZE(scratch->sizes);
	for(i = 0; i < count; i++) {
		dexpr = &lnode->def->param[i];
		vexpr = &scratch->param[i];
		*vexpr = *dexpr;
		if (((i < TRIPLE_MISC_OFF(scratch->sizes)) ||
			(i >= TRIPLE_TARG_OFF(scratch->sizes))) &&
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
#warning "FIXME see if simplify does anything bad"
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
	count = TRIPLE_SIZE(scratch->sizes);
	for(i = 0; i < count; i++) {
		vexpr = &scratch->param[i];
		if (*vexpr) {
			unuse_triple(*vexpr, scratch);
		}
	}
	if (!is_const(scratch)) {
		for(i = 0; i < count; i++) {
			dexpr = &lnode->def->param[i];
			if (((i < TRIPLE_MISC_OFF(scratch->sizes)) ||
				(i >= TRIPLE_TARG_OFF(scratch->sizes))) &&
				*dexpr) {
				struct lattice_node *tmp;
				tmp = triple_to_lattice(state, scc, *dexpr);
				if (!tmp->val) {
					lnode->val = 0;
				}
			}
		}
	}
	if (lnode->val && 
		(lnode->val->op == lnode->def->op) &&
		(memcmp(lnode->val->param, lnode->def->param, 
			count * sizeof(lnode->val->param[0])) == 0) &&
		(memcmp(&lnode->val->u, &lnode->def->u, sizeof(lnode->def->u)) == 0)) {
		lnode->val = lnode->def;
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
	if (lnode->val && 
		((	!triple_is_def(state, lnode->def)  &&
			!triple_is_cond_branch(state, lnode->def)) ||
			(lnode->def->op == OP_PIECE))) {
#warning "FIXME constant propogate through expressions with multiple left hand sides"
		if (changed) {
			internal_warning(state, lnode->def, "non def changes value?");
		}
		lnode->val = 0;
	}
	/* Report what has just happened */
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM2) {
		display_triple_changes(stderr, scratch, lnode->def);
	}

	/* See if we need to free the scratch value */
	if (lnode->val != scratch) {
		xfree(scratch);
	}
	return changed;
}

static void scc_visit_branch(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	struct lattice_node *cond;
	struct flow_edge *left, *right;
	if (state->compiler->debug & DEBUG_SCC_TRANSFORM) {
		struct flow_edge *fedge;
		fprintf(stderr, "%s: %d (",
			tops(lnode->def->op),
			lnode->def->id);
		
		for(fedge = lnode->fblock->out; fedge; fedge = fedge->out_next) {
			fprintf(stderr, " %d", fedge->dst->block->vertex);
		}
		fprintf(stderr, " )");
		if (TRIPLE_RHS(lnode->def->sizes) > 0) {
			fprintf(stderr, " <- %d",
				RHS(lnode->def, 0)->id);
		}
		fprintf(stderr, "\n");
	}
	if (!triple_is_branch(state, lnode->def)) {
		internal_error(state, lnode->def, "not branch");
	}
	/* This only applies to conditional branches */
	if (!triple_is_cond_branch(state, lnode->def)) {
		return;
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
	if (cond->val && !is_const(cond->val)) {
#warning "FIXME do I need to do something here?"
		warning(state, cond->def, "condition not constant?");
		return;
	}
	if (cond->val == 0) {
		scc_add_fedge(state, scc, left);
		scc_add_fedge(state, scc, right);
	}
	else if (cond->val->u.cval) {
		scc_add_fedge(state, scc, right);
	} else {
		scc_add_fedge(state, scc, left);
	}

}

static void scc_visit_expr(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	int changed;

	changed = compute_lnode_val(state, scc, lnode);
	scc_debug_lnode(state, lnode, changed);

	if (triple_is_branch(state, lnode->def)) {
		scc_visit_branch(state, scc, lnode);
	}
	else if (changed) {
		struct ssa_edge *sedge;
		for(sedge = lnode->out; sedge; sedge = sedge->out_next) {
			scc_add_sedge(state, scc, sedge);
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
			if (lnode->val && 
				!is_const(lnode->val) &&
				!triple_is_uncond_branch(state, lnode->val) &&
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
					internal_warning(state, lnode->val,
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
				fprintf(stderr, "vertex: %d reps: %d\n", 
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
				else if (reps == 1) {
					scc_visit_expr(state, &scc, lnode);
				}
			}
			/* Add unconditional branch edges */
			if (!triple_is_cond_branch(state, fblock->block->last)) {
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
				fprintf(stderr, "sedge: %5d (%5d -> %5d)\n",
					sedge - scc.ssa_edges,
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
	
	print_blocks(state, __func__, stdout);
}


static void transform_to_arch_instructions(struct compile_state *state)
{
	struct triple *ins, *first;
	first = state->first;
	ins = first;
	do {
		ins = transform_to_arch_instruction(state, ins);
	} while(ins != first);
	
	print_blocks(state, __func__, stdout);
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
		ins = ins->next;
	} while(ins != first);
	
}
static void verify_blocks_present(struct compile_state *state)
{
	struct triple *first, *ins;
	if (!state->first_block) {
		return;
	}
	first = state->first;
	ins = first;
	do {
		valid_ins(state, ins);
		if (triple_stores_block(state, ins)) {
			if (!ins->u.block) {
				internal_error(state, ins, 
					"%p not in a block?\n", ins);
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
	block = state->first_block;
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
			if ((block == state->last_block) &&
				(user->member == state->first_block)) {
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
			expr = triple_targ(state, block->last, 0);
			for(;expr; expr = triple_targ(state, block->last, expr)) {
				if (*expr && !edge_present(state, block, *expr)) {
					internal_error(state, block->last, "no edge to targ");
				}
			}
		}
		if (!triple_is_uncond_branch(state, block->last) &&
			(block != state->last_block) &&
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
				"computed users %d != stored users %d\n",
				users, block->users);
		}
		if (!triple_stores_block(state, block->last->next)) {
			internal_error(state, block->last->next, 
				"cannot find next block");
		}
		block = block->last->next->u.block;
		if (!block) {
			internal_error(state, block->last->next,
				"bad next block");
		}
	} while(block != state->first_block);
	if (blocks != state->last_vertex) {
		internal_error(state, 0, "computed blocks != stored blocks %d\n",
			blocks, state->last_vertex);
	}
}

static void verify_domination(struct compile_state *state)
{
	struct triple *first, *ins;
	struct triple_set *set;
	if (!state->first_block) {
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
			zrhs = TRIPLE_RHS(set->member->sizes);
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
							"no edge for phi rhs %d\n", i);
					}
					use_point = bset->member->last;
				}
			}
			if (use_point &&
				!tdominates(state, ins, use_point)) {
				internal_error(state, use_point, 
					"non dominated rhs use point?");
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
		zrhs = TRIPLE_RHS(ins->sizes);
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
		lhs = TRIPLE_LHS(ins->sizes);
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
static void verify_consistency(struct compile_state *state)
{
	verify_uses(state);
	verify_blocks_present(state);
	verify_blocks(state);
	verify_domination(state);
	verify_rhs(state);
	verify_piece(state);
	verify_ins_colors(state);
}
#else 
static void verify_consistency(struct compile_state *state) {}
#endif /* DEBUG_CONSISTENCY */

static void optimize(struct compile_state *state)
{
	/* Dump what the instruction graph intially looks like */
	print_triples(state);

	/* Replace structures with simpler data types */
	flatten_structures(state);
	print_triples(state);

	verify_consistency(state);
	/* Analize the intermediate code */
	analyze_basic_blocks(state);

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
	/* Propogate constants throughout the code */
	scc_transform(state);
	verify_consistency(state);
#warning "WISHLIST implement single use constants (least possible register pressure)"
#warning "WISHLIST implement induction variable elimination"
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
	free_basic_blocks(state);
}

static void print_op_asm(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	struct asm_info *info;
	const char *ptr;
	unsigned lhs, rhs, i;
	info = ins->u.ainfo;
	lhs = TRIPLE_LHS(ins->sizes);
	rhs = TRIPLE_RHS(ins->sizes);
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
#define X86_MMX_REGS (1<<0)
#define X86_XMM_REGS (1<<1)

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
#warning "WISHLIST figure out how to use pinsrw and pextrw to better use extended regs"
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

static void init_arch_state(struct arch_state *arch)
{
	memset(arch, 0, sizeof(*arch));
	arch->features = 0;
}

static int arch_encode_flag(struct arch_state *arch, const char *flag)
{
	static const struct compiler_flag flags[] = {
		{ "mmx", X86_MMX_REGS },
		{ "sse", X86_XMM_REGS },
		{ 0,     0 },
	};
	static const struct compiler_flag cpus[] = {
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
	int result;
	int act;

	act = 1;
	result = -1;
	if (strncmp(flag, "no-", 3) == 0) {
		flag += 3;
		act = 0;
	}
	if (act && strncmp(flag, "cpu=", 4) == 0) {
		flag += 4;
		result = set_flag(cpus, &arch->features, 1, flag);
	}
	else {
		result = set_flag(flags, &arch->features, act, flag);
	}
	return result;
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
	else if (strcmp(clobber, "%eax") == 0) {
		result.reg = REG_EAX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "%ebx") == 0) {
		result.reg = REG_EBX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "%ecx") == 0) {
		result.reg = REG_ECX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "%edx") == 0) {
		result.reg = REG_EDX;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "%esi") == 0) {
		result.reg = REG_ESI;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "%edi") == 0) {
		result.reg = REG_EDI;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "%ebp") == 0) {
		result.reg = REG_EBP;
		result.regcm = REGCM_GPR32;
	}
	else if (strcmp(clobber, "%esp") == 0) {
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
	else if ((strncmp(clobber, "mmx", 3) == 0) &&
		octdigitp(clobber[3]) && (clobber[4] == '\0')) {
		result.reg = REG_MMX0 + octdigval(clobber[3]);
		result.regcm = REGCM_MMX;
	}
	else {
		error(state, 0, "Invalid register clobber");
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
#warning "FIXME force types smaller (if legal) before I get here"
	unsigned mask;
	mask = 0;
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
	default:
		internal_error(state, 0, "no register class for type");
		break;
	}
	mask = arch_regcm_normalize(state, mask);
	return mask;
}

static int is_imm32(struct triple *imm)
{
	return ((imm->op == OP_INTCONST) && (imm->u.cval <= 0xffffffffUL)) ||
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
#define TEMPLATE_COPY8_REG     3
#define TEMPLATE_COPY16_REG    4
#define TEMPLATE_COPY32_REG    5
#define TEMPLATE_COPY_IMM8     6
#define TEMPLATE_COPY_IMM16    7
#define TEMPLATE_COPY_IMM32    8
#define TEMPLATE_PHI8          9
#define TEMPLATE_PHI16        10
#define TEMPLATE_PHI32        11
#define TEMPLATE_STORE8       12
#define TEMPLATE_STORE16      13
#define TEMPLATE_STORE32      14
#define TEMPLATE_LOAD8        15
#define TEMPLATE_LOAD16       16
#define TEMPLATE_LOAD32       17
#define TEMPLATE_BINARY8_REG  18
#define TEMPLATE_BINARY16_REG 19
#define TEMPLATE_BINARY32_REG 20
#define TEMPLATE_BINARY8_IMM  21
#define TEMPLATE_BINARY16_IMM 22
#define TEMPLATE_BINARY32_IMM 23
#define TEMPLATE_SL8_CL       24
#define TEMPLATE_SL16_CL      25
#define TEMPLATE_SL32_CL      26
#define TEMPLATE_SL8_IMM      27
#define TEMPLATE_SL16_IMM     28
#define TEMPLATE_SL32_IMM     29
#define TEMPLATE_UNARY8       30
#define TEMPLATE_UNARY16      31
#define TEMPLATE_UNARY32      32
#define TEMPLATE_CMP8_REG     33
#define TEMPLATE_CMP16_REG    34
#define TEMPLATE_CMP32_REG    35
#define TEMPLATE_CMP8_IMM     36
#define TEMPLATE_CMP16_IMM    37
#define TEMPLATE_CMP32_IMM    38
#define TEMPLATE_TEST8        39
#define TEMPLATE_TEST16       40
#define TEMPLATE_TEST32       41
#define TEMPLATE_SET          42
#define TEMPLATE_JMP          43
#define TEMPLATE_RET          44
#define TEMPLATE_INB_DX       45
#define TEMPLATE_INB_IMM      46
#define TEMPLATE_INW_DX       47
#define TEMPLATE_INW_IMM      48
#define TEMPLATE_INL_DX       49
#define TEMPLATE_INL_IMM      50
#define TEMPLATE_OUTB_DX      51
#define TEMPLATE_OUTB_IMM     52
#define TEMPLATE_OUTW_DX      53
#define TEMPLATE_OUTW_IMM     54
#define TEMPLATE_OUTL_DX      55
#define TEMPLATE_OUTL_IMM     56
#define TEMPLATE_BSF          57
#define TEMPLATE_RDMSR        58
#define TEMPLATE_WRMSR        59
#define TEMPLATE_UMUL8        60
#define TEMPLATE_UMUL16       61
#define TEMPLATE_UMUL32       62
#define TEMPLATE_DIV8         63
#define TEMPLATE_DIV16        64
#define TEMPLATE_DIV32        65
#define LAST_TEMPLATE       TEMPLATE_DIV32
#if LAST_TEMPLATE >= MAX_TEMPLATES
#error "MAX_TEMPLATES to low"
#endif

#define COPY8_REGCM     (REGCM_DIVIDEND64 | REGCM_DIVIDEND32 | REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO | REGCM_MMX | REGCM_XMM)
#define COPY16_REGCM    (REGCM_DIVIDEND64 | REGCM_DIVIDEND32 | REGCM_GPR32 | REGCM_GPR16 | REGCM_MMX | REGCM_XMM)  
#define COPY32_REGCM    (REGCM_DIVIDEND64 | REGCM_DIVIDEND32 | REGCM_GPR32 | REGCM_MMX | REGCM_XMM)


static struct ins_template templates[] = {
	[TEMPLATE_NOP]      = {},
	[TEMPLATE_INTCONST8] = { 
		.lhs = { [0] = { REG_UNNEEDED, REGCM_IMM8 } },
	},
	[TEMPLATE_INTCONST32] = { 
		.lhs = { [0] = { REG_UNNEEDED, REGCM_IMM32 } },
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
		.rhs = { 
			[ 0] = { REG_VIRT0, COPY8_REGCM },
			[ 1] = { REG_VIRT0, COPY8_REGCM },
			[ 2] = { REG_VIRT0, COPY8_REGCM },
			[ 3] = { REG_VIRT0, COPY8_REGCM },
			[ 4] = { REG_VIRT0, COPY8_REGCM },
			[ 5] = { REG_VIRT0, COPY8_REGCM },
			[ 6] = { REG_VIRT0, COPY8_REGCM },
			[ 7] = { REG_VIRT0, COPY8_REGCM },
			[ 8] = { REG_VIRT0, COPY8_REGCM },
			[ 9] = { REG_VIRT0, COPY8_REGCM },
			[10] = { REG_VIRT0, COPY8_REGCM },
			[11] = { REG_VIRT0, COPY8_REGCM },
			[12] = { REG_VIRT0, COPY8_REGCM },
			[13] = { REG_VIRT0, COPY8_REGCM },
			[14] = { REG_VIRT0, COPY8_REGCM },
			[15] = { REG_VIRT0, COPY8_REGCM },
		}, },
	[TEMPLATE_PHI16] = { 
		.lhs = { [0] = { REG_VIRT0, COPY16_REGCM } },
		.rhs = { 
			[ 0] = { REG_VIRT0, COPY16_REGCM },
			[ 1] = { REG_VIRT0, COPY16_REGCM },
			[ 2] = { REG_VIRT0, COPY16_REGCM },
			[ 3] = { REG_VIRT0, COPY16_REGCM },
			[ 4] = { REG_VIRT0, COPY16_REGCM },
			[ 5] = { REG_VIRT0, COPY16_REGCM },
			[ 6] = { REG_VIRT0, COPY16_REGCM },
			[ 7] = { REG_VIRT0, COPY16_REGCM },
			[ 8] = { REG_VIRT0, COPY16_REGCM },
			[ 9] = { REG_VIRT0, COPY16_REGCM },
			[10] = { REG_VIRT0, COPY16_REGCM },
			[11] = { REG_VIRT0, COPY16_REGCM },
			[12] = { REG_VIRT0, COPY16_REGCM },
			[13] = { REG_VIRT0, COPY16_REGCM },
			[14] = { REG_VIRT0, COPY16_REGCM },
			[15] = { REG_VIRT0, COPY16_REGCM },
		}, },
	[TEMPLATE_PHI32] = { 
		.lhs = { [0] = { REG_VIRT0, COPY32_REGCM } },
		.rhs = { 
			[ 0] = { REG_VIRT0, COPY32_REGCM },
			[ 1] = { REG_VIRT0, COPY32_REGCM },
			[ 2] = { REG_VIRT0, COPY32_REGCM },
			[ 3] = { REG_VIRT0, COPY32_REGCM },
			[ 4] = { REG_VIRT0, COPY32_REGCM },
			[ 5] = { REG_VIRT0, COPY32_REGCM },
			[ 6] = { REG_VIRT0, COPY32_REGCM },
			[ 7] = { REG_VIRT0, COPY32_REGCM },
			[ 8] = { REG_VIRT0, COPY32_REGCM },
			[ 9] = { REG_VIRT0, COPY32_REGCM },
			[10] = { REG_VIRT0, COPY32_REGCM },
			[11] = { REG_VIRT0, COPY32_REGCM },
			[12] = { REG_VIRT0, COPY32_REGCM },
			[13] = { REG_VIRT0, COPY32_REGCM },
			[14] = { REG_VIRT0, COPY32_REGCM },
			[15] = { REG_VIRT0, COPY32_REGCM },
		}, },
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
			if (TRIPLE_RHS(cmp->sizes) > 1) {
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
	struct triple *set;

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
	set = post_triple(state, ins, set_op, &char_type, ins, 0);
	use_triple(ins, set);
	set->template_id = TEMPLATE_SET;

	for(entry = ins->use; entry; entry = next) {
		next = entry->next;
		if (entry->member == set) {
			continue;
		}
		replace_rhs_use(state, ins, set, entry->member);
	}
	fixup_branches(state, ins, set, jmp_op);
}

static struct triple *after_lhs(struct compile_state *state, struct triple *ins)
{
	struct triple *next;
	int lhs, i;
	lhs = TRIPLE_LHS(ins->sizes);
	for(next = ins->next, i = 0; i < lhs; i++, next = next->next) {
		if (next != LHS(ins, i)) {
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
	}
	return next;
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
	zlhs = TRIPLE_LHS(ins->sizes);
	if (triple_is_def(state, ins)) {
		zlhs = 1;
	}
	if (index >= zlhs) {
		internal_error(state, ins, "index %d out of range for %s\n",
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
	if ((index > TRIPLE_RHS(ins->sizes)) ||
		(ins->op == OP_PIECE)) {
		internal_error(state, ins, "index %d out of range for %s\n",
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
	struct triple *div, *piece0, *piece1;
	
	/* Generate a piece to hold the remainder */
	piece1 = post_triple(state, ins, OP_PIECE, ins->type, 0, 0);
	piece1->u.cval = 1;

	/* Generate a piece to hold the quotient */
	piece0 = post_triple(state, ins, OP_PIECE, ins->type, 0, 0);
	piece0->u.cval = 0;

	/* Generate the appropriate division instruction */
	div = post_triple(state, ins, div_op, ins->type, 0, 0);
	RHS(div, 0) = RHS(ins, 0);
	RHS(div, 1) = RHS(ins, 1);
	LHS(div, 0) = piece0;
	LHS(div, 1) = piece1;
	div->template_id  = TEMPLATE_DIV32;
	use_triple(RHS(div, 0), div);
	use_triple(RHS(div, 1), div);
	use_triple(LHS(div, 0), div);
	use_triple(LHS(div, 1), div);

	/* Hook on piece0 */
	MISC(piece0, 0) = div;
	use_triple(div, piece0);

	/* Hook on piece1 */
	MISC(piece1, 0) = div;
	use_triple(div, piece1);
	
	/* Replate uses of ins with the appropriate piece of the div */
	propogate_use(state, ins, LHS(div, index));
	release_triple(state, ins);

	/* Return the address of the next instruction */
	return piece1->next;
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
	struct triple *next;
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
	case OP_NOOP:
	case OP_SDECL:
	case OP_BLOBCONST:
	case OP_LABEL:
		ins->template_id = TEMPLATE_NOP;
		break;
	case OP_COPY:
		size = size_of(state, ins->type);
		if (is_imm8(RHS(ins, 0)) && (size <= 1)) {
			ins->template_id = TEMPLATE_COPY_IMM8;
		}
		else if (is_imm16(RHS(ins, 0)) && (size <= 2)) {
			ins->template_id = TEMPLATE_COPY_IMM16;
		}
		else if (is_imm32(RHS(ins, 0)) && (size <= 4)) {
			ins->template_id = TEMPLATE_COPY_IMM32;
		}
		else if (is_const(RHS(ins, 0))) {
			internal_error(state, ins, "bad constant passed to copy");
		}
		else if (size <= 1) {
			ins->template_id = TEMPLATE_COPY8_REG;
		}
		else if (size <= 2) {
			ins->template_id = TEMPLATE_COPY16_REG;
		}
		else if (size <= 4) {
			ins->template_id = TEMPLATE_COPY32_REG;
		}
		else {
			internal_error(state, ins, "bad type passed to copy");
		}
		break;
	case OP_PHI:
		size = size_of(state, ins->type);
		if (size <= 1) {
			ins->template_id = TEMPLATE_PHI8;
		}
		else if (size <= 2) {
			ins->template_id = TEMPLATE_PHI16;
		}
		else if (size <= 4) {
			ins->template_id = TEMPLATE_PHI32;
		}
		else {
			internal_error(state, ins, "bad type passed to phi");
		}
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
		/* FIXME UMUL does not work yet.. */
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
		} else if (size_of(state, RHS(ins, 1)->type) > 1) {
			typed_pre_copy(state, &char_type, ins, 1);
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
		/* Unhandled instructions */
	case OP_PIECE:
	default:
		internal_error(state, ins, "unhandled ins: %d %s\n",
			ins->op, tops(ins->op));
		break;
	}
	return next;
}

static long next_label(struct compile_state *state)
{
	static long label_counter = 0;
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

static const char *arch_reg_str(int reg)
{
#if REG_XMM7 != 44
#error "Registers have renumberd fix arch_reg_str"
#endif
	static const char *regs[] = {
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
	if (!((reg >= REG_EFLAGS) && (reg <= REG_XMM7))) {
		reg = 0;
	}
	return regs[reg];
}


static const char *reg(struct compile_state *state, struct triple *triple,
	int classes)
{
	int reg;
	reg = check_reg(state, triple, classes);
	return arch_reg_str(reg);
}

const char *type_suffix(struct compile_state *state, struct type *type)
{
	const char *suffix;
	switch(size_of(state, type)) {
	case 1: suffix = "b"; break;
	case 2: suffix = "w"; break;
	case 4: suffix = "l"; break;
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
			internal_error(state, ins, "Unknown constant type");
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
		size = size_of(state, ins->type);
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
	struct compile_state *state, struct triple *ins, FILE *fp)
{
	long ref;
	ref = next_label(state);
	fprintf(fp, ".section \"" DATA_SECTION "\"\n");
	fprintf(fp, ".balign %d\n", align_of(state, ins->type));
	fprintf(fp, "L%s%lu:\n", state->compiler->label_prefix, ref);
	print_const(state, ins, fp);
	fprintf(fp, ".section \"" TEXT_SECTION "\"\n");
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
	mask = 0;
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
	mask = 0;
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
	 */
	int omit_copy = 1; /* Is it o.k. to omit a noop copy? */
	struct triple *dst, *src;
	if (ins->op == OP_COPY) {
		src = RHS(ins, 0);
		dst = ins;
	}
	else {
		internal_error(state, ins, "unknown move operation");
		src = dst = 0;
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
		else {
			internal_error(state, ins, "unknown copy type");
		}
	}
	else {
		int dst_reg;
		int dst_regcm;
		dst_reg = ID_REG(dst->id);
		dst_regcm = arch_reg_regcm(state, dst_reg);
		if (dst_regcm & (REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO)) {
			fprintf(fp, "\tmov ");
			print_const_val(state, src, fp);
			fprintf(fp, ", %s\n",
				reg(state, dst, REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8_LO));
		}
		else if (dst_regcm & REGCM_DIVIDEND64) {
			if (size_of(state, dst->type) > 4) {
				internal_error(state, ins, "64bit constant...");
			}
			fprintf(fp, "\tmov $0, %%edx\n");
			fprintf(fp, "\tmov ");
			print_const_val(state, src, fp);
			fprintf(fp, ", %%eax\n");
		}
		else if (dst_regcm & REGCM_DIVIDEND32) {
			if (size_of(state, dst->type) > 2) {
				internal_error(state, ins, "32bit constant...");
			}
			fprintf(fp, "\tmov $0, %%dx\n");
			fprintf(fp, "\tmov ");
			print_const_val(state, src, fp);
			fprintf(fp, ", %%ax");
		}
		else if (dst_regcm & (REGCM_XMM | REGCM_MMX)) {
			long ref;
			ref = get_const_pool_ref(state, src, fp);
			fprintf(fp, "\tmovd L%s%lu, %s\n",
				state->compiler->label_prefix, ref,
				reg(state, dst, (REGCM_XMM | REGCM_MMX)));
		}
		else {
			internal_error(state, ins, "unknown copy immediate type");
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
		if (TRIPLE_RHS(branch->sizes) != 0) {
			internal_error(state, branch, "jmp with condition?");
		}
		bop = "jmp";
	}
	else {
		struct triple *ptr;
		if (TRIPLE_RHS(branch->sizes) != 1) {
			internal_error(state, branch, "jmpcc without condition?");
		}
		check_reg(state, RHS(branch, 0), REGCM_FLAGS);
		if ((RHS(branch, 0)->op != OP_CMP) &&
			(RHS(branch, 0)->op != OP_TEST)) {
			internal_error(state, branch, "bad branch test");
		}
#warning "FIXME I have observed instructions between the test and branch instructions"
		ptr = RHS(branch, 0);
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
	if (TRIPLE_RHS(set->sizes) != 1) {
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
	fprintf(fp, ".balign %d\n", align_of(state, ins->type));
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
	case OP_INTCONST:
	case OP_ADDRCONST:
	case OP_BLOBCONST:
		/* Don't generate anything here for constants */
	case OP_PHI:
		/* Don't generate anything for variable declarations. */
		break;
	case OP_SDECL:
		print_sdecl(state, ins, fp);
		break;
	case OP_COPY:	
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
	struct occurance *last_occurance;
	FILE *fp;
	int max_inline_depth;
	max_inline_depth = 0;
	print_location = 1;
	last_occurance = 0;
	fp = state->output;
	fprintf(fp, ".section \"" TEXT_SECTION "\"\n");
	first = state->first;
	ins = first;
	do {
		if (print_location && 
			last_occurance != ins->occurance) {
			if (!ins->occurance->parent) {
				fprintf(fp, "\t/* %s,%s:%d.%d */\n",
					ins->occurance->function,
					ins->occurance->filename,
					ins->occurance->line,
					ins->occurance->col);
			}
			else {
				struct occurance *ptr;
				int inline_depth;
				fprintf(fp, "\t/*\n");
				inline_depth = 0;
				for(ptr = ins->occurance; ptr; ptr = ptr->parent) {
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
			if (last_occurance) {
				put_occurance(last_occurance);
			}
			get_occurance(ins->occurance);
			last_occurance = ins->occurance;
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

static void print_tokens(struct compile_state *state)
{
	struct token *tk;
	tk = &state->token[0];
	do {
#if 1
		token(state, 0);
#else
		next_token(state, 0);
#endif
		loc(stdout, state, 0);
		printf("%s <- `%s'\n",
			tokens[tk->tok],
			tk->ident ? tk->ident->name :
			tk->str_len ? tk->val.str : "");
		
	} while(tk->tok != TOK_EOF);
}

static void compile(const char *filename, 
	struct compiler_state *compiler, struct arch_state *arch)
{
	int i;
	struct compile_state state;
	struct triple *ptr;
	memset(&state, 0, sizeof(state));
	state.compiler = compiler;
	state.arch     = arch;
	state.file = 0;
	for(i = 0; i < sizeof(state.token)/sizeof(state.token[0]); i++) {
		memset(&state.token[i], 0, sizeof(state.token[i]));
		state.token[i].tok = -1;
	}
	/* Remember the output filename */
	state.output    = fopen(state.compiler->ofilename, "w");
	if (!state.output) {
		error(&state, 0, "Cannot open output file %s\n",
			state.compiler->ofilename);
	}
	/* Prep the preprocessor */
	state.if_depth = 0;
	state.if_value = 0;
	/* register the C keywords */
	register_keywords(&state);
	/* register the keywords the macro preprocessor knows */
	register_macro_keywords(&state);
	/* Memorize where some special keywords are. */
	state.i_switch   = lookup(&state, "switch", 6);
	state.i_case     = lookup(&state, "case", 4);
	state.i_continue = lookup(&state, "continue", 8);
	state.i_break    = lookup(&state, "break", 5);
	state.i_default  = lookup(&state, "default", 7);
	state.i_return   = lookup(&state, "return", 6);

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
#if 0
	print_tokens(&state);
#endif	
	decls(&state);

	/* Exit the global definition scope */
	end_scope(&state);

	/* Join all of the functions into one giant function */
	join_functions(&state);

	/* Now that basic compilation has happened 
	 * optimize the intermediate code 
	 */
	optimize(&state);

	generate_code(&state);
	if (state.compiler->debug) {
		fprintf(stderr, "done\n");
	}
}

static void version(void)
{
	printf("romcc " VERSION " released " RELEASE_DATE "\n");
}

static void usage(void)
{
	version();
	printf(
		"Usage: romcc <source>.c\n"
		"Compile a C source file without using ram\n"
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

int main(int argc, char **argv)
{
	const char *filename;
	struct compiler_state compiler;
	struct arch_state arch;
	int all_opts;
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
			else if (strncmp(argv[1],"-O", 2) == 0) {
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
