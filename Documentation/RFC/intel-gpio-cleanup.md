# Background

CB:31250 ("soc/intel/cannonlake: Configure GPIOs again after FSP-S is
done") introduced a workaround in coreboot for `soc/intel/cannonlake`
platforms to save and restore GPIO configuration performed by
mainboard across call to FSP Silicon Init (FSP-S). This workaround was
required because FSP-S was configuring GPIOs differently than
mainboard resulting in boot and runtime issues because of
misconfigured GPIOs. This issue was observed on `google/hatch`
mainboard and was raised with Intel to get the FSP behavior
fixed. Until the fix in FSP was available, this workaround was used to
ensure that the mainboards can operate correctly and were not impacted
by the GPIO misconfiguration in FSP-S.

The issues observed on `google/hatch` mainboard were fixed by adding
(if required) and initializing appropriate FSP UPDs. This UPD
initialization ensured that FSP did not configure any GPIOs
differently than the mainboard configuration. Fixes included:
 * CB:31375 ("soc/intel/cannonlake: Configure serial debug uart")
 * CB:31520 ("soc/intel/cannonlake: Assign FSP UPDs for HPD and Data/CLK of DDI ports")
 * CB:32176 ("mb/google/hatch: Update GPIO settings for SD card and SPI1 Chip select")
 * CB:34900 ("soc/intel/cnl: Add provision to configure SD controller write protect pin")

With the above changes merged, it was verified on `google/hatch`
mainboard that the workaround for GPIO reconfiguration was not
needed. However, at the time, we missed dropping the workaround in
'soc/intel/cannonlake`. Currently, this workaround is used by the
following mainboards:
 * `google/drallion`
 * `google/sarien`
 * `purism/librem_cnl`
 * `system76/lemp9`

As verified on `google/hatch`, FSP v1263 included all UPD additions
that were required for addressing this issue.

# Proposal

* The workaround can be safely dropped from `soc/intel/cannonlake`
  only after the above mainboards have verified that FSP-S does not
  configure any pads differently than the mainboard in coreboot. Since
  the fix included initialization of FSP UPDs correctly, the above
  mainboards can use the following diff to check what pads change
  after FSP-S has run:

```
diff --git a/src/soc/intel/common/block/gpio/gpio.c b/src/soc/intel/common/block/gpio/gpio.c
index 28e78fb366..0cce41b316 100644
--- a/src/soc/intel/common/block/gpio/gpio.c
+++ b/src/soc/intel/common/block/gpio/gpio.c
@@ -303,10 +303,10 @@ static void gpio_configure_pad(const struct pad_config *cfg)
                /* Patch GPIO settings for SoC specifically */
                soc_pad_conf = soc_gpio_pad_config_fixup(cfg, i, soc_pad_conf);

-               if (CONFIG(DEBUG_GPIO))
+               if (soc_pad_conf != pad_conf)
                        printk(BIOS_DEBUG,
-                       "gpio_padcfg [0x%02x, %02zd] DW%d [0x%08x : 0x%08x"
-                       " : 0x%08x]\n",
+                       "%d: gpio_padcfg [0x%02x, %02zd] DW%d [0x%08x : 0x%08x"
+                       " : 0x%08x]\n", cfg->pad,
                        comm->port, relative_pad_in_comm(comm, cfg->pad), i,
                        pad_conf,/* old value */
                        cfg->pad_config[i],/* value passed from gpio table */
```

Depending upon the pads that are misconfigured by FSP-S, these
mainboards will have to set UPDs appropriately. Once this is verified
by the above mainboards, the workaround implemented in CB:31250 can be
dropped.

* The fix implemented in FSP/coreboot for `soc/intel/cannonlake`
  platforms is not really the right long term solution for the
  problem. Ideally, FSP should not be touching any GPIO configuration
  and letting coreboot configure the pads as per mainboard
  design. This recommendation was accepted and implemented by Intel
  starting with Jasper Lake and Tiger Lake platforms using a single
  UPD `GpioOverride` that coreboot can set so that FSP does not change
  any GPIO configuration. However, this implementation is not
  backported to any older platforms. Given the issues that we have
  observed across different platforms, the second proposal is to:

  - Add a Kconfig `CHECK_GPIO_CONFIG_CHANGES` that enables checks
    in coreboot to stash GPIO pad configuration before various calls
    to FSP and compares the configuration on return from FSP.
  - This will have to be implemented as part of
    drivers/intel/fsp/fsp2_0/ to check for the above config selection
    and make callbacks `gpio_snapshot()` and `gpio_verify_snapshot()`
    to identify and print information about pads that have changed
    configuration after calls to FSP.
  - This config can be kept disabled by default and mainboard
    developers can enable them as and when required for debug.
  - This will be helpful not just for the `soc/intel/cannonlake`
    platforms that want to get rid of the above workaround, but also
    for all future platforms using FSP to identify and catch any GPIO
    misconfigurations that might slip in to any platforms (in case the
    `GpioOverride` UPD is not honored by any code path within FSP).

