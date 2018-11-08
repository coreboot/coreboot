# Measured Boot
coreboot measured boot is implemented as Google Verified Boot extension. This
means in order to use it, vboot needs to be available for your platform.

## IBB/CRTM
The "Initial Boot Block" or "Core Root of Trust for Measurement" is the first
code block loaded at reset vector and measured by a DRTM solution.
In case SRTM mode is active, the IBB measures itself before measuring the next
code block. In coreboot, cbfs files which are part of the IBB are identified
by a metatdata tag. This makes it possible to have platform specific IBB
measurements without hardcoding them.

## Known Limitations
At the moment measuring IBB dynamically and FMAP partitions are not possible but
will be added later to the implementation.

Also SoCs making use of VBOOT_RETURN_FROM_VERSTAGE are not able to use the
measured boot extension because of platform constraints.

## SRTM Mode
The "Static Root of Trust for Measurement" is the easiest way doing measurements
by measuring code before it is loaded.

![][srtm]

[srtm]: srtm.png

## DRTM Mode
The "Dynamic Root of Trust for Measurement" is realised by platform features
like Intel TXT or Boot Guard. The features provide a way of loading a signed
"Authenticated Code Module" aka signed blob. Most of these features are also
a "Trusted Execution Environment", e.g. Intel TXT.

DRTM gives you the ability of measuring the IBB from a higher Root of Trust
instead of doing it yourself without any hardware support.

## Platform Configuration Register
Normally PCR 0-7 are reserved for firmware usage. In coreboot we use just 4 PCR
banks in order to store the measurements. coreboot uses the SHA-1 or SHA-256
hash algorithm depending on the TPM specification for measurements. PCR-4 to
PCR-7 are left empty.

### PCR-0
_Hash:_ SHA1
_Description:_ Google VBoot GBB flags.

### PCR-1
_Hash:_ SHA1/SHA256
_Description:_ Google VBoot GBB HWID.

### PCR-2
_Hash:_ SHA1/SHA256
_Description:_ Core Root of Trust for Measurement which includes all stages,
data and blobs.

### PCR-3
_Hash:_ SHA1/SHA256
_Description:_ Runtime data like hwinfo.hex or MRC cache.
