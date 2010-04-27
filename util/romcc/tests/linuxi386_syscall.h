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

/* Standard file descriptors */
#define STDIN_FILENO    0  /* Standard input */
#define STDOUT_FILENO   1  /* Standard output */
#define STDERR_FILENO   2  /* Standard error output */

typedef long ssize_t;
typedef unsigned long size_t;

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
