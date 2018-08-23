struct syscall_result {
	long val;
	int errno;
};

static struct syscall_result syscall_return(long result)
{
	struct syscall_result res;
	if (((unsigned long)result) >= ((unsigned long)-125)) {
		res.errno = - result;
		res.val = -1;
	} else {
		res.errno = 0;
		res.val = result;
	}
	return res;
}

static struct syscall_result syscall0(unsigned long nr)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr));
	return syscall_return(res);
}

static struct syscall_result syscall1(unsigned long nr, unsigned long arg1)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr), "b" (arg1));
	return syscall_return(res);

}

static struct syscall_result syscall2(unsigned long nr, unsigned long arg1, unsigned long arg2)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr), "b" (arg1), "c" (arg2));
	return syscall_return(res);

}


static struct syscall_result syscall3(unsigned long nr, unsigned long arg1, unsigned long arg2,
	unsigned long arg3)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr), "b" (arg1), "c" (arg2), "d" (arg3));
	return syscall_return(res);

}

static struct syscall_result syscall4(unsigned long nr, unsigned long arg1, unsigned long arg2,
	unsigned long arg3, unsigned long arg4)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr), "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4));
	return syscall_return(res);

}

static struct syscall_result syscall5(unsigned long nr, unsigned long arg1, unsigned long arg2,
	unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
	long res;
	asm volatile(
		"int $0x80"
		: "=a" (res)
		: "a" (nr), "b" (arg1), "c" (arg2), "d" (arg3),
		"S" (arg4), "D" (arg5));
	return syscall_return(res);

}

#define NR_exit                 1
#define NR_fork                 2
#define NR_read                 3
#define NR_write                4
#define NR_open                 5
#define NR_close                6
#define NR_waitpid              7
#define NR_creat                8
#define NR_link                 9
#define NR_unlink              10
#define NR_execve              11
#define NR_chdir               12
#define NR_time                13
#define NR_mknod               14
#define NR_chmod               15
#define NR_lchown              16
#define NR_break               17
#define NR_oldstat             18
#define NR_lseek               19
#define NR_getpid              20
#define NR_mount               21
#define NR_umount              22
#define NR_setuid              23
#define NR_getuid              24
#define NR_stime               25
#define NR_ptrace              26
#define NR_alarm               27
#define NR_oldfstat            28
#define NR_pause               29
#define NR_utime               30
#define NR_stty                31
#define NR_gtty                32
#define NR_access              33
#define NR_nice                34
#define NR_ftime               35
#define NR_sync                36
#define NR_kill                37
#define NR_rename              38
#define NR_mkdir               39
#define NR_rmdir               40
#define NR_dup                 41
#define NR_pipe                42
#define NR_times               43
#define NR_prof                44
#define NR_brk                 45
#define NR_setgid              46
#define NR_getgid              47
#define NR_signal              48
#define NR_geteuid             49
#define NR_getegid             50
#define NR_acct                51
#define NR_umount2             52
#define NR_lock                53
#define NR_ioctl               54
#define NR_fcntl               55
#define NR_mpx                 56
#define NR_setpgid             57
#define NR_ulimit              58
#define NR_oldolduname         59
#define NR_umask               60
#define NR_chroot              61
#define NR_ustat               62
#define NR_dup2                63
#define NR_getppid             64
#define NR_getpgrp             65
#define NR_setsid              66
#define NR_sigaction           67
#define NR_sgetmask            68
#define NR_ssetmask            69
#define NR_setreuid            70
#define NR_setregid            71
#define NR_sigsuspend          72
#define NR_sigpending          73
#define NR_sethostname         74
#define NR_setrlimit           75
#define NR_getrlimit           76
#define NR_getrusage           77
#define NR_gettimeofday        78
#define NR_settimeofday        79
#define NR_getgroups           80
#define NR_setgroups           81
#define NR_select              82
#define NR_symlink             83
#define NR_oldlstat            84
#define NR_readlink            85
#define NR_uselib              86
#define NR_swapon              87
#define NR_reboot              88
#define NR_readdir             89
#define NR_mmap                90
#define NR_munmap              91
#define NR_truncate            92
#define NR_ftruncate           93
#define NR_fchmod              94
#define NR_fchown              95
#define NR_getpriority         96
#define NR_setpriority         97
#define NR_profil              98
#define NR_statfs              99
#define NR_fstatfs            100
#define NR_ioperm             101
#define NR_socketcall         102
#define NR_syslog             103
#define NR_setitimer          104
#define NR_getitimer          105
#define NR_stat               106
#define NR_lstat              107
#define NR_fstat              108
#define NR_olduname           109
#define NR_iopl               110
#define NR_vhangup            111
#define NR_idle               112
#define NR_vm86old            113
#define NR_wait4              114
#define NR_swapoff            115
#define NR_sysinfo            116
#define NR_ipc                117
#define NR_fsync              118
#define NR_sigreturn          119
#define NR_clone              120
#define NR_setdomainname      121
#define NR_uname              122
#define NR_modify_ldt         123
#define NR_adjtimex           124
#define NR_mprotect           125
#define NR_sigprocmask        126
#define NR_create_module      127
#define NR_init_module        128
#define NR_delete_module      129
#define NR_get_kernel_syms    130
#define NR_quotactl           131
#define NR_getpgid            132
#define NR_fchdir             133
#define NR_bdflush            134
#define NR_sysfs              135
#define NR_personality        136
#define NR_afs_syscall        137 /* Syscall for Andrew File System */
#define NR_setfsuid           138
#define NR_setfsgid           139
#define NR__llseek            140
#define NR_getdents           141
#define NR__newselect         142
#define NR_flock              143
#define NR_msync              144
#define NR_readv              145
#define NR_writev             146
#define NR_getsid             147
#define NR_fdatasync          148
#define NR__sysctl            149
#define NR_mlock              150
#define NR_munlock            151
#define NR_mlockall           152
#define NR_munlockall         153
#define NR_sched_setparam             154
#define NR_sched_getparam             155
#define NR_sched_setscheduler         156
#define NR_sched_getscheduler         157
#define NR_sched_yield                158
#define NR_sched_get_priority_max     159
#define NR_sched_get_priority_min     160
#define NR_sched_rr_get_interval      161
#define NR_nanosleep          162
#define NR_mremap             163
#define NR_setresuid          164
#define NR_getresuid          165
#define NR_vm86               166
#define NR_query_module       167
#define NR_poll               168
#define NR_nfsservctl         169
#define NR_setresgid          170
#define NR_getresgid          171
#define NR_prctl              172
#define NR_rt_sigreturn       173
#define NR_rt_sigaction       174
#define NR_rt_sigprocmask     175
#define NR_rt_sigpending      176
#define NR_rt_sigtimedwait    177
#define NR_rt_sigqueueinfo    178
#define NR_rt_sigsuspend      179
#define NR_pread              180
#define NR_pwrite             181
#define NR_chown              182
#define NR_getcwd             183
#define NR_capget             184
#define NR_capset             185
#define NR_sigaltstack        186
#define NR_sendfile           187
#define NR_getpmsg            188     /* some people actually want streams */
#define NR_putpmsg            189     /* some people actually want streams */
#define NR_vfork              190

