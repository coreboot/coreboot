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
#include <ctype.h>
#include <limits.h>

#define DEBUG_ERROR_MESSAGES 0
#define DEBUG_COLOR_GRAPH 0
#define DEBUG_SCC 0
#define X86_4_8BIT_GPRS 1

#warning "FIXME static constant variables"
#warning "FIXME enable pointers"
#warning "FIXME enable string constants"

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

/* Long on the destination platform */
typedef unsigned long ulong_t;
typedef long long_t;

struct file_state {
	struct file_state *prev;
	const char *basename;
	char *dirname;
	char *buf;
	off_t size;
	char *pos;
	int line;
	char *line_start;
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

#define OP_SMUL       0
#define OP_UMUL       1
#define OP_SDIV       2
#define OP_UDIV       3
#define OP_SMOD       4
#define OP_UMOD       5
#define OP_ADD        6
#define OP_SUB        7
#define OP_SL         8
#define OP_USR        9
#define OP_SSR       10 
#define OP_AND       11 
#define OP_XOR       12
#define OP_OR        13
#define OP_POS       14 /* Dummy positive operator don't use it */
#define OP_NEG       15
#define OP_INVERT    16
		     
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

#define OP_NOOP      34

#define OP_MIN_CONST 50
#define OP_MAX_CONST 59
#define IS_CONST_OP(X) (((X) >= OP_MIN_CONST) && ((X) <= OP_MAX_CONST))
#define OP_INTCONST  50
#define OP_BLOBCONST 51
/* For OP_BLOBCONST ->type holds the layout and size
 * information.  u.blob holds a pointer to the raw binary
 * data for the constant initializer.
 */
#define OP_ADDRCONST 52
/* For OP_ADDRCONST ->type holds the type.
 * RHS(0) holds the reference to the static variable.
 * ->u.cval holds an offset from that value.
 */

#define OP_WRITE     60 
/* OP_WRITE moves one pseudo register to another.
 * LHS(0) holds the destination pseudo register, which must be an OP_DECL.
 * RHS(0) holds the psuedo to move.
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
 * RHS(0) is the instruction
 * u.cval is the LHS piece of the instruction to return.
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

#define OP_CALL      72
/* OP_CALL performs a procedure call. 
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
/* OP_LIST Holds a list of statements, and a result value.
 * RHS(0) holds the list of statements.
 * MISC(0) holds the value of the statements.
 */

#define OP_BRANCH    81 /* branch */
/* For branch instructions
 * TARG(0) holds the branch target.
 * RHS(0) if present holds the branch condition.
 * ->next holds where to branch to if the branch is not taken.
 * The branch target can only be a decl...
 */

#define OP_LABEL     83
/* OP_LABEL is a triple that establishes an target for branches.
 * ->use is the list of all branches that use this label.
 */

#define OP_ADECL     84 
/* OP_DECL is a triple that establishes an lvalue for assignments.
 * ->use is a list of statements that use the variable.
 */

#define OP_SDECL     85
/* OP_VAR is a triple that establishes a variable of static
 * storage duration.
 * ->use is a list of statements that use the variable.
 * MISC(0) holds the initializer expression.
 */


#define OP_PHI       86
/* OP_PHI is a triple used in SSA form code.  
 * It is used when multiple code paths merge and a variable needs
 * a single assignment from any of those code paths.
 * The operation is a cross between OP_DECL and OP_WRITE, which
 * is what OP_PHI is geneared from.
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
#define PURE   1
#define IMPURE 2
#define PURE_BITS(FLAGS) ((FLAGS) & 0x3)
#define DEF    4
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
[OP_SMUL       ] = OP( 0,  2, 0, 0, PURE | DEF, "smul"),
[OP_UMUL       ] = OP( 0,  2, 0, 0, PURE | DEF, "umul"),
[OP_SDIV       ] = OP( 0,  2, 0, 0, PURE | DEF, "sdiv"),
[OP_UDIV       ] = OP( 0,  2, 0, 0, PURE | DEF, "udiv"),
[OP_SMOD       ] = OP( 0,  2, 0, 0, PURE | DEF, "smod"),
[OP_UMOD       ] = OP( 0,  2, 0, 0, PURE | DEF, "umod"),
[OP_ADD        ] = OP( 0,  2, 0, 0, PURE | DEF, "add"),
[OP_SUB        ] = OP( 0,  2, 0, 0, PURE | DEF, "sub"),
[OP_SL         ] = OP( 0,  2, 0, 0, PURE | DEF, "sl"),
[OP_USR        ] = OP( 0,  2, 0, 0, PURE | DEF, "usr"),
[OP_SSR        ] = OP( 0,  2, 0, 0, PURE | DEF, "ssr"),
[OP_AND        ] = OP( 0,  2, 0, 0, PURE | DEF, "and"),
[OP_XOR        ] = OP( 0,  2, 0, 0, PURE | DEF, "xor"),
[OP_OR         ] = OP( 0,  2, 0, 0, PURE | DEF, "or"),
[OP_POS        ] = OP( 0,  1, 0, 0, PURE | DEF, "pos"),
[OP_NEG        ] = OP( 0,  1, 0, 0, PURE | DEF, "neg"),
[OP_INVERT     ] = OP( 0,  1, 0, 0, PURE | DEF, "invert"),

[OP_EQ         ] = OP( 0,  2, 0, 0, PURE | DEF, "eq"),
[OP_NOTEQ      ] = OP( 0,  2, 0, 0, PURE | DEF, "noteq"),
[OP_SLESS      ] = OP( 0,  2, 0, 0, PURE | DEF, "sless"),
[OP_ULESS      ] = OP( 0,  2, 0, 0, PURE | DEF, "uless"),
[OP_SMORE      ] = OP( 0,  2, 0, 0, PURE | DEF, "smore"),
[OP_UMORE      ] = OP( 0,  2, 0, 0, PURE | DEF, "umore"),
[OP_SLESSEQ    ] = OP( 0,  2, 0, 0, PURE | DEF, "slesseq"),
[OP_ULESSEQ    ] = OP( 0,  2, 0, 0, PURE | DEF, "ulesseq"),
[OP_SMOREEQ    ] = OP( 0,  2, 0, 0, PURE | DEF, "smoreeq"),
[OP_UMOREEQ    ] = OP( 0,  2, 0, 0, PURE | DEF, "umoreeq"),
[OP_LFALSE     ] = OP( 0,  1, 0, 0, PURE | DEF, "lfalse"),
[OP_LTRUE      ] = OP( 0,  1, 0, 0, PURE | DEF, "ltrue"),

[OP_LOAD       ] = OP( 0,  1, 0, 0, IMPURE | DEF, "load"),
[OP_STORE      ] = OP( 1,  1, 0, 0, IMPURE, "store"),

[OP_NOOP       ] = OP( 0,  0, 0, 0, PURE, "noop"),

[OP_INTCONST   ] = OP( 0,  0, 0, 0, PURE, "intconst"),
[OP_BLOBCONST  ] = OP( 0,  0, 0, 0, PURE, "blobconst"),
[OP_ADDRCONST  ] = OP( 0,  1, 0, 0, PURE, "addrconst"),

[OP_WRITE      ] = OP( 1,  1, 0, 0, PURE, "write"),
[OP_READ       ] = OP( 0,  1, 0, 0, PURE | DEF, "read"),
[OP_COPY       ] = OP( 0,  1, 0, 0, PURE | DEF, "copy"),
[OP_PIECE      ] = OP( 0,  1, 0, 0, PURE | DEF, "piece"),
[OP_DEREF      ] = OP( 0,  1, 0, 0, 0 | DEF, "deref"), 
[OP_DOT        ] = OP( 0,  1, 0, 0, 0 | DEF, "dot"),

[OP_VAL        ] = OP( 0,  1, 1, 0, 0 | DEF, "val"),
[OP_LAND       ] = OP( 0,  2, 0, 0, 0 | DEF, "land"),
[OP_LOR        ] = OP( 0,  2, 0, 0, 0 | DEF, "lor"),
[OP_COND       ] = OP( 0,  3, 0, 0, 0 | DEF, "cond"),
[OP_COMMA      ] = OP( 0,  2, 0, 0, 0 | DEF, "comma"),
/* Call is special most it can stand in for anything so it depends on context */
[OP_CALL       ] = OP(-1, -1, 1, 0, 0, "call"),
/* The sizes of OP_CALL and OP_VAL_VEC depend upon context */
[OP_VAL_VEC    ] = OP( 0, -1, 0, 0, 0, "valvec"),

[OP_LIST       ] = OP( 0,  1, 1, 0, 0 | DEF, "list"),
/* The number of targets for OP_BRANCH depends on context */
[OP_BRANCH     ] = OP( 0, -1, 0, 1, PURE, "branch"),
[OP_LABEL      ] = OP( 0,  0, 0, 0, PURE, "label"),
[OP_ADECL      ] = OP( 0,  0, 0, 0, PURE, "adecl"),
[OP_SDECL      ] = OP( 0,  0, 1, 0, PURE, "sdecl"),
/* The number of RHS elements of OP_PHI depend upon context */
[OP_PHI        ] = OP( 0, -1, 1, 0, PURE | DEF, "phi"),

[OP_CMP        ] = OP( 0,  2, 0, 0, PURE | DEF, "cmp"),
[OP_TEST       ] = OP( 0,  1, 0, 0, PURE | DEF, "test"),
[OP_SET_EQ     ] = OP( 0,  1, 0, 0, PURE | DEF, "set_eq"),
[OP_SET_NOTEQ  ] = OP( 0,  1, 0, 0, PURE | DEF, "set_noteq"),
[OP_SET_SLESS  ] = OP( 0,  1, 0, 0, PURE | DEF, "set_sless"),
[OP_SET_ULESS  ] = OP( 0,  1, 0, 0, PURE | DEF, "set_uless"),
[OP_SET_SMORE  ] = OP( 0,  1, 0, 0, PURE | DEF, "set_smore"),
[OP_SET_UMORE  ] = OP( 0,  1, 0, 0, PURE | DEF, "set_umore"),
[OP_SET_SLESSEQ] = OP( 0,  1, 0, 0, PURE | DEF, "set_slesseq"),
[OP_SET_ULESSEQ] = OP( 0,  1, 0, 0, PURE | DEF, "set_ulesseq"),
[OP_SET_SMOREEQ] = OP( 0,  1, 0, 0, PURE | DEF, "set_smoreq"),
[OP_SET_UMOREEQ] = OP( 0,  1, 0, 0, PURE | DEF, "set_umoreq"),
[OP_JMP        ] = OP( 0,  0, 0, 1, PURE, "jmp"),
[OP_JMP_EQ     ] = OP( 0,  1, 0, 1, PURE, "jmp_eq"),
[OP_JMP_NOTEQ  ] = OP( 0,  1, 0, 1, PURE, "jmp_noteq"),
[OP_JMP_SLESS  ] = OP( 0,  1, 0, 1, PURE, "jmp_sless"),
[OP_JMP_ULESS  ] = OP( 0,  1, 0, 1, PURE, "jmp_uless"),
[OP_JMP_SMORE  ] = OP( 0,  1, 0, 1, PURE, "jmp_smore"),
[OP_JMP_UMORE  ] = OP( 0,  1, 0, 1, PURE, "jmp_umore"),
[OP_JMP_SLESSEQ] = OP( 0,  1, 0, 1, PURE, "jmp_slesseq"),
[OP_JMP_ULESSEQ] = OP( 0,  1, 0, 1, PURE, "jmp_ulesseq"),
[OP_JMP_SMOREEQ] = OP( 0,  1, 0, 1, PURE, "jmp_smoreq"),
[OP_JMP_UMOREEQ] = OP( 0,  1, 0, 1, PURE, "jmp_umoreq"),

[OP_INB        ] = OP( 0,  1, 0, 0, IMPURE | DEF, "__inb"),
[OP_INW        ] = OP( 0,  1, 0, 0, IMPURE | DEF, "__inw"),
[OP_INL        ] = OP( 0,  1, 0, 0, IMPURE | DEF, "__inl"),
[OP_OUTB       ] = OP( 0,  2, 0, 0, IMPURE, "__outb"),
[OP_OUTW       ] = OP( 0,  2, 0, 0, IMPURE, "__outw"),
[OP_OUTL       ] = OP( 0,  2, 0, 0, IMPURE, "__outl"),
[OP_BSF        ] = OP( 0,  1, 0, 0, PURE | DEF, "__bsf"),
[OP_BSR        ] = OP( 0,  1, 0, 0, PURE | DEF, "__bsr"),
[OP_RDMSR      ] = OP( 2,  1, 0, 0, IMPURE, "__rdmsr"),
[OP_WRMSR      ] = OP( 0,  3, 0, 0, IMPURE, "__wrmsr"),
[OP_HLT        ] = OP( 0,  0, 0, 0, IMPURE, "__hlt"),
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

struct triple;
struct block;
struct triple_set {
	struct triple_set *next;
	struct triple *member;
};

#define MAX_LHS  15
#define MAX_RHS  15
#define MAX_MISC 15
#define MAX_TARG 15

struct triple {
	struct triple *next, *prev;
	struct triple_set *use;
	struct type *type;
	short op;
	unsigned short sizes;
#define TRIPLE_LHS(SIZES)  (((SIZES) >>  0) & 0x0f)
#define TRIPLE_RHS(SIZES)  (((SIZES) >>  4) & 0x0f)
#define TRIPLE_MISC(SIZES) (((SIZES) >>  8) & 0x0f)
#define TRIPLE_TARG(SIZES) (((SIZES) >> 12) & 0x0f)
#define TRIPLE_SIZE(SIZES) \
	((((SIZES) >> 0) & 0x0f) + \
	(((SIZES) >>  4) & 0x0f) + \
	(((SIZES) >>  8) & 0x0f) + \
	(((SIZES) >> 12) & 0x0f))
#define TRIPLE_SIZES(LHS, RHS, MISC, TARG) \
	((((LHS) & 0x0f) <<  0) | \
	(((RHS) & 0x0f)  <<  4) | \
	(((MISC) & 0x0f) <<  8) | \
	(((TARG) & 0x0f) << 12))
#define TRIPLE_LHS_OFF(SIZES)  (0)
#define TRIPLE_RHS_OFF(SIZES)  (TRIPLE_LHS_OFF(SIZES) + TRIPLE_LHS(SIZES))
#define TRIPLE_MISC_OFF(SIZES) (TRIPLE_RHS_OFF(SIZES) + TRIPLE_RHS(SIZES))
#define TRIPLE_TARG_OFF(SIZES) (TRIPLE_MISC_OFF(SIZES) + TRIPLE_MISC(SIZES))
#define LHS(PTR,INDEX) ((PTR)->param[TRIPLE_LHS_OFF((PTR)->sizes) + (INDEX)])
#define RHS(PTR,INDEX) ((PTR)->param[TRIPLE_RHS_OFF((PTR)->sizes) + (INDEX)])
#define TARG(PTR,INDEX) ((PTR)->param[TRIPLE_TARG_OFF((PTR)->sizes) + (INDEX)])
#define MISC(PTR,INDEX) ((PTR)->param[TRIPLE_MISC_OFF((PTR)->sizes) + (INDEX)])
	unsigned id; /* A scratch value and finally the register */
#define TRIPLE_FLAG_FLATTENED 1
	const char *filename;
	int line;
	int col;
	union {
		ulong_t cval;
		struct block  *block;
		void *blob;
		struct hash_entry *field;
	} u;
	struct triple *param[2];
};

struct block_set {
	struct block_set *next;
	struct block *member;
};
struct block {
	struct block *work_next;
	struct block *left, *right;
	struct triple *first, *last;
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
	struct symbol *sym_struct;
	struct symbol *sym_ident;
};

#define HASH_TABLE_SIZE 2048

struct compile_state {
	struct triple *vars;
	struct file_state *file;
	struct token token[4];
	struct hash_entry *hash_table[HASH_TABLE_SIZE];
	struct hash_entry *i_continue;
	struct hash_entry *i_break;
	int scope_depth;
	int if_depth, if_value;
	int macro_line;
	struct file_state *macro_file;
	struct triple *main_function;
	struct block *first_block, *last_block;
	int last_vertex;
	int debug;
	int optimize;
};

/* visibility global/local */
/* static/auto duration */
/* typedef, register, inline */
#define STOR_SHIFT         0
#define STOR_MASK     0x000f
/* Visibility */
#define STOR_GLOBAL   0x0001
/* Duration */
#define STOR_PERM     0x0002
/* Storage specifiers */
#define STOR_AUTO     0x0000
#define STOR_STATIC   0x0002
#define STOR_EXTERN   0x0003
#define STOR_REGISTER 0x0004
#define STOR_TYPEDEF  0x0008
#define STOR_INLINE   0x000c

#define QUAL_SHIFT         4
#define QUAL_MASK     0x0070
#define QUAL_NONE     0x0000
#define QUAL_CONST    0x0010
#define QUAL_VOLATILE 0x0020
#define QUAL_RESTRICT 0x0040

#define TYPE_SHIFT         8
#define TYPE_MASK     0x1f00
#define TYPE_INTEGER(TYPE)    (((TYPE) >= TYPE_CHAR) && ((TYPE) <= TYPE_ULLONG))
#define TYPE_ARITHMETIC(TYPE) (((TYPE) >= TYPE_CHAR) && ((TYPE) <= TYPE_LDOUBLE))
#define TYPE_UNSIGNED(TYPE)   ((TYPE) & 0x0100)
#define TYPE_SIGNED(TYPE)     (!TYPE_UNSIGNED(TYPE))
#define TYPE_MKUNSIGNED(TYPE) ((TYPE) | 0x0100)
#define TYPE_RANK(TYPE)       ((TYPE) & ~0x0100)
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
#define TYPE_STRUCT   0x1000
#define TYPE_ENUM     0x1100
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
#define TYPE_ARRAY    0x1600
/* TYPE_ARRAY is a basic building block when definitng arrays.
 * type->left holds the type we are an array of.
 * type-> holds the number of elements.
 */

#define ELEMENT_COUNT_UNSPECIFIED (~0UL)

struct type {
	unsigned int type;
	struct type *left, *right;
	ulong_t elements;
	struct hash_entry *field_ident;
	struct hash_entry *type_ident;
};

#define MAX_REGISTERS      75
#define MAX_REG_EQUIVS     16
#define MAX_REGC           12
#define REG_UNSET          0

/* Provision for 8 register classes */
#define REGC_MASK ((1 << MAX_REGC) - 1)
#define ID_REG_CLASSES(ID)      ((ID) & REGC_MASK)
#define ID_REG(ID)              ((ID) >> MAX_REGC)
#define MK_REG_ID(REG, CLASSES) (((REG) << MAX_REGC) | ((CLASSES) & REGC_MASK))

static unsigned alloc_virtual_reg(void)
{
	static unsigned virtual_reg = MAX_REGISTERS;
	virtual_reg += 1;
	return virtual_reg;
}

static unsigned arch_reg_regcm(struct compile_state *state, int reg);
static void arch_reg_equivs(
	struct compile_state *state, unsigned *equiv, int reg);
static int arch_select_free_register(
	struct compile_state *state, char *used, int classes);
static unsigned arch_regc_size(struct compile_state *state, int class);
static int arch_regcm_intersect(unsigned regcm1, unsigned regcm2);
static unsigned arch_type_to_regcm(struct compile_state *state, struct type *type);
static const char *arch_reg_str(int reg);

#define DEBUG_ABORT_ON_ERROR    0x0001
#define DEBUG_INTERMEDIATE_CODE 0x0002
#define DEBUG_CONTROL_FLOW      0x0004
#define DEBUG_BASIC_BLOCKS      0x0008
#define DEBUG_FDOMINATORS       0x0010
#define DEBUG_RDOMINATORS       0x0020
#define DEBUG_TRIPLES           0x0040
#define DEBUG_INTERFERENCE      0x0080
#define DEBUG_ARCH_CODE         0x0100
#define DEBUG_CODE_ELIMINATION  0x0200

#define GLOBAL_SCOPE_DEPTH 1

static void compile_file(struct compile_state *old_state, char *filename, int local);

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
	if (triple) {
		fprintf(fp, "%s:%d.%d: ", 
			triple->filename, triple->line, triple->col);
		return;
	}
	if (!state->file) {
		return;
	}
	col = get_col(state->file);
	fprintf(fp, "%s:%d.%d: ", 
		state->file->basename, state->file->line, col);
}

