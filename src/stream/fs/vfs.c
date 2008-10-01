/* Interface between GRUB's fs drivers and application code */

#include <console/console.h>
#include <fs/fs.h>
#include <string.h>
#include <stdlib.h>

int filepos;
int filemax;
fs_error_t errnum;
void (*disk_read_hook) (int, int, int);
void (*disk_read_func) (int, int, int);
char FSYS_BUF[FSYS_BUFLEN];
int fsmax;

struct fsys_entry {
    char *name;
    int (*mount_func) (void);
    int (*read_func) (char *buf, int len);
    int (*dir_func) (char *dirname);
    void (*close_func) (void);
    int (*embed_func) (int *start_sector, int needed_sectors);
};

struct fsys_entry fsys_table[] = {
# if CONFIG_FS_FAT == 1
    {"fat", fat_mount, fat_read, fat_dir, 0, 0},
# endif
# if CONFIG_FS_EXT2 == 1
    {"ext2fs", ext2fs_mount, ext2fs_read, ext2fs_dir, 0, 0},
# endif
# if CONFIG_FS_ISO9660 == 1
    {"iso9660", iso9660_mount, iso9660_read, iso9660_dir, 0, 0},
# endif
};

/* NULLFS is used to read images from raw device */
static int nullfs_dir(char *name)
{
    uint64_t dev_size;

    if (name) {
	printk_debug("can't have a named file\n");
	return 0;
    }

    dev_size = (uint64_t) part_length << 9;
    /* GRUB code doesn't like 2GB or bigger files */
    if (dev_size > 0x7fffffff)
	dev_size = 0x7fffffff;
    filemax = dev_size;
    return 1;
}

static int nullfs_read(char *buf, int len)
{
    if (devread(filepos>>9, filepos&0x1ff, len, buf)) {
	filepos += len;
	return len;
    } else
	return 0;
}

static struct fsys_entry nullfs =
    {"nullfs", 0, nullfs_read, nullfs_dir, 0, 0};

static struct fsys_entry *fsys;

int mount_fs(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(fsys_table); i++) {
	if (fsys_table[i].mount_func()) {
	    fsys = &fsys_table[i];
	    printk_info("Mounted %s\n", fsys->name);
	    return 1;
	}
    }
    fsys = 0;
    printk_info("Unknown filesystem type\n");
    return 0;
}

int file_open(const char *filename)
{
    char *dev = 0;
    const char *path;
    int len;
    int retval = 0;
    int reopen;

    path = strchr(filename, ':');
    if (path) {
	len = path - filename;
	path++;
	dev = malloc(len + 1);
	memcpy(dev, filename, len);
	dev[len] = '\0';
    } else {
	/* No colon is given. Is this device or filename? */
	if (filename[0] == '/') {
	    /* Anything starts with '/' must be a filename */
	    dev = 0;
	    path = filename;
	} else {
	    dev = strdup(filename);
	    path = 0;
	}
    }

    if (dev && dev[0]) {
	if (!devopen(dev, &reopen)) {
	    fsys = 0;
	    goto out;
	}
	if (!reopen)
	    fsys = 0;
    }

    if (path) {
	if (!fsys || fsys==&nullfs) {
	    if (!mount_fs())
		goto out;
	}
	using_devsize = 0;
	if (!path[0]) {
	    printk_info("No filename is given\n");
	    goto out;
	}
    } else
	fsys = &nullfs;

    filepos = 0;
    errnum = 0;
    if (!fsys->dir_func((char *) path)) {
	printk_info("File not found\n");
	goto out;
    }
    retval = 1;
out:
    if (dev)
	free(dev);
    return retval;
}

int file_read(void *buf, unsigned long len)
{
    if (filepos < 0 || filepos > filemax)
	filepos = filemax;
    if (len < 0 || len > filemax-filepos)
	len = filemax - filepos;
    errnum = 0;
    return fsys->read_func(buf, len);
}

int file_seek(unsigned long offset)
{
    filepos = offset;
    return filepos;
}

unsigned long file_pos(void)
{
    return filepos;
}

unsigned long file_size(void)
{
    return filemax;
}

void file_close(void)
{
}

