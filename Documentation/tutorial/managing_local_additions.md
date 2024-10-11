# Managing local additions

This section describes the site-local mechanism, what it is good for and
how it can be used.

## What is site-local?
site-local is the name of a directory that won't ever appear in the
upstream coreboot repository but is referred to in several key places of its
configuration and build system. The intent is provide a single location to
store local modifications.

By keeping local additions to builds in this place, it can be versioned
independently from upstream (e.g. controlled by git in another repository)
and any changes made there won't ever conflict with upstream changes.

This optional directory is searched for in the top-level of the coreboot
repo and is called `site-local`.

A common approach for developing and testing internal additions to coreboot
from the site-local directory is to use `symlink` targets. By replicating
the coreboot directory structure within site-local and creating a
`symlink.txt` file that contains the path (relative to the root of the
coreboot directory), the `symlink` target can recursively scan the
site-local directory and create symbolic links into the coreboot tree,
allowing the build process to proceed as if the additions were integrated
directly into the main coreboot tree. The `symlink.txt` file must be placed
at the root of the new directory.

The following targets can be used to create/remove the symlinks:

`make symlink` - Creates symbolic links from site-local into coreboot tree
`make clean-symlink` - Removes symbolic links created by `make symlink`
`make cleanall-symlink` - Removes all symbolic links in the coreboot tree

### Example symlink usage
Directory structure with symlink from site-local into coreboot:

```
coreboot/
├── src/
│   └── soc/
│       ├── amd/
│       ├── cavium/
│       ├── example/
│       ├── ...
│       └── test-soc-from-site-local -> ../../site-local/src/soc/test-soc-from-site-local/
└── site-local/
    ├── Kconfig
    ├── Makefile.mk
    └── src/
        └── soc/
            └── test-soc-from-site-local/
                └── symlink.txt
```

Contents of `symlink.txt`:

```
src/soc/test-soc-from-site-local
```

*Note:* To keep the symlinks updated throughout development, the following
line may be added to `site-local/Makefile.mk` to declare symlink as a
`site-local-target` dependency, ensuring the symlink target is run anytime
`make` is executed:
```
site-local-target:: symlink
```

## Integration into the configuration system
Kconfig includes `site-local/Kconfig` relatively early, so it can be used
to pre-define some configuration before coreboot's regular ruleset sets
up defaults.

## Integration into the build system
The build system includes, if present, `site-local/Makefile.mk`. The main
purpose so far has been to add additional files to a CBFS image. A single
Makefile.inc can serve multiple boards, for example:

    cbfs-files-$(CONFIG_BOARD_INTEL_D945GCLF) += pci8086,2772.rom
    pci8086,2772.rom-file := intel_d945gclf/pci8086,2772.rom
    pci8086,2772.rom-type := optionrom

    cbfs-files-$(CONFIG_BOARD_KONTRON_986LCD_M) += pci8086,27a2.rom
    pci8086,27a2.rom-file := kontron_986lcd-m/pci8086,27a2.rom
    pci8086,27a2.rom-type := optionrom

This adds the correct Option ROM binary (which are non-redistributable and
therefore can't become part of the coreboot.org repos) to coreboot.rom when
built for intel/d945gclf or kontron/986lcd-m.
