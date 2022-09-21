# Intel DPTF implementations in coreboot

## Introduction

Intel Dynamic Platform and Thermal Framework is a framework that can be used to
help regulate the thermal properties (i.e., temperature) of an Intel-based
computer. It does this by allowing the system designer to specify the different
components that can generate heat, and/or dissipate heat. Under DPTF, the
different components are referred to as `participants`. The different types of
functionality available in DPTF are specified in terms of different `policies`.

## Components ("Participants")

The participants that can be involved in the current implementation are:
- CPU (monolithic from a DPTF point-of-view)
  - Note that the CPU's internal temperature sensor is used here
- 1 fan
- Up to 4 temperature sensors (TSRs)
- Battery charger

## Policies

In the current implementation, there are 3 different policies available:

### Passive Policy

The purpose of this policy is to monitor participant temperatures and is capable
of controlling performance and throttling available on platform devices in order
to regulate the temperatures of each participant. The temperature threshold
points are defined by a `_PSV` ACPI object within each participant.

### Critical Policy

The Critical Policy is used for gracefully suspending or powering off the system
when the temperature of participants exceeds critical threshold
temperatures. Suspend is effected by specifying temperatures in a `_CRT` object
for a participant, and poweroff is effected by specifying a temperature
threshold in a `_HOT` ACPI object.

### Active Policy

This policy monitors the temperature of participants and controls fans to spin
at varying speeds. These speeds are defined by the platform, and will be enabled
depending on the various temperatures reported by participants.

## Note about units

ACPI uses unusual units for specifying various physical measurements. For
example, temperatures are specified in 10ths of a degree K, and time is measured
in tenths of a second. Those oddities are abstracted away in the DPTF library,
by using degrees C for temperature, milliseconds for time, mW for power, and mA
for current.

## Differences from the static ASL files (soc/intel/common/acpi/dptf/*.asl)

1) TCPU had many redundant methods. The many references to \_SB.CP00._* are not
created anymore in recent SoCs and the ACPI spec says these are optional objects
anyway. The defaults that were returned by these methods were redundant (all
data was a 0). The following Methods were removed:

* _TSS
* _TPC
* _PTC
* _TSD
* _TDL
* _PSS
* _PDL

2) There is no more implicit inclusion of _ACn methods for TCPU (these must be
   specified in the devicetree entries or by calling the DPTF acpigen API).

## ACPI Tables

DPTF relies on an assortment of ACPI tables to provide parameters to the DPTF
application. We will discuss the more important ones here.

1) _TRT - Thermal Relationship Table

This table is used when the Passive Policy is enabled, and is used to represent
the thermal relationships in the system that can be controlled passively (i.e.,
by throttling participants). A passive policy is defined by a Source (which
generates heat), a Target (typically a temperature sensor), a Sampling Period
(how often to check the temperature), an activation temperature threshold (for
when to begin throttling), and a relative priority.

2) _ART - Active Relationship Table

This table is used when the Active Policy is enabled, and is used to represent
active cooling relationships (i.e., which TSRs the fan can cool). An active
policy contains a Target (the device the fan can cool), a Weight to control
which participant needs more attention than others, and a list of temperature /
fan percentage pairs. The list of pairs defines the fan control percentage that
should be applied when the TSR reaches each successive threshold (_AC0 is the
highest threshold, and represents the highest fan control percentage).

3) PPCC - Participant Power Control Capabilities

This table is used to describe parameters for controlling the SoC's Running
Average Power Limits (RAPL, see below).

4) _FPS - Fan Performance States

This table describes the various fan speeds available for DPTF to use, along with
various informational properties.

5) PPSS - Participant Performance Supported States

This table describes performance states supported by a participant (typically
the battery charger).

## ACPI Methods

The Active and Passive policies also provide for short Methods to define
different kinds of temperature thresholds.

1) _AC0, _AC1, _AC2, _AC3, ..., _AC9