static void __internal_error(struct compile_state *state, struct triple *ptr, 
	char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	loc(stderr, state, ptr);
	fprintf(stderr, "Internal compiler error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}


static void __internal_warning(struct compile_state *state, struct triple *ptr, 
	char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	loc(stderr, state, ptr);
	fprintf(stderr, "Internal compiler warning: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}



static void __error(struct compile_state *state, struct triple *ptr, 
	char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	loc(stderr, state, ptr);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	if (state->debug & DEBUG_ABORT_ON_ERROR) {
		abort();
	}
	exit(1);
}

static void __warning(struct compile_state *state, struct triple *ptr, 
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

#if DEBUG_ERROR_MESSAGES 
#  define internal_error fprintf(stderr,  "@ %s.%s:%d \t", __FILE__, __func__, __LINE__),__internal_error
#  define internal_warning fprintf(stderr,  "@ %s.%s:%d \t", __FILE__, __func__, __LINE__),__internal_warning
#  define error fprintf(stderr, "@ %s.%s:%d \t", __FILE__, __func__, __LINE__),__error
#  define warning fprintf(stderr, "@ %s.%s:%d \t", __FILE__, __func__, __LINE__),__warning
#else
#  define internal_error __internal_error
#  define internal_warning __internal_warning
#  define error __error
#  define warning __warning
#endif
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

static void push_triple(struct triple *used, struct triple *user)
{
	struct triple_set *new;
	if (!used)
		return;
	if (!user)
		return;
	/* Append new to the head of the list,
	 * it's the only sensible behavoir for a stack.
	 */
	new = xcmalloc(sizeof(*new), "triple_set");
	new->member = user;
	new->next   = used->use;
	used->use   = new;
}

static void pop_triple(struct triple *used, struct triple *unuser)
{
	struct triple_set *use, **ptr;
	ptr = &used->use;
	while(*ptr) {
		use = *ptr;
		if (use->member == unuser) {
			*ptr = use->next;
			xfree(use);
			/* Only free one occurance from the stack */
			return;
		}
		else {
			ptr = &use->next;
		}
	}
}


/* The zero triple is used as a place holder when we are removing pointers
 * from a triple.  Having allows certain sanity checks to pass even
 * when the original triple that was pointed to is gone.
 */
static struct triple zero_triple = {
	.next     = &zero_triple,
	.prev     = &zero_triple,
	.use      = 0,
	.op       = OP_INTCONST,
	.sizes    = TRIPLE_SIZES(0, 0, 0, 0),
	.id       = -1, /* An invalid id */
	.u = { .cval   = 0, },
	.filename = __FILE__,
	.line     = __LINE__,
	.col      = 0,
	.param { [0] = 0, [1] = 0, },
};


static unsigned short triple_sizes(struct compile_state *state,
	int op, struct type *type, int rhs_wanted)
{
	int lhs, rhs, misc, targ;
	valid_op(state, op);
	lhs = table_ops[op].lhs;
	rhs = table_ops[op].rhs;
	misc = table_ops[op].misc;
	targ = table_ops[op].targ;
	
	
	if (op == OP_CALL) {
		struct type *param;
		rhs = 0;
		param = type->right;
		while((param->type & TYPE_MASK) == TYPE_PRODUCT) {
			rhs++;
			param = param->right;
		}
		if ((param->type & TYPE_MASK) != TYPE_VOID) {
			rhs++;
		}
		lhs = 0;
		if ((type->left->type & TYPE_MASK) == TYPE_STRUCT) {
			lhs = type->left->elements;
		}
	}
	else if (op == OP_VAL_VEC) {
		rhs = type->elements;
	}
	else if ((op == OP_BRANCH) || (op == OP_PHI)) {
		rhs = rhs_wanted;
	}
	if ((rhs < 0) || (rhs > MAX_RHS)) {
		internal_error(state, 0, "bad rhs");
	}
	if ((lhs < 0) || (lhs > MAX_LHS)) {
		internal_error(state, 0, "bad lhs");
	}
	if ((misc < 0) || (misc > MAX_MISC)) {
		internal_error(state, 0, "bad misc");
	}
	if ((targ < 0) || (targ > MAX_TARG)) {
		internal_error(state, 0, "bad targs");
	}
	return TRIPLE_SIZES(lhs, rhs, misc, targ);
}

static struct triple *alloc_triple(struct compile_state *state, 
	int op, struct type *type, int rhs,
	const char *filename, int line, int col)
{
	size_t size, sizes, extra_count, min_count;
	struct triple *ret;
	sizes = triple_sizes(state, op, type, rhs);

	min_count = sizeof(ret->param)/sizeof(ret->param[0]);
	extra_count = TRIPLE_SIZE(sizes);
	extra_count = (extra_count < min_count)? 0 : extra_count - min_count;

	size = sizeof(*ret) + sizeof(ret->param[0]) * extra_count;
	ret = xcmalloc(size, "tripple");
	ret->op       = op;
	ret->sizes    = sizes;
	ret->type     = type;
	ret->next     = ret;
	ret->prev     = ret;
	ret->filename = filename;
	ret->line     = line;
	ret->col      = col;
	return ret;
}

struct triple *dup_triple(struct compile_state *state, struct triple *src)
{
	struct triple *dup;
	int src_rhs;
	src_rhs = TRIPLE_RHS(src->sizes);
	dup = alloc_triple(state, src->op, src->type, src_rhs,
		src->filename, src->line, src->col);
	memcpy(dup, src, sizeof(*src));
	memcpy(dup->param, src->param, src_rhs * sizeof(src->param[0]));
	return dup;
}

static struct triple *new_triple(struct compile_state *state, 
	int op, struct type *type, int rhs)
{
	struct triple *ret;
	const char *filename;
	int line, col;
	filename = 0;
	line = 0;
	col  = 0;
	if (state->file) {
		filename = state->file->basename;
		line     = state->file->line;
		col      = get_col(state->file);
	}
	ret = alloc_triple(state, op, type, rhs,
		filename, line, col);
	return ret;
}

static struct triple *build_triple(struct compile_state *state, 
	int op, struct type *type, struct triple *left, struct triple *right,
	const char *filename, int line, int col)
{
	struct triple *ret;
	size_t count;
	ret = alloc_triple(state, op, type, -1, filename, line, col);
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
	ret = new_triple(state, op, type, -1);
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
	ret = new_triple(state, OP_BRANCH, &void_type, test?1:0);
	if (test) {
		RHS(ret, 0) = test;
	}
	TARG(ret, 0) = targ;
	/* record the branch target was used */
	if (!targ || (targ->op != OP_LABEL)) {
		internal_error(state, 0, "branch not to label");
		use_triple(targ, ret);
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
		if ((ptr->prev->op == OP_BRANCH) && 
			TRIPLE_RHS(ptr->prev->sizes)) {
			unuse_triple(first, ptr->prev);
			use_triple(ptr, ptr->prev);
		}
	}
}

static struct triple *pre_triple(struct compile_state *state,
	struct triple *base,
	int op, struct type *type, struct triple *left, struct triple *right)
{
	/* Careful this assumes it can do the easy thing to get the block */
	struct triple *ret;
	ret = build_triple(state, op, type, left, right, 
		base->filename, base->line, base->col);
	ret->u.block = base->u.block;
	insert_triple(state, base, ret);
	return ret;
}

static struct triple *post_triple(struct compile_state *state,
	struct triple *base,
	int op, struct type *type, struct triple *left, struct triple *right)
{
	/* Careful this assumes it can do the easy thing to get the block */
	struct triple *ret;
	ret = build_triple(state, op, type, left, right, 
		base->filename, base->line, base->col);
	ret->u.block = base->u.block;
	insert_triple(state, base->next, ret);
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
	if (ins->op == OP_INTCONST) {
		fprintf(fp, "(%p) %3d %-10s 0x%08lx            @ %s:%d.%d\n",
			ins, ID_REG(ins->id), tops(ins->op), ins->u.cval,
			ins->filename, ins->line, ins->col);
	}
	else if (ins->op == OP_SDECL) {
		fprintf(fp, "(%p) %3d %-10s %-10p            @ %s:%d.%d\n",
			ins, ID_REG(ins->id), tops(ins->op), MISC(ins, 0),
			ins->filename, ins->line, ins->col);
#if 0
		print_ins(state, MISC(ins, 0));
#endif
	}
	else {
		int i, count;
		fprintf(fp, "(%p) %3d %-10s", 
			ins, ID_REG(ins->id), tops(ins->op));
		count = TRIPLE_SIZE(ins->sizes);
		for(i = 0; i < count; i++) {
			fprintf(fp, " %-10p", ins->param[i]);
		}
		for(; i < 2; i++) {
			printf("           ");
		}
		fprintf(fp, " @ %s:%d.%d\n", 
			ins->filename, ins->line, ins->col);
	}
	fflush(fp);
}

static int triple_is_pure(struct compile_state *state, struct triple *ins)
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
	return pure == PURE;
}

static int triple_is_branch(struct compile_state *state, struct triple *ins)
{
	/* This function is used to determine which triples need
	 * a register.
	 */
	int is_branch;
	valid_ins(state, ins);
	is_branch = (table_ops[ins->op].targ != 0);
	return is_branch;
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
	return triple_iter(state, TRIPLE_TARG(ins->sizes), &TARG(ins,0), 
		ins, last);
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
	memset(ptr, -1, size);
	xfree(ptr);
}

static void release_triple(struct compile_state *state, struct triple *ptr)
{
	struct triple_set *set, *next;
	struct triple **expr;
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
	expr = triple_targ(state, ptr, 0);
	for(; expr; expr = triple_targ(state, ptr, expr)) {
		if (*expr) {
			unuse_triple(*expr, ptr);
		}
	}
	/* Reomve ptr from use chains where it is used */
	for(set = ptr->use; set; set = next) {
		next = set->next;
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

static void print_triple(struct compile_state *state, struct triple *ptr);

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
			end_scope_syms(&entry->sym_label,  depth);
			end_scope_syms(&entry->sym_struct, depth);
			end_scope_syms(&entry->sym_ident,  depth);
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
	hash_keyword(state, "while",         TOK_WHILE);
	hash_keyword(state, "asm",           TOK_ASM);
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
		char *end;
		meat(state, index, TOK_LIT_INT);
		errno = 0;
		val = strtol(state->token[index].val.str, &end, 0);
		if (((val == LONG_MIN) || (val == LONG_MAX)) &&
			(errno == ERANGE)) {
			error(state, 0, "Integer constant to large");
		}
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
	case TOK_UNDEF:
	case TOK_LINE:
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
			if (!isspace(*ptr)) {
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

static void compile_file(struct compile_state *state, char *filename, int local)
{
	char cwd[4096];
	char *subdir, *base;
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
			result = new_triple(state, OP_VAL_VEC, type, -1);
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
	case STOR_EXTERN:
		fprintf(fp, "extern ");
		break;
	case STOR_REGISTER:
		fprintf(fp, "register ");
		break;
	case STOR_TYPEDEF:
		fprintf(fp, "typedef ");
		break;
	case STOR_INLINE:
		fprintf(fp, "inline ");
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
		fprintf(fp, " [%ld]", type->elements);
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
		size = size_of(state, type->left);
		while((type->right->type & TYPE_MASK) == TYPE_PRODUCT) {
			type = type->right;
			align = align_of(state, type->left);
			pad = align - (size % align);
			size = size + pad + size_of(state, type->left);
		}
		align = align_of(state, type->right);
		pad = align - (size % align);
		size = size + pad + sizeof(type->right);
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
		size = size_of(state, type->left);
		break;
	default:
		error(state, 0, "sizeof not yet defined for type\n");
		break;
	}
	return size;
}

static size_t field_offset(struct compile_state *state, 
	struct type *type, struct hash_entry *field)
{
	size_t size, align, pad;
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		internal_error(state, 0, "field_offset only works on structures");
	}
	size = 0;
	type = type->left;
	while((type->type & TYPE_MASK) == TYPE_PRODUCT) {
		if (type->left->field_ident == field) {
			type = type->left;
		}
		size += size_of(state, type->left);
		type = type->right;
		align = align_of(state, type->left);
		pad = align - (size % align);
		size += pad;
	}
	if (type->field_ident != field) {
		internal_error(state, 0, "field_offset: member %s not present",
			field->name);
	}
	return size;
}

static struct type *field_type(struct compile_state *state, 
	struct type *type, struct hash_entry *field)
{
	if ((type->type & TYPE_MASK) != TYPE_STRUCT) {
		internal_error(state, 0, "field_type only works on structures");
	}
	type = type->left;
	while((type->type & TYPE_MASK) == TYPE_PRODUCT) {
		if (type->left->field_ident == field) {
			type = type->left;
			break;
		}
		type = type->right;
	}
	if (type->field_ident != field) {
		internal_error(state, 0, "field_type: member %s not present", 
			field->name);
	}
	return type;
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
	if (TYPE_INTEGER(type) && 
		TYPE_RANK(type) < TYPE_RANK(TYPE_INT)) {
		type = TYPE_INT;
	}
	return type;
}

static unsigned int do_arithmetic_conversion(
	unsigned int left, unsigned int right)
{
	left &= TYPE_MASK;
	right &= TYPE_MASK;
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
			def->type = new_type(int_type, 0, 0);
		}
	}
	return def;
}


static void arithmetic(struct compile_state *state, struct triple *def)
{
	if (!TYPE_ARITHMETIC(def->type->type)) {
		error(state, def, "arithmetic type expexted");
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
		(def->op == OP_BLOBCONST)) {
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
	if (def->type->type & QUAL_CONST) {
		ret = 0;
	}
	else if (def->op == OP_DOT) {
		ret = is_lvalue(state, RHS(def, 0));
	}
	return ret;
}

static void lvalue(struct compile_state *state, struct triple *def)
{
	if (!def) {
		internal_error(state, def, "nothing where lvalue expected?");
	}
	if (!is_lvalue(state, def)) { 
		error(state, def, "lvalue expected");
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


static struct triple *do_mk_addr_expr(struct compile_state *state, 
	struct triple *expr, struct type *type, ulong_t offset)
{
	struct triple *result;
	lvalue(state, expr);

	result = 0;
	if (expr->op == OP_ADECL) {
		error(state, expr, "address of auto variables not supported");
	}
	else if (expr->op == OP_SDECL) {
		result = triple(state, OP_ADDRCONST, type, expr, 0);
		result->u.cval = offset;
	}
	else if (expr->op == OP_DEREF) {
		result = triple(state, OP_ADD, type,
			RHS(expr, 0),
			int_const(state, &ulong_type, offset));
	}
	return result;
}

static struct triple *mk_addr_expr(
	struct compile_state *state, struct triple *expr, ulong_t offset)
{
	struct type *type;
	
	type = new_type(
		TYPE_POINTER | (expr->type->type & QUAL_MASK),
		expr->type, 0);

	return do_mk_addr_expr(state, expr, type, offset);
}

static struct triple *mk_deref_expr(
	struct compile_state *state, struct triple *expr)
{
	struct type *base_type;
	pointer(state, expr);
	base_type = expr->type->left;
	if (!TYPE_PTR(base_type->type) && !TYPE_ARITHMETIC(base_type->type)) {
		error(state, 0, 
			"Only pointer and arithmetic values can be dereferenced");
	}
	return triple(state, OP_DEREF, base_type, expr, 0);
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
	member = type->left;
	while((member->type & TYPE_MASK) == TYPE_PRODUCT) {
		if (member->left->field_ident == field) {
			member = member->left;
			break;
		}
		member = member->right;
	}
	if (member->field_ident != field) {
		error(state, 0, "%s is not a member", field->name);
	}
	if ((type->type & STOR_MASK) == STOR_PERM) {
		/* Do the pointer arithmetic to get a deref the field */
		ulong_t offset;
		offset = field_offset(state, type, field);
		result = do_mk_addr_expr(state, expr, member, offset);
		result = mk_deref_expr(state, result);
	}
	else {
		/* Find the variable for the field I want. */
		result = triple(state, OP_DOT, 
			field_type(state, type, field), expr, 0);
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
	if (!is_stable(state, def)) {
		return def;
	}
	/* Tranform an array to a pointer to the first element */
#warning "CHECK_ME is this the right place to transform arrays to pointers?"
	if ((def->type->type & TYPE_MASK) == TYPE_ARRAY) {
		struct type *type;
		type = new_type(
			TYPE_POINTER | (def->type->type & QUAL_MASK),
			def->type->left, 0);
		return triple(state, OP_ADDRCONST, type, def, 0);
	}
	if (is_in_reg(state, def)) {
		op = OP_READ;
	} else {
		op = OP_LOAD;
	}
	return triple(state, op, def->type, def, 0);
}

static void write_compatible(struct compile_state *state,
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
	if (!compatible) {
		error(state, 0, "Incompatible types in assignment");
	}
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
	def = new_triple(state, OP_COND, result_type, 3);
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
	else if (ins->op == OP_CALL) {
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
	struct compile_state *state, struct triple *first, struct triple *ptr)
{
	struct rhs_vector {
		int depth;
		struct triple **ins;
	} vector[MAX_RHS];
	int i, rhs, lhs;
	/* Only operations with just a rhs should come here */
	rhs = TRIPLE_RHS(ptr->sizes);
	lhs = TRIPLE_LHS(ptr->sizes);
	if (TRIPLE_SIZE(ptr->sizes) != lhs + rhs) {
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

struct triple *copy_func(struct compile_state *state, struct triple *ofunc)
{
	struct triple *nfunc;
	struct triple *nfirst, *ofirst;
	struct triple *new, *old;

#if 0
	fprintf(stdout, "\n");
	loc(stdout, state, 0);
	fprintf(stdout, "\n__________ copy_func _________\n");
	print_triple(state, ofunc);
	fprintf(stdout, "__________ copy_func _________ done\n\n");
#endif

	/* Make a new copy of the old function */
	nfunc = triple(state, OP_LIST, ofunc->type, 0, 0);
	nfirst = 0;
	ofirst = old = RHS(ofunc, 0);
	do {
		struct triple *new;
		int old_rhs;
		old_rhs = TRIPLE_RHS(old->sizes);
		new = alloc_triple(state, old->op, old->type, old_rhs,
			old->filename, old->line, old->col);
		if (IS_CONST_OP(new->op)) {
			memcpy(&new->u, &old->u, sizeof(new->u));
		}
#warning "WISHLIST find a way to handle SDECL without a special case..."
		/* The problem is that I don't flatten the misc field,
		 * so I cannot look the value the misc field should have.
		 */
		else if (new->op == OP_SDECL) {
			MISC(new, 0) = MISC(old, 0);
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
			if (!*nexpr && *oexpr && (*oexpr)->use) {
				*nexpr = (*oexpr)->use->member;
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
		old = old->next;
		new = new->next;
	} while ((old != ofirst) && (new != nfirst));
	return nfunc;
}

static struct triple *flatten_call(
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
	nfunc = copy_func(state, ofunc);
	nfirst = RHS(nfunc, 0)->next;
	/* Prepend the parameter reading into the new function list */
	ptype = nfunc->type->right;
	param = RHS(nfunc, 0)->next;
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
#if 0
	fprintf(stdout, "\n");
	loc(stdout, state, 0);
	fprintf(stdout, "\n__________ flatten_call _________\n");
	print_triple(state, nfunc);
	fprintf(stdout, "__________ flatten_call _________ done\n\n");
#endif

	/* Get rid of the extra triples */
	nfirst = RHS(nfunc, 0)->next;
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
		case OP_WRITE:
		case OP_STORE:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			LHS(ptr, 0) = flatten(state, first, LHS(ptr, 0));
			use_triple(LHS(ptr, 0), ptr);
			use_triple(RHS(ptr, 0), ptr);
			break;
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
		case OP_CALL:
			ptr = flatten_call(state, first, ptr);
			break;
		case OP_READ:
		case OP_LOAD:
			RHS(ptr, 0) = flatten(state, first, RHS(ptr, 0));
			use_triple(RHS(ptr, 0), ptr);
			break;
		case OP_BRANCH:
			use_triple(TARG(ptr, 0), ptr);
			if (TRIPLE_RHS(ptr->sizes)) {
				use_triple(RHS(ptr, 0), ptr);
				if (ptr->next != ptr) {
					use_triple(ptr->next, ptr);
				}
			}
			break;
		case OP_BLOBCONST:
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
			base = flatten(state, first, base);
			if (base->op == OP_VAL_VEC) {
				ptr = struct_field(state, base, ptr->u.field);
			}
			break;
		}
		case OP_SDECL:
		case OP_ADECL:
			break;
		default:
			/* Flatten the easy cases we don't override */
			ptr = flatten_generic(state, first, ptr);
			break;
		}
	} while(ptr && (ptr != orig_ptr));
	if (ptr) {
		insert_triple(state, first, ptr);
		ptr->id |= TRIPLE_FLAG_FLATTENED;
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

/*
 * Compile time evaluation
 * ===========================
 */
static int is_const(struct triple *ins)
{
	return IS_CONST_OP(ins->op);
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
			if ((RHS(left, 0) == RHS(right, 0)) &&
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
	return is_const(ins) && (ins->u.cval == 0);
}

static int is_one(struct triple *ins)
{
	return is_const(ins) && (ins->u.cval == 1);
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
	struct triple *ins, struct triple **expr)
{
	struct triple *rhs;
	rhs = *expr;
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
	return rhs->u.cval;
}

static long_t read_sconst(struct triple *ins, struct triple **expr)
{
	struct triple *rhs;
	rhs = *expr;
	return (long_t)(rhs->u.cval);
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
	wipe_ins(state, ins);
	ins->op = OP_COPY;
	ins->sizes = TRIPLE_SIZES(0, 1, 0, 0);
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
	wipe_ins(state, ins);
	ins->op = OP_ADDRCONST;
	ins->sizes = TRIPLE_SIZES(0, 1, 0, 0);
	RHS(ins, 0) = sdecl;
	ins->u.cval = value;
	use_triple(sdecl, ins);
}

/* Transform multicomponent variables into simple register variables */
static void flatten_structures(struct compile_state *state)
{
	struct triple *ins, *first;
	first = RHS(state->main_function, 0);
	ins = first;
	/* Pass one expand structure values into valvecs.
	 */
	ins = first;
	do {
		struct triple *next;
		next = ins->next;
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
				next = alloc_triple(state, OP_VAL_VEC, ins->type, -1,
					ins->filename, ins->line, ins->col);

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
					vector[i]->filename = next->filename;
					vector[i]->line = next->line;
					vector[i]->col = next->col;
					tptr = tptr->right;
				}
				propogate_use(state, ins, next);
				flatten(state, ins, next);
				free_triple(state, ins);
			}
			else if ((ins->op == OP_STORE) || (ins->op == OP_WRITE)) {
				struct triple *src, *dst, **vector;
				struct type *tptr;
				int op;
				ulong_t i;

				op = ins->op;
				src = RHS(ins, 0);
				dst = LHS(ins, 0);
				next = alloc_triple(state, OP_VAL_VEC, ins->type, -1,
					ins->filename, ins->line, ins->col);
				
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
					vector[i]->filename = next->filename;
					vector[i]->line = next->line;
					vector[i]->col = next->col;
					tptr = tptr->right;
				}
				propogate_use(state, ins, next);
				flatten(state, ins, next);
				free_triple(state, ins);
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
			release_triple(state, ins);
		} 
		ins = next;
	} while(ins != first);
	/* Pass three verify the state and set ->id to 0.
	 */
	ins = first;
	do {
		ins->id = 0;
		if ((ins->type->type & TYPE_MASK) == TYPE_STRUCT) {
			internal_error(state, 0, "STRUCT_TYPE remains?");
		}
		if (ins->op == OP_DOT) {
			internal_error(state, 0, "OP_DOT remains?");
		}
		if (ins->op == OP_VAL_VEC) {
			internal_error(state, 0, "OP_VAL_VEC remains?");
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
		left  = read_sconst(ins, &RHS(ins, 0));
		right = read_sconst(ins, &RHS(ins, 1));
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
		insert_triple(state, ins, val);
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
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
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
		insert_triple(state, ins, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_sdiv(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(ins, &RHS(ins, 0));
		right = read_sconst(ins, &RHS(ins, 1));
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
		insert_triple(state, ins, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_udiv(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
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
		insert_triple(state, ins, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}

static void simplify_smod(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
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
		insert_triple(state, ins, val);
		unuse_triple(RHS(ins, 1), ins);
		use_triple(val, ins);
		RHS(ins, 1) = val;
	}
}
static void simplify_umod(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
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
		insert_triple(state, ins, val);
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
		if (!is_pointer(RHS(ins, 0))) {
			ulong_t left, right;
			left  = read_const(state, ins, &RHS(ins, 0));
			right = read_const(state, ins, &RHS(ins, 1));
			mkconst(state, ins, left + right);
		}
		else /* op == OP_ADDRCONST */ {
			struct triple *sdecl;
			ulong_t left, right;
			sdecl = RHS(RHS(ins, 0), 0);
			left  = RHS(ins, 0)->u.cval;
			right = RHS(ins, 1)->u.cval;
			mkaddr_const(state, ins, sdecl, left + right);
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
		if (!is_pointer(RHS(ins, 0))) {
			ulong_t left, right;
			left  = read_const(state, ins, &RHS(ins, 0));
			right = read_const(state, ins, &RHS(ins, 1));
			mkconst(state, ins, left - right);
		}
		else /* op == OP_ADDRCONST */ {
			struct triple *sdecl;
			ulong_t left, right;
			sdecl = RHS(RHS(ins, 0), 0);
			left  = RHS(ins, 0)->u.cval;
			right = RHS(ins, 1)->u.cval;
			mkaddr_const(state, ins, sdecl, left - right);
		}
	}
}

static void simplify_sl(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, &RHS(ins, 1));
		if (right >= (size_of(state, ins->type)*8)) {
			warning(state, ins, "left shift count >= width of type");
		}
	}
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins,  left << right);
	}
}

static void simplify_usr(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, &RHS(ins, 1));
		if (right >= (size_of(state, ins->type)*8)) {
			warning(state, ins, "right shift count >= width of type");
		}
	}
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left >> right);
	}
}

static void simplify_ssr(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 1))) {
		ulong_t right;
		right = read_const(state, ins, &RHS(ins, 1));
		if (right >= (size_of(state, ins->type)*8)) {
			warning(state, ins, "right shift count >= width of type");
		}
	}
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(ins, &RHS(ins, 0));
		right = read_sconst(ins, &RHS(ins, 1));
		mkconst(state, ins, left >> right);
	}
}

static void simplify_and(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left & right);
	}
}

static void simplify_or(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left | right);
	}
}