typedef long ssize_t;
typedef unsigned long size_t;

/* Standard file descriptors */
#define STDIN_FILENO    0  /* Standard input */
#define STDOUT_FILENO   1  /* Standard output */
#define STDERR_FILENO   2  /* Standard error output */

static ssize_t write(int fd, const void *buf, size_t count)
{
	struct syscall_result res;
	res = syscall3(NR_write, fd, (unsigned long)buf, count);
	return res.val;
}

static void _exit(int status)
{
	struct syscall_result res;
	res = syscall1(NR_exit, status);
}

static const char *addr_of_char(unsigned char ch)
{
	static const char byte[] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
		0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
		0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
		0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
		0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
		0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
		0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
		0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
		0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
		0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
	};
	return byte + ch;
}

static void console_tx_byte(unsigned char ch)
{
	write(STDOUT_FILENO, addr_of_char(ch), 1);
}

static void console_tx_nibble(unsigned nibble)
{
	unsigned char digit;
	digit = nibble + '0';
	if (digit > '9') {
		digit += 39;
	}
	console_tx_byte(digit);
}

static void console_tx_char(unsigned char byte)
{
	console_tx_byte(byte);
}

static void console_tx_hex8(unsigned char value)
{
	console_tx_nibble((value >> 4U) & 0x0fU);
	console_tx_nibble(value & 0x0fU);
}

static void console_tx_hex16(unsigned short value)
{
	console_tx_nibble((value >> 12U) & 0x0FU);
	console_tx_nibble((value >>  8U) & 0x0FU);
	console_tx_nibble((value >>  4U) & 0x0FU);
	console_tx_nibble(value & 0x0FU);
}

static void console_tx_hex32(unsigned short value)
{
	console_tx_nibble((value >> 28U) & 0x0FU);
	console_tx_nibble((value >> 24U) & 0x0FU);
	console_tx_nibble((value >> 20U) & 0x0FU);
	console_tx_nibble((value >> 16U) & 0x0FU);
	console_tx_nibble((value >> 12U) & 0x0FU);
	console_tx_nibble((value >>  8U) & 0x0FU);
	console_tx_nibble((value >>  4U) & 0x0FU);
	console_tx_nibble(value & 0x0FU);
}

