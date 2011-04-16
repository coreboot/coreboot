static void *walkcbfs(char *target)
{
	void *entry;
	asm volatile (
		"mov $1f, %%esp\n\t"
		"jmp walkcbfs_asm\n\t"
		"1:\n\t" : "=a" (entry) : "S" (target) : "ebx", "ecx", "edi", "esp");
	return entry;
}

/* just enough to support findstage. copied because the original version doesn't easily pass through romcc */
struct cbfs_stage_restricted {
	unsigned long compression;
	unsigned long entry; // this is really 64bit, but properly endianized
};

static inline unsigned long findstage(char* target)
{
	return ((struct cbfs_stage_restricted *)walkcbfs(target))->entry;
}

static inline void call(unsigned long addr, unsigned long bist)
{
	asm volatile ("jmp *%0\n\t" : : "r" (addr), "a" (bist));
}

