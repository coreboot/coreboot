# ACPI Active Cooling with Five-Level Fan Control

## Overview

This document describes an ACPI-based thermal management pattern used across multiple coreboot mainboards. The implementation uses ACPI thermal zones with active cooling policies to control fan speed based on CPU temperature through a five-level power resource state machine.

This pattern is particularly prevalent on Intel-based mainboards using SuperIO environmental controllers for fan PWM control.

## Mainboards Using This Pattern

The following mainboards implement this five-level ACPI fan control pattern:

### Google Chromebooks
- **google/beltino** - Haswell Chromebox
  - All variants (mccloud, monroe, panther, tricky, zako) use a single implementation
- **google/jecht** - Broadwell Chromebox
  - Each variant (jecht, rikku, guado, tidus) has a unique implementation

### Samsung
- **samsung/stumpy** - Sandy Bridge Chromebox

### Intel Reference Boards
- **intel/wtm2** - Haswell ULT reference board
- **intel/baskingridge** - Haswell desktop reference board
- **intel/emeraldlake2** - Ivy Bridge reference board

## Architecture

### Hardware Components

Common hardware elements across implementations:

- **Temperature Sensor**: SuperIO TMPIN3 reads CPU temperature via PECI (Platform Environment Control Interface)
- **Fan Controller**: SuperIO Environmental Controller (ENVC) with PWM output
  - Fan2 (F2PS) on Google Chromebooks
  - Fan3 (F3PS) on Intel/Samsung boards
- **CPU**: Provides temperature data as an offset from Tj_max (maximum junction temperature)

### ACPI Components

- **Thermal Zone**: `\_TZ.THRM` - Main thermal management zone
- **Fan Devices**: `FAN0` through `FAN4` - Five fan speed levels
- **Power Resources**: `FNP0` through `FNP4` - Control fan state transitions
- **Active Cooling Levels**: `_AC0` through `_AC4` - Temperature thresholds for each fan level

## Fan Speed Levels

The system implements **5 fan speed levels** (0-4), where:

- **Level 0**: Maximum fan speed (highest cooling, activated at highest temperature)
- **Level 1**: High fan speed
- **Level 2**: Medium fan speed
- **Level 3**: Low fan speed
- **Level 4**: Minimum fan speed (idle/baseline cooling, default state)

The system starts at **Level 4** (minimum speed) and increases to lower-numbered levels as temperature rises.

## Temperature Management

### Temperature Reading Process

1. **Raw PECI Reading**: Read from `\_SB.PCI0.LPCB.SIO.ENVC.TIN3`
   - Returns a value representing offset from Tj_max
   - Value 0x80 indicates "no reading available"
   - Values 0 or 255 are invalid

2. **Temperature Calculation**:
   ```
   actual_temperature = Tj_max - (255 - raw_value)
   ```

3. **Conversion to ACPI Format**: Convert from Celsius to deci-Kelvin:
   ```
   deci_kelvin = (celsius * 10) + 2732
   ```

### Critical Temperature Handling

Most implementations include safety logic in the `_TMP` method:
- If temperature reaches `\TMAX` (Tj_max), logs a critical event
- Waits 1 second for sensor re-poll
- Re-reads temperature to confirm
- Reports the current temperature to the OS

### Temperature Thresholds

Thresholds are defined in board-specific headers (typically `thermal.h`):

- `FAN0_THRESHOLD_ON/OFF` - Trigger points for maximum fan speed
- `FAN1_THRESHOLD_ON/OFF` - Trigger points for high fan speed
- `FAN2_THRESHOLD_ON/OFF` - Trigger points for medium fan speed
- `FAN3_THRESHOLD_ON/OFF` - Trigger points for low fan speed
- `FAN4_PWM` - PWM value for minimum fan speed

Each level has hysteresis (different ON/OFF thresholds) to prevent rapid cycling.

## Active Cooling Implementation

### Active Cooling Methods (`_ACx`)

Each `_ACx` method returns a temperature threshold:
- When system temperature **exceeds** the threshold, the OS activates the corresponding fan level
- When temperature **falls below** the threshold, the OS may deactivate that level

**Hysteresis Logic**:
```
Method (_AC0) {
    If (\FLVL <= 0) {
        Return (CTOK (FAN0_THRESHOLD_OFF))  // Higher temp to turn off
    } Else {
        Return (CTOK (FAN0_THRESHOLD_ON))   // Lower temp to turn on
    }
}
```

This prevents oscillation by requiring different temperatures to activate vs. deactivate a fan level.

### Active Cooling Lists (`_ALx`)

Each `_ALx` associates a cooling threshold with a fan device:
```
Name (_AL0, Package () { FAN0 })  // FAN0 activated at _AC0 threshold
Name (_AL1, Package () { FAN1 })  // FAN1 activated at _AC1 threshold
Name (_AL2, Package () { FAN2 })  // FAN2 activated at _AC2 threshold
Name (_AL3, Package () { FAN3 })  // FAN3 activated at _AC3 threshold
Name (_AL4, Package () { FAN4 })  // FAN4 activated at _AC4 threshold
```

