# AMD PSP ROM Armor

## Overview

ROM Armor is a security feature provided by AMD's Platform Security Processor (PSP)
that protects SPI flash memory from unauthorized modifications. It enforces access
control to specific regions of the SPI flash, allowing only whitelisted operations
in System Management Mode (SMM). The ownership of the SPI flash changes from the
x86 to the PSP.

There are two versions of ROM Armor:
- **ROM Armor 2**: Earlier implementation requiring PSP-mediated access for all
                   flash operations (read, write, erase)
- **ROM Armor 3**: Enhanced implementation allowing direct read access while
                   maintaining PSP-mediated write and erase operations

## ROM Armor 2

### Overview

ROM Armor 2 is the earlier implementation of AMD's flash protection mechanism.
Once enabled, all SPI flash operations (read, write, and erase) must be performed
through PSP mailbox commands. This ensures complete PSP oversight of all flash access.

### Key Features

- **Complete PSP Mediation**: All flash operations (READ, WRITE, ERASE) go through PSP firmware
- **SMM-Only Mode**: Flash access is restricted to System Management Mode
- **Whitelist Enforcement**: Only regions marked as writable in PSP/BIOS directories can be modified
- **Mailbox Communication**: Uses PSP-to-BIOS mailbox for all SPI transactions

### Configuring Writable Regions

ROM Armor protects the SPI flash by default, but certain regions can be configured
as writable through two mechanisms:

#### 1. PSP and BIOS Directory Entry Writable Bit

Entries in the PSP and BIOS directories can be marked as writable by setting the
`writable` bit in the directory entry. When this bit is set, ROM Armor will allow
write and erase operations to the flash region occupied by that entry.

**Common writable PSP entries:**
- **fTPM NVRAM** - Stores fTPM (firmware TPM) non-volatile data
- **PSP NVRAM (RPMC)** - Stores Replay Protected Monotonic Counter data

**Common writable BIOS entries:**
- **APCB (AGESA PSP Customization Block)** - Platform configuration data
- **APOB NV Copy** - AGESA Parameter Output Block non-volatile storage
- **Variable NVRAM** - Flash area used for UEFI variables

These entries are configured during firmware build using the `amdfwtool` utility,
which sets the writable bit based on the entry type and platform requirements.

#### 2. Arbitrary Flash Region Whitelisting (BIOS Directory Type 0x6D)

For flash regions that don't correspond to a specific PSP or BIOS directory entry,
you can create a whitelist entry using BIOS directory type `0x6D` (AMD_BIOS_NV_ST).
This entry contains a list of flash address ranges that should be writable under
ROM Armor. Typically used for the Variable NVRAM area.

**Important considerations:**
- Keep the whitelist minimal to reduce attack surface
- Ensure writable regions don't overlap with critical boot code
- Align regions to flash erase block boundaries (4KB or 64KB)
- Document why each region needs to be writable

### Limitations

- **Performance Overhead**: All operations require PSP mailbox communication
- **Read Performance**: Even read operations are mediated by PSP, impacting performance
                        (ROM Armor 2 only)
- **Complexity**: Requires careful synchronization between BIOS and PSP firmware

## ROM Armor 3

### Overview

ROM Armor 3 is the enhanced version that improves performance by allowing direct
read access to SPI flash while maintaining PSP-mediated write and erase operations.
This provides better performance for read-heavy workloads while preserving security
for write operations. EDK2 default variable store implementation is also assuming
that the storage is memory mapped and doesn't need to go through an accessor library.

### Key Features

- **Direct Read Access**: Flash reads are bypassing PSP and use FCH SPI controller directly
- **PSP-Mediated Writes**: Write and erase operations still require PSP approval
- **Enhanced Performance**: Eliminates mailbox overhead for read operations
- **Same Security Model**: Maintains whitelist enforcement for writes/erases

### Operation Details

#### Initialization

ROM Armor 3 is initialized during SMM setup. The initialization process:
1. Sends the "Enter SMM-only Mode" command to PSP firmware
2. Receives the total flash size from PSP
3. Sets up the PSP mailbox communication buffer in TSEG (SMM memory)
4. Verifies ROM Armor enforcement via HSTI state
5. Registers SMM handlers for flash operations

#### Read Operations

In ROM Armor 3, read operations bypass the PSP and use the FCH SPI controller directly.
This provides near-native read performance since no PSP mailbox communication is required.

#### Write Operations

Write operations are chunked into 4KB blocks and sent to the PSP via mailbox commands.
The PSP firmware validates each write request against the configured whitelist before
allowing the operation. This ensures that even compromised SMM code cannot write to
protected flash regions.

#### Erase Operations

Erase operations support both 4KB and 64KB block sizes for efficiency. The implementation
automatically selects 64KB erases when:
- The flash device supports 64KB erase commands
- The offset is 64KB-aligned
- At least 64KB needs to be erased
- The `SOC_AMD_PSP_ROM_ARMOR_64K_ERASE` option is enabled

Otherwise, 4KB erase operations are used.

### S3 Resume Support