static void console_tx_string(const char *str)
{
	unsigned char ch;
	while((ch = *str++) != '\0') {
		console_tx_byte(ch);
	}
}

static void print_debug_char(unsigned char byte) { console_tx_char(byte); }
static void print_debug_hex8(unsigned char value) { console_tx_hex8(value); }
static void print_debug_hex16(unsigned short value){ console_tx_hex16(value); }
static void print_debug_hex32(unsigned int value) { console_tx_hex32(value); }
static void print_debug(const char *str) { console_tx_string(str); }


static void setup_coherent_ht_domain(void)
{
	static const unsigned int register_values[] = {
#if 1
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x40) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x44) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x48) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x4c) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x50) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x54) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x58) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x5c) & 0xFF)), 0xfff0f0f0, 0x00010101,
# 983 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x68) & 0xFF)), 0x00800000, 0x0f00840f,
# 1005 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x6C) & 0xFF)), 0xffffff8c, 0x00000000 | (1 << 6) |(1 << 5)| (1 << 4),
# 1082 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00009c05, 0x11110020,
# 1127 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x88) & 0xFF)), 0xfffff0ff, 0x00000200,
# 1148 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x94) & 0xFF)), 0xff000000, 0x00ff0000,
# 1182 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x44) & 0xFF)), 0x0000f8f8, 0x003f0000,




        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x4C) & 0xFF)), 0x0000f8f8, 0x00000001,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x54) & 0xFF)), 0x0000f8f8, 0x00000002,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x0000f8f8, 0x00000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x64) & 0xFF)), 0x0000f8f8, 0x00000004,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x6C) & 0xFF)), 0x0000f8f8, 0x00000005,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x74) & 0xFF)), 0x0000f8f8, 0x00000006,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x7C) & 0xFF)), 0x0000f8f8, 0x00000007,
