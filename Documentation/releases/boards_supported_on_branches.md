# Platforms supported on branches

For one reason or another, platforms have been deleted from the master
branch at times in the past.  Early on, there was no real policy on
removing boards.  Now the policy is that boards will only be removed if
they're causing issues in the tree or if they're preventing progress.

This does not mean that these boards are gone forever.  The release or
commit prior to where they were removed can be checked out, and the
boards can still be built there and updated in a release branch if
desired.

Currently, [jenkins](https://qa.coreboot.org), our continuous
integration system is configured to build the 4.11, 4.12, 4.14, 4.15,
and 4.16 branches.  Builders for other branches can be created on
request.  Likewise, some releases are only marked with tags, and
branches would need to be created to push new code to.  These branches
can also be created on request.

Patches can be backported from the master branch to any of these other
branches as needed. The coreboot project will take care of backporting
critical security fixes, but other patches will need to handled by
anyone using that release.

## [4.16 Release](coreboot-4.16-relnotes.md)
Branch created, builder configured

* No platforms maintained on this release


## [4.15 Release](coreboot-4.15-relnotes.md)
Branch created, builder configured

* No platforms maintained on this release


## [4.14 Release](coreboot-4.14-relnotes.md)
Branch created, builder configured

* No platforms maintained on this release


## [4.13 Release](coreboot-4.13-relnotes.md)
Tag only

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| intel/cannonlake_rvp        | INTEL_CANNONLAKE       | 2017-07-19 | eval     |


## [4.12 Release](coreboot-4.12-relnotes.md)

Branch created, builder configured

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| bap/ode_e21XX               | AMD_PI_00730F01        | 2016-07-30 | eval     |
| lippert/toucan-af           | AMD_FAMILY14           | 2013-03-02 | half     |
| ocp/sonorapass              | INTEL_COOPERLAKE_SP    | 2020-05-01 | server   |


## [4.11 Release](coreboot-4.11-relnotes.md)

Branch created, builder configured

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| adi/rcc-dff                 | INTEL_FSP_RANGELEY     | 2016-06-08 | eval     |
| advansus/a785e-i            | AMD_AMDFAM10           | 2011-05-07 | mini     |
| amd/bettong                 | AMD_PI_00660F01        | 2015-06-23 | eval     |
| amd/bimini_fam10            | AMD_AMDFAM10           | 2011-01-01 | eval     |
| amd/db-ft3b-lc              | AMD_PI_00730F01        | 2016-07-20 | eval     |
| amd/gardenia                | AMD_STONEYRIDGE_FP4    | 2016-12-16 | eval     |
| amd/lamar                   | AMD_PI_00630F01        | 2015-04-23 | eval     |
| amd/mahogany_fam10          | AMD_AMDFAM10           | 2010-03-16 | eval     |
| amd/olivehillplus           | AMD_PI_00730F01        | 2014-09-04 | eval     |
| amd/serengeti_cheetah_fam10 | AMD_AMDFAM10           | 2009-10-09 | server   |
| amd/tilapia_fam10           | AMD_AMDFAM10           | 2010-04-23 | eval     |
| amd/torpedo                 | AMD_FAMILY12           | 2011-06-28 | eval     |
| asus/kcma-d8                | AMD_AMDFAM10           | 2016-02-05 | server   |
| asus/kfsn4-dre              | AMD_AMDFAM10           | 2015-01-28 | server   |
| asus/kgpe-d16               | AMD_AMDFAM10           | 2015-10-28 | server   |
| asus/m4a785-m               | AMD_AMDFAM10           | 2010-09-13 | desktop  |
| asus/m4a785t-m              | AMD_AMDFAM10           | 2011-12-02 | desktop  |
| asus/m4a78-em               | AMD_AMDFAM10           | 2010-12-06 | desktop  |
| asus/m5a88-v                | AMD_AMDFAM10           | 2011-10-28 | desktop  |
| avalue/eax-785e             | AMD_AMDFAM10           | 2011-09-14 | desktop  |
| esd/atom15                  | INTEL_FSP_BAYTRAIL     | 2015-12-04 | sbc      |
| facebook/watson             | INTEL_FSP_BROADWELL_DE | 2018-06-26 | server   |
| gigabyte/ma785gm            | AMD_AMDFAM10           | 2012-04-23 | desktop  |
| gigabyte/ma785gmt           | AMD_AMDFAM10           | 2010-08-17 | desktop  |
| gigabyte/ma78gm             | AMD_AMDFAM10           | 2010-08-17 | desktop  |
| google/urara                | IMGTEC_PISTACHIO       | 2015-03-27 | eval     |
| hp/dl165_g6_fam10           | AMD_AMDFAM10           | 2010-09-24 | server   |
| iei/kino-780am2-fam10       | AMD_AMDFAM10           | 2010-09-13 | half     |
| intel/bayleybay_fsp         | INTEL_FSP_BAYTRAIL     | 2014-05-30 | eval     |
| intel/camelbackmountain_fsp | INTEL_FSP_BROADWELL_DE | 2016-04-15 | eval     |
| intel/littleplains          | INTEL_FSP_RANGELEY     | 2015-11-30 | eval     |
| intel/minnowmax             | INTEL_FSP_BAYTRAIL     | 2014-08-11 | sbc      |
| intel/mohonpeak             | INTEL_FSP_RANGELEY     | 2014-07-30 | eval     |
| jetway/pa78vm5              | AMD_AMDFAM10           | 2010-08-17 | desktop  |
| msi/ms9652_fam10            | AMD_AMDFAM10           | 2010-03-01 | desktop  |
| ocp/monolake                | INTEL_FSP_BROADWELL_DE | 2018-05-05 | server   |
| ocp/wedge100s               | INTEL_FSP_BROADWELL_DE | 2018-05-05 | server   |
| opencellular/rotundu        | INTEL_FSP_BAYTRAIL     | 2018-06-26 | sbc      |
| siemens/mc_bdx1             | INTEL_FSP_BROADWELL_DE | 2016-04-29 | misc     |
| siemens/mc_tcu3             | INTEL_FSP_BAYTRAIL     | 2015-03-05 | misc     |
| siemens/mc_tcu3             | INTEL_FSP_BAYTRAIL_MD  | 2015-03-05 | misc     |
| supermicro/h8dmr_fam10      | AMD_AMDFAM10           | 2009-10-09 | server   |
| supermicro/h8qme_fam10      | AMD_AMDFAM10           | 2010-02-03 | server   |
| supermicro/h8scm_fam10      | AMD_AMDFAM10           | 2011-03-28 | server   |
| tyan/s2912_fam10            | AMD_AMDFAM10           | 2009-10-08 | server   |
| via/epia-m850               | VIA_NANO               | 2013-06-10 | mini     |
| via/epia-m850               | VIA_VX900              | 2013-06-10 | mini     |


## [4.10 Release](coreboot-4.10-relnotes.md)
Branch created

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| cubietech/cubieboard        | ALLWINNER_A10          | 2014-01-08 | sbc      |


## [4.9 Release](coreboot-4.9-relnotes.md)
Tag only

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| pcengines/alix1c            | AMD_GEODE_LX           | 2009-10-08 | half     |
| pcengines/alix1c            | AMD_LX                 | 2009-10-08 | half     |
| pcengines/alix2d            | AMD_GEODE_LX           | 2010-08-31 | half     |
| pcengines/alix2d            | AMD_LX                 | 2010-08-31 | half     |


## [4.8.1 Release](coreboot-4.8.1-relnotes.md)
Branch created

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| aaeon/pfm-540i_revb         | AMD_GEODE_LX           | 2011-06-29 | half     |
| amd/db800                   | AMD_GEODE_LX           | 2009-10-09 | eval     |
| amd/dbm690t                 | AMD_AMDK8              | 2009-10-09 | eval     |
| amd/f2950                   | AMD_GEODE_LX           | 2016-07-17 | mini     |
| amd/mahogany                | AMD_AMDK8              | 2010-03-16 | eval     |
| amd/norwich                 | AMD_GEODE_LX           | 2009-10-09 | eval     |
| amd/pistachio               | AMD_AMDK8              | 2009-10-09 | eval     |
| amd/serengeti_cheetah       | AMD_AMDK8              | 2009-08-12 | server   |
| artecgroup/dbe61            | AMD_GEODE_LX           | 2009-10-08 | settop   |
| asrock/939a785gmh           | AMD_AMDK8              | 2010-04-05 | desktop  |
| asus/a8n_e                  | AMD_AMDK8              | 2009-10-09 | desktop  |
| asus/a8v-e_deluxe           | AMD_AMDK8              | 2010-11-14 | desktop  |
| asus/a8v-e_se               | AMD_AMDK8              | 2009-10-09 | desktop  |
| asus/k8v-x                  | AMD_AMDK8              | 2011-12-02 | desktop  |
| asus/kfsn4-dre_k8           | AMD_AMDK8              | 2015-10-30 | server   |
| asus/m2n-e                  | AMD_AMDK8              | 2010-12-13 | desktop  |
| asus/m2v                    | AMD_AMDK8              | 2010-11-07 | desktop  |
| asus/m2v-mx_se              | AMD_AMDK8              | 2009-08-26 | desktop  |
| bachmann/ot200              | AMD_GEODE_LX           | 2012-07-13 | settop   |
| bcom/winnetp680             | VIA_C7                 | 2009-10-07 | settop   |
| broadcom/blast              | AMD_AMDK8              | 2009-10-09 | eval     |
| digitallogic/msm800sev      | AMD_GEODE_LX           | 2009-10-09 | half     |
| gigabyte/ga_2761gxdk        | AMD_AMDK8              | 2009-10-07 | desktop  |
| gigabyte/m57sli             | AMD_AMDK8              | 2009-10-03 | desktop  |
| google/purin                | BROADCOM_CYGNUS        | 2015-04-17 | eval     |
| google/rotor                | MARVELL_MVMAP2315      | 2016-09-13 | laptop   |
| google/zoombini             | INTEL_CANNONLAKE       | 2017-09-28 | laptop   |
| hp/dl145_g1                 | AMD_AMDK8              | 2010-08-20 | server   |
| hp/dl145_g3                 | AMD_AMDK8              | 2009-10-09 | server   |
| iei/pcisa-lx-800-r10        | AMD_GEODE_LX           | 2009-10-08 | half     |
| iei/pm-lx2-800-r10          | AMD_GEODE_LX           | 2012-10-28 | half     |
| iei/pm-lx-800-r11           | AMD_GEODE_LX           | 2012-07-06 | half     |
| intel/cougar_canyon2        | INTEL_FSP_IVYBRIDGE    | 2013-12-04 | eval     |
| intel/stargo2               | INTEL_FSP_IVYBRIDGE    | 2015-11-10 | eval     |
| iwill/dk8_htx               | AMD_AMDK8              | 2009-10-09 | server   |
| jetway/j7f2                 | VIA_C7                 | 2014-01-19 | mini     |
| kontron/kt690               | AMD_AMDK8              | 2009-10-15 | mini     |
| lippert/hurricane-lx        | AMD_GEODE_LX           | 2010-09-10 | half     |
| lippert/literunner-lx       | AMD_GEODE_LX           | 2010-09-07 | half     |
| lippert/roadrunner-lx       | AMD_GEODE_LX           | 2009-10-08 | half     |
| lippert/spacerunner-lx      | AMD_GEODE_LX           | 2009-10-08 | half     |
| lowrisc/nexys4ddr           | LOWRISC_LOWRISC        | 2016-10-28 | eval     |
| msi/ms7135                  | AMD_AMDK8              | 2009-10-07 | desktop  |
| msi/ms7260                  | AMD_AMDK8              | 2009-10-07 | desktop  |
| msi/ms9185                  | AMD_AMDK8              | 2009-10-07 | server   |
| msi/ms9282                  | AMD_AMDK8              | 2009-10-07 | server   |
| nvidia/l1_2pvv              | AMD_AMDK8              | 2009-10-07 | eval     |
| siemens/sitemp_g1p1         | AMD_AMDK8              | 2011-05-11 | half     |
| sunw/ultra40                | AMD_AMDK8              | 2009-09-25 | desktop  |
| sunw/ultra40m2              | AMD_AMDK8              | 2015-11-10 | desktop  |
| supermicro/h8dme            | AMD_AMDK8              | 2009-09-25 | server   |
| supermicro/h8dmr            | AMD_AMDK8              | 2009-10-09 | server   |
| technexion/tim5690          | AMD_AMDK8              | 2009-10-13 | half     |
| technexion/tim8690          | AMD_AMDK8              | 2009-10-08 | half     |
| traverse/geos               | AMD_GEODE_LX           | 2010-05-20 | half     |
| tyan/s2912                  | AMD_AMDK8              | 2009-10-08 | server   |
| via/epia-cn                 | VIA_C7                 | 2009-09-25 | mini     |
| via/epia-m700               | VIA_C7                 | 2009-09-25 | mini     |
| via/pc2500e                 | VIA_C7                 | 2009-09-25 | mini     |
| via/vt8454c                 | VIA_C7                 | 2009-08-20 | eval     |
| winent/mb6047               | AMD_AMDK8              | 2013-10-19 | half     |
| winent/pl6064               | AMD_GEODE_LX           | 2010-02-24 | desktop  |
| winnet/g170                 | VIA_C7                 | 2017-08-28 | mini     |


## [4.7 Release](coreboot-4.7-relnotes.md)
Tag only

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| abit/be6-ii_v2_0            | INTEL_I440BX           | 2009-08-26 | desktop  |
| amd/dinar                   | AMD_FAMILY15           | 2012-02-17 | eval     |
| amd/rumba                   | AMD_GEODE_GX2          | 2009-08-29 | half     |
| asus/dsbf                   | INTEL_I5000            | 2012-07-14 | server   |
| asus/mew-am                 | INTEL_I82810           | 2009-08-28 | desktop  |
| asus/mew-vm                 | INTEL_I82810           | 2009-08-28 | desktop  |
| a-trend/atc-6220            | INTEL_I440BX           | 2009-08-26 | desktop  |
| a-trend/atc-6240            | INTEL_I440BX           | 2009-08-26 | desktop  |
| azza/pt-6ibd                | INTEL_I440BX           | 2009-08-26 | desktop  |
| biostar/m6tba               | INTEL_I440BX           | 2009-08-26 | desktop  |
| compaq/deskpro_en_sff_p600  | INTEL_I440BX           | 2009-08-26 | desktop  |
| dmp/vortex86ex              | DMP_VORTEX86EX         | 2013-07-05 | sbc      |
| ecs/p6iwp-fe                | INTEL_I82810           | 2010-06-09 | desktop  |
| gigabyte/ga-6bxc            | INTEL_I440BX           | 2009-08-26 | desktop  |
| gigabyte/ga-6bxe            | INTEL_I440BX           | 2010-05-14 | desktop  |
| hp/e_vectra_p2706t          | INTEL_I82810           | 2009-10-20 | desktop  |
| intel/d810e2cb              | INTEL_I82810           | 2010-06-21 | desktop  |
| intel/eagleheights          | INTEL_I3100            | 2009-09-25 | eval     |
| intel/mtarvon               | INTEL_I3100            | 2009-09-25 | eval     |
| intel/truxton               | INTEL_I3100            | 2009-09-25 | eval     |
| iwave/iWRainbowG6           | INTEL_SCH              | 2010-12-18 | half     |
| lanner/em8510               | INTEL_I855             | 2010-08-30 | desktop  |
| lippert/frontrunner         | AMD_GEODE_GX2          | 2009-10-08 | half     |
| mitac/6513wu                | INTEL_I82810           | 2009-08-28 | desktop  |
| msi/ms6119                  | INTEL_I440BX           | 2009-08-26 | desktop  |
| msi/ms6147                  | INTEL_I440BX           | 2009-08-26 | desktop  |
| msi/ms6156                  | INTEL_I440BX           | 2009-10-13 | desktop  |
| msi/ms6178                  | INTEL_I82810           | 2009-08-28 | desktop  |
| nec/powermate2000           | INTEL_I82810           | 2009-08-28 | desktop  |
| nokia/ip530                 | INTEL_I440BX           | 2010-04-19 | server   |
| rca/rm4100                  | INTEL_I82830           | 2009-10-07 | settop   |
| soyo/sy-6ba-plus-iii        | INTEL_I440BX           | 2009-08-26 | desktop  |
| supermicro/h8qgi            | AMD_FAMILY15           | 2011-07-22 | server   |
| supermicro/h8scm            | AMD_FAMILY15           | 2012-11-30 | server   |
| supermicro/x7db8            | INTEL_I5000            | 2012-06-23 | server   |
| thomson/ip1000              | INTEL_I82830           | 2009-10-08 | settop   |
| tyan/s1846                  | INTEL_I440BX           | 2009-08-26 | desktop  |
| tyan/s8226                  | AMD_FAMILY15           | 2012-10-04 | server   |
| wyse/s50                    | AMD_GEODE_GX2          | 2010-05-08 | settop   |


## [4.6](coreboot-4.6-relnotes.md)
Tag only

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| bifferos/bifferboard        | RDC_R8610              | 2012-03-27 | half     |
| google/cosmos               | MARVELL_BG4CD          | 2015-04-09 | eval     |
| intel/bakersport_fsp        | INTEL_FSP_BAYTRAIL     | 2014-08-11 | eval     |


## [4.5](coreboot-4.5-relnotes.md)
Tag only

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| google/enguarde             | INTEL_BAYTRAIL         | 2016-09-21 | laptop   |
| google/falco                | INTEL_HASWELL          | 2013-11-25 | laptop   |
| google/guado                | INTEL_BROADWELL        | 2016-01-12 | half     |
| google/ninja                | INTEL_BAYTRAIL         | 2016-05-31 | half     |
| google/panther              | INTEL_HASWELL          | 2014-07-12 | half     |
| google/peppy                | INTEL_HASWELL          | 2013-11-25 | laptop   |
| google/rikku                | INTEL_BROADWELL        | 2016-06-16 | half     |
| google/samus                | INTEL_BROADWELL        | 2014-08-29 | laptop   |
| google/tidus                | INTEL_BROADWELL        | 2016-01-21 | half     |


## [4.4](coreboot-4.4-relnotes.md)
Branch created

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| google/bolt                 | INTEL_HASWELL          | 2013-12-12 | eval     |
| google/rush                 | NVIDIA_TEGRA132        | 2015-01-26 | eval     |
| google/rush_ryu             | NVIDIA_TEGRA132        | 2015-03-05 | eval     |
| google/slippy               | INTEL_HASWELL          | 2013-11-24 | eval     |
| intel/amenia                | INTEL_APOLLOLAKE       | 2016-04-20 | eval     |


## [4.3](coreboot-4.3-relnotes.md)
Branch created

* No platforms maintained on this release


## [4.2](coreboot-4.2-relnotes.md)
Branch created

| Vendor/Board                | Processor              | Date added | Brd type |
|-----------------------------|------------------------|------------|----------|
| arima/hdama                 | AMD_AMDK8              | 2009-10-09 | server   |
| digitallogic/adl855pc       | INTEL_I855             | 2009-10-09 | half     |
| ibm/e325                    | AMD_AMDK8              | 2009-10-09 | server   |
| ibm/e326                    | AMD_AMDK8              | 2009-10-09 | server   |
| intel/sklrvp                | INTEL_SKYLAKE          | 2015-07-17 | eval     |
| iwill/dk8s2                 | AMD_AMDK8              | 2009-10-09 | server   |
| iwill/dk8x                  | AMD_AMDK8              | 2009-10-09 | server   |
| newisys/khepri              | AMD_AMDK8              | 2009-10-07 | server   |
| tyan/s2735                  | INTEL_E7501            | 2009-10-08 | server   |
| tyan/s2850                  | AMD_AMDK8              | 2009-09-25 | server   |
| tyan/s2875                  | AMD_AMDK8              | 2009-09-25 | desktop  |
| tyan/s2880                  | AMD_AMDK8              | 2009-10-08 | server   |
| tyan/s2881                  | AMD_AMDK8              | 2009-09-23 | server   |
| tyan/s2882                  | AMD_AMDK8              | 2009-10-08 | server   |
| tyan/s2885                  | AMD_AMDK8              | 2009-10-08 | desktop  |
| tyan/s2891                  | AMD_AMDK8              | 2009-09-22 | server   |
| tyan/s2892                  | AMD_AMDK8              | 2009-09-22 | server   |
| tyan/s2895                  | AMD_AMDK8              | 2009-09-22 | desktop  |
| tyan/s4880                  | AMD_AMDK8              | 2009-10-08 | server   |
| tyan/s4882                  | AMD_AMDK8              | 2009-10-08 | server   |


## [4.1](coreboot-4.1-relnotes.md)
Branch Created

* No platforms maintained on this release