ROM Armor 3 protection must be re-enabled after S3 (suspend-to-RAM) resume. The system
registers an S3 resume handler that re-sends the "Enter SMM-only Mode" command to the PSP
firmware during the resume process. Without this, the SPI flash would become directly
writable after S3 resume, defeating the security protection.

## Comparison: ROM Armor 2 vs ROM Armor 3

| Feature               | ROM Armor 2                 | ROM Armor 3                     |
|-----------------------|-----------------------------|---------------------------------|
| **Read Operations**   | PSP-mediated                | Direct FCH SPI access           |
| **Write Operations**  | PSP-mediated                | PSP-mediated                    |
| **Erase Operations**  | PSP-mediated                | PSP-mediated                    |
| **Performance**       | Lower (all ops through PSP) | Higher (direct MMIO reads)      |
| **Security**          | Complete PSP oversight      | Same write/erase protection     |
| **Use Case**          | Maximum security            | Balanced security & performance |
| **Implementation**    | Simpler                     | More complex (dual paths)       |

## Configuration

### Kconfig Options

```kconfig
config SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3
    bool "Enable PSP ROM Armor support"
    help
      Enable PSP ROM Armor flash protection. This requires PSP firmware
      support and appropriate directory entries.

config SOC_AMD_PSP_ROM_ARMOR_64K_ERASE
    bool "Enable 64KB erase block support"
    depends on SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3
    help
      Enable 64KB erase blocks for improved erase performance on compatible
      flash devices.
```

## Security Considerations

### Allowlist Configuration

ROM Armor enforces access control through multiple layers:

1. **PSP Directory Entries**: Directory entries with the 'writable' bit set allow
                              modification of their associated flash regions
2. **BIOS Directory Entries**: Similar to PSP entries, BIOS directory entries
                               can be marked writable
3. **BIOS Directory Type 0x6D (Allowlist)**: Defines arbitrary flash address ranges
                                             that should be writable
4. **PSP Firmware Validation**: The PSP firmware verifies all write/erase requests against
                                the combined allowlist before allowing access

The allowlist is processed by PSP firmware during ROM Armor initialization. Any write or
erase operation outside the whitelisted regions will be rejected by the PSP, returning
an error status to the SMM code.

### Threat Model

ROM Armor protects against:
- Unauthorized BIOS modification from malware in OS
- Unauthorized flash writes from compromised SMM code (limited to allowlist)
- Flash corruption during runtime

ROM Armor does NOT protect against:
- Physical SPI programmer attacks
- Vulnerabilities in PSP firmware itself
- Attacks before ROM Armor is enabled

### Best Practices

1. **Minimize Allowlist**: Only allowlist absolutely necessary regions
2. **Validate Inputs**: Always validate offsets and sizes before PSP transactions
3. **Error Handling**: Properly handle PSP command failures
4. **S3 Resume**: Ensure ROM Armor is re-enabled after S3 resume
5. **HSTI Verification**: Check HSTI state to verify ROM Armor is enforced

## HSTI (Hardware Security Test Interface) Integration

ROM Armor status is tracked through the Hardware Security Test Interface (HSTI).
The `PSP_ROM_ARMOR_ENFORCED` bit in the HSTI state indicates whether ROM Armor
has been successfully activated.

Software can query this state to verify that ROM Armor protection is active.
The HSTI state is typically stored in a reserved memory region accessible to
both firmware and OS components for security validation purposes.

## APM Call Interface (Non-SMM Access)

For non-SMM code that needs flash access after ROM Armor is enabled (ramstage only),
an APM call (APMC SMI) interface is provided.

This interface allows non-privileged code to request flash operations by triggering
a System Management Interrupt (SMI). The SMI handler then:
1. Validates the request parameters
2. Ensures the requested region is within allowed bounds
3. Executes the flash operation in SMM context with ROM Armor access
4. Returns the result to the caller

This provides a controlled mechanism for flash access while maintaining the security
guarantees of ROM Armor. The SMM handler performs thorough validation to prevent malicious
code from bypassing ROM Armor protections. the SMM handler is disabled before the payload
is executed. The payload can use the SMMSTORE APMC SMI interface if needed.

## Debugging

### Debug Output

Enable verbose console output (BIOS_SPEW level) to see detailed ROM Armor operations
including initialization, transaction details, and error conditions. This can help
diagnose whitelist configuration issues or PSP communication problems.

### Common Errors

**Error: "Failed to initialize PSP ROM Armor driver"**
- Check PSP firmware includes ROM Armor support
- Verify PSP mailbox is accessible
- Ensure SMM is properly initialized

**Error: "PSP ROM Armor: Transaction failed"**
- Verify offset/size are within flash bounds
- Check region is whitelisted for write/erase
- Ensure ROM Armor is enforced (check HSTI state)

**Error: "PSP returned error status"**
- PSP rejected the operation (not whitelisted)
- PSP firmware error
- Check PSP logs for details

## Conclusion

ROM Armor is a critical security feature for protecting AMD platform firmware.
ROM Armor 3 provides an improved implementation that balances security and performance
by allowing direct read access while maintaining PSP-mediated write protection.
Proper configuration and testing are essential to ensure both security and functionality.