static void simplify_xor(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
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
		left = read_const(state, ins, &RHS(ins, 0));
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
		left = read_const(state, ins, &RHS(ins, 0));
		mkconst(state, ins, ~left);
	}
}

static void simplify_eq(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left == right);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 1);
	}
}

static void simplify_noteq(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left != right);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 0);
	}
}

static void simplify_sless(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(ins, &RHS(ins, 0));
		right = read_sconst(ins, &RHS(ins, 1));
		mkconst(state, ins, left < right);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 0);
	}
}

static void simplify_uless(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left < right);
	}
	else if (is_zero(RHS(ins, 0))) {
		mkconst(state, ins, 1);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 0);
	}
}

static void simplify_smore(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(ins, &RHS(ins, 0));
		right = read_sconst(ins, &RHS(ins, 1));
		mkconst(state, ins, left > right);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 0);
	}
}

static void simplify_umore(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left > right);
	}
	else if (is_zero(RHS(ins, 1))) {
		mkconst(state, ins, 1);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 0);
	}
}


static void simplify_slesseq(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		long_t left, right;
		left  = read_sconst(ins, &RHS(ins, 0));
		right = read_sconst(ins, &RHS(ins, 1));
		mkconst(state, ins, left <= right);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 1);
	}
}

static void simplify_ulesseq(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left <= right);
	}
	else if (is_zero(RHS(ins, 0))) {
		mkconst(state, ins, 1);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 1);
	}
}

static void simplify_smoreeq(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 0))) {
		long_t left, right;
		left  = read_sconst(ins, &RHS(ins, 0));
		right = read_sconst(ins, &RHS(ins, 1));
		mkconst(state, ins, left >= right);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 1);
	}
}

static void simplify_umoreeq(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0)) && is_const(RHS(ins, 1))) {
		ulong_t left, right;
		left  = read_const(state, ins, &RHS(ins, 0));
		right = read_const(state, ins, &RHS(ins, 1));
		mkconst(state, ins, left >= right);
	}
	else if (is_zero(RHS(ins, 1))) {
		mkconst(state, ins, 1);
	}
	else if (RHS(ins, 0) == RHS(ins, 1)) {
		mkconst(state, ins, 1);
	}
}

static void simplify_lfalse(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, &RHS(ins, 0));
		mkconst(state, ins, left == 0);
	}
	/* Otherwise if I am the only user... */
	else if ((RHS(ins, 0)->use->member == ins) && (RHS(ins, 0)->use->next == 0)) {
		int need_copy = 1;
		/* Invert a boolean operation */
		switch(RHS(ins, 0)->op) {
		case OP_LTRUE:   RHS(ins, 0)->op = OP_LFALSE;  break;
		case OP_LFALSE:  RHS(ins, 0)->op = OP_LTRUE;   break;
		case OP_EQ:      RHS(ins, 0)->op = OP_NOTEQ;   break;
		case OP_NOTEQ:   RHS(ins, 0)->op = OP_EQ;      break;
		case OP_SLESS:   RHS(ins, 0)->op = OP_SMOREEQ; break;
		case OP_ULESS:   RHS(ins, 0)->op = OP_UMOREEQ; break;
		case OP_SMORE:   RHS(ins, 0)->op = OP_SLESSEQ; break;
		case OP_UMORE:   RHS(ins, 0)->op = OP_ULESSEQ; break;
		case OP_SLESSEQ: RHS(ins, 0)->op = OP_SMORE;   break;
		case OP_ULESSEQ: RHS(ins, 0)->op = OP_UMORE;   break;
		case OP_SMOREEQ: RHS(ins, 0)->op = OP_SLESS;   break;
		case OP_UMOREEQ: RHS(ins, 0)->op = OP_ULESS;   break;
		default:
			need_copy = 0;
			break;
		}
		if (need_copy) {
			mkcopy(state, ins, RHS(ins, 0));
		}
	}
}

static void simplify_ltrue (struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, &RHS(ins, 0));
		mkconst(state, ins, left != 0);
	}
	else switch(RHS(ins, 0)->op) {
	case OP_LTRUE:   case OP_LFALSE:  case OP_EQ:      case OP_NOTEQ:
	case OP_SLESS:   case OP_ULESS:   case OP_SMORE:   case OP_UMORE:
	case OP_SLESSEQ: case OP_ULESSEQ: case OP_SMOREEQ: case OP_UMOREEQ:
		mkcopy(state, ins, RHS(ins, 0));
	}

}

static void simplify_copy(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		switch(RHS(ins, 0)->op) {
		case OP_INTCONST:
		{
			ulong_t left;
			left = read_const(state, ins, &RHS(ins, 0));
			mkconst(state, ins, left);
			break;
		}
		case OP_ADDRCONST:
		{
			struct triple *sdecl;
			ulong_t offset;
			sdecl  = RHS(ins, 0);
			offset = ins->u.cval;
			mkaddr_const(state, ins, sdecl, offset);
			break;
		}
		default:
			internal_error(state, ins, "uknown constant");
			break;
		}
	}
}

static void simplify_branch(struct compile_state *state, struct triple *ins)
{
	struct block *block;
	if (ins->op != OP_BRANCH) {
		internal_error(state, ins, "not branch");
	}
	if (ins->use != 0) {
		internal_error(state, ins, "branch use");
	}
#warning "FIXME implement simplify branch."
	/* The challenge here with simplify branch is that I need to 
	 * make modifications to the control flow graph as well
	 * as to the branch instruction itself.
	 */
	block = ins->u.block;
	
	if (TRIPLE_RHS(ins->sizes) && is_const(RHS(ins, 0))) {
		struct triple *targ;
		ulong_t value;
		value = read_const(state, ins, &RHS(ins, 0));
		unuse_triple(RHS(ins, 0), ins);
		targ = TARG(ins, 0);
		ins->sizes = TRIPLE_SIZES(0, 0, 0, 1);
		if (value) {
			unuse_triple(ins->next, ins);
			TARG(ins, 0) = targ;
		}
		else {
			unuse_triple(targ, ins);
			TARG(ins, 0) = ins->next;
		}
#warning "FIXME handle the case of making a branch unconditional"
	}
	if (TARG(ins, 0) == ins->next) {
		unuse_triple(ins->next, ins);
		if (TRIPLE_RHS(ins->sizes)) {
			unuse_triple(RHS(ins, 0), ins);
			unuse_triple(ins->next, ins);
		}
		ins->sizes = TRIPLE_SIZES(0, 0, 0, 0);
		ins->op = OP_NOOP;
		if (ins->use) {
			internal_error(state, ins, "noop use != 0");
		}
#warning "FIXME handle the case of killing a branch"
	}
}

static void simplify_phi(struct compile_state *state, struct triple *ins)
{
	struct triple **expr;
	ulong_t value;
	expr = triple_rhs(state, ins, 0);
	if (!*expr || !is_const(*expr)) {
		return;
	}
	value = read_const(state, ins, expr);
	for(;expr;expr = triple_rhs(state, ins, expr)) {
		if (!*expr || !is_const(*expr)) {
			return;
		}
		if (value != read_const(state, ins, expr)) {
			return;
		}
	}
	mkconst(state, ins, value);
}


static void simplify_bsf(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, &RHS(ins, 0));
		mkconst(state, ins, bsf(left));
	}
}

static void simplify_bsr(struct compile_state *state, struct triple *ins)
{
	if (is_const(RHS(ins, 0))) {
		ulong_t left;
		left = read_const(state, ins, &RHS(ins, 0));
		mkconst(state, ins, bsr(left));
	}
}


typedef void (*simplify_t)(struct compile_state *state, struct triple *ins);
static const simplify_t table_simplify[] = {
#if 0
#define simplify_smul     simplify_noop
#define simplify_umul	  simplify_noop
#define simplify_sdiv	  simplify_noop
#define simplify_udiv	  simplify_noop
#define simplify_smod	  simplify_noop
#define simplify_umod	  simplify_noop
#endif
#if 0
#define simplify_add	  simplify_noop
#define simplify_sub	  simplify_noop
#endif
#if 0
#define simplify_sl	  simplify_noop
#define simplify_usr	  simplify_noop
#define simplify_ssr	  simplify_noop
#endif
#if 0
#define simplify_and	  simplify_noop
#define simplify_xor	  simplify_noop
#define simplify_or	  simplify_noop
#endif
#if 0
#define simplify_pos	  simplify_noop
#define simplify_neg	  simplify_noop
#define simplify_invert	  simplify_noop
#endif

#if 0
#define simplify_eq	  simplify_noop
#define simplify_noteq	  simplify_noop
#endif
#if 0
#define simplify_sless	  simplify_noop
#define simplify_uless	  simplify_noop
#define simplify_smore	  simplify_noop
#define simplify_umore	  simplify_noop
#endif
#if 0
#define simplify_slesseq  simplify_noop
#define simplify_ulesseq  simplify_noop
#define simplify_smoreeq  simplify_noop
#define simplify_umoreeq  simplify_noop
#endif
#if 0
#define simplify_lfalse	  simplify_noop
#endif
#if 0
#define simplify_ltrue	  simplify_noop
#endif

#if 0
#define simplify_copy	  simplify_noop
#endif

#if 0
#define simplify_branch	  simplify_noop
#endif

#if 0
#define simplify_phi	  simplify_noop
#endif

#if 0
#define simplify_bsf	  simplify_noop
#define simplify_bsr      simplify_noop
#endif

[OP_SMUL       ] = simplify_smul,
[OP_UMUL       ] = simplify_umul,
[OP_SDIV       ] = simplify_sdiv,
[OP_UDIV       ] = simplify_udiv,
[OP_SMOD       ] = simplify_smod,
[OP_UMOD       ] = simplify_umod,
[OP_ADD        ] = simplify_add,
[OP_SUB        ] = simplify_sub,
[OP_SL         ] = simplify_sl,
[OP_USR        ] = simplify_usr,
[OP_SSR        ] = simplify_ssr,
[OP_AND        ] = simplify_and,
[OP_XOR        ] = simplify_xor,
[OP_OR         ] = simplify_or,
[OP_POS        ] = simplify_pos,
[OP_NEG        ] = simplify_neg,
[OP_INVERT     ] = simplify_invert,

[OP_EQ         ] = simplify_eq,
[OP_NOTEQ      ] = simplify_noteq,
[OP_SLESS      ] = simplify_sless,
[OP_ULESS      ] = simplify_uless,
[OP_SMORE      ] = simplify_smore,
[OP_UMORE      ] = simplify_umore,
[OP_SLESSEQ    ] = simplify_slesseq,
[OP_ULESSEQ    ] = simplify_ulesseq,
[OP_SMOREEQ    ] = simplify_smoreeq,
[OP_UMOREEQ    ] = simplify_umoreeq,
[OP_LFALSE     ] = simplify_lfalse,
[OP_LTRUE      ] = simplify_ltrue,

[OP_LOAD       ] = simplify_noop,
[OP_STORE      ] = simplify_noop,

[OP_NOOP       ] = simplify_noop,

[OP_INTCONST   ] = simplify_noop,
[OP_BLOBCONST  ] = simplify_noop,
[OP_ADDRCONST  ] = simplify_noop,

[OP_WRITE      ] = simplify_noop,
[OP_READ       ] = simplify_noop,
[OP_COPY       ] = simplify_copy,
[OP_PIECE      ] = simplify_noop,

[OP_DOT        ] = simplify_noop,
[OP_VAL_VEC    ] = simplify_noop,

[OP_LIST       ] = simplify_noop,
[OP_BRANCH     ] = simplify_branch,
[OP_LABEL      ] = simplify_noop,
[OP_ADECL      ] = simplify_noop,
[OP_SDECL      ] = simplify_noop,
[OP_PHI        ] = simplify_phi,

[OP_INB        ] = simplify_noop,
[OP_INW        ] = simplify_noop,
[OP_INL        ] = simplify_noop,
[OP_OUTB       ] = simplify_noop,
[OP_OUTW       ] = simplify_noop,
[OP_OUTL       ] = simplify_noop,
[OP_BSF        ] = simplify_bsf,
[OP_BSR        ] = simplify_bsr,
[OP_RDMSR      ] = simplify_noop,
[OP_WRMSR      ] = simplify_noop,                    
[OP_HLT        ] = simplify_noop,
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
		else {
			do_simplify = table_simplify[op];
		}
		if (!do_simplify) {
			internal_error(state, ins, "cannot simplify op: %d %s\n",
				op, tops(op));
			return;
		}
		do_simplify(state, ins);
	} while(ins->op != op);
}

static void simplify_all(struct compile_state *state)
{
	struct triple *ins, *first;
	first = RHS(state->main_function, 0);
	ins = first;
	do {
		simplify(state, ins);
		ins = ins->next;
	} while(ins != first);
}

/*
 * Builtins....
 * ============================
 */

static void register_builtin_function(struct compile_state *state,
	const char *name, int op, struct type *rtype, ...)
{
	struct type *ftype, *atype, *param, **next;
	struct triple *def, *arg, *result, *work, *last, *first;
	struct hash_entry *ident;
	struct file_state file;
	int parameters;
	int name_len;
	va_list args;
	int i;

	/* Dummy file state to get debug handling right */
	memset(&file, 0, sizeof(file));
	file.basename = name;
	file.line = 1;
	file.prev = state->file;
	state->file = &file;

	/* Find the Parameter count */
	valid_op(state, op);
	parameters = table_ops[op].rhs;
	if (parameters < 0 ) {
		internal_error(state, 0, "Invalid builtin parameter count");
	}

	/* Find the function type */
	ftype = new_type(TYPE_FUNCTION, rtype, 0);
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
	work = new_triple(state, op, rtype, parameters);
	for(i = 0, arg = first->next; i < parameters; i++, arg = arg->next) {
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
		val = new_triple(state, OP_VAL_VEC, rtype, -1);
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
	name_len = strlen(name);
	ident = lookup(state, name, name_len);
	symbol(state, ident, &ident->sym_ident, def, ftype);
	
	state->file = file.prev;
#if 0
	fprintf(stdout, "\n");
	loc(stdout, state, 0);
	fprintf(stdout, "\n__________ builtin_function _________\n");
	print_triple(state, def);
	fprintf(stdout, "__________ builtin_function _________ done\n\n");
#endif
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
		symbol(state, ident, &ident->sym_struct, 0, type);
		type->type_ident = ident;
		type->elements = elements;
	}
	symbol(state, ident, &ident->sym_ident, 0, type);
	ident->tok = TOK_TYPE_NAME;
	return type;
}


