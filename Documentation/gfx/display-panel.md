Display Panel Specifics
=======================

Timing Parameters
-----------------

From the binary file `edid` in the sys filesystem on Linux, the panel can be
identified. The exact path may differ slightly. Here is an example:

```sh
$ strings /sys/devices/pci0000:00/0000:00:02.0/drm/card0/card0-eDP-1/edid
@0 5
LG Display
LP140WF3-SPD1
```

To figure out the timing parameters, refer to the [Intel Programmer's Reference
Manuals](https://01.org/linuxgraphics/documentation/hardware-specification-prms)
and try to find the datasheet of the panel using the information from `edid`.
In the example above, you would search for `LP140WF3-SPD1`. Find a table listing
the power sequence timing parameters, which are usually named T[N] and also
referenced in Intel's respective registers listing. You need the values for
`PP_ON_DELAYS`, `PP_OFF_DELAYS` and `PP_DIVISOR` for your `devicetree.cb`:

```eval_rst
+-----------------------------+---------------------------------------+-----+
| Intel docs                  | devicetree.cb                         | eDP |
+-----------------------------+---------------------------------------+-----+
| Power up delay              | `gpu_panel_power_up_delay`            | T3  |
+-----------------------------+---------------------------------------+-----+
| Power on to backlight on    | `gpu_panel_power_backlight_on_delay`  | T7  |
+-----------------------------+---------------------------------------+-----+
| Power Down delay            | `gpu_panel_power_down_delay`          | T10 |
+-----------------------------+---------------------------------------+-----+
| Backlight off to power down | `gpu_panel_power_backlight_off_delay` | T9  |
+-----------------------------+---------------------------------------+-----+
| Power Cycle Delay           | `gpu_panel_power_cycle_delay`         | T12 |
+-----------------------------+---------------------------------------+-----+
```

Intel GPU Tools and VBT
-----------------------

The Intel GPU tools are in a package called either `intel-gpu-tools` or
`igt-gpu-tools` in most distributions of Linux-based operating systems.
In the coreboot `util/` directory, you can find `intelvbttool`.

From a running system, you can dump the register values directly:
```sh
$ intel_reg dump --all | grep PCH_PP
                      PCH_PP_STATUS (0x000c7200): 0x80000008
                     PCH_PP_CONTROL (0x000c7204): 0x00000007
                   PCH_PP_ON_DELAYS (0x000c7208): 0x07d00001
                  PCH_PP_OFF_DELAYS (0x000c720c): 0x01f40001
                     PCH_PP_DIVISOR (0x000c7210): 0x0004af06
```

You can obtain the timing values from a VBT (Video BIOS Table), which you can
dump from a vendor UEFI image:
```sh
$ intel_vbt_decode data.vbt | grep T3
                Power Sequence: T3 2000 T7 10 T9 2000 T10 500 T12 5000
                T3 optimization: no
```
