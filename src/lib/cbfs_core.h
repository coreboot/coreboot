#ifndef __LIB_CBFS_CORE
#define __LIB_CBFS_CORE

#include <cbfs.h>
#include <string.h>

#ifdef LIBPAYLOAD
# include <libpayload-config.h>
# ifdef CONFIG_LZMA
#  include <lzma.h>
#  define CBFS_CORE_WITH_LZMA
# endif
# define CBFS_MINI_BUILD
#elif defined(__SMM__)
# define CBFS_MINI_BUILD
#elif defined(__BOOTBLOCK__)
  /* No LZMA in boot block. */
#elif defined(__VERSTAGE__)
  /* No LZMA in verstage. */
#elif defined(__PRE_RAM__) && !CONFIG_COMPRESS_RAMSTAGE
  /* No LZMA in romstage if ramstage is not compressed. */
#else
# define CBFS_CORE_WITH_LZMA
# include <lib.h>
#endif

#include <cbfs.h>
#include <string.h>
#include <cbmem.h>

#ifdef LIBPAYLOAD
# include <stdio.h>
# define DEBUG(x...)
# define LOG(x...) printf(x)
# define ERROR(x...) printf(x)
#else
# include <console/console.h>
# define ERROR(x...) printk(BIOS_ERR, "CBFS: " x)
# define LOG(x...) printk(BIOS_INFO, "CBFS: " x)
# if CONFIG_DEBUG_CBFS
#  define DEBUG(x...) printk(BIOS_SPEW, "CBFS: " x)
# else
#  define DEBUG(x...)
# endif
#endif

#endif /* __LIB_CBFS_CORE */