static void register_builtins(struct compile_state *state)
{
	struct type *msr_type;

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
	def = new_triple(state, OP_CALL, func->type, -1);
	def->type = type;

	pvals = TRIPLE_RHS(def->sizes);
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
	if ((val == ULONG_MAX) && (errno == ERANGE)) {
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
		if (!decimal && (val > LONG_MAX)) {
			type = &ulong_type;
		}
	}
	else if (u) {
		type = &uint_type;
		if (val > UINT_MAX) {
			type = &ulong_type;
		}
	}
	else {
		type = &int_type;
		if (!decimal && (val > INT_MAX) && (val <= UINT_MAX)) {
			type = &uint_type;
		}
		else if (!decimal && (val > LONG_MAX)) {
			type = &ulong_type;
		}
		else if (val > INT_MAX) {
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
		 * an enumeration constant.
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
		/* Here ident is an enumeration constant */
		eat(state, TOK_ENUM_CONST);
		def = 0;
		FINISHME();
		break;
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
		def = read_expr(state, cast_expr(state));
		def = triple(state, OP_COPY, type, def, 0);
#warning "FIXME do I need an OP_CAST expr to be semantically correct here?"
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
		internal_error(state, 0, "Not a constant expression");
	}
	/* Free the intermediate expressions */
	while(head->next != head) {
		release_triple(state, head->next);
	}
	free_triple(state, head);
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
	case TOK_PLUSEQ:
	case TOK_MINUSEQ:
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
		case TOK_PLUSEQ:  op = OP_ADD; break;
		case TOK_MINUSEQ: op = OP_SUB; break;
		}
		def = write_expr(state, left,
			triple(state, op, left->type, 
				read_expr(state, left), right));
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
		flatten(state, first, expr(state));
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
	dest = RHS(state->main_function, 0)->prev;
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
	FINISHME();
	eat(state, TOK_GOTO);
	eat(state, TOK_IDENT);
	eat(state, TOK_SEMI);
	error(state, 0, "goto is not implemeted");
	FINISHME();
}

static void labeled_statement(struct compile_state *state, struct triple *first)
{
	FINISHME();
	eat(state, TOK_IDENT);
	eat(state, TOK_COLON);
	statement(state, first);
	error(state, 0, "labeled statements are not implemented");
	FINISHME();
}

static void switch_statement(struct compile_state *state, struct triple *first)
{
	FINISHME();
	eat(state, TOK_SWITCH);
	eat(state, TOK_LPAREN);
	expr(state);
	eat(state, TOK_RPAREN);
	statement(state, first);
	error(state, 0, "switch statements are not implemented");
	FINISHME();
}

static void case_statement(struct compile_state *state, struct triple *first)
{
	FINISHME();
	eat(state, TOK_CASE);
	constant_expr(state);
	eat(state, TOK_COLON);
	statement(state, first);
	error(state, 0, "case statements are not implemented");
	FINISHME();
}

static void default_statement(struct compile_state *state, struct triple *first)
{
	FINISHME();
	eat(state, TOK_DEFAULT);
	eat(state, TOK_COLON);
	statement(state, first);
	error(state, 0, "default statements are not implemented");
	FINISHME();
}

static void asm_statement(struct compile_state *state, struct triple *first)
{
	FINISHME();
	error(state, 0, "FIXME finish asm_statement");
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
	ftype = new_type(TYPE_FUNCTION, type, param_decl(state));
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
	struct compile_state *state, unsigned int specifiers)
{
	int tok;
	struct type *type;
	type = 0;
	FINISHME();
	eat(state, TOK_ENUM);
	tok = peek(state);
	if (tok == TOK_IDENT) {
		eat(state, TOK_IDENT);
	}
	if ((tok != TOK_IDENT) || (peek(state) == TOK_LBRACE)) {
		eat(state, TOK_LBRACE);
		do {
			eat(state, TOK_IDENT);
			if (peek(state) == TOK_EQ) {
				eat(state, TOK_EQ);
				constant_expr(state);
			}
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
			}
		} while(peek(state) != TOK_RBRACE);
		eat(state, TOK_RBRACE);
	}
	FINISHME();
	return type;
}

#if 0
static struct type *struct_declarator(
	struct compile_state *state, struct type *type, struct hash_entry **ident)
{
	int tok;
#warning "struct_declarator is complicated because of bitfields, kill them?"
	tok = peek(state);
	if (tok != TOK_COLON) {
		type = declarator(state, type, ident, 1);
	}
	if ((tok == TOK_COLON) || (peek(state) == TOK_COLON)) {
		eat(state, TOK_COLON);
		constant_expr(state);
	}
	FINISHME();
	return type;
}
#endif