These Methods can provide up to 10 temperature thresholds. What these do is set
temperatures which act as the thresholds to active rows (fan speeds) in the
ART. _AC0 is intended to be the highest temperature thresholds, and the lowest
one can be any of them; leave the rest defined as 0 and they will be omitted
from the output.

These are optional and are enabled by selecting the Active Policy.

2) _PSV

_PSV is a temperature threshold that is used to indicate to DPTF that it should
begin taking passive measures (i.e., throttling of the Source) in order to
reduce the temperature of the Target in question. It will check on the
temperature according to the given sampling period.

This is optional and is enabled by selecting the Passive Policy.

3) _CRT and _HOT

When the temperature of the Source reaches the threshold specified in _CRT, then
the system is supposed to execute a "graceful suspend". Similarly, when the Source
reaches the temperature specified in _HOT, then the system is supposed to execute
a "graceful shutdown".

These are optional, and are enabled by selecting the Critical Policy.

## How to use the devicetree entries

The `drivers/intel/dptf` chip driver is organized into several sections:
- Policies
- Controls
- Options

The Policies section (`policies.active`, `policies.passive`, and
`policies.critical`) is where the components of each policy are defined.

### Active Policy

Each Active Policy is defined in terms of 4 parts:
1) A Source (this is implicitly defined as TFN1, the system fan)
2) A Target (this is the device that can be affected by the policy, i.e.,
   this is a device that can be cooled by the fan)
3) A 'Weight', which is defined as the Source's contribution to the Target's
   cooling capability (as a percentage, 0-100, often just left at 100).
4) A list of temperature-fan percentage pairs, which define temperature
   thresholds that, when the Target reaches, the fan is defined to spin
   at the corresponding percentage of full duty cycle.

An example definition in the devicetree:
```C
register "policies.active[0]" = "{
    .target=DPTF_CPU,
    .weight=100,
    .thresholds={TEMP_PCT(85, 90),
                 TEMP_PCT(80, 69),
                 TEMP_PCT(75, 56),
                 TEMP_PCT(70, 46),
                 TEMP_PCT(65, 36),}}"
```

This example sets up a policy wherein the CPU temperature sensor can be cooled
by the fan. The 'weight' of this policy is 100% (this policy contributes 100% of
the CPU's active cooling capability). When the CPU temperature first crosses
65C, the fan is defined to spin at 36% of its duty cycle, and so forth up the
rest of the table (note that it *must* be defined from highest temperature/
percentage on down to the lowest).

### Passive Policy

Each Passive Policy is defined in terms of 5 parts:
1) Source - The device that can be throttled
2) Target - The device that controls the amount of throttling
3) Period - How often to check the temperature of the Target
4) Trip point - What temperature threshold to start throttling
5) Priority - A number indicating the relative priority between different
   Policies

An example definition in the devicetree:
```C
register "policies.passive[0]" = "DPTF_PASSIVE(CHARGER, TEMP_SENSOR_1, 65, 60000)"
```

This example sets up a policy to begin throttling the charger performance when
temperature sensor 1 reaches 65C. The sampling period here is 60000 ms (60 s).
The Priority is defaulted to 100 in this case.

### Critical Policy

Each Critical Policy is defined in terms of 3 parts:
1) Source - A device that can trigger a critical event
2) Type - What type of critical event to trigger (S4-entry or shutdown)
3) Temperature - The temperature threshold that will cause the entry into S4 or
   to shutdown the system.

An example definition in the devicetree:

```C
register "policies.critical[1]" = "DPTF_CRITICAL(CPU, 75, SHUTDOWN)"
```

This example sets up a policy wherein ACPI will cause the system to shutdown
(in a "graceful" manner) when the CPU temperature reaches 75C.

### Power Limits

Control over the SoC's Running Average Power Limits (RAPL) is one of the tools
that DPTF uses to enact Passive policies. DPTF can control both PL1 and PL2, if
the PPCC table is provided for the TCPU object. Each power limit is given the
following options:
1) Minimum power (in mW)
2) Maximum power (in mW)
3) Minimum time window (in ms)
4) Maximum time window (in ms)
5) Granularity, or minimum step size to control limits (in mW)