# 1224 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x40) & 0xFF)), 0x0000f8fc, 0x00000003,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x48) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x50) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x58) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x60) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x68) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x70) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x78) & 0xFF)), 0x0000f8fc, 0x00400000,
# 1276 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00000048, 0x00e1ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x8C) & 0xFF)), 0x00000048, 0x00dfff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x94) & 0xFF)), 0x00000048, 0x00e3ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x9C) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA4) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xAC) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB4) & 0xFF)), 0x00000048, 0x00000b00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xBC) & 0xFF)), 0x00000048, 0x00fe0b00,
# 1311 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x80) & 0xFF)), 0x000000f0, 0x00e00003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x88) & 0xFF)), 0x000000f0, 0x00d80003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x90) & 0xFF)), 0x000000f0, 0x00e20003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x98) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA0) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA8) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB0) & 0xFF)), 0x000000f0, 0x00000a03,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB8) & 0xFF)), 0x000000f0, 0x00400003,
# 1350 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC4) & 0xFF)), 0xFE000FC8, 0x0000d000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xCC) & 0xFF)), 0xFE000FC8, 0x000ff000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD4) & 0xFF)), 0xFE000FC8, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xDC) & 0xFF)), 0xFE000FC8, 0x00000000,
# 1380 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC0) & 0xFF)), 0xFE000FCC, 0x0000d003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC8) & 0xFF)), 0xFE000FCC, 0x00001013,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD0) & 0xFF)), 0xFE000FCC, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD8) & 0xFF)), 0xFE000FCC, 0x00000000,
# 1421 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE0) & 0xFF)), 0x0000FC88, 0xff000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE4) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE8) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xEC) & 0xFF)), 0x0000FC88, 0x00000000,
#else
#define PCI_ADDR(BUS, DEV, FN, WHERE) ( \
	(((BUS) & 0xFF) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN) & 0x07) << 8) | \
	((WHERE) & 0xFF))

	/* Routing Table Node i
	 * F0:0x40 i = 0,
	 * F0:0x44 i = 1,
	 * F0:0x48 i = 2,
	 * F0:0x4c i = 3,
	 * F0:0x50 i = 4,
	 * F0:0x54 i = 5,
	 * F0:0x58 i = 6,
	 * F0:0x5c i = 7
	 * [ 0: 3] Request Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [11: 8] Response Route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 * [19:16] Broadcast route
	 *     [0] Route to this node
	 *     [1] Route to Link 0
	 *     [2] Route to Link 1
	 *     [3] Route to Link 2
	 */
	PCI_ADDR(0, 0x18, 0, 0x40), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x44), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x48), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x4c), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x50), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x54), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x58), 0xfff0f0f0, 0x00010101,
	PCI_ADDR(0, 0x18, 0, 0x5c), 0xfff0f0f0, 0x00010101,

	/* Hypetransport Transaction Control Register
	 * F0:0x68
	 * [ 0: 0] Disable read byte probe
	 *         0 = Probes issues
	 *         1 = Probes not issued
	 * [ 1: 1] Disable Read Doubleword probe
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 2: 2] Disable write byte probes
	 *         0 = Probes issued
	 *         1 = Probes not issued
	 * [ 3: 3] Disable Write Doubleword Probes
	 *         0 = Probes issued
	 *         1 = Probes not issued.
	 * [ 4: 4] Disable Memroy Controller Target Start
	 *         0 = TgtStart packets are generated
	 *         1 = TgtStart packets are not generated.
	 * [ 5: 5] CPU1 Enable
	 *         0 = Second CPU disabled or not present
	 *         1 = Second CPU enabled.
	 * [ 6: 6] CPU Request PassPW
	 *         0 = CPU requests do not pass posted writes
	 *         1 = CPU requests pass posted writes.
	 * [ 7: 7] CPU read Respons PassPW
	 *         0 = CPU Responses do not pass posted writes
	 *         1 = CPU responses pass posted writes.
	 * [ 8: 8] Disable Probe Memory Cancel
	 *         0 = Probes may generate MemCancels
	 *         1 = Probes may not generate MemCancels
	 * [ 9: 9] Disable Remote Probe Memory Cancel.
	 *         0 = Probes hitting dirty blocks generate memory cancel packets
	 *         1 = Only probed caches on the same node as the memory controller
	 *              generate cancel packets.
	 * [10:10] Disable Fill Probe
	 *         0 = Probes issued for cache fills
	 *         1 = Probes not issued for cache fills.
	 * [11:11] Response PassPw
	 *         0 = Downstream response PassPW based on original request
	 *         1 = Downstream response PassPW set to 1
	 * [12:12] Change ISOC to Ordered
	 *         0 = Bit 1 of coherent HT RdSz/WrSz command used for iosynchronous prioritization
	 *         1 = Bit 1 of coherent HT RdSz/WrSz command used for ordering.
	 * [14:13] Buffer Release Priority select
	 *         00 = 64
	 *         01 = 16
	 *         10 = 8
	 *         11 = 2
	 * [15:15] Limit Coherent HT Configuration Space Range
	 *         0 = No coherent HT configuration space restrictions
	 *         1 = Limit coherent HT configuration space based on node count
	 * [16:16] Local Interrupt Conversion Enable.
	 *         0 = ExtInt/NMI interrupts unaffected.
	 *         1 = ExtInt/NMI broadcast interrupts converted to LINT0/1
	 * [17:17] APIC Extended Broadcast Enable.
	 *         0 = APIC broadcast is 0F
	 *         1 = APIC broadcast is FF
	 * [18:18] APIC Extended ID Enable
	 *         0 = APIC ID is 4 bits.
	 *         1 = APIC ID is 8 bits.
	 * [19:19] APIC Extended Spurious Vector Enable
	 *         0 = Lower 4 bits of spurious vector are read-only 1111
	 *         1 = Lower 4 bits of spurious vecotr are writeable.
	 * [20:20] Sequence ID Source Node Enable
	 *         0 = Normal operation
	 *         1 = Keep SeqID on routed packets for debugging.
	 * [22:21] Downstream non-posted request limit
	 *         00 = No limit
	 *         01 = Limited to 1
	 *         10 = Limited to 4
	 *         11 = Limited to 8
	 * [23:23] RESERVED
	 * [25:24] Medium-Priority Bypass Count
	 *         - Maximum # of times a medium priority access can pass a low
	 *           priority access before Medium-Priority mode is disabled for one access.
	 * [27:26] High-Priority Bypass Count
	 *         - Maximum # of times a high prioirty access can pass a medium or low
	 *           priority access before High-prioirty mode is disabled for one access.
	 * [28:28] Enable High Priority CPU Reads
	 *         0 = Cpu reads are medium prioirty
	 *         1 = Cpu reads are high prioirty
	 * [29:29] Disable Low Priority Writes
	 *         0 = Non-isochronous writes are low priority
	 *         1 = Non-isochronous writes are medium prioirty
	 * [30:30] Disable High Priority Isochronous writes
	 *         0 = Isochronous writes are high priority
	 *         1 = Isochronous writes are medium priority
	 * [31:31] Disable Medium Priority Isochronous writes
	 *         0 = Isochronous writes are medium are high
	 *         1 = With bit 30 set makes Isochrouns writes low priority.
	 */
	PCI_ADDR(0, 0x18, 0, 0x68), 0x00800000, 0x0f00840f,
	/* HT Initialization Control Register
	 * F0:0x6C
	 * [ 0: 0] Routing Table Disable
	 *         0 = Packets are routed according to routing tables
	 *         1 = Packets are routed according to the default link field
	 * [ 1: 1] Request Disable (BSP should clear this)
	 *         0 = Request packets may be generated
	 *         1 = Request packets may not be generated.
	 * [ 3: 2] Default Link (Read-only)
	 *         00 = LDT0
	 *         01 = LDT1
	 *         10 = LDT2
	 *         11 = CPU on same node
	 * [ 4: 4] Cold Reset
	 *         - Scratch bit cleared by a cold reset
	 * [ 5: 5] BIOS Reset Detect
	 *         - Scratch bit cleared by a cold reset
	 * [ 6: 6] INIT Detect
	 *         - Scratch bit cleared by a warm or cold reset not by an INIT
	 *
	 */
	PCI_ADDR(0, 0x18, 0, 0x6C), 0xffffff8c, 0x00000000 | (1 << 6) |(1 << 5)| (1 << 4),
	/* LDTi Capabilities Registers
	 * F0:0x80 i = 0,
	 * F0:0xA0 i = 1,
	 * F0:0xC0 i = 2,
	 */
	/* LDTi Link Control Registrs
	 * F0:0x84 i = 0,
	 * F0:0xA4 i = 1,
	 * F0:0xC4 i = 2,
	 * [ 1: 1] CRC Flood Enable
	 *         0 = Do not generate sync packets on CRC error
	 *         1 = Generate sync packets on CRC error
	 * [ 2: 2] CRC Start Test (Read-Only)
	 * [ 3: 3] CRC Force Frame Error
	 *         0 = Do not generate bad CRC
	 *         1 = Generate bad CRC
	 * [ 4: 4] Link Failure
	 *         0 = No link failure detected
	 *         1 = Link failure detected
	 * [ 5: 5] Initialization Complete
	 *         0 = Initialization not complete
	 *         1 = Initialization complete
	 * [ 6: 6] Receiver off
	 *         0 = Recevier on
	 *         1 = Receiver off
	 * [ 7: 7] Transmitter Off
	 *         0 = Transmitter on
	 *         1 = Transmitter off
	 * [ 9: 8] CRC_Error
	 *         00 = No error
	 *         [0] = 1 Error on byte lane 0
	 *         [1] = 1 Error on byte lane 1
	 * [12:12] Isochrnous Enable  (Read-Only)
	 * [13:13] HT Stop Tristate Enable
	 *         0 = Driven during an LDTSTOP_L
	 *         1 = Tristated during and LDTSTOP_L
	 * [14:14] Extended CTL Time
	 *         0 = CTL is asserted for 16 bit times during link initialization
	 *         1 = CTL is asserted for 50us during link initialization
	 * [18:16] Max Link Width In (Read-Only?)
	 *         000 = 8 bit link
	 *         001 = 16bit link
	 * [19:19] Doubleword Flow Control in (Read-Only)
	 *         0 = This link does not support doubleword flow control
	 *         1 = This link supports doubleword flow control
	 * [22:20] Max Link Width Out (Read-Only?)
	 *         000 = 8 bit link
	 *         001 = 16bit link
	 * [23:23] Doubleworld Flow Control out (Read-Only)
	 *         0 = This link does not support doubleword flow control
	 *         1 = This link supports doubleworkd flow control
	 * [26:24] Link Width In
	 *         000 = Use 8 bits
	 *         001 = Use 16 bits
	 *         010 = reserved
	 *         011 = Use 32 bits
	 *         100 = Use 2 bits
	 *         101 = Use 4 bits
	 *         110 = reserved
	 *         111 = Link physically not connected
	 * [27:27] Doubleword Flow Control In Enable
	 *         0 = Doubleword flow control disabled
	 *         1 = Doubleword flow control enabled (Not currently supported)
	 * [30:28] Link Width Out
	 *         000 = Use 8 bits
	 *         001 = Use 16 bits
	 *         010 = reserved
	 *         011 = Use 32 bits
	 *         100 = Use 2 bits
	 *         101 = Use 4 bits
	 *         110 = reserved
	 *         111 = Link physically not connected
	 * [31:31] Doubleworld Flow Control Out Enable
	 *         0 = Doubleworld flow control disabled
	 *         1 = Doubleword flow control enabled (Not currently supported)
	 */
	PCI_ADDR(0, 0x18, 0, 0x84), 0x00009c05, 0x11110020,
	/* LDTi Frequency/Revision Registers
	 * F0:0x88 i = 0,
	 * F0:0xA8 i = 1,
	 * F0:0xC8 i = 2,
	 * [ 4: 0] Minor Revision
	 *         Contains the HT Minor revision
	 * [ 7: 5] Major Revision
	 *         Contains the HT Major revision
	 * [11: 8] Link Frequency  (Takes effect the next time the link is reconnected)
	 *         0000 = 200Mhz
	 *         0001 = reserved
	 *         0010 = 400Mhz
	 *         0011 = reserved
	 *         0100 = 600Mhz
	 *         0101 = 800Mhz
	 *         0110 = 1000Mhz
	 *         0111 = reserved
	 *         1000 = reserved
	 *         1001 = reserved
	 *         1010 = reserved
	 *         1011 = reserved
	 *         1100 = reserved
	 *         1101 = reserved
	 *         1110 = reserved
	 *         1111 = 100 Mhz
	 * [15:12] Error (Not currently Implemented)
	 * [31:16] Indicates the frequency capabilities of the link
	 *         [16] = 1 encoding 0000 of freq supported
	 *         [17] = 1 encoding 0001 of freq supported
	 *         [18] = 1 encoding 0010 of freq supported
	 *         [19] = 1 encoding 0011 of freq supported
	 *         [20] = 1 encoding 0100 of freq supported
	 *         [21] = 1 encoding 0101 of freq supported
	 *         [22] = 1 encoding 0110 of freq supported
	 *         [23] = 1 encoding 0111 of freq supported
	 *         [24] = 1 encoding 1000 of freq supported
	 *         [25] = 1 encoding 1001 of freq supported
	 *         [26] = 1 encoding 1010 of freq supported
	 *         [27] = 1 encoding 1011 of freq supported
	 *         [28] = 1 encoding 1100 of freq supported
	 *         [29] = 1 encoding 1101 of freq supported
	 *         [30] = 1 encoding 1110 of freq supported
	 *         [31] = 1 encoding 1111 of freq supported
	 */
	PCI_ADDR(0, 0x18, 0, 0x88), 0xfffff0ff, 0x00000200,
	/* LDTi Feature Capability
	 * F0:0x8C i = 0,
	 * F0:0xAC i = 1,
	 * F0:0xCC i = 2,
	 */
	/* LDTi Buffer Count Registers
	 * F0:0x90 i = 0,
	 * F0:0xB0 i = 1,
	 * F0:0xD0 i = 2,
	 */
	/* LDTi Bus Number Registers
	 * F0:0x94 i = 0,
	 * F0:0xB4 i = 1,
	 * F0:0xD4 i = 2,
	 * For NonCoherent HT specifies the bus number downstream (behind the host bridge)
	 * [ 0: 7] Primary Bus Number
	 * [15: 8] Secondary Bus Number
	 * [23:15] Subordiante Bus Number
	 * [31:24] reserved
	 */
	PCI_ADDR(0, 0x18, 0, 0x94), 0xff000000, 0x00ff0000,
	/* LDTi Type Registers
	 * F0:0x98 i = 0,
	 * F0:0xB8 i = 1,
	 * F0:0xD8 i = 2,
	 */
	/* Careful set limit registers before base registers which contain the enables */
	/* DRAM Limit i Registers
	 * F1:0x44 i = 0
	 * F1:0x4C i = 1
	 * F1:0x54 i = 2
	 * F1:0x5C i = 3
	 * F1:0x64 i = 4
	 * F1:0x6C i = 5
	 * F1:0x74 i = 6
	 * F1:0x7C i = 7
	 * [ 2: 0] Destination Node ID
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 7: 3] Reserved
	 * [10: 8] Interleave select
	 *         specifies the values of A[14:12] to use with interleave enable.
	 * [15:11] Reserved
	 * [31:16] DRAM Limit Address i Bits 39-24
	 *         This field defines the upper address bits of a 40 bit  address
	 *         that define the end of the DRAM region.
	 */
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 1, 0x44), 0x0000f8f8, 0x003f0000,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 1, 0x44), 0x0000f8f8, 0x001f0000,
#endif
	PCI_ADDR(0, 0x18, 1, 0x4C), 0x0000f8f8, 0x00000001,
	PCI_ADDR(0, 0x18, 1, 0x54), 0x0000f8f8, 0x00000002,
	PCI_ADDR(0, 0x18, 1, 0x5C), 0x0000f8f8, 0x00000003,
	PCI_ADDR(0, 0x18, 1, 0x64), 0x0000f8f8, 0x00000004,
	PCI_ADDR(0, 0x18, 1, 0x6C), 0x0000f8f8, 0x00000005,
	PCI_ADDR(0, 0x18, 1, 0x74), 0x0000f8f8, 0x00000006,
	PCI_ADDR(0, 0x18, 1, 0x7C), 0x0000f8f8, 0x00000007,
	/* DRAM Base i Registers
	 * F1:0x40 i = 0
	 * F1:0x48 i = 1
	 * F1:0x50 i = 2
	 * F1:0x58 i = 3
	 * F1:0x60 i = 4
	 * F1:0x68 i = 5
	 * F1:0x70 i = 6
	 * F1:0x78 i = 7
	 * [ 0: 0] Read Enable
	 *         0 = Reads Disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes Disabled
	 *         1 = Writes Enabled
	 * [ 7: 2] Reserved
	 * [10: 8] Interleave Enable
	 *         000 = No interleave
	 *         001 = Interleave on A[12] (2 nodes)
	 *         010 = reserved
	 *         011 = Interleave on A[12] and A[14] (4 nodes)
	 *         100 = reserved
	 *         101 = reserved
	 *         110 = reserved
	 *         111 = Interleve on A[12] and A[13] and A[14] (8 nodes)
	 * [15:11] Reserved
	 * [13:16] DRAM Base Address i Bits 39-24
	 *         This field defines the upper address bits of a 40-bit address
	 *         that define the start of the DRAM region.
	 */
	PCI_ADDR(0, 0x18, 1, 0x40), 0x0000f8fc, 0x00000003,
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 1, 0x48), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x50), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x58), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x60), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x68), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x70), 0x0000f8fc, 0x00400000,
	PCI_ADDR(0, 0x18, 1, 0x78), 0x0000f8fc, 0x00400000,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 1, 0x48), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x50), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x58), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x60), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x68), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x70), 0x0000f8fc, 0x00200000,
	PCI_ADDR(0, 0x18, 1, 0x78), 0x0000f8fc, 0x00200000,
