Upcoming release - coreboot 4.14
================================

The 4.14 release is planned for May 2021.

Update this document with changes that should be in the release notes.

* Please use Markdown.
* See the past few release notes for the general format.
* The chip and board additions and removals will be updated right
  before the release, so those do not need to be added.

Deprecations and incompatible changes
-------------------------------------

### SAR support in VPD for Chrome OS

SAR support in VPD has been deprecated for Chrome OS platforms for > 1
year now. All new Chrome OS platforms have switched to using SAR
tables from CBFS. For the next release, coreboot is updated to align
with the Chrome OS factory changes and hence SAR support in VPD is
deprecated in [CB:51483](https://review.coreboot.org/51483). Starting
with this release, anyone building coreboot for an already released
Chrome OS platform with SAR table in VPD will have to extract the
"wifi_sar" key from VPD and add it as a file to CBFS using following
steps:
 * On DUT, read SAR value using `vpd -i RO_VPD -g wifi_sar`
 * In coreboot repo, generate CBFS SAR file using:
    `echo ${SAR_STRING} > site-local/${BOARD}-sar.hex`
 * Add to site-local/Kconfig:
    ```
     config WIFI_SAR_CBFS_FILEPATH
       string
       default "site-local/${BOARD}-sar.hex"
    ```

### CBFS stage file format change

[CB:46484](https://review.coreboot.org/46484) changed the in-flash
file format of coreboot stages to prepare for per-file signature
verification. As described in the commit message in more details,
when manipulating stages in a CBFS, the cbfstool build must match the
coreboot image so that they're using the same format: coreboot.rom
and cbfstool must be built from coreboot sources that either both
contain this change or both do not contain this change.

Since stages are usually only handled by the coreboot build system
which builds its own cbfstool (and therefore it always matches
coreboot.rom) this shouldn't be a concern in the vast majority of
scenarios.

Significant changes
-------------------

### Add significant changes here