An example:
```C
register "controls.power_limits.pl1" = "{
        .min_power = 3000,
        .max_power = 15000,
        .time_window_min = 28 * MSECS_PER_SEC,
        .time_window_max = 32 * MSECS_PER_SEC,
        .granularity = 200,}"
```

This example allow DPTF to control the SoC's PL1 level to between 3W and 15W,
over a time interval ranging from 28 to 32 seconds, and it can move PL1 in
increments of 200 mW.

### Charger Performance

The battery charger can be a large contributor of unwanted heat in a system that
has one. Controlling the rate of charging is another tool that DPTF uses to enact
Passive Policies. Each entry in the PPSS table consists of:
1) A 'Control' value - an opaque value that the platform firmware uses
   to initiate a transition to the specified performance state. DPTF will call an
   ACPI method called `TCHG.SPPC` (Set Participant Performance Capability) if
   applicable, and will pass this opaque control value as its argument.
2) The intended charging rate (in mA).

Example:
```C
register "controls.charger_perf[0]" = "{ 255, 1700 }"
register "controls.charger_perf[1]" = "{  24, 1500 }"
register "controls.charger_perf[2]" = "{  16, 1000 }"
register "controls.charger_perf[3]" = "{   8,  500 }"
```

In this example, when DPTF decides to throttle the charger, it has four different
performance states to choose from.

### Fan Performance

When using DPTF, the system fan (`TFN1`) is the device responsible for actively
cooling the other temperature sensors on the mainboard. A fan speed table can be
provided to DPTF to assist with fan control. Each entry holds the following:
1) Percentage of full duty to spin the fan at
2) Speed - Speed of the fan at that percentage; informational only, but given in
   RPM
3) Noise - Amount of noise created by the fan at that percentage; informational
   only, but given in tenths of a decibel (centibel).
4) Power - Amount of power consumed by the fan at that percentage; informational
   only, but given in mA.

Example:
```C
register "controls.fan_perf[0]" = "{  90, 6700, 220, 2200, }"
register "controls.fan_perf[1]" = "{  80, 5800, 180, 1800, }"
register "controls.fan_perf[2]" = "{  70, 5000, 145, 1450, }"
register "controls.fan_perf[3]" = "{  60, 4900, 115, 1150, }"
register "controls.fan_perf[4]" = "{  50, 3838,  90,  900, }"
register "controls.fan_perf[5]" = "{  40, 2904,  55,  550, }"
register "controls.fan_perf[6]" = "{  30, 2337,  30,  300, }"
register "controls.fan_perf[7]" = "{  20, 1608,  15,  150, }"
register "controls.fan_perf[8]" = "{  10,  800,  10,  100, }"
register "controls.fan_perf[9]" = "{   0,    0,   0,   50, }"
```

In this example, the fan has 10 different performance states, each in an even
increment of 10 percentage points. This is common when specifying fine-grained
control of the fan, wherein DPTF will interpolate between the percentages in the
table for a given temperature threshold.

### Options

#### Fan
1) Fine-grained control - a boolean (see Fan Performance section above)
2) Step-size - Recommended minimum step size (in percentage points) to adjust
   the fan speed when using fine-grained control (ranges from 1 - 9).
3) Low-speed notify - If true, the platform will issue a `Notify (0x80)` to the
   fan device if a low fan speed is detected.

#### Temperature sensors
1) Hysteresis - The amount of hysteresis implemented in either circuitry or
   the firmware that reads the temperature sensor (in degrees C).
2) Name - This name is applied to the _STR property of the sensor

### OEM Variables
Platform vendors can define an array of OEM-specific values as OEM variables
to be used under DPTF policy. There are total six OEM variables available.
These can be used in AP policy for more specific actions. These OEM variables
can be defined as below mentioned example and can be used any variable between
[0], [1],...,[5]. Platform vendors can enable and use this for specific platform
by defining OEM variables macro under board variant.

Example:
```C
register "oem_data.oem_variables" = "{
   [1] = 0x6,
   [3] = 0x1
}"
```