static struct type *struct_or_union_specifier(
	struct compile_state *state, unsigned int specifiers)
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
	if ((tok == TOK_IDENT) || (tok == TOK_TYPE_NAME)) {
		eat(state, tok);
		ident = state->token[0].ident;
	}
	if (!ident || (peek(state) == TOK_LBRACE)) {
		ulong_t elements;
		elements = 0;
		eat(state, TOK_LBRACE);
		do {
			struct type *base_type;
			struct type **next;
			int done;
			base_type = specifier_qualifier_list(state);
			next = &struct_type;
			do {
				struct type *type;
				struct hash_entry *fident;
				done = 1;
				type = declarator(state, base_type, &fident, 1);
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
		struct_type = new_type(TYPE_STRUCT, struct_type, 0);
		struct_type->type_ident = ident;
		struct_type->elements = elements;
		symbol(state, ident, &ident->sym_struct, 0, struct_type);
	}
	if (ident && ident->sym_struct) {
		struct_type = ident->sym_struct->type;
	}
	else if (ident && !ident->sym_struct) {
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
			specifiers = STOR_STATIC;
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

static unsigned designator(struct compile_state *state)
{
	int tok;
	unsigned index;
	index = -1U;
	do {
		switch(peek(state)) {
		case TOK_LBRACKET:
		{
			struct triple *value;
			eat(state, TOK_LBRACKET);
			value = constant_expr(state);
			eat(state, TOK_RBRACKET);
			index = value->u.cval;
			break;
		}
		case TOK_DOT:
			eat(state, TOK_DOT);
			eat(state, TOK_IDENT);
			error(state, 0, "Struct Designators not currently supported");
			break;
		default:
			error(state, 0, "Invalid designator");
		}
		tok = peek(state);
	} while((tok == TOK_LBRACKET) || (tok == TOK_DOT));
	eat(state, TOK_EQ);
	return index;
}

static struct triple *initializer(
	struct compile_state *state, struct type *type)
{
	struct triple *result;
	if (peek(state) != TOK_LBRACE) {
		result = assignment_expr(state);
	}
	else {
		int comma;
		unsigned index, max_index;
		void *buf;
		max_index = index = 0;
		if ((type->type & TYPE_MASK) == TYPE_ARRAY) {
			max_index = type->elements;
			if (type->elements == ELEMENT_COUNT_UNSPECIFIED) {
				type->elements = 0;
			}
		} else {
			error(state, 0, "Struct initializers not currently supported");
		}
		buf = xcmalloc(size_of(state, type), "initializer");
		eat(state, TOK_LBRACE);
		do {
			struct triple *value;
			struct type *value_type;
			size_t value_size;
			int tok;
			comma = 0;
			tok = peek(state);
			if ((tok == TOK_LBRACKET) || (tok == TOK_DOT)) {
				index = designator(state);
			}
			if ((max_index != ELEMENT_COUNT_UNSPECIFIED) &&
				(index > max_index)) {
				error(state, 0, "element beyond bounds");
			}
			value_type = 0;
			if ((type->type & TYPE_MASK) == TYPE_ARRAY) {
				value_type = type->left;
			}
			value = eval_const_expr(state, initializer(state, value_type));
			value_size = size_of(state, value_type);
			if (((type->type & TYPE_MASK) == TYPE_ARRAY) &&
				(max_index == ELEMENT_COUNT_UNSPECIFIED) &&
				(type->elements <= index)) {
				void *old_buf;
				size_t old_size;
				old_buf = buf;
				old_size = size_of(state, type);
				type->elements = index + 1;
				buf = xmalloc(size_of(state, type), "initializer");
				memcpy(buf, old_buf, old_size);
				xfree(old_buf);
			}
			if (value->op == OP_BLOBCONST) {
				memcpy((char *)buf + index * value_size, value->u.blob, value_size);
			}
			else if ((value->op == OP_INTCONST) && (value_size == 1)) {
				*(((uint8_t *)buf) + index) = value->u.cval & 0xff;
			}
			else if ((value->op == OP_INTCONST) && (value_size == 2)) {
				*(((uint16_t *)buf) + index) = value->u.cval & 0xffff;
			}
			else if ((value->op == OP_INTCONST) && (value_size == 4)) {
				*(((uint32_t *)buf) + index) = value->u.cval & 0xffffffff;
			}
			else {
				fprintf(stderr, "%d %d\n",
					value->op, value_size);
				internal_error(state, 0, "unhandled constant initializer");
			}
			if (peek(state) == TOK_COMMA) {
				eat(state, TOK_COMMA);
				comma = 1;
			}
			index += 1;
		} while(comma && (peek(state) != TOK_RBRACE));
		eat(state, TOK_RBRACE);
		result = triple(state, OP_BLOBCONST, type, 0, 0);
		result->u.blob = buf;
	}
	return result;
}

static struct triple *function_definition(
	struct compile_state *state, struct type *type)
{
	struct triple *def, *tmp, *first, *end;
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

	/* Remove the parameter scope */
	end_scope(state);
#if 0
	fprintf(stdout, "\n");
	loc(stdout, state, 0);
	fprintf(stdout, "\n__________ function_definition _________\n");
	print_triple(state, def);
	fprintf(stdout, "__________ function_definition _________ done\n\n");
#endif

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
	if (((type->type & STOR_MASK) == STOR_STATIC) &&
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
		def = function_definition(state, type);
		symbol(state, ident, &ident->sym_ident, def, type);
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
 * Data structurs for optimation.
 */

static void do_use_block(
	struct block *used, struct block_set **head, struct block *user, 
	int front)
{
	struct block_set **ptr, *new;
	if (!used)
		return;
	if (!user)
		return;
	ptr = head;
	while(*ptr) {
		if ((*ptr)->member == user) {
			return;
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
}
static void do_unuse_block(
	struct block *used, struct block_set **head, struct block *unuser)
{
	struct block_set *use, **ptr;
	ptr = head;
	while(*ptr) {
		use = *ptr;
		if (use->member == unuser) {
			*ptr = use->next;
			memset(use, -1, sizeof(*use));
			xfree(use);
		}
		else {
			ptr = &use->next;
		}
	}
}

static void use_block(struct block *used, struct block *user)
{
	/* Append new to the head of the list, print_block
	 * depends on this.
	 */
	do_use_block(used, &used->use, user, 1); 
	used->users++;
}
static void unuse_block(struct block *used, struct block *unuser)
{
	do_unuse_block(used, &used->use, unuser); 
	used->users--;
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



static int do_walk_triple(struct compile_state *state,
	struct triple *ptr, int depth,
	int (*cb)(struct compile_state *state, struct triple *ptr, int depth)) 
{
	int result;
	result = cb(state, ptr, depth);
	if ((result == 0) && (ptr->op == OP_LIST)) {
		struct triple *list;
		list = ptr;
		ptr = RHS(list, 0);
		do {
			result = do_walk_triple(state, ptr, depth + 1, cb);
			if (ptr->next->prev != ptr) {
				internal_error(state, ptr->next, "bad prev");
			}
			ptr = ptr->next;
			
		} while((result == 0) && (ptr != RHS(list, 0)));
	}
	return result;
}

static int walk_triple(
	struct compile_state *state, 
	struct triple *ptr, 
	int (*cb)(struct compile_state *state, struct triple *ptr, int depth))
{
	return do_walk_triple(state, ptr, 0, cb);
}

static void do_print_prefix(int depth)
{
	int i;
	for(i = 0; i < depth; i++) {
		printf("  ");
	}
}

#define PRINT_LIST 1
static int do_print_triple(struct compile_state *state, struct triple *ins, int depth)
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
	do_print_prefix(depth);
	display_triple(stdout, ins);

	if ((ins->op == OP_BRANCH) && ins->use) {
		internal_error(state, ins, "branch used?");
	}
#if 0
	{
		struct triple_set *user;
		for(user = ins->use; user; user = user->next) {
			printf("use: %p\n", user->member);
		}
	}
#endif
	if (triple_is_branch(state, ins)) {
		printf("\n");
	}
	return 0;
}

static void print_triple(struct compile_state *state, struct triple *ins)
{
	walk_triple(state, ins, do_print_triple);
}

static void print_triples(struct compile_state *state)
{
	print_triple(state, state->main_function);
}

struct cf_block {
	struct block *block;
};
static void find_cf_blocks(struct cf_block *cf, struct block *block)
{
	if (!block || (cf[block->vertex].block == block)) {
		return;
	}
	cf[block->vertex].block = block;
	find_cf_blocks(cf, block->left);
	find_cf_blocks(cf, block->right);
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
		block = cf[i].block;
		if (!block)
			continue;
		printf("(%p) %d:", block, block->vertex);
		if (block->left) {
			printf(" %d", block->left->vertex);
		}
		if (block->right && (block->right != block->left)) {
			printf(" %d", block->right->vertex);
		}
		printf("\n");
	}

	xfree(cf);
}


static struct block *basic_block(struct compile_state *state,
	struct triple *first)
{
	struct block *block;
	struct triple *ptr;
	int op;
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
		if ((ptr != first) && (ptr->op == OP_LABEL) && ptr->use) {
			break;
		}
		block->last = ptr;
		/* If ptr->u is not used remember where the baic block is */
		if (!is_const(ptr)) {
			ptr->u.block = block;
		}
		if (ptr->op == OP_BRANCH) {
			break;
		}
		ptr = ptr->next;
	} while (ptr != RHS(state->main_function, 0));
	if (ptr == RHS(state->main_function, 0))
		return block;
	op = ptr->op;
	if (op == OP_LABEL) {
		block->left = basic_block(state, ptr);
		block->right = 0;
		use_block(block->left, block);
	}
	else if (op == OP_BRANCH) {
		block->left = 0;
		/* Trace the branch target */
		block->right = basic_block(state, TARG(ptr, 0));
		use_block(block->right, block);
		/* If there is a test trace the branch as well */
		if (TRIPLE_RHS(ptr->sizes)) {
			block->left = basic_block(state, ptr->next);
			use_block(block->left, block);
		}
	}
	else {
		internal_error(state, 0, "Bad basic block split");
	}
	return block;
}


static void walk_blocks(struct compile_state *state,
	void (*cb)(struct compile_state *state, struct block *block, void *arg),
	void *arg)
{
	struct triple *ptr, *first;
	struct block *last_block;
	last_block = 0;
	first = RHS(state->main_function, 0);
	ptr = first;
	do {
		struct block *block;
		if (ptr->op == OP_LABEL) {
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
	struct triple *ptr;

	printf("\nblock: %p (%d), %p<-%p %p<-%p\n", 
		block, 
		block->vertex,
		block->left, 
		block->left && block->left->use?block->left->use->member : 0,
		block->right, 
		block->right && block->right->use?block->right->use->member : 0);
	if (block->first->op == OP_LABEL) {
		printf("%p:\n", block->first);
	}
	for(ptr = block->first; ; ptr = ptr->next) {
		struct triple_set *user;
		int op = ptr->op;
		
		if (!IS_CONST_OP(op)) {
			if (ptr->u.block != block) {
				internal_error(state, ptr, 
					"Wrong block pointer: %p\n",
					ptr->u.block);
			}
		}
		if (op == OP_ADECL) {
			for(user = ptr->use; user; user = user->next) {
				if (!user->member->u.block) {
					internal_error(state, user->member, 
						"Use %p not in a block?\n",
						user->member);
				}
			}
		}
		display_triple(stdout, ptr);

		/* Sanity checks... */
		valid_ins(state, ptr);
		for(user = ptr->use; user; user = user->next) {
			struct triple *use;
			use = user->member;
			valid_ins(state, use);
			if (!IS_CONST_OP(user->member->op) &&
				!user->member->u.block) {
				internal_error(state, user->member,
					"Use %p not in a block?",
					user->member);
			}
		}

		if (ptr == block->last)
			break;
	}
	printf("\n");
}


static void print_blocks(struct compile_state *state)
{
	printf("--------------- blocks ---------------\n");
	walk_blocks(state, print_block, 0);
}

static void prune_nonblock_triples(struct compile_state *state)
{
	struct block *block;
	struct triple *first, *ins;
	/* Delete the triples not in a basic block */
	first = RHS(state->main_function, 0);
	block = 0;
	ins = first;
	do {
		if (ins->op == OP_LABEL) {
			block = ins->u.block;
		}
		ins = ins->next;
		if (!block) {
			release_triple(state, ins->prev);
		}
	} while(ins != first);
}

static void setup_basic_blocks(struct compile_state *state)
{
	/* Find the basic blocks */
	state->last_vertex = 0;
	state->first_block = basic_block(state, RHS(state->main_function,0));
	/* Delete the triples not in a basic block */
	prune_nonblock_triples(state);
	/* Find the last basic block */
	state->last_block = RHS(state->main_function, 0)->prev->u.block;
	if (!state->last_block) {
		internal_error(state, 0, "end not used?");
	}
	/* Insert an extra unused edge from start to the end 
	 * This helps with reverse control flow calculations.
	 */
	use_block(state->first_block, state->last_block);
	/* If we are debugging print what I have just done */
	if (state->debug & DEBUG_BASIC_BLOCKS) {
		print_blocks(state);
		print_control_flow(state);
	}
}

static void free_basic_block(struct compile_state *state, struct block *block)
{
	struct block_set *entry, *next;
	struct block *child;
	if (!block) {
		return;
	}
	if (block->vertex == -1) {
		return;
	}
	block->vertex = -1;
	if (block->left) {
		unuse_block(block->left, block);
	}
	if (block->right) {
		unuse_block(block->right, block);
	}
	if (block->idom) {
		unidom_block(block->idom, block);
	}
	block->idom = 0;
	if (block->ipdom) {
		unipdom_block(block->ipdom, block);
	}
	block->ipdom = 0;
	for(entry = block->use; entry; entry = next) {
		next = entry->next;
		child = entry->member;
		unuse_block(block, child);
		if (child->left == block) {
			child->left = 0;
		}
		if (child->right == block) {
			child->right = 0;
		}
	}
	for(entry = block->idominates; entry; entry = next) {
		next = entry->next;
		child = entry->member;
		unidom_block(block, child);
		child->idom = 0;
	}
	for(entry = block->domfrontier; entry; entry = next) {
		next = entry->next;
		child = entry->member;
		undomf_block(block, child);
	}
	for(entry = block->ipdominates; entry; entry = next) {
		next = entry->next;
		child = entry->member;
		unipdom_block(block, child);
		child->ipdom = 0;
	}
	for(entry = block->ipdomfrontier; entry; entry = next) {
		next = entry->next;
		child = entry->member;
		unipdomf_block(block, child);
	}
	if (block->users != 0) {
		internal_error(state, 0, "block still has users");
	}
	free_basic_block(state, block->left);
	block->left = 0;
	free_basic_block(state, block->right);
	block->right = 0;
	memset(block, -1, sizeof(*block));
	xfree(block);
}

static void free_basic_blocks(struct compile_state *state)
{
	struct triple *first, *ins;
	free_basic_block(state, state->first_block);
	state->last_vertex = 0;
	state->first_block = state->last_block = 0;
	first = RHS(state->main_function, 0);
	ins = first;
	do {
		if (!is_const(ins)) {
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
	vertex = initialize_sdblock(sd, block, block->left, vertex);
	vertex = initialize_sdblock(sd, block, block->right, vertex);
	return vertex;
}

static int initialize_sdpblock(struct sdom_block *sd,
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
		vertex = initialize_sdpblock(sd, block, user->member, vertex);
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
		struct block *block;
		block = sd[i].block;
		parent = sd[i].parent;
		/* Step 2 */
		if (block->left) {
			v = &sd[block->left->vertex];
			u = !(v->ancestor)? v : (compress_ancestors(v), v->label);
			if (u->sdom->vertex < sd[i].sdom->vertex) {
				sd[i].sdom = u->sdom;
			}
		}
		if (block->right && (block->right != block->left)) {
			v = &sd[block->right->vertex];
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
	/* Step 1 initialize the basic block information */
	sd = xcmalloc(sizeof(*sd) * (state->last_vertex + 1), "sdom_state");

	initialize_sdpblock(sd, 0, state->last_block, 0);

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
	struct block_set *user;
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
	if (block->left && block->left->idom != block) {
		domf_block(block, block->left);
	}
	if (block->right && block->right->idom != block) {
		domf_block(block, block->right);
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
	if (block->left && block->left->ipdom != block) {
		ipdomf_block(block, block->left);
	}
	if (block->right && block->right->ipdom != block) {
		ipdomf_block(block, block->right);
	}
	for(user = block->idominates; user; user = user->next) {
		struct block_set *frontier;
		child = user->member;
		for(frontier = child->ipdomfrontier; frontier; frontier = frontier->next) {
			if (frontier->member->ipdom != block) {
				ipdomf_block(block, frontier->member);
			}
		}
	}
}

static int print_dominated(
	struct compile_state *state, struct block *block, int vertex)
{
	struct block_set *user;

	if (!block || (block->vertex != vertex + 1)) {
		return vertex;
	}
	vertex += 1;

	printf("%d:", block->vertex);
	for(user = block->idominates; user; user = user->next) {
		printf(" %d", user->member->vertex);
		if (user->member->idom != block) {
			internal_error(state, user->member->first, "bad idom");
		}
	}
	printf("\n");
	vertex = print_dominated(state, block->left, vertex);
	vertex = print_dominated(state, block->right, vertex);
	return vertex;
}

static void print_dominators(struct compile_state *state)
{
	printf("\ndominates\n");
	print_dominated(state, state->first_block, 0);
}


static int print_frontiers(
	struct compile_state *state, struct block *block, int vertex)
{
	struct block_set *user;

	if (!block || (block->vertex != vertex + 1)) {
		return vertex;
	}
	vertex += 1;

	printf("%d:", block->vertex);
	for(user = block->domfrontier; user; user = user->next) {
		printf(" %d", user->member->vertex);
	}
	printf("\n");

	vertex = print_frontiers(state, block->left, vertex);
	vertex = print_frontiers(state, block->right, vertex);
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
	if (state->debug & DEBUG_FDOMINATORS) {
		print_dominators(state);
		print_dominance_frontiers(state);
		print_control_flow(state);
	}
}



static int print_ipdominated(
	struct compile_state *state, struct block *block, int vertex)
{
	struct block_set *user;

	if (!block || (block->vertex != vertex + 1)) {
		return vertex;
	}
	vertex += 1;

	printf("%d:", block->vertex);
	for(user = block->ipdominates; user; user = user->next) {
		printf(" %d", user->member->vertex);
		if (user->member->ipdom != block) {
			internal_error(state, user->member->first, "bad ipdom");
		}
	}
	printf("\n");
	for(user = block->use; user; user = user->next) {
		vertex = print_ipdominated(state, user->member, vertex);
	}
	return vertex;
}

static void print_ipdominators(struct compile_state *state)
{
	printf("\nipdominates\n");
	print_ipdominated(state, state->last_block, 0);
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
	if (state->debug & DEBUG_RDOMINATORS) {
		print_ipdominators(state);
		print_ipdominance_frontiers(state);
		print_control_flow(state);
	}
}


static void insert_phi_operations(struct compile_state *state)
{
	size_t size;
	struct triple *first;
	int *has_already, *work;
	struct block *work_list, **work_list_tail;
	int iter;
	struct triple *var;

	size = sizeof(int) * (state->last_vertex + 1);
	has_already = xcmalloc(size, "has_already");
	work =        xcmalloc(size, "work");
	iter = 0;

	first = RHS(state->main_function, 0);
	for(var = first->next; var != first ; var = var->next) {
		struct block *block;
		struct triple_set *user;
		if ((var->op != OP_ADECL) || !var->use) {
			continue;
		}
		iter += 1;
		work_list = 0;
		work_list_tail = &work_list;
		for(user = var->use; user; user = user->next) {
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
				phi = alloc_triple(
					state, OP_PHI, var->type, in_edges, 
					front->first->filename, 
					front->first->line,
					front->first->col);
				phi->u.block = front;
				MISC(phi, 0) = var;
				use_triple(var, phi);
				/* Insert the phi functions immediately after the label */
				insert_triple(state, front->first->next, phi);
				if (front->first == front->last) {
					front->last = front->first->next;
				}
				has_already[front->vertex] = iter;
				
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

/*
 * C(V)
 * S(V)
 */
static void fixup_block_phi_variables(
	struct compile_state *state, struct block *parent, struct block *block)
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
			/* Find the current value of the variable */
			val = var->use->member;
			if ((val->op == OP_WRITE) || (val->op == OP_READ)) {
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
	struct compile_state *state, struct block *block)
{
	struct block_set *user;
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
			if (!var->use) {
				error(state, ptr, "variable used without being set");
			}
			/* Find the current value of the variable */
			val = var->use->member;
			if ((val->op == OP_WRITE) || (val->op == OP_READ)) {
				internal_error(state, val, "bad value in read");
			}
			propogate_use(state, ptr, val);
			release_triple(state, ptr);
			continue;
		}
		/* LHS(A) */
		if (ptr->op == OP_WRITE) {
			struct triple *var, *val;
			var = LHS(ptr, 0);
			val = RHS(ptr, 0);
			if ((val->op == OP_WRITE) || (val->op == OP_READ)) {
				internal_error(state, val, "bad value in write");
			}
			propogate_use(state, ptr, val);
			unuse_triple(var, ptr);
			/* Push OP_WRITE ptr->right onto a stack of variable uses */
			push_triple(var, val);
		}
		if (ptr->op == OP_PHI) {
			struct triple *var;
			var = MISC(ptr, 0);
			/* Push OP_PHI onto a stack of variable uses */
			push_triple(var, ptr);
		}
		last = ptr;
	}
	block->last = last;

	/* Fixup PHI functions in the cf successors */
	fixup_block_phi_variables(state, block, block->left);
	fixup_block_phi_variables(state, block, block->right);
	/* rename variables in the dominated nodes */
	for(user = block->idominates; user; user = user->next) {
		rename_block_variables(state, user->member);
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
			var = LHS(ptr, 0);
			/* Pop OP_WRITE ptr->right from the stack of variable uses */
			pop_triple(var, RHS(ptr, 0));
			release_triple(state, ptr);
			continue;
		}
		if (ptr->op == OP_PHI) {
			struct triple *var;
			var = MISC(ptr, 0);
			/* Pop OP_WRITE ptr->right from the stack of variable uses */
			pop_triple(var, ptr);
		}
		last = ptr;
	}
	block->last = last;
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

static void transform_to_ssa_form(struct compile_state *state)
{
	insert_phi_operations(state);
#if 0
	printf("@%s:%d\n", __FILE__, __LINE__);
	print_blocks(state);
#endif
	rename_block_variables(state, state->first_block);
	prune_block_variables(state, state->first_block);
}


static void transform_from_ssa_form(struct compile_state *state)
{
	/* To get out of ssa form we insert moves on the incoming
	 * edges to blocks containting phi functions.
	 */
	struct triple *first;
	struct triple *phi, *next;

	/* Walk all of the operations to find the phi functions */
	first = RHS(state->main_function, 0);
	for(phi = first->next; phi != first ; phi = next) {
		struct block_set *set;
		struct block *block;
		struct triple **slot;
		struct triple *var, *read;
		int edge;
		next = phi->next;
		if (phi->op != OP_PHI) {
			continue;
		}
		block = phi->u.block;
		slot  = &RHS(phi, 0);

		/* A variable to replace the phi function */
		var = post_triple(state, phi, OP_ADECL, phi->type, 0,0);
		/* A read of the single value that is set into the variable */
		read = post_triple(state, var, OP_READ, phi->type, var, 0);
		use_triple(var, read);

		/* Replaces uses of the phi with variable reads */
		propogate_use(state, phi, read);

		/* Walk all of the incoming edges/blocks and insert moves.
		 */
		for(edge = 0, set = block->use; set; set = set->next, edge++) {
			struct block *eblock;
			struct triple *move;
			struct triple *val;
			eblock = set->member;
			val = slot[edge];
			unuse_triple(val, phi);

			if (val == phi) {
				continue;
			}

			move = post_triple(state, 
				val, OP_WRITE, phi->type, var, val);
			use_triple(val, move);
			use_triple(var, move);
		}
		release_triple(state, phi);
	}
	
}

static void insert_copies_to_phi(struct compile_state *state)
{
	/* To get out of ssa form we insert moves on the incoming
	 * edges to blocks containting phi functions.
	 */
	struct triple *first;
	struct triple *phi;

	/* Walk all of the operations to find the phi functions */
	first = RHS(state->main_function, 0);
	for(phi = first->next; phi != first ; phi = phi->next) {
		struct block_set *set;
		struct block *block;
		struct triple **slot;
		int edge;
		if (phi->op != OP_PHI) {
			continue;
		}
		if (ID_REG(phi->id) == REG_UNSET) {
			phi->id = MK_REG_ID(alloc_virtual_reg(), 
				ID_REG_CLASSES(phi->id));
		}
		block = phi->u.block;
		slot  = &RHS(phi, 0);
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

			move = build_triple(state, OP_COPY, phi->type, val, 0,
				val->filename, val->line, val->col);
			move->u.block = eblock;
			move->id = phi->id;
			use_triple(val, move);
			
			slot[edge] = move;
			unuse_triple(val, phi);
			use_triple(move, phi);

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
		}
	}
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
			/* If the triple is not a definition skip it. */
			if (!triple_is_def(state, ptr)) {
				continue;
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
			struct reg_block *rb;
			rb = &blocks[i];
			/* Add the left successor's input set to in */
			if (rb->block->left) {
				change |= reg_in(state, blocks, rb, rb->block->left);
			}
			/* Add the right successor's input set to in */
			if ((rb->block->right) && 
				(rb->block->right != rb->block->left)) {
				change |= reg_in(state, blocks, rb, rb->block->right);
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

typedef struct triple *(*wvl_cb_t)(
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
			
			/* Remove the current definition from live */
			do_triple_unset(&live, ptr);
			
			/* If the current instruction was deleted continue */
			if (!cb(state, blocks, live, rb, ptr, arg)) {
				if (block->last == ptr) {
					block->last = prev;
				}
				continue;
			}
			
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
	first = RHS(state->main_function, 0);
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
	int color;
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
		internal_warning(state, triple, "awakening noop?");
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
	struct triple *first, *ins;

	/* Setup the work list */
	work_list = 0;
	work_list_tail = &work_list;

	first = RHS(state->main_function, 0);

	/* Count how many triples I have */
	triples = count_triples(state);

	/* Now put then in an array and mark all of the triples dead */
	dtriple = xcmalloc(sizeof(*dtriple) * (triples + 1), "dtriples");
	
	ins = first;
	i = 1;
	block = 0;
	do {
		if (ins->op == OP_LABEL) {
			block = ins->u.block;
		}
		dtriple[i].triple = ins;
		dtriple[i].block  = block;
		dtriple[i].flags  = 0;
		dtriple[i].color  = ins->id;
		ins->id = i;
		/* See if it is an operation we always keep */
#warning "FIXME handle the case of killing a branch instruction"
		if (!triple_is_pure(state, ins) || triple_is_branch(state, ins)) {
			awaken(state, dtriple, &ins, &work_list_tail);
		}
		i++;
		ins = ins->next;
	} while(ins != first);
	while(work_list) {
		struct dead_triple *dt;
		struct block_set *user;
		struct triple **expr;
		dt = work_list;
		work_list = dt->work_next;
		if (!work_list) {
			work_list_tail = &work_list;
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
		/* Wake up the forward control dependencies */
		do {
			expr = triple_targ(state, dt->triple, expr);
			awaken(state, dtriple, expr, &work_list_tail);
		} while(expr);
		/* Wake up the reverse control dependencies of this triple */
		for(user = dt->block->ipdomfrontier; user; user = user->next) {
			awaken(state, dtriple, &user->member->last, &work_list_tail);
		}
	}
	for(dt = &dtriple[1]; dt <= &dtriple[triples]; dt++) {
		if ((dt->triple->op == OP_NOOP) && 
			(dt->flags & TRIPLE_FLAG_ALIVE)) {
			internal_error(state, dt->triple, "noop effective?");
		}
		dt->triple->id = dt->color;	/* Restore the color */
		if (!(dt->flags & TRIPLE_FLAG_ALIVE)) {
#warning "FIXME handle the case of killing a basic block"
			if (dt->block->first == dt->triple) {
				continue;
			}
			if (dt->block->last == dt->triple) {
				dt->block->last = dt->triple->prev;
			}
			release_triple(state, dt->triple);
		}
	}
	xfree(dtriple);
}


struct live_range_edge;
struct live_range {
	struct live_range_edge *edges;
	struct triple *def;
	unsigned color;
	unsigned classes;
	unsigned degree;
	struct live_range *group_next, **group_prev;
};

struct live_range_edge {
	struct live_range_edge *next;
	struct live_range *node;
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
	struct live_range *lr;
	struct live_range *low, **low_tail;
	struct live_range *high, **high_tail;
	unsigned ranges;
};


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
	arch_reg_equivs(state, equivs, reg);
	for(i = 0; (i < MAX_REG_EQUIVS) && equivs[i] != REG_UNSET; i++) {
		used[equivs[i]] = 1;
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
	while((*ptr) && ((*ptr)->left != left) && ((*ptr)->right != right)) {
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
			break;
		}
	}
	for(ptr = &right->edges; *ptr; ptr = &(*ptr)->next) {
		edge = *ptr;
		if (edge->node == left) {
			*ptr = edge->next;
			memset(edge, 0, sizeof(*edge));
			xfree(edge);
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
	lr = &rstate->lr[ins->id];
	expr = triple_rhs(state, ins, 0);
	for(;expr; expr = triple_rhs(state, ins, expr)) {
		struct live_range *lr2;
		if (!*expr || (*expr == parent) || (*expr == ins)) {
			continue;
		}
		lr2 = &rstate->lr[(*expr)->id];
		if (lr->color == lr2->color) {
			internal_error(state, ins, "live range too big");
		}
	}
}

static void initialize_live_ranges(
	struct compile_state *state, struct reg_state *rstate)
{
	struct triple *ins, *first;
	size_t size;
	int i;

	first = RHS(state->main_function, 0);
	/* First count how many live ranges I will need.
	 */
	rstate->ranges = count_triples(state);
	size = sizeof(rstate->lr[0]) * (rstate->ranges + 1);
	rstate->lr = xcmalloc(size, "live_range");
	/* Setup the dummy live range */
	rstate->lr[0].classes = 0;
	rstate->lr[0].color = REG_UNSET;
	rstate->lr[0].def = 0;
	i = 0;
	ins = first;
	do {
		unsigned color, classes;
		/* Find the architecture specific color information */
		color = ID_REG(ins->id);
		classes = ID_REG_CLASSES(ins->id);
		if ((color != REG_UNSET) && (color < MAX_REGISTERS)) {
			classes = arch_reg_regcm(state, color);
		}

		/* If the triple is a variable definition give it a live range. */
		if (triple_is_def(state, ins)) {
			i++;
			ins->id = i;
			rstate->lr[i].def     = ins;
			rstate->lr[i].color   = color;
			rstate->lr[i].classes = classes;
			rstate->lr[i].degree  = 0;
			if (!classes) {
				internal_error(state, ins, 
					"live range without a class");
			}
		}
		/* Otherwise give the triple the dummy live range. */
		else {
			ins->id = 0;
		}
		ins = ins->next;
	} while(ins != first);
	rstate->ranges = i;
	/* Make a second pass to handle achitecture specific register
	 * constraints.
	 */
	ins = first;
	do {
		struct live_range *lr;
		lr = &rstate->lr[ins->id];
		if (lr->color != REG_UNSET) {
			struct triple **expr;
			/* This assumes the virtual register is only
			 * used by one input operation.
			 */
			expr = triple_rhs(state, ins, 0);
			for(;expr; expr = triple_rhs(state, ins, expr)) {
				struct live_range *lr2;
				if (!*expr || (ins == *expr)) {
					continue;
				}
				lr2 = &rstate->lr[(*expr)->id];
				if (lr->color == lr2->color) {
					different_colored(state, rstate, 
						ins, *expr);
					(*expr)->id = ins->id;
					
				}
			}
		}
		ins = ins->next;
	} while(ins != first);

	/* Make a third pass and forget the virtual registers */
	for(i = 1; i <= rstate->ranges; i++) {
		if (rstate->lr[i].color >= MAX_REGISTERS) {
			rstate->lr[i].color = REG_UNSET;
		}
	}
}

static struct triple *graph_ins(
	struct compile_state *state, 
	struct reg_block *blocks, struct triple_reg_set *live, 
	struct reg_block *rb, struct triple *ins, void *arg)
{
	struct reg_state *rstate = arg;
	struct live_range *def;
	struct triple_reg_set *entry;

	/* If the triple does not start a live range
	 * we do not have a definition to add to
	 * the interference graph.
	 */
	if (ins->id <= 0) {
		return ins;
	}
	def = &rstate->lr[ins->id];
	
	/* Create an edge between ins and everything that is
	 * alive, unless the live_range cannot share
	 * a physical register with ins.
	 */
	for(entry = live; entry; entry = entry->next) {
		struct live_range *lr;
		lr= &rstate->lr[entry->member->id];
		if (!arch_regcm_intersect(def->classes, lr->classes)) {
			continue;
		}
		add_live_edge(rstate, def, lr);
	}
	return ins;
}


static struct triple *print_interference_ins(
	struct compile_state *state, 
	struct reg_block *blocks, struct triple_reg_set *live, 
	struct reg_block *rb, struct triple *ins, void *arg)
{
	struct reg_state *rstate = arg;
	struct live_range *lr;

	lr = &rstate->lr[ins->id];
	display_triple(stdout, ins);
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
		printf("        edges:");
		for(entry = lr->edges; entry; entry = entry->next) {
			printf(" %-10p", entry->node->def);
		}
		printf("\n");
	}
	if (triple_is_branch(state, ins)) {
		printf("\n");
	}
	return ins;
}

#if DEBUG_COLOR_GRAPH > 1
#define cgdebug_printf(...) fprintf(stdout, __VA_ARGS__)
#define cgdebug_flush() fflush(stdout)
#elif DEBUG_COLOR_GRAPH == 1
#define cgdebug_printf(...) fprintf(stderr, __VA_ARGS__)
#define cgdebug_flush() fflush(stderr)
#else
#define cgdebug_printf(...)
#define cgdebug_flush()
#endif

static void select_free_color(struct compile_state *state, 
	struct reg_state *rstate, struct live_range *range)
{
	struct triple_set *entry;
	struct live_range *phi;
	struct live_range_edge *edge;
	char used[MAX_REGISTERS];
	struct triple **expr;

	/* If a color is already assigned don't change it */
	if (range->color != REG_UNSET) {
		return;
	}
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
#if DEBUG_COLOR_GRAPH > 1
	{
		int i;
		i = 0;
		for(edge = range->edges; edge; edge = edge->next) {
			i++;
		}
		cgdebug_printf("\n%s edges: %d @%s:%d.%d\n", 
			tops(range->def->op), i, 
			range->def->filename, range->def->line, range->def->col);
		for(i = 0; i < MAX_REGISTERS; i++) {
			if (used[i]) {
				cgdebug_printf("used: %s\n",
					arch_reg_str(i));
			}
		}
	}	
#endif

	/* If I feed into an expression reuse it's color.
	 * This should help remove copies in the case of 2 register instructions
	 * and phi functions.
	 */
	phi = 0;
	entry = range->def->use;
	for(;(range->color == REG_UNSET) && entry; entry = entry->next) {
		struct live_range *lr;
		lr = &rstate->lr[entry->member->id];
		if (entry->member->id == 0) {
			continue;
		}
		if (!phi && (lr->def->op == OP_PHI) && 
			!interfere(rstate, range, lr)) {
			phi = lr;
		}
		if ((lr->color == REG_UNSET) ||
			((lr->classes & range->classes) == 0) ||
			(used[lr->color])) {
			continue;
		}
		if (interfere(rstate, range, lr)) {
			continue;
		}
		range->color = lr->color;
	}
	/* If I feed into a phi function reuse it's color of the color
	 * of something else that feeds into the phi function.
	 */
	if (phi) {
		if (phi->color != REG_UNSET) {
			if (used[phi->color]) {
				range->color = phi->color;
			}
		}
		else {
			expr = triple_rhs(state, phi->def, 0);
			for(; expr; expr = triple_rhs(state, phi->def, expr)) {
				struct live_range *lr;
				if (!*expr) {
					continue;
				}
				lr = &rstate->lr[(*expr)->id];
				if ((lr->color == REG_UNSET) || 
					((lr->classes & range->classes) == 0) ||
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
	if (range->color == REG_UNSET) {
		expr = triple_rhs(state, range->def, 0);
		for(; expr; expr = triple_rhs(state, range->def, expr)) {
			struct live_range *lr;
			if (!*expr) {
				continue;
			}
			lr = &rstate->lr[(*expr)->id];
			if ((lr->color == -1) || 
				((lr->classes & range->classes) == 0) ||
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
		int i;
		for(edge = range->edges; edge; edge = edge->next) {
			if (edge->node->color == REG_UNSET) {
				continue;
			}
			warning(state, edge->node->def, "reg %s", 
				arch_reg_str(edge->node->color));
		}
		warning(state, range->def, "classes: %x",
			range->classes);
		for(i = 0; i < MAX_REGISTERS; i++) {
			if (used[i]) {
				warning(state, range->def, "used: %s",
					arch_reg_str(i));
			}
		}
#if DEBUG_COLOR_GRAPH < 2
		error(state, range->def, "too few registers");
#else
		internal_error(state, range->def, "too few registers");
#endif
	}
	range->classes = arch_reg_regcm(state, range->color);
	return;
}

static void color_graph(struct compile_state *state, struct reg_state *rstate)
{
	struct live_range_edge *edge;
	struct live_range *range;
	if (rstate->low) {
		cgdebug_printf("Lo: ");
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
		cgdebug_printf("Hi: ");
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
		return;
	}
	cgdebug_printf(" %d\n", range - rstate->lr);
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
			cgdebug_printf("Moving...%d to low\n", node - rstate->lr);
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
	color_graph(state, rstate);
	cgdebug_printf("Coloring %d @%s:%d.%d:", 
		range - rstate->lr,
		range->def->filename, range->def->line, range->def->col);
	cgdebug_flush();
	select_free_color(state, rstate, range);
	if (range->color == -1) {
		internal_error(state, range->def, "select_free_color did not?");
	}
	cgdebug_printf(" %s\n", arch_reg_str(range->color));
}

static void color_triples(struct compile_state *state, struct reg_state *rstate)
{
	struct live_range *lr;
	struct triple *first, *triple;
	first = RHS(state->main_function, 0);
	triple = first;
	do {
		if ((triple->id < 0) || (triple->id > rstate->ranges)) {
			internal_error(state, triple, 
				"triple without a live range");
		}
		lr = &rstate->lr[triple->id];
		triple->id = MK_REG_ID(lr->color, 0);
		triple = triple->next;
	} while (triple != first);
}

static void print_interference_block(
	struct compile_state *state, struct block *block, void *arg)

{
	struct reg_state *rstate = arg;
	struct reg_block *rb;
	struct triple *ptr;
	int phi_present;
	int done;
	rb = &rstate->blocks[block->vertex];

	printf("\nblock: %p (%d), %p<-%p %p<-%p\n", 
		block, 
		block->vertex,
		block->left, 
		block->left && block->left->use?block->left->use->member : 0,
		block->right, 
		block->right && block->right->use?block->right->use->member : 0);
	if (rb->in) {
		struct triple_reg_set *in_set;
		printf("        in:");
		for(in_set = rb->in; in_set; in_set = in_set->next) {
			printf(" %-10p", in_set->member);
		}
		printf("\n");
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
			printf("     in(%d):", edge);
			for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
				struct triple **slot;
				done = (ptr == block->last);
				if (ptr->op != OP_PHI) {
					continue;
				}
				slot = &RHS(ptr, 0);
				printf(" %-10p", slot[edge]);
			}
			printf("\n");
		}
	}
	if (block->first->op == OP_LABEL) {
		printf("%p:\n", block->first);
	}
	for(done = 0, ptr = block->first; !done; ptr = ptr->next) {
		struct triple_set *user;
		struct live_range *lr;
		unsigned id;
		int op;
		op = ptr->op;
		done = (ptr == block->last);
		lr = &rstate->lr[ptr->id];
		
		if (!IS_CONST_OP(op)) {
			if (ptr->u.block != block) {
				internal_error(state, ptr, 
					"Wrong block pointer: %p",
					ptr->u.block);
			}
		}
		if (op == OP_ADECL) {
			for(user = ptr->use; user; user = user->next) {
				struct live_range *lr;
				lr = &rstate->lr[user->member->id];
				if (!user->member->u.block) {
					internal_error(state, user->member, 
						"Use %p not in a block?",
						user->member);
				}
				
			}
		}
		id = ptr->id;
		ptr->id = MK_REG_ID(lr->color, 0);
		display_triple(stdout, ptr);
		ptr->id = id;

		if (lr->edges > 0) {
			struct live_range_edge *edge;
			printf("           ");
			for(edge = lr->edges; edge; edge = edge->next) {
				printf(" %-10p", edge->node->def);
			}
			printf("\n");
		}
		/* Do a bunch of sanity checks */
		valid_ins(state, ptr);
		if ((ptr->id < 0) || (ptr->id > rstate->ranges)) {
			internal_error(state, ptr, "Invalid triple id: %d",
				ptr->id);
		}
		for(user = ptr->use; user; user = user->next) {
			struct triple *use;
			struct live_range *ulr;
			use = user->member;
			valid_ins(state, use);
			if ((use->id < 0) || (use->id > rstate->ranges)) {
				internal_error(state, use, "Invalid triple id: %d",
					use->id);
			}
			ulr = &rstate->lr[user->member->id];
			if (!IS_CONST_OP(user->member->op) &&
				!user->member->u.block) {
				internal_error(state, user->member,
					"Use %p not in a block?",
					user->member);
			}
		}
	}
	if (rb->out) {
		struct triple_reg_set *out_set;
		printf("       out:");
		for(out_set = rb->out; out_set; out_set = out_set->next) {
			printf(" %-10p", out_set->member);
		}
		printf("\n");
	}
	printf("\n");
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
			if (first->degree <= mid->degree) {
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
		pick->group_prev = join_tail;
	}
	else {
		if (!first->def) {
			first = 0;
		}
		join = first;
	}
	return join;
}

static void allocate_registers(struct compile_state *state)
{
	struct reg_state rstate;
	struct live_range **point, **next;
	int i;

	/* Clear out the reg_state */
	memset(&rstate, 0, sizeof(rstate));

	/* Compute the variable lifetimes */
	rstate.blocks = compute_variable_lifetimes(state);

	/* Allocate and initialize the live ranges */
	initialize_live_ranges(state, &rstate);

	/* Compute the interference graph */
	walk_variable_lifetimes(
		state, rstate.blocks, graph_ins, &rstate);

	/* Display the interference graph if desired */
	if (state->debug & DEBUG_INTERFERENCE) {
		printf("\nlive variables by block\n");
		walk_blocks(state, print_interference_block, &rstate);
		printf("\nlive variables by instruction\n");
		walk_variable_lifetimes(
			state, rstate.blocks, 
			print_interference_ins, &rstate);
	}

	/* Do not perform coalescing!  It is a neat idea but it limits what
	 * we can do later.  It has no benefits that decrease register pressure.
	 * It only decreases instruction count.
	 *
	 * It might be worth testing this reducing the number of
	 * live_ragnes as opposed to splitting them seems to help.
	 */

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
			cgdebug_printf("Lo: %5d degree %5d%s\n", 
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
			cgdebug_printf("hi: %5d degree %5d%s\n", 
				range - rstate.lr, range->degree,
				(range->color != REG_UNSET) ? " (colored)": "");
		}
		
	}
	/* Color the live_ranges */
	color_graph(state, &rstate);

	/* Move the colors from the graph to the triples */
	color_triples(state, &rstate);

	/* Free the edges on each node */
	for(i = 1; i <= rstate.ranges; i++) {
		remove_live_edges(&rstate, &rstate.lr[i]);
	}
	xfree(rstate.lr);

	/* Free the variable lifetime information */
	free_variable_lifetimes(state, rstate.blocks);

}

/* Sparce Conditional Constant Propogation
 * =========================================
 */
struct ssa_edge;
struct flow_block;
struct lattice_node {
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
struct flow_block {
	struct block *block;
	struct flow_edge *in;
	struct flow_edge *out;
	struct flow_edge left, right;
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
	}
	return fedge;
}

static void scc_add_sedge(struct compile_state *state, struct scc_state *scc,
	struct ssa_edge *sedge)
{
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
	first = RHS(state->main_function, 0);
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
#if DEBUG_SCC
	fprintf(stderr, "ins_count: %d ssa_edge_count: %d vertex_count: %d\n",
		ins_count, ssa_edge_count, state->last_vertex);
#endif
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
		ins->id = 0;
		if ((ins->op == OP_LABEL) && (block != ins->u.block)) {
			block = ins->u.block;
			if (!block) {
				internal_error(state, ins, "label without block");
			}
			fblock_index += 1;
			block->vertex = fblock_index;
			fblock = &scc->flow_blocks[fblock_index];
			fblock->block = block;
		}
		{
			struct lattice_node *lnode;
			ins_index += 1;
			ins->id = ins_index;
			lnode = &scc->lattice[ins_index];
			lnode->def = ins;
			lnode->out = 0;
			lnode->fblock = fblock;
			lnode->val = ins; /* LATTICE HIGH */
		}
		ins = ins->next;
	} while(ins != first);
	/* Initialize pass two collect up the edges */
	block = 0;
	fblock = 0;
	ins = first;
	do {
		if ((ins->op == OP_LABEL) && (block != ins->u.block)) {
			struct flow_edge *fedge, **ftail;
			struct block_set *bedge;
			block = ins->u.block;
			fblock = &scc->flow_blocks[block->vertex];
			fblock->in = 0;
			fblock->out = 0;
			ftail = &fblock->out;
			if (block->left) {
				fblock->left.dst = &scc->flow_blocks[block->left->vertex];
				if (fblock->left.dst->block != block->left) {
					internal_error(state, 0, "block mismatch");
				}
				fblock->left.out_next = 0;
				*ftail = &fblock->left;
				ftail = &fblock->left.out_next;
			}
			if (block->right) {
				fblock->right.dst = &scc->flow_blocks[block->right->vertex];
				if (fblock->right.dst->block != block->right) {
					internal_error(state, 0, "block mismatch");
				}
				fblock->right.out_next = 0;
				*ftail = &fblock->right;
				ftail = &fblock->right.out_next;
			}
			for(fedge = fblock->out; fedge; fedge = fedge->out_next) {
				fedge->src = fblock;
				fedge->work_next = fedge->work_prev = fedge;
				fedge->executable = 0;
			}
			ftail = &fblock->in;
			for(bedge = block->use; bedge; bedge = bedge->next) {
				struct block *src_block;
				struct flow_block *sfblock;
				struct flow_edge *sfedge;
				src_block = bedge->member;
				sfblock = &scc->flow_blocks[src_block->vertex];
				sfedge = 0;
				if (src_block->left == block) {
					sfedge = &sfblock->left;
				} else {
					sfedge = &sfblock->right;
				}
				*ftail = sfedge;
				ftail = &sfedge->in_next;
				sfedge->in_next = 0;
			}
		}
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
		ins = ins->next;
	} while(ins != first);
	/* Setup a dummy block 0 as a node above the start node */
	{
		struct flow_block *fblock, *dst;
		struct flow_edge *fedge;
		fblock = &scc->flow_blocks[0];
		fblock->block = 0;
		fblock->in = 0;
		fblock->out = &fblock->left;
		dst = &scc->flow_blocks[state->first_block->vertex];
		fedge = &fblock->left;
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
#if DEBUG_SCC
	fprintf(stderr, "ins_index: %d ssa_edge_index: %d fblock_index: %d\n",
		ins_index, ssa_edge_index, fblock_index);
#endif
}

	
static void free_scc_state(
	struct compile_state *state, struct scc_state *scc)
{
	int i;
	for(i = 0; i < scc->ins_count; i++) {
		if (scc->lattice[i].val && 
			(scc->lattice[i].val != scc->lattice[i].def)) {
			xfree(scc->lattice[i].val);
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

static void scc_visit_phi(struct compile_state *state, struct scc_state *scc, 
	struct lattice_node *lnode)
{
	struct lattice_node *tmp;
	struct triple **slot;
	struct flow_edge *fedge;
	int index;
	if (lnode->def->op != OP_PHI) {
		internal_error(state, lnode->def, "not phi");
	}
	/* default to lattice high */
	lnode->val = lnode->def;
	slot = &RHS(lnode->def, 0);
	index = 0;
	for(fedge = lnode->fblock->in; fedge; index++, fedge = fedge->in_next) {
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
			lnode->val = tmp->val;
		}
		/* meet(const, const) = const or lattice low */
		else if (!constants_equal(state, lnode->val, tmp->val)) {
			lnode->val = 0;
		}
		if (!lnode->val) {
			break;
		}
	}
	/* Do I need to update any work lists here? */
#if DEBUG_SCC
	fprintf(stderr, "phi: %d -> %s\n",
		lnode->def->id,
		(!lnode->val)? "lo": is_const(lnode->val)? "const": "hi");
#endif
}

static int compute_lnode_val(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	int changed;
	struct triple *old, *scratch;
	struct triple **dexpr, **vexpr;
	int count, i;
	
	if (lnode->def->op != OP_STORE) {
		check_lhs(state,  lnode->def);
	}

	/* Store the original value */
	if (lnode->val) {
		old = dup_triple(state, lnode->val);
		if (lnode->val != lnode->def) {
			xfree(lnode->val);
		}
		lnode->val = 0;

	} else {
		old = 0;
	}
	/* Reinitialize the value */
	lnode->val = scratch = dup_triple(state, lnode->def);
	scratch->next     = scratch;
	scratch->prev     = scratch;
	scratch->use      = 0;

	count = TRIPLE_SIZE(scratch->sizes);
	for(i = 0; i < count; i++) {
		struct lattice_node *tmp;
		dexpr = &lnode->def->param[i];
		vexpr = &scratch->param[i];
		*vexpr = 0;
		if (*dexpr) {
			tmp = triple_to_lattice(state, scc, *dexpr);
			*vexpr = (tmp->val)? tmp->val : tmp->def;
		}
	}
	if (scratch->op == OP_BRANCH) {
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
			((lnode->def->op != OP_BRANCH) ||
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
			if (*dexpr) {
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
		!triple_is_pure(state, lnode->val)) {
		lnode->val = 0;
	}
#if 1
	if (lnode->val && 
		(lnode->val->op == OP_SDECL) && 
		(lnode->val != lnode->def)) {
		internal_error(state, lnode->def, "bad sdecl");
	}
#endif
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
		(lnode->val->op == old->op) &&
		(memcmp(lnode->val->param, old->param,
			count * sizeof(lnode->val->param[0])) == 0) &&
		(memcmp(&lnode->val->u, &old->u, sizeof(old->u)) == 0)) {
		changed = 0;
	}
	if (old) {
		xfree(old);
	}
	if (lnode->val != scratch) {
		xfree(scratch);
	}
	return changed;
}

static void scc_visit_branch(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	struct lattice_node *cond;
#if DEBUG_SCC
	{
		struct flow_edge *fedge;
		fprintf(stderr, "branch: %d (",
			lnode->def->id);
		
		for(fedge = lnode->fblock->out; fedge; fedge = fedge->out_next) {
			fprintf(stderr, " %d", fedge->dst->block->vertex);
		}
		fprintf(stderr, " )");
		if (TRIPLE_RHS(lnode->def->sizes) > 0) {
			fprintf(stderr, " <- %d",
				RHS(lnode->def)->id);
		}
		fprintf(stderr, "\n");
	}
#endif
	if (lnode->def->op != OP_BRANCH) {
		internal_error(state, lnode->def, "not branch");
	}
	/* This only applies to conditional branches */
	if (TRIPLE_RHS(lnode->def->sizes) == 0) {
		return;
	}
	cond = triple_to_lattice(state, scc, RHS(lnode->def,0));
	if (cond->val && !is_const(cond->val)) {
#warning "FIXME do I need to do something here?"
		warning(state, cond->def, "condition not constant?");
		return;
	}
	if (cond->val == 0) {
		scc_add_fedge(state, scc, cond->fblock->out);
		scc_add_fedge(state, scc, cond->fblock->out->out_next);
	}
	else if (cond->val->u.cval) {
		scc_add_fedge(state, scc, cond->fblock->out->out_next);
		
	} else {
		scc_add_fedge(state, scc, cond->fblock->out);
	}

}

static void scc_visit_expr(struct compile_state *state, struct scc_state *scc,
	struct lattice_node *lnode)
{
	int changed;

	changed = compute_lnode_val(state, scc, lnode);
#if DEBUG_SCC
	{
		struct triple **expr;
		fprintf(stderr, "expr: %3d %10s (",
			lnode->def->id, tops(lnode->def->op));
		expr = triple_rhs(state, lnode->def, 0);
		for(;expr;expr = triple_rhs(state, lnode->def, expr)) {
			if (*expr) {
				fprintf(stderr, " %d", (*expr)->id);
			}
		}
		fprintf(stderr, " ) -> %s\n",
			(!lnode->val)? "lo": is_const(lnode->val)? "const": "hi");
	}
#endif
	if (lnode->def->op == OP_BRANCH) {
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
	first = RHS(state->main_function, 0);
	ins = first;
	do {
		struct lattice_node *lnode;
		lnode = triple_to_lattice(state, scc, ins);
#if DEBUG_SCC
		if (lnode->val && !is_const(lnode->val)) {
			warning(state, lnode->def, 
				"lattice node still high?");
		}
#endif
		if (lnode->val && (lnode->val != ins)) {
			/* See if it something I know how to write back */
			switch(lnode->val->op) {
			case OP_INTCONST:
				mkconst(state, ins, lnode->val->u.cval);
				break;
			case OP_ADDRCONST:
				mkaddr_const(state, ins, 
					RHS(lnode->val, 0), lnode->val->u.cval);
				break;
			default:
				/* By default don't copy the changes,
				 * recompute them in place instead.
				 */
				simplify(state, ins);
				break;
			}
		}
		ins = ins->next;
	} while(ins != first);
}

static void scc_transform(struct compile_state *state)
{
	struct scc_state scc;

	initialize_scc_state(state, &scc);

	while(scc.flow_work_list || scc.ssa_work_list) {
		struct flow_edge *fedge;
		struct ssa_edge *sedge;
		struct flow_edge *fptr;
		while((fedge = scc_next_fedge(state, &scc))) {
			struct block *block;
			struct triple *ptr;
			struct flow_block *fblock;
			int time;
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
			time = 0;
			for(fptr = fblock->in; fptr; fptr = fptr->in_next) {
				if (fptr->executable) {
					time++;
				}
			}
#if DEBUG_SCC
			fprintf(stderr, "vertex: %d time: %d\n", 
				block->vertex, time);
			
#endif
			done = 0;
			for(ptr = block->first; !done; ptr = ptr->next) {
				struct lattice_node *lnode;
				done = (ptr == block->last);
				lnode = &scc.lattice[ptr->id];
				if (ptr->op == OP_PHI) {
					scc_visit_phi(state, &scc, lnode);
				}
				else if (time == 1) {
					scc_visit_expr(state, &scc, lnode);
				}
			}
			if (fblock->out && !fblock->out->out_next) {
				scc_add_fedge(state, &scc, fblock->out);
			}
		}
		while((sedge = scc_next_sedge(state, &scc))) {
			struct lattice_node *lnode;
			struct flow_block *fblock;
			lnode = sedge->dst;
			fblock = lnode->fblock;
#if DEBUG_SCC
			fprintf(stderr, "sedge: %5d (%5d -> %5d)\n",
				sedge - scc.ssa_edges,
				sedge->src->def->id,
				sedge->dst->def->id);
#endif
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
}


static void transform_to_arch_instructions(struct compile_state *state);


static void optimize(struct compile_state *state)
{
	if (state->debug & DEBUG_TRIPLES) {
		print_triples(state);
	}
	/* Replace structures with simpler data types */
	flatten_structures(state);
	if (state->debug & DEBUG_TRIPLES) {
		print_triples(state);
	}
	/* Analize the intermediate code */
	setup_basic_blocks(state);
	analyze_idominators(state);
	analyze_ipdominators(state);
	/* Transform the code to ssa form */
	transform_to_ssa_form(state);
	/* Do strength reduction and simple constant optimizations */
	if (state->optimize >= 1) {
		simplify_all(state);
	}
	/* Propogate constants throughout the code */
	if (state->optimize >= 2) {
		scc_transform(state);
		transform_from_ssa_form(state);
		free_basic_blocks(state);
		setup_basic_blocks(state);
		analyze_idominators(state);
		analyze_ipdominators(state);
		transform_to_ssa_form(state);
		
	}
#warning "WISHLIST implement single use constants (least possible register pressure)"
#warning "WISHLIST implement induction variable elimination"
#warning "WISHLIST implement strength reduction"
	/* Select architecture instructions and an initial partial
	 * coloring based on architecture constraints.
	 */
	transform_to_arch_instructions(state);
	if (state->debug & DEBUG_ARCH_CODE) {
		printf("After transform_to_arch_instructions\n");
		print_blocks(state);
		print_control_flow(state);
	}
	eliminate_inefectual_code(state);
	if (state->debug & DEBUG_CODE_ELIMINATION) {
		printf("After eliminate_inefectual_code\n");
		print_blocks(state);
		print_control_flow(state);
	}
	/* Color all of the variables to see if they will fit in registers */
	insert_copies_to_phi(state);
	allocate_registers(state);
	if (state->debug & DEBUG_INTERMEDIATE_CODE) {
		print_blocks(state);
	}
	if (state->debug & DEBUG_CONTROL_FLOW) {
		print_control_flow(state);
	}
	/* Remove the optimization information.
	 * This is more to check for memory consistency than to free memory.
	 */
	free_basic_blocks(state);
}

/* The x86 register classes */
#define REGC_FLAGS   0
#define REGC_GPR8    1
#define REGC_GPR16   2
#define REGC_GPR32   3
#define REGC_GPR64   4
#define REGC_MMX     5
#define REGC_XMM     6
#define REGC_GPR32_8 7
#define REGC_GPR16_8 8
#define LAST_REGC  REGC_GPR16_8
#if LAST_REGC >= MAX_REGC
#error "MAX_REGC is to low"
#endif

/* Register class masks */
#define REGCM_FLAGS   (1 << REGC_FLAGS)
#define REGCM_GPR8    (1 << REGC_GPR8)
#define REGCM_GPR16   (1 << REGC_GPR16)
#define REGCM_GPR32   (1 << REGC_GPR32)
#define REGCM_GPR64   (1 << REGC_GPR64)
#define REGCM_MMX     (1 << REGC_MMX)
#define REGCM_XMM     (1 << REGC_XMM)
#define REGCM_GPR32_8 (1 << REGC_GPR32_8)
#define REGCM_GPR16_8 (1 << REGC_GPR16_8)

/* The x86 registers */
#define REG_EFLAGS  1
#define REGC_FLAGS_FIRST REG_EFLAGS
#define REGC_FLAGS_LAST  REG_EFLAGS
#define REG_AL      2
#define REG_BL      3
#define REG_CL      4
#define REG_DL      5
#define REG_AH      6
#define REG_BH      7
#define REG_CH      8
#define REG_DH      9
#define REGC_GPR8_FIRST  REG_AL
#if X86_4_8BIT_GPRS
#define REGC_GPR8_LAST   REG_DL
#else 
#define REGC_GPR8_LAST   REG_DH
#endif
#define REG_AX     10
#define REG_BX     11
#define REG_CX     12
#define REG_DX     13
#define REG_SI     14
#define REG_DI     15
#define REG_BP     16
#define REG_SP     17
#define REGC_GPR16_FIRST REG_AX
#define REGC_GPR16_LAST  REG_SP
#define REG_EAX    18
#define REG_EBX    19
#define REG_ECX    20
#define REG_EDX    21
#define REG_ESI    22
#define REG_EDI    23
#define REG_EBP    24
#define REG_ESP    25
#define REGC_GPR32_FIRST REG_EAX
#define REGC_GPR32_LAST  REG_ESP
#define REG_EDXEAX 26
#define REGC_GPR64_FIRST REG_EDXEAX
#define REGC_GPR64_LAST  REG_EDXEAX
#define REG_MMX0   27
#define REG_MMX1   28
#define REG_MMX2   29
#define REG_MMX3   30
#define REG_MMX4   31
#define REG_MMX5   32
#define REG_MMX6   33
#define REG_MMX7   34
#define REGC_MMX_FIRST REG_MMX0
#define REGC_MMX_LAST  REG_MMX7
#define REG_XMM0   35
#define REG_XMM1   36
#define REG_XMM2   37
#define REG_XMM3   38
#define REG_XMM4   39
#define REG_XMM5   40
#define REG_XMM6   41
#define REG_XMM7   42
#define REGC_XMM_FIRST REG_XMM0
#define REGC_XMM_LAST  REG_XMM7
#warning "WISHLIST figure out how to use pinsrw and pextrw to better use extended regs"
#define LAST_REG   REG_XMM7

#define REGC_GPR32_8_FIRST REG_EAX
#define REGC_GPR32_8_LAST  REG_EDX
#define REGC_GPR16_8_FIRST REG_AX
#define REGC_GPR16_8_LAST  REG_DX

#if LAST_REG >= MAX_REGISTERS
#error "MAX_REGISTERS to low"
#endif

static unsigned arch_regc_size(struct compile_state *state, int class)
{
	static unsigned regc_size[LAST_REGC +1] = {
		[REGC_FLAGS]   = REGC_FLAGS_LAST   - REGC_FLAGS_FIRST + 1,
		[REGC_GPR8]    = REGC_GPR8_LAST    - REGC_GPR8_FIRST + 1,
		[REGC_GPR16]   = REGC_GPR16_LAST   - REGC_GPR16_FIRST + 1,
		[REGC_GPR32]   = REGC_GPR32_LAST   - REGC_GPR32_FIRST + 1,
		[REGC_GPR64]   = REGC_GPR64_LAST   - REGC_GPR64_FIRST + 1,
		[REGC_MMX]     = REGC_MMX_LAST     - REGC_MMX_FIRST + 1,
		[REGC_XMM]     = REGC_XMM_LAST     - REGC_XMM_FIRST + 1,
		[REGC_GPR32_8] = REGC_GPR32_8_LAST - REGC_GPR32_8_FIRST + 1,
		[REGC_GPR16_8] = REGC_GPR16_8_LAST - REGC_GPR16_8_FIRST + 1,
	};
	if ((class < 0) || (class > LAST_REGC)) {
		return 0;
	}
	return regc_size[class];
}
static int arch_regcm_intersect(unsigned regcm1, unsigned regcm2)
{
	/* See if two register classes may have overlapping registers */
	unsigned gpr_mask = REGCM_GPR8 | REGCM_GPR16_8 | REGCM_GPR16 |
		REGCM_GPR32_8 | REGCM_GPR32 | REGCM_GPR64;

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
	case REG_AH:
		*equiv++ = REG_AX;
		*equiv++ = REG_EAX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_BL:  
	case REG_BH:
		*equiv++ = REG_BX;
		*equiv++ = REG_EBX;
		break;
	case REG_CL:
	case REG_CH:
		*equiv++ = REG_CX;
		*equiv++ = REG_ECX;
		break;
	case REG_DL:
	case REG_DH:
		*equiv++ = REG_DX;
		*equiv++ = REG_EDX;
		*equiv++ = REG_EDXEAX;
		break;
	case REG_AX:
		*equiv++ = REG_AL;
		*equiv++ = REG_AH;
		*equiv++ = REG_EAX;
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
	case REG_EDXEAX: 
		*equiv++ = REG_AL;
		*equiv++ = REG_AH;
		*equiv++ = REG_DL;
		*equiv++ = REG_DH;
		*equiv++ = REG_AX;
		*equiv++ = REG_DX;
		*equiv++ = REG_EAX;
		*equiv++ = REG_EDX;
		break;
	}
	*equiv++ = REG_UNSET; 
}


static unsigned arch_reg_regcm(struct compile_state *state, int reg)
{
	static const struct {
		int first, last;
	} bound[LAST_REGC + 1] = {
		[REGC_FLAGS]   = { REGC_FLAGS_FIRST,   REGC_FLAGS_LAST },
		[REGC_GPR8]    = { REGC_GPR8_FIRST,    REGC_GPR8_LAST },
		[REGC_GPR16]   = { REGC_GPR16_FIRST,   REGC_GPR16_LAST },
		[REGC_GPR32]   = { REGC_GPR32_FIRST,   REGC_GPR32_LAST },
		[REGC_GPR64]   = { REGC_GPR64_FIRST,   REGC_GPR64_LAST },
		[REGC_MMX]     = { REGC_MMX_FIRST,     REGC_MMX_LAST },
		[REGC_XMM]     = { REGC_XMM_FIRST,     REGC_XMM_LAST },
		[REGC_GPR32_8] = { REGC_GPR32_8_FIRST, REGC_GPR32_8_LAST },
		[REGC_GPR16_8] = { REGC_GPR16_8_FIRST, REGC_GPR16_8_LAST },
	};
	unsigned mask;
	int class;
	mask = 0;
	for(class = 0; class <= LAST_REGC; class++) {
		if ((reg >= bound[class].first) &&
			(reg <= bound[class].last)) {
			mask |= (1 << class);
		}
	}
	if (!mask) {
		internal_error(state, 0, "reg %d not in any class", reg);
	}
	return mask;
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
	/* Preference: flags, 8bit gprs, 32bit gprs, other 32bit reg
	 * other types of registers.
	 */
	int i, reg;
	reg = REG_UNSET;
	for(i = REGC_FLAGS_FIRST; (reg == REG_UNSET) && (i <= REGC_FLAGS_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR8_FIRST; (reg == REG_UNSET) && (i <= REGC_GPR8_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR32_FIRST; (reg == REG_UNSET) && (i <= REGC_GPR32_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_MMX_FIRST; (reg == REG_UNSET) && (i <= REGC_MMX_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_XMM_FIRST; (reg == REG_UNSET) && (i <= REGC_XMM_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR16_FIRST; (reg == REG_UNSET) && (i <= REGC_GPR16_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	for(i = REGC_GPR64_FIRST; (reg == REG_UNSET) && (i <= REGC_GPR64_LAST); i++) {
		reg = do_select_reg(state, used, i, classes);
	}
	return reg;
}

static unsigned arch_type_to_regcm(struct compile_state *state, struct type *type) 
{
#warning "FIXME force types smaller (if legal) before I get here"
	int use_mmx = 0;
	int use_sse = 0;
	unsigned avail_mask;
	unsigned mask;
	avail_mask = REGCM_GPR8 | REGCM_GPR16_8 | REGCM_GPR16 | 
		REGCM_GPR32 | REGCM_GPR32_8 | REGCM_GPR64;
#if 1
	/* Don't enable 8 bit values until I can force both operands
	 * to be 8bits simultaneously.
	 */
	avail_mask &= ~(REGCM_GPR8 | REGCM_GPR16_8 | REGCM_GPR16);
#endif
	if (use_mmx) {
		avail_mask |= REGCM_MMX;
	}
	if (use_sse) {
		avail_mask |= REGCM_XMM;
	}
	mask = 0;
	switch(type->type & TYPE_MASK) {
	case TYPE_ARRAY:
	case TYPE_VOID: 
		mask = 0; 
		break;
	case TYPE_CHAR:
	case TYPE_UCHAR:
		mask = REGCM_GPR8 | 
			REGCM_GPR16_8 | REGCM_GPR16 | 
			REGCM_GPR32 | REGCM_GPR32_8 |
			REGCM_GPR64 |
			REGCM_MMX | REGCM_XMM;
		break;
	case TYPE_SHORT:
	case TYPE_USHORT:
		mask = REGCM_GPR16 | REGCM_GPR16_8 |
			REGCM_GPR32 | REGCM_GPR32_8 |
			REGCM_GPR64 |
			REGCM_MMX | REGCM_XMM;
		break;
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_LONG:
	case TYPE_ULONG:
	case TYPE_POINTER:
		mask = REGCM_GPR32 | REGCM_GPR32_8 |
			REGCM_GPR64 | REGCM_MMX | REGCM_XMM;
		break;
	default:
		internal_error(state, 0, "no register class for type");
		break;
	}
	mask &= avail_mask;
	return mask;
}

static void get_imm32(struct triple *ins, struct triple **expr)
{
	struct triple *imm;
	if ((*expr)->op != OP_COPY) {
		return;
	}
	imm = RHS((*expr), 0);
	while(imm->op == OP_COPY) {
		imm = RHS(imm, 0);
	}
	if (imm->op != OP_INTCONST) {
		return;
	}
	*expr = imm;
	unuse_triple(*expr, ins);
	use_triple(*expr, ins);
}

static void get_imm8(struct triple *ins, struct triple **expr)
{
	struct triple *imm;
	if ((*expr)->op != OP_COPY) {
		return;
	}
	imm = RHS((*expr), 0);
	while(imm->op == OP_COPY) {
		imm = RHS(imm, 0);
	}
	if (imm->op != OP_INTCONST) {
		return;
	}
	/* For imm8 only a sufficienlty small constant can be used */
	if (imm->u.cval > 0xff) {
		return;
	}
	*expr = imm;
	unuse_triple(*expr, ins);
	use_triple(*expr, ins);
}

static struct triple *pre_copy(struct compile_state *state, 
	struct triple *ins, struct triple **expr,
	unsigned reg, unsigned mask)
{
	/* Carefully insert enough operations so that I can
	 * enter any operation with a GPR32.
	 */
	struct triple *in;
	/* See if I can directly reach the result from a GPR32 */
	if (mask & (REGCM_GPR32 | REGCM_GPR16 | REGCM_MMX | REGCM_XMM)) {
		in = triple(state, OP_COPY, (*expr)->type, *expr,  0);
	}
	/* If it is a byte value force a earlier copy to a GPR32_8 */
	else if (mask & REGCM_GPR8) {
		struct triple *tmp;
		tmp = triple(state, OP_COPY, (*expr)->type, *expr, 0);
		tmp->filename = ins->filename;
		tmp->line     = ins->line;
		tmp->col      = ins->col;
		tmp->u.block  = ins->u.block;
		tmp->id = MK_REG_ID(REG_UNSET, REGCM_GPR32_8 | REGCM_GPR16_8);
		use_triple(RHS(tmp, 0), tmp);
		insert_triple(state, ins, tmp);

		in = triple(state, OP_COPY, tmp->type, tmp, 0);
	}
	else {
		internal_error(state, ins, "bad copy type");
		in = 0;
	}
	in->filename  = ins->filename;
	in->line      = ins->line;
	in->col       = ins->col;
	in->u.block   = ins->u.block;
	in->id        = MK_REG_ID(reg, mask);
	unuse_triple(*expr, ins);
	*expr = in;
	use_triple(RHS(in, 0), in);
	use_triple(in, ins);
	insert_triple(state, ins, in);
	return in;
}

static struct triple *post_copy(struct compile_state *state, struct triple *ins)
{
	struct triple_set *entry, *next;
	struct triple *out, *label;
	struct block *block;
	label = ins;
	while(label->op != OP_LABEL) {
		label = label->prev;
	}
	block = label->u.block;
	out = triple(state, OP_COPY, ins->type, ins, 0);
	out->filename = ins->filename;
	out->line     = ins->line;
	out->col      = ins->col;
	out->u.block  = block;
	out->id       = MK_REG_ID(REG_UNSET, 
		arch_type_to_regcm(state, ins->type));
	use_triple(ins, out);
	insert_triple(state, ins->next, out);
	if (block->last == ins) {
		block->last = out;
	}
	/* Get the users of ins to use out instead */
	for(entry = ins->use; entry; entry = next) {
		next = entry->next;
		if (entry->member == out) {
			continue;
		}
		replace_rhs_use(state, ins, out, entry->member);
	}
	return out;
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
		else if (entry->member->op == OP_BRANCH) {
			struct triple *branch, *test;
			struct triple *left, *right;
			left = right = 0;
			left = RHS(cmp, 0);
			if (TRIPLE_RHS(cmp->sizes) > 1) {
				right = RHS(cmp, 1);
			}
			branch = entry->member;
			test = pre_triple(state, branch,
				cmp->op, cmp->type, left, right);
			test->id = MK_REG_ID(REG_EFLAGS, REGCM_FLAGS);
			unuse_triple(RHS(branch, 0), branch);
			RHS(branch, 0) = test;
			branch->op = jmp_op;
			use_triple(RHS(branch, 0), branch);
		}
	}
}

static void bool_cmp(struct compile_state *state, 
	struct triple *ins, int cmp_op, int jmp_op, int set_op)
{
	struct block *block;
	struct triple_set *entry, *next;
	struct triple *set, *tmp1, *tmp2;

#warning "WISHLIST implement an expression simplifier to reduce the use of set?"

	block = ins->u.block;

	/* Put a barrier up before the cmp which preceeds the
	 * copy instruction.  If a set actually occurs this gives
	 * us a chance to move variables in registers out of the way.
	 */

	/* Modify the comparison operator */
	ins->op = cmp_op;
	ins->id = MK_REG_ID(REG_EFLAGS, REGCM_FLAGS);
	if (cmp_op == OP_CMP) {
		get_imm32(ins, &RHS(ins, 1));
	}
	/* Generate the instruction sequence that will transform the
	 * result of the comparison into a logical value.
	 */
	tmp1 = triple(state, set_op, ins->type, ins, 0);
	tmp1->filename = ins->filename;
	tmp1->line     = ins->line;
	tmp1->col      = ins->col;
	tmp1->u.block  = block;
	tmp1->id       = MK_REG_ID(REG_UNSET, REGCM_GPR8);
	use_triple(ins, tmp1);
	insert_triple(state, ins->next, tmp1);
	
	tmp2 = triple(state, OP_COPY, ins->type, tmp1, 0);
	tmp2->filename = ins->filename;
	tmp2->line     = ins->line;
	tmp2->col      = ins->col;
	tmp2->u.block  = block;
	tmp2->id       = MK_REG_ID(REG_UNSET, 
		REGCM_GPR32 | REGCM_GPR32_8 | REGCM_GPR16 | REGCM_GPR16_8 | REGCM_GPR8);
	use_triple(tmp1, tmp2);
	insert_triple(state, tmp1->next, tmp2);

	if (block->last == ins) {
		block->last = tmp2;
	}

	set = tmp2;
	for(entry = ins->use; entry; entry = next) {
		next = entry->next;
		if (entry->member == tmp1) {
			continue;
		}
		replace_rhs_use(state, ins, set, entry->member);
	}
	fixup_branches(state, ins, set, jmp_op);
}

static void verify_lhs(struct compile_state *state, struct triple *ins)
{
	struct triple *next;
	int lhs, i;
	lhs = TRIPLE_LHS(ins->sizes);
	for(next = ins->next, i = 0; i < lhs; i++, next = next->next) {
		if (next != LHS(ins, i)) {
			internal_error(state, ins, "malformed lhs on %s",
				tops(ins->op));
		}
	}
}

static void transform_to_arch_instructions(struct compile_state *state)
{
	/* Transform from generic 3 address instructions
	 * to archtecture specific instructions.
	 * And apply architecture specific constrains to instructions.
	 * Copies are inserted to preserve the register flexibility
	 * of 3 address instructions.
	 */
	struct triple *ins, *first, *next;
	struct triple *in, *in2;
	first = RHS(state->main_function, 0);
	ins = first;
	do {
		next = ins->next;
		ins->id = MK_REG_ID(REG_UNSET, arch_type_to_regcm(state, ins->type));
		switch(ins->op) {
		case OP_INTCONST:
		case OP_ADDRCONST:
			ins->id = 0;
			post_copy(state, ins);
			break;
		case OP_NOOP:
		case OP_SDECL:
		case OP_BLOBCONST:
		case OP_LABEL:
			ins->id = 0;
			break;
			/* instructions that can be used as is */
		case OP_COPY:
		case OP_PHI:
			break;
		case OP_STORE:
		{
			unsigned mask;
			ins->id = 0;
			switch(ins->type->type & TYPE_MASK) {
			case TYPE_CHAR:    case TYPE_UCHAR:
				mask = REGCM_GPR8;
				break;
			case TYPE_SHORT:   case TYPE_USHORT:
				mask = REGCM_GPR16;
				break;
			case TYPE_INT:     case TYPE_UINT:
			case TYPE_LONG:    case TYPE_ULONG:
			case TYPE_POINTER:
				mask  = REGCM_GPR32;
				break;
			default:
				internal_error(state, ins, "unknown type in store");
				mask = 0;
				break;
			}
			in = pre_copy(state, ins, &RHS(ins, 0), REG_UNSET, mask);
			break;
		}
		case OP_LOAD:
			switch(ins->type->type & TYPE_MASK) {
			case TYPE_CHAR:   case TYPE_UCHAR:
				ins->id = MK_REG_ID(REG_UNSET, REGCM_GPR8);
				break;
			case TYPE_SHORT:
			case TYPE_USHORT:
				ins->id = MK_REG_ID(REG_UNSET, REGCM_GPR16);
				break;
			case TYPE_INT:
			case TYPE_UINT:
			case TYPE_LONG:
			case TYPE_ULONG:
			case TYPE_POINTER:
				ins->id = MK_REG_ID(REG_UNSET, REGCM_GPR32);
				break;
			default:
				internal_error(state, ins, "unknown type in load");
				break;
			}
			break;
		case OP_ADD:
		case OP_SUB:
		case OP_AND:
		case OP_XOR:
		case OP_OR:
			get_imm32(ins, &RHS(ins, 1));
			in = pre_copy(state, ins, &RHS(ins, 0),
				alloc_virtual_reg(), ID_REG_CLASSES(ins->id));
			ins->id = in->id;
			break;
		case OP_SL:
		case OP_SSR:
		case OP_USR:
			get_imm8(ins, &RHS(ins, 1));
			in = pre_copy(state, ins, &RHS(ins, 0),
				alloc_virtual_reg(), ID_REG_CLASSES(ins->id));
			ins->id = in->id;
			if (!is_const(RHS(ins, 1))) {
				in2 = pre_copy(state, ins, &RHS(ins, 1),
					REG_CL, REGCM_GPR8);
			}
			break;
		case OP_INVERT:
		case OP_NEG:
			in = pre_copy(state, ins, &RHS(ins, 0),
				alloc_virtual_reg(), ID_REG_CLASSES(ins->id));
			ins->id = in->id;
			break;
		case OP_SMUL:
			get_imm32(ins, &RHS(ins, 1));
			in = pre_copy(state, ins, &RHS(ins, 0),
				alloc_virtual_reg(), ID_REG_CLASSES(ins->id));
			ins->id = in->id;
			if (!is_const(RHS(ins, 1))) {
				in2 = pre_copy(state, ins, &RHS(ins, 1),
					REG_UNSET, REGCM_GPR32);
			}
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
			if (TRIPLE_RHS(ins->sizes) > 0) {
				internal_error(state, ins, "bad branch test");
			}
			ins->op = OP_JMP;
			break;

		case OP_INB:
		case OP_INW:
		case OP_INL:
			get_imm8(ins, &RHS(ins, 0));
			switch(ins->op) {
			case OP_INB: ins->id = MK_REG_ID(REG_AL,  REGCM_GPR8); break;
			case OP_INW: ins->id = MK_REG_ID(REG_AX,  REGCM_GPR16); break;
			case OP_INL: ins->id = MK_REG_ID(REG_EAX, REGCM_GPR32); break;
			}
			if (!is_const(RHS(ins, 0))) {
				in = pre_copy(state, ins, &RHS(ins, 0),
					REG_DX, REGCM_GPR16);
			}
			break;
		case OP_OUTB:
		case OP_OUTW:
		case OP_OUTL:
		{
			unsigned reg, mask;
			get_imm8(ins, &RHS(ins, 1));
			switch(ins->op) {
			case OP_OUTB: reg = REG_AL;  mask = REGCM_GPR8; break;
			case OP_OUTW: reg = REG_AX;  mask = REGCM_GPR16; break;
			case OP_OUTL: reg = REG_EAX; mask = REGCM_GPR32; break;
			default: reg = REG_UNSET; mask = 0; break;
			}
			in = pre_copy(state, ins, &RHS(ins, 0), reg, mask);
			if (!is_const(RHS(ins, 1))) {
				in2 = pre_copy(state, ins, &RHS(ins, 1),
					REG_DX, REGCM_GPR16);
			}
			break;
		}
		case OP_BSF:
		case OP_BSR:
			in = pre_copy(state, ins, &RHS(ins, 0),
				REG_UNSET, REGCM_GPR32);
			ins->id = MK_REG_ID(REG_UNSET, REGCM_GPR32 | REGCM_GPR32_8);
			break;
		case OP_RDMSR:
			in = pre_copy(state, ins, &RHS(ins, 0),
				REG_ECX, REGCM_GPR32);
			verify_lhs(state, ins);
			LHS(ins,0)->id = MK_REG_ID(REG_EAX, REGCM_GPR32);
			LHS(ins,1)->id = MK_REG_ID(REG_EDX, REGCM_GPR32);
			next = ins->next->next->next;
			break;
		case OP_WRMSR:
			pre_copy(state, ins, &RHS(ins, 0), REG_ECX, REGCM_GPR32);
			pre_copy(state, ins, &RHS(ins, 1), REG_EAX, REGCM_GPR32);
			pre_copy(state, ins, &RHS(ins, 2), REG_EDX, REGCM_GPR32);
			break;
		case OP_HLT:
			break;
			/* Already transformed instructions */
		case OP_CMP:
		case OP_TEST:
			ins->id = MK_REG_ID(REG_EFLAGS, REGCM_FLAGS);
			break;
		case OP_JMP_EQ:      case OP_JMP_NOTEQ:
		case OP_JMP_SLESS:   case OP_JMP_ULESS:
		case OP_JMP_SMORE:   case OP_JMP_UMORE:
		case OP_JMP_SLESSEQ: case OP_JMP_ULESSEQ:
		case OP_JMP_SMOREEQ: case OP_JMP_UMOREEQ:
		case OP_SET_EQ:      case OP_SET_NOTEQ:
		case OP_SET_SLESS:   case OP_SET_ULESS:
		case OP_SET_SMORE:   case OP_SET_UMORE:
		case OP_SET_SLESSEQ: case OP_SET_ULESSEQ:
		case OP_SET_SMOREEQ: case OP_SET_UMOREEQ:
			break;
			/* Unhandled instructions */
		case OP_PIECE:
		default:
			internal_error(state, ins, "unhandled ins: %d %s\n",
				ins->op, tops(ins->op));
			break;
		}
		ins = next;
	} while(ins != first);
}

static void generate_local_labels(struct compile_state *state)
{
	struct triple *first, *label;
	int label_counter;
	label_counter = 0;
	first = RHS(state->main_function, 0);
	label = first;
	do {
		if ((label->op == OP_LABEL) || 
			(label->op == OP_SDECL)) {
			if (label->use) {
				label->u.cval = ++label_counter;
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
	if (ID_REG_CLASSES(triple->id)) {
		internal_error(state, triple, "class specifier present");
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
	static const char *regs[] = {
		"%bad_register",
		"%eflags",
		"%al", "%bl", "%cl", "%dl", "%ah", "%bh", "%ch", "%dh",
		"%ax", "%bx", "%cx", "%dx", "%si", "%di", "%bp", "%sp",
		"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi", "%ebp", "%esp",
		"%edx:%eax",
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

static void print_binary_op(struct compile_state *state,
	const char *op, struct triple *ins, FILE *fp) 
{
	unsigned mask;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8;
	if (RHS(ins, 0)->id != ins->id) {
		internal_error(state, ins, "invalid register assignment");
	}
	if (is_const(RHS(ins, 1))) {
		fprintf(fp, "\t%s $%lu, %s\n",
			op,
			RHS(ins, 1)->u.cval,
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
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8;
	fprintf(fp, "\t%s %s\n",
		op,
		reg(state, RHS(ins, 0), mask));
}

static void print_op_shift(struct compile_state *state,
	const char *op, struct triple *ins, FILE *fp)
{
	unsigned mask;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8;
	if (RHS(ins, 0)->id != ins->id) {
		internal_error(state, ins, "invalid register assignment");
	}
	if (is_const(RHS(ins, 1))) {
		fprintf(fp, "\t%s $%lu, %s\n",
			op,
			RHS(ins, 1)->u.cval,
			reg(state, RHS(ins, 0), mask));
		
	}
	else {
		fprintf(fp, "\t%s %s, %s\n",
			op,
			reg(state, RHS(ins, 1), REGCM_GPR8),
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
	case OP_INB: op = "inb", mask = REGCM_GPR8; break;
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
		fprintf(fp, "\t%s $%lu, %s\n",
			op, RHS(ins, 0)->u.cval, 
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
	case OP_OUTB: op = "outb", mask = REGCM_GPR8; break;
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
		fprintf(fp, "\t%s %s, $%lu\n",
			op, reg(state, RHS(ins, 0), mask),
			RHS(ins, 1)->u.cval);
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
	 */
	int omit_copy = 1; /* Is it o.k. to omit a noop copy? */
	struct triple *dst, *src;
	if (ins->op == OP_COPY) {
		src = RHS(ins, 0);
		dst = ins;
	}
	else if (ins->op == OP_WRITE) {
		dst = LHS(ins, 0);
		src = RHS(ins, 0);
	}
	else {
		internal_error(state, ins, "unknown move operation");
		src = dst = 0;
	}
	if (!is_const(src)) {
		int src_reg, dst_reg;
		int src_regcm, dst_regcm;
		src_reg = ID_REG(src->id);
		dst_reg   = ID_REG(dst->id);
		src_regcm = arch_reg_regcm(state, src_reg);
		dst_regcm   = arch_reg_regcm(state, dst_reg);
		/* If the class is the same just move the register */
		if (src_regcm & dst_regcm & 
			(REGCM_GPR8 | REGCM_GPR16 | REGCM_GPR32)) {
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
		/* Move 32bit to 8bit */
		else if ((src_regcm & REGCM_GPR32_8) &&
			(dst_regcm & REGCM_GPR8))
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
			(dst_regcm & REGCM_GPR8))
		{
			src_reg = (src_reg - REGC_GPR16_8_FIRST) + REGC_GPR8_FIRST;
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmovb %s, %s\n",
					arch_reg_str(src_reg),
					arch_reg_str(dst_reg));
			}
		}
		/* Move 8/16bit to 16/32bit */
		else if ((src_regcm & (REGCM_GPR8 | REGCM_GPR16)) &&
			(dst_regcm & (REGC_GPR16 | REGCM_GPR32))) {
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
				fprintf(fp, "\tmovdqa %s %s\n",
					reg(state, src, src_regcm),
					reg(state, dst, dst_regcm));
			}
		}
		/* Move between mmx registers or mmx & sse  registers */
		else if ((src_regcm & (REGCM_MMX | REGCM_XMM)) &&
			(dst_regcm & (REGCM_MMX | REGCM_XMM))) {
			if ((src_reg != dst_reg) || !omit_copy) {
				fprintf(fp, "\tmovq %s %s\n",
					reg(state, src, src_regcm),
					reg(state, dst, dst_regcm));
			}
		}
		/* Move between 32bit gprs & mmx/sse registers */
		else if ((src_regcm & (REGCM_GPR32 | REGCM_MMX | REGCM_XMM)) &&
			(dst_regcm & (REGCM_GPR32 | REGCM_MMX | REGCM_XMM))) {
			fprintf(fp, "\tmovd %s, %s\n",
				reg(state, src, src_regcm),
				reg(state, dst, dst_regcm));
		}
		else {
			internal_error(state, ins, "unknown copy type");
		}
	}
	else switch(src->op) {
	case OP_INTCONST:
	{
		long_t value;
		value = (long_t)(src->u.cval);
		fprintf(fp, "\tmov $%ld, %s\n",
			value,
			reg(state, dst, REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8));
		break;
	}
	case OP_ADDRCONST:
		fprintf(fp, "\tmov $L%lu+%lu, %s\n",
			RHS(src, 0)->u.cval,
			src->u.cval,
			reg(state, dst, REGCM_GPR32));
		break;
	default:
		internal_error(state, ins, "uknown copy operation");
	}
}

static void print_op_load(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	struct triple *dst, *src;
	dst = ins;
	src = RHS(ins, 0);
	if (is_const(src) || is_const(dst)) {
		internal_error(state, ins, "unknown load operation");
	}
	fprintf(fp, "\tmov (%s), %s\n",
		reg(state, src, REGCM_GPR32),
		reg(state, dst, REGCM_GPR8 | REGCM_GPR16 | REGCM_GPR32));
}


static void print_op_store(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	struct triple *dst, *src;
	dst = LHS(ins, 0);
	src = RHS(ins, 0);
	if (is_const(src) && (src->op == OP_INTCONST)) {
		long_t value;
		value = (long_t)(src->u.cval);
		fprintf(fp, "\tmov%s $%ld, (%s)\n",
			type_suffix(state, src->type),
			value,
			reg(state, dst, REGCM_GPR32));
	}
	else if (is_const(dst) && (dst->op == OP_INTCONST)) {
		fprintf(fp, "\tmov%s %s, 0x%08lx\n",
			type_suffix(state, src->type),
			reg(state, src, REGCM_GPR8 | REGCM_GPR16 | REGCM_GPR32),
			dst->u.cval);
	}
	else {
		if (is_const(src) || is_const(dst)) {
			internal_error(state, ins, "unknown store operation");
		}
		fprintf(fp, "\tmov%s %s, (%s)\n",
			type_suffix(state, src->type),
			reg(state, src, REGCM_GPR8 | REGCM_GPR16 | REGCM_GPR32),
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
		fprintf(fp, "\timul $%ld, %s\n",
			RHS(ins, 1)->u.cval,
			reg(state, RHS(ins, 0), REGCM_GPR32));
	}
}

static void print_op_cmp(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	unsigned mask;
	int dreg;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8;
	dreg = check_reg(state, ins, REGCM_FLAGS);
	if (!reg_is_reg(state, dreg, REG_EFLAGS)) {
		internal_error(state, ins, "bad dest register for cmp");
	}
	if (is_const(RHS(ins, 1))) {
		fprintf(fp, "\tcmp $%lu, %s\n",
			RHS(ins, 1)->u.cval,
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
		fprintf(fp, "\tcmp %s, %s\n",
			reg(state, RHS(ins, 1), mask),
			reg(state, RHS(ins, 0), mask));
	}
}

static void print_op_test(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	unsigned mask;
	mask = REGCM_GPR32 | REGCM_GPR16 | REGCM_GPR8;
	fprintf(fp, "\ttest %s, %s\n",
		reg(state, RHS(ins, 0), mask),
		reg(state, RHS(ins, 0), mask));
}

static void print_op_branch(struct compile_state *state,
	struct triple *branch, FILE *fp)
{
	const char *bop = "j";
	if (branch->op == OP_JMP) {
		if (TRIPLE_RHS(branch->sizes) != 0) {
			internal_error(state, branch, "jmp with condition?");
		}
		bop = "jmp";
	}
	else {
		if (TRIPLE_RHS(branch->sizes) != 1) {
			internal_error(state, branch, "jmpcc without condition?");
		}
		check_reg(state, RHS(branch, 0), REGCM_FLAGS);
		if ((RHS(branch, 0)->op != OP_CMP) &&
			(RHS(branch, 0)->op != OP_TEST)) {
			internal_error(state, branch, "bad branch test");
		}
#warning "FIXME I have observed instructions between the test and branch instructions"
		if (RHS(branch, 0)->next != branch) {
			internal_error(state, branch, "branch does not follow test");
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
	fprintf(fp, "\t%s L%lu\n",
		bop, TARG(branch, 0)->u.cval);
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
		sop, reg(state, set, REGCM_GPR8));
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

static void print_const(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	switch(ins->op) {
	case OP_INTCONST:
		switch(ins->type->type & TYPE_MASK) {
		case TYPE_CHAR:
		case TYPE_UCHAR:
			fprintf(fp, ".byte 0x%02lx\n", ins->u.cval);
			break;
		case TYPE_SHORT:
		case TYPE_USHORT:
			fprintf(fp, ".short 0x%04lx\n", ins->u.cval);
			break;
		case TYPE_INT:
		case TYPE_UINT:
		case TYPE_LONG:
		case TYPE_ULONG:
			fprintf(fp, ".int %lu\n", ins->u.cval);
			break;
		default:
			internal_error(state, ins, "Unknown constant type");
		}
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
#if 0
	case OP_ADDRCONST:
		fprintf(fp, ".int $L%lu+%lu",
			ins->left->u.cval,
			ins->u.cval);
		break;
#endif
	default:
		internal_error(state, ins, "Unknown constant type");
		break;
	}
}

static void print_sdecl(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	fprintf(fp, ".section \".rom.data\"\n");
	fprintf(fp, ".balign %d\n", align_of(state, ins->type));
	fprintf(fp, "L%lu:\n", ins->u.cval);
	print_const(state, MISC(ins, 0), fp);
	fprintf(fp, ".section \".rom.text\"\n");
		
}

static void print_instruction(struct compile_state *state,
	struct triple *ins, FILE *fp)
{
	/* Assumption: after I have exted the register allocator
	 * everything is in a valid register. 
	 */
	switch(ins->op) {
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
		/* Don't generate anything here for constants */
	case OP_PHI:
		/* Don't generate anything for variable declarations. */
		break;
	case OP_SDECL:
		print_sdecl(state, ins, fp);
		break;
	case OP_WRITE: 
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
		print_op_branch(state, ins, fp);
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
		verify_lhs(state, ins);
		fprintf(fp, "\trdmsr\n");
		break;
	case OP_WRMSR:
		fprintf(fp, "\twrmsr\n");
		break;
	case OP_HLT:
		fprintf(fp, "\thlt\n");
		break;
	case OP_LABEL:
		if (!ins->use) {
			return;
		}
		fprintf(fp, "L%lu:\n", ins->u.cval);
		break;
		/* Ignore OP_PIECE */
	case OP_PIECE:
		break;
		/* Operations I am not yet certain how to handle */
	case OP_UMUL:
	case OP_SDIV: case OP_UDIV:
	case OP_SMOD: case OP_UMOD:
		/* Operations that should never get here */
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
	int last_line;
	int last_col;
	const char *last_filename;
	FILE *fp;
	print_location = 1;
	last_line = -1;
	last_col  = -1;
	last_filename = 0;
	fp = stdout;
	fprintf(fp, ".section \".rom.text\"\n");
	first = RHS(state->main_function, 0);
	ins = first;
	do {
		if (print_location &&
			((last_filename != ins->filename) ||
				(last_line != ins->line) ||
				(last_col  != ins->col))) {
			fprintf(fp, "\t/* %s:%d */\n",
				ins->filename, ins->line);
			last_filename = ins->filename;
			last_line = ins->line;
			last_col  = ins->col;
		}

		print_instruction(state, ins, fp);
		ins = ins->next;
	} while(ins != first);
	
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

static void compile(char *filename, int debug, int opt)
{
	int i;
	struct compile_state state;
	memset(&state, 0, sizeof(state));
	state.file = 0;
	for(i = 0; i < sizeof(state.token)/sizeof(state.token[0]); i++) {
		memset(&state.token[i], 0, sizeof(state.token[i]));
		state.token[i].tok = -1;
	}
	/* Remember the debug settings */
	state.debug = debug;
	state.optimize = opt;
	/* Prep the preprocessor */
	state.if_depth = 0;
	state.if_value = 0;
	/* register the C keywords */
	register_keywords(&state);
	/* register the keywords the macro preprocessor knows */
	register_macro_keywords(&state);
	/* Memorize where some special keywords are. */
	state.i_continue = lookup(&state, "continue", 8);
	state.i_break    = lookup(&state, "break", 5);
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

	/* Now that basic compilation has happened 
	 * optimize the intermediate code 
	 */
	optimize(&state);
	generate_code(&state);
	if (state.debug) {
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
	char *filename;
	int last_argc;
	int debug;
	int optimize;
	optimize = 0;
	debug = 0;
	last_argc = -1;
	while((argc > 1) && (argc != last_argc)) {
		last_argc = argc;
		if (strncmp(argv[1], "--debug=", 8) == 0) {
			debug = atoi(argv[1] + 8);
			argv++;
			argc--;
		}
		else if ((strcmp(argv[1],"-O") == 0) ||
			(strcmp(argv[1], "-O1") == 0)) {
			optimize = 1;
			argv++;
			argc--;
		}
		else if (strcmp(argv[1],"-O2") == 0) {
			optimize = 2;
			argv++;
			argc--;
		}
	}
	if (argc != 2) {
		arg_error("Wrong argument count %d\n", argc);
	}
	filename = argv[1];
	compile(filename, debug, optimize);

	return 0;
}