## Power Resource State Machine

Each fan level has an associated power resource (`FNP0` through `FNP4`) that manages state transitions.

### State Transitions

**FNP0-FNP3** (Levels 0-3):
- `_STA`: Returns 1 (ON) if `\FLVL <= level`, else 0 (OFF)
- `_ON`: Transitions **to** this level from a higher-numbered level
- `_OFF`: Transitions **away** from this level to the next higher-numbered level

Example for FNP0 (maximum cooling):
```
PowerResource (FNP0, 0, 0) {
    Method (_STA) {
        If (\FLVL <= 0) { Return (1) }  // Active if at max cooling
        Else { Return (0) }
    }
    Method (_ON) {
        If (!_STA ()) {                  // If not already active
            \FLVL = 0                     // Set to max cooling
            \_SB.PCI0.LPCB.SIO.ENVC.F2PS = FAN0_PWM  // Set fan PWM
            Notify (\_TZ.THRM, 0x81)       // Notify thermal zone
        }
    }
    Method (_OFF) {
        If (_STA ()) {                   // If currently active
            \FLVL = 1                     // Transition to level 1
            \_SB.PCI0.LPCB.SIO.ENVC.F2PS = FAN1_PWM  // Set corresponding PWM
            Notify (\_TZ.THRM, 0x81)       // Notify thermal zone
        }
    }
}
```

**FNP4** (Minimum cooling - Level 4):
- `_STA`: Returns 1 if `\FLVL <= 4` (always true in normal operation)
- `_ON`: Transitions to minimum cooling state
- `_OFF`: **No-op** - This is the minimum state, cannot transition lower

### Critical: FNP4._OFF Must Be a No-Op

This is **essential for Windows compatibility**:

**Problem**: Early implementations had `_OFF` setting `\FLVL = 4` and PWM values, identical to `_ON`. This violated ACPI power resource requirements:
- After `_ON` is called, `_STA` must eventually return 1 (ON)
- After `_OFF` is called, `_STA` must eventually return 0 (OFF)

Since both methods resulted in `\FLVL = 4`, and `_STA` returns 1 when `\FLVL <= 4`, the power resource could never properly transition to OFF state.

**Solution**: Make `_OFF` a no-op since FAN4 is the minimum cooling state:
```
PowerResource (FNP4, 0, 0) {
    Method (_STA) {
        If (\FLVL <= 4) { Return (1) }
        Else { Return (0) }
    }
    Method (_ON) {
        If (!_STA ()) {
            \FLVL = 4
            \_SB.PCI0.LPCB.SIO.ENVC.F2PS = FAN4_PWM
            Notify (\_TZ.THRM, 0x81)
        }
    }
    Method (_OFF) {
        // FAN4 is the minimum cooling state (idle/lowest fan speed)
        // There is no lower state to transition to, so _OFF is a no-op
        // to maintain proper ACPI power resource state machine semantics
    }
}
```

This maintains proper ACPI state machine semantics and ensures Windows compatibility while maintaining Linux compatibility.

## Passive Cooling

In addition to active (fan-based) cooling, most implementations support passive cooling:

- `_PSV`: Returns passive cooling threshold temperature
- `_PSL`: Returns list of processors to throttle
- `_TC1`, `_TC2`: Thermal constants for passive cooling algorithm
- `_TSP`: Thermal sampling period (typically 20 deciseconds = 2 seconds)

When temperature exceeds `_PSV` threshold, the OS throttles CPUs listed in `_PSL` to reduce heat generation.

## Polling and Notification

- `_TZP`: Thermal zone polling period (typically 100 deciseconds = 10 seconds)
  - OS polls `_TMP` at this interval to check temperature

- `Notify (\_TZ.THRM, 0x81)`: Thermal zone change notification
  - Sent whenever fan level changes
  - Tells OS to re-evaluate thermal zone immediately

## Initialization

The `_INI` method runs when the thermal zone is initialized:

```
Method (_INI) {
    \FLVL = 4                             // Start at minimum cooling
    \_SB.PCI0.LPCB.SIO.ENVC.F2PS = FAN4_PWM  // Set initial fan PWM
    Notify (\_TZ.THRM, 0x81)               // Initial notification
}
```

## Operating System Interaction

### Thermal Policy Flow

1. **OS boots** → Executes `_INI` → Fan starts at Level 4
2. **OS polls `_TMP`** periodically → Gets current temperature
3. **Temperature rises** → Exceeds `_AC3` threshold
4. **OS calls `FAN3._ON`** → Power resource FNP3 activated → `\FLVL = 3`
5. **Temperature continues rising** → Exceeds `_AC2` threshold
6. **OS calls `FAN2._ON`** → Power resource FNP2 activated → `\FLVL = 2`
7. **Temperature drops** → Falls below `_AC2` threshold
8. **OS calls `FAN2._OFF`** → `\FLVL = 3` → Returns to Level 3
9. **Cycle continues** based on temperature changes

### Critical Temperature

