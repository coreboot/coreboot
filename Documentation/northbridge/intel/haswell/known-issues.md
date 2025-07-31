# Known issues with Haswell

These issues are specific to the Haswell architecture. For a given
mainboard, there might be additional issues to those listed here.

## PCIe graphics

Using a PCIe graphics card for display output is not currently
supported. This is because <project:mrc.bin.md> requires workarounds to
have such a feature working correctly.

However, there is a [patch on Gerrit][hsw-gfx-gerrit] that allows PCIe
graphics to be used for display output. This patch is not guaranteed to
be of the same level of quality as code committed to coreboot.

Still, in some cases, a PCIe graphics card can be used for rendering,
while the integrated graphics device is used for display output. This
can be achieved under GNU/Linux by using [PRIME GPU offloading][PRIME].

## PCIe 3.0

Only PCIe 2.0 has been tested so far. PCIe 3.0 could potentially have
stability issues.

[PRIME]: https://wiki.archlinux.org/index.php/PRIME
[hsw-gfx-gerrit]: https://review.coreboot.org/c/30456