#endif

	/* Memory-Mapped I/O Limit i Registers
	 * F1:0x84 i = 0
	 * F1:0x8C i = 1
	 * F1:0x94 i = 2
	 * F1:0x9C i = 3
	 * F1:0xA4 i = 4
	 * F1:0xAC i = 5
	 * F1:0xB4 i = 6
	 * F1:0xBC i = 7
	 * [ 2: 0] Destination Node ID
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 3: 3] Reserved
	 * [ 5: 4] Destination Link ID
	 *         00 = Link 0
	 *         01 = Link 1
	 *         10 = Link 2
	 *         11 = Reserved
	 * [ 6: 6] Reserved
	 * [ 7: 7] Non-Posted
	 *         0 = CPU writes may be posted
	 *         1 = CPU writes must be non-posted
	 * [31: 8] Memory-Mapped I/O Limit Address i (39-16)
	 *         This field defines the upp address bits of a 40-bit address that
	 *         defines the end of a memory-mapped I/O region n
	 */
	PCI_ADDR(0, 0x18, 1, 0x84), 0x00000048, 0x00e1ff00,
	PCI_ADDR(0, 0x18, 1, 0x8C), 0x00000048, 0x00dfff00,
	PCI_ADDR(0, 0x18, 1, 0x94), 0x00000048, 0x00e3ff00,
	PCI_ADDR(0, 0x18, 1, 0x9C), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA4), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xAC), 0x00000048, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xB4), 0x00000048, 0x00000b00,
	PCI_ADDR(0, 0x18, 1, 0xBC), 0x00000048, 0x00fe0b00,

	/* Memory-Mapped I/O Base i Registers
	 * F1:0x80 i = 0
	 * F1:0x88 i = 1
	 * F1:0x90 i = 2
	 * F1:0x98 i = 3
	 * F1:0xA0 i = 4
	 * F1:0xA8 i = 5
	 * F1:0xB0 i = 6
	 * F1:0xB8 i = 7
	 * [ 0: 0] Read Enable
	 *         0 = Reads disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes disabled
	 *         1 = Writes Enabled
	 * [ 2: 2] Cpu Disable
	 *         0 = Cpu can use this I/O range
	 *         1 = Cpu requests do not use this I/O range
	 * [ 3: 3] Lock
	 *         0 = base/limit registers i are read/write
	 *         1 = base/limit registers i are read-only
	 * [ 7: 4] Reserved
	 * [31: 8] Memory-Mapped I/O Base Address i (39-16)
	 *         This field defines the upper address bits of a 40bit address
	 *         that defines the start of memory-mapped I/O region i
	 */
	PCI_ADDR(0, 0x18, 1, 0x80), 0x000000f0, 0x00e00003,
	PCI_ADDR(0, 0x18, 1, 0x88), 0x000000f0, 0x00d80003,
	PCI_ADDR(0, 0x18, 1, 0x90), 0x000000f0, 0x00e20003,
	PCI_ADDR(0, 0x18, 1, 0x98), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA0), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xA8), 0x000000f0, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xB0), 0x000000f0, 0x00000a03,
