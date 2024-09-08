coreboot 4.10 release notes
===========================

The 4.10 release covers commit a2faaa9a2 to commit ae317695e3
There is a pgp signed 4.10 tag in the git repository, and a branch will
be created as needed.

In nearly 8 months since 4.9 we had 198 authors commit 2538 changes
to master.  Of these, 85 authors made their first commit to coreboot:
Welcome!

Between the releases the tree grew by about 11000 lines of code plus
5000 lines of comments.

Again, a big Thank You to all contributors who helped shape the coreboot
project, community and code with their effort, no matter if through
development, review, testing, documentation or by helping people asking
questions on our venues like IRC or our mailing list.

What's New
----------

Most of the changes were to mainboards, and on the chipset side, lots
of activity concentrated on x86. However compared to previous releases
activity (and therefore interest, probably) increased in vboot and in
non-x86 architectures. However it's harder this time to give this release
a single topic like the last: This release accumulates some of everything.

Clean Up
--------
As usual, there was a lot of cleaning up going on, and there notably,
a good chunk of this year's Google Summer of Code project to clean out
the issues reported by Coverity Scan is already in.

The only larger scale change that was registered in the pre-release
notes was also about cleaning up the tree:

### `device_t` is no more
coreboot used to have a data type, `device_t` that changed shape depending on
whether it is compiled for romstage (with limited memory) or ramstage (with
unlimited memory as far as coreboot is concerned). It's an old relic from the
time when romstage wasn't operated in Cache-As-RAM mode, but compiled with
our romcc compiler.

That data type is now gone.

Release Notes maintenance
-------------------------
Speaking of pre-release notes: After 4.10 we'll start a document for
4.11 in the git repository. Feel free to add notable achievements there
so we remember to give them a shout out in the next release's notes.

Known Issues
------------
Sadly, Google Cyan is broken in this release. It doesn't work with the
"C environment" bootblock (as compared to the old romcc type bootblock)
which is now the default. Sadly it doesn't help to simply revert that
change because doing so breaks other boards.

If you want to use Google Cyan with the release (or if
you're tracking the master branch), please keep an eye on
<https://review.coreboot.org/c/coreboot/+/34304> where a solution for this
issue is sought.

Deprecations
------------
As announced in the 4.9 release notes, there are no deprecations after 4.10.
While 4.10 is also released late and we target a 4.11 release in October we
nonetheless want to announce deprecations this time: These are under
discussion since January, people are working on mitigations for about as long
and so it should be possible to resolve the outstanding issues by the end of
October.

Specifically, we want to require code to work with the following Kconfig
options so we can remove the options and the code they disable:

* C\_ENVIRONMENT\_BOOTBLOCK
* NO\_CAR\_GLOBAL\_MIGRATION
* RELOCATABLE\_RAMSTAGE

These only affect x86. If your platform only works without them, please
look into fixing that.

Added 28 mainboards:
--------------------
* ASROCK H110M-DVS
* ASUS H61M-CS
* ASUS P5G41T-M-LX
* ASUS P5QPL-AM
* ASUS P8Z77-M-PRO
* FACEBOOK FBG1701
* FOXCONN G41M
* GIGABYTE GA-H61MA-D3V
* GOOGLE BLOOG
* GOOGLE FLAPJACK
* GOOGLE GARG
* GOOGLE HATCH-WHL
* GOOGLE HELIOS
* GOOGLE KINDRED
* GOOGLE KODAMA
* GOOGLE KOHAKU
* GOOGLE KRANE
* GOOGLE MISTRAL
* HP COMPAQ-8200-ELITE-SFF-PC
* INTEL COMETLAKE-RVP
* INTEL KBLRVP11
* LENOVO R500
* LENOVO X1
* MSI MS7707
* PORTWELL M107
* PURISM LIBREM13-V4
* PURISM LIBREM15-V4
* SUPERMICRO X10SLM-PLUS-F
* UP SQUARED

Removed 7 mainboards:
---------------------
* GOOGLE BIP
* GOOGLE DELAN
* GOOGLE ROWAN
* PCENGINES ALIX1C
* PCENGINES ALIX2C
* PCENGINES ALIX2D
* PCENGINES ALIX6

Removed 3 processors:
---------------------
* src/cpu/amd/geode\_lx
* src/cpu/intel/model\_69x
* src/cpu/intel/model\_6dx

Added 2 socs:
-------------
* src/soc/amd/picasso
* src/soc/qualcomm/qcs405

Toolchain
---------
* Update to gcc 8.3.0, binutils 2.32, IASL 20190509, clang 8