If temperature reaches `_CRT` threshold:
- OS initiates emergency shutdown
- Prevents hardware damage from overheating

## Global Variables

Standard variables used across implementations:

- `\FLVL`: Current fan level (0-4)
- `\TMAX`: Maximum junction temperature (Tj_max)
- `\TCRT`: Critical shutdown temperature
- `\TPSV`: Passive cooling threshold temperature

## Configuration

Fan thresholds and PWM values are defined in board-specific headers, typically:
```
src/mainboard/<vendor>/<board>/include/thermal.h
```
or
```
src/mainboard/<vendor>/<board>/variants/<variant>/include/variant/thermal.h
```

Example configuration:
```c
#define FAN0_THRESHOLD_ON   75  // Temperature to activate max fan (°C)
#define FAN0_THRESHOLD_OFF  65  // Temperature to deactivate max fan (°C)
#define FAN0_PWM            0xFF // PWM duty cycle value (max)

#define FAN1_THRESHOLD_ON   65
#define FAN1_THRESHOLD_OFF  55
#define FAN1_PWM            0xC0

#define FAN2_THRESHOLD_ON   55
#define FAN2_THRESHOLD_OFF  45
#define FAN2_PWM            0x80

#define FAN3_THRESHOLD_ON   45
#define FAN3_THRESHOLD_OFF  35
#define FAN3_PWM            0x40

#define FAN4_PWM            0x20  // Idle fan speed
```

## Implementation Variations

While the core pattern is consistent, there are some variations:

### PWM Output Selection
- **Google boards**: Use Fan2 PWM (`F2PS`)
- **Intel/Samsung boards**: Use Fan3 PWM (`F3PS`)

### Guard Checks
Some implementations wrap state changes with `_STA()` checks:
```
Method (_ON) {
    If (!_STA ()) {  // Only change state if not already active
        // ... state change
    }
}
```

Others omit the guard and always perform the state change.

### Temperature Reading
- Most implementations read from SuperIO TMPIN3 via PECI
- Some (like intel/wtm2) use simplified stub implementations for reference

### Dynamic Thermal Tables
The google/jecht/tidus variant includes multiple thermal tables that can be switched based on system temperature sensors, allowing more sophisticated thermal management.

## Compatibility Notes

### Linux
- More lenient ACPI parser
- Tolerates minor state machine violations
- Worked with buggy FNP4._OFF implementations

### Windows
- Stricter ACPI compliance checking
- Requires proper power resource state machine behavior
- **Requires the FNP4._OFF no-op fix** to function correctly
- May disable thermal zone entirely if ACPI violations detected

## Debugging

To debug fan control issues:

1. **Check ACPI errors**: Look for thermal zone errors in OS logs
   - Linux: `dmesg | grep -i acpi` or check `/sys/class/thermal/`
   - Windows: Event Viewer → System → ACPI errors

2. **Monitor temperature**: Use OS tools to check `_TMP` readings
   - Linux: `/sys/class/thermal/thermal_zone*/temp`
   - Windows: HWiNFO64, HWMonitor

3. **Check fan level**: Monitor `\FLVL` value (ACPI debugger or custom logging)

4. **Verify thresholds**: Ensure threshold values are appropriate for the hardware

5. **Test state transitions**: Verify each fan level activates at correct temperature

6. **ACPI table inspection**: Decompile DSDT/SSDT tables with `acpidump` and `iasl` to verify implementation

## Implementation Checklist

When implementing this pattern on a new board:

- [ ] Define all 5 fan threshold pairs (ON/OFF) with appropriate hysteresis
- [ ] Define PWM values for all 5 fan levels
- [ ] Implement temperature sensor reading (typically PECI via SuperIO)
- [ ] Implement CTOK conversion method (°C to deci-Kelvin)
- [ ] Create all 5 PowerResource objects (FNP0-FNP4)
- [ ] **Critical**: Ensure FNP4._OFF is a no-op (not setting state)
- [ ] Create all 5 Fan Device objects (FAN0-FAN4) with correct `_PR0` references
- [ ] Implement _ACx methods with hysteresis logic
- [ ] Define _ALx packages linking to fan devices
- [ ] Implement _INI to set initial state
- [ ] Implement _TMP with error handling
- [ ] Define _CRT, _PSV, _PSL for critical/passive cooling
- [ ] Set appropriate _TZP polling interval
- [ ] Test on both Linux and Windows

## References

- [ACPI Specification 6.5 - Thermal Management](https://uefi.org/specs/ACPI/6.5/)
- [ACPI Specification - ThermalZone Objects](https://uefi.org/specs/ACPI/6.5/11_Thermal_Management.html)
- [ACPI Specification - PowerResource Objects](https://uefi.org/specs/ACPI/6.5/07_Power_and_Performance_Mgmt.html#power-resources)
- Intel PECI Specification
- SuperIO vendor datasheets (ITE, Nuvoton, Winbond, etc.)

## See Also

- [Intel DPTF](dptf.md) - More sophisticated Intel Dynamic Platform and Thermal Framework
- [ACPI GPIO Documentation](../acpi/gpio.md)

