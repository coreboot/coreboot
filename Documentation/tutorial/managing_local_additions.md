Managing local additions
========================

This section describes the site-local mechanism, what it is good for and
how it can be used.

What is site-local?
-------------------
site-local is the name of a directory that won't ever appear in the
upstream coreboot repository but is referred to in several key places of its
configuration and build system. The intent is provide a single location to
store local modifications.

By keeping local additions to builds in this place, it can be versioned
independently from upstream (e.g. controlled by git in another repository)
and any changes made there won't ever conflict with upstream changes.

This optional directory is searched for in the top-level of the coreboot
repo and is called `site-local`.

Integration into the configuration system
-----------------------------------------
Kconfig includes `site-local/Kconfig` relatively early, so it can be used
to pre-define some configuration before coreboot's regular ruleset sets
up defaults.

Integration into the build system
---------------------------------
The build system includes, if present, `site-local/Makefile.inc`. The main
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