#if MEMORY_1024MB
	PCI_ADDR(0, 0x18, 1, 0xB8), 0x000000f0, 0x00400003,
#endif
#if MEMORY_512MB
	PCI_ADDR(0, 0x18, 1, 0xB8), 0x000000f0, 0x00200003,
#endif

	/* PCI I/O Limit i Registers
	 * F1:0xC4 i = 0
	 * F1:0xCC i = 1
	 * F1:0xD4 i = 2
	 * F1:0xDC i = 3
	 * [ 2: 0] Destination Node ID
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 3: 3] Reserved
	 * [ 5: 4] Destination Link ID
	 *         00 = Link 0
	 *         01 = Link 1
	 *         10 = Link 2
	 *         11 = reserved
	 * [11: 6] Reserved
	 * [24:12] PCI I/O Limit Address i
	 *         This field defines the end of PCI I/O region n
	 * [31:25] Reserved
	 */
	PCI_ADDR(0, 0x18, 1, 0xC4), 0xFE000FC8, 0x0000d000,
	PCI_ADDR(0, 0x18, 1, 0xCC), 0xFE000FC8, 0x000ff000,
	PCI_ADDR(0, 0x18, 1, 0xD4), 0xFE000FC8, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xDC), 0xFE000FC8, 0x00000000,

	/* PCI I/O Base i Registers
	 * F1:0xC0 i = 0
	 * F1:0xC8 i = 1
	 * F1:0xD0 i = 2
	 * F1:0xD8 i = 3
	 * [ 0: 0] Read Enable
	 *         0 = Reads Disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes Disabled
	 *         1 = Writes Enabled
	 * [ 3: 2] Reserved
	 * [ 4: 4] VGA Enable
	 *         0 = VGA matches Disabled
	 *         1 = matches all address < 64K and where A[9:0] is in the
	 *             range 3B0-3BB or 3C0-3DF independen of the base & limit registers
	 * [ 5: 5] ISA Enable
	 *         0 = ISA matches Disabled
	 *         1 = Blocks address < 64K and in the last 768 bytes of eack 1K block
	 *             from matching agains this base/limit pair
	 * [11: 6] Reserved
	 * [24:12] PCI I/O Base i
	 *         This field defines the start of PCI I/O region n
	 * [31:25] Reserved
	 */
	PCI_ADDR(0, 0x18, 1, 0xC0), 0xFE000FCC, 0x0000d003,
	PCI_ADDR(0, 0x18, 1, 0xC8), 0xFE000FCC, 0x00001013,
	PCI_ADDR(0, 0x18, 1, 0xD0), 0xFE000FCC, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xD8), 0xFE000FCC, 0x00000000,

	/* Config Base and Limit i Registers
	 * F1:0xE0 i = 0
	 * F1:0xE4 i = 1
	 * F1:0xE8 i = 2
	 * F1:0xEC i = 3
	 * [ 0: 0] Read Enable
	 *         0 = Reads Disabled
	 *         1 = Reads Enabled
	 * [ 1: 1] Write Enable
	 *         0 = Writes Disabled
	 *         1 = Writes Enabled
	 * [ 2: 2] Device Number Compare Enable
	 *         0 = The ranges are based on bus number
	 *         1 = The ranges are ranges of devices on bus 0
	 * [ 3: 3] Reserved
	 * [ 6: 4] Destination Node
	 *         000 = Node 0
	 *         001 = Node 1
	 *         010 = Node 2
	 *         011 = Node 3
	 *         100 = Node 4
	 *         101 = Node 5
	 *         110 = Node 6
	 *         111 = Node 7
	 * [ 7: 7] Reserved
	 * [ 9: 8] Destination Link
	 *         00 = Link 0
	 *         01 = Link 1
	 *         10 = Link 2
	 *         11 - Reserved
	 * [15:10] Reserved
	 * [23:16] Bus Number Base i
	 *         This field defines the lowest bus number in configuration region i
	 * [31:24] Bus Number Limit i
	 *         This field defines the highest bus number in configuration regin i
	 */
	PCI_ADDR(0, 0x18, 1, 0xE0), 0x0000FC88, 0xff000003,
	PCI_ADDR(0, 0x18, 1, 0xE4), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xE8), 0x0000FC88, 0x00000000,
	PCI_ADDR(0, 0x18, 1, 0xEC), 0x0000FC88, 0x00000000,
#endif
	};
	int i;
	int max;
	print_debug("setting up coherent ht domain....\r\n");
	max = sizeof(register_values)/sizeof(register_values[0]);
	for(i = 0; i < max; i += 3) {
		unsigned long reg;
#if 1
		print_debug_hex32(register_values[i]);
		print_debug(" <-");
		print_debug_hex32(register_values[i+2]);
		print_debug("\r\n");
#endif
#if 0
		reg = pci_read_config32(register_values[i]);
		reg &= register_values[i+1];
		reg |= register_values[i+2] & ~register_values[i+1];
		pci_write_config32(register_values[i], reg);
#endif
	}
	print_debug("done.\r\n");
}

static void main(void)
{
	static const char msg[] = "hello world\r\n";
#if 0
	write(STDOUT_FILENO, msg, sizeof(msg));
#endif
#if 1
	setup_coherent_ht_domain();
#endif
	_exit(0);
}
