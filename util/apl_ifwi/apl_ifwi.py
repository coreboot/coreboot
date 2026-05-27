#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only

"""Create an Apollo Lake/Gemini Lake IFWI image from build inputs."""

import argparse
import hashlib
import os
import re
import struct
import subprocess
import sys
import tempfile
import time
from pathlib import Path


KiB = 1024
BPDT_SIGNATURE = 0x55AA
BPDT_HEADER_SIZE = 24
BPDT_ENTRY_SIZE = 12
CPD_HEADER_SIZE = 16
CPD_ENTRY_SIZE = 24
MN2_HEADER_SIZE = 0x284
MN2_HEADER_DWORDS = MN2_HEADER_SIZE // 4
MN2_SIGNATURE_OFFSET = 0x184
RSA2048_SIZE = 256
SHA256_SIZE = 32

IBB_SUBPARTITION_LENGTH = 0xa0000
IBBP_OFFSET = 0x1000
IBBL_OFFSET = 0x1000
BPM_METADATA_OFFSET = 0x380
OBBP_DATA_OFFSET = 0x1000

EXT_KEY_MANIFEST = 0x0e
EXT_SIGNED_PACKAGE_INFO = 0x0f
EXT_BOOT_POLICY_MANIFEST = 0x13
EXT_SIZE = 0x68
BPM_METADATA_SIZE = 0xb4

HASH_ALG_SHA256 = 2
MODULE_TYPE_METADATA = 3
MODULE_TYPE_SMIP = 2
KEY_TYPE_OEM = 2
OEM_KEY_ID = 1
RSA_EXPONENT = 65537

# Usage bitmap for:
# IfwiManifest | OemSmipManifest | OemDnxIfwiManifest | BootPolicyManifest
OEM_KEY_USAGE_BITMAP = bytes.fromhex("00000000210102000000000000000000")

# Usage bitmaps emitted by MEU for the APL/GLK BIOS and SMIP manifests.
IBBP_USAGE_BITMAP = bytes.fromhex("00000000010000000000000000000000")
SMIP_USAGE_BITMAP = bytes.fromhex("00000000000100000000000000000000")
SMIP_IAFW_OFFSET = 0x4f2


BPDT_DLMP = 9
BPDT_IFP_OVERRIDE = 10
BPDT_S_BPDT = 5
BPDT_RBEP = 1
BPDT_UFS_PHY = 12
BPDT_UFS_GPP = 13
BPDT_FTPR = 2
BPDT_UEP = 17
BPDT_SMIP = 0
BPDT_PMCP = 14
BPDT_UCOD = 3
BPDT_IBBP = 4
BPDT_DEBUG_TOKENS = 11
BPDT_NFTP = 7
BPDT_OBBP = 6


def align_up(value, align):
	if value % align:
		value += align - value % align
	return value


def read_file(filename):
	return Path(filename).read_bytes()


def run_openssl(*args, input_data=None):
	cmd = ["openssl", *args]
	result = subprocess.run(cmd, input=input_data, stdout=subprocess.PIPE,
				stderr=subprocess.PIPE, check=False)
	if result.returncode:
		stderr = result.stderr.decode("utf-8", "replace").strip()
		raise ValueError(f"openssl failed: {' '.join(cmd[:3])}: {stderr}")
	return result.stdout


def rsa_key_material(private_key):
	modulus = run_openssl("rsa", "-in", private_key, "-noout", "-modulus")
	match = re.search(rb"Modulus=([0-9a-fA-F]+)", modulus)
	if not match:
		raise ValueError("unable to extract RSA modulus")

	modulus_be = bytes.fromhex(match.group(1).decode("ascii"))
	if len(modulus_be) > RSA2048_SIZE and modulus_be[0] == 0:
		modulus_be = modulus_be[1:]
	if len(modulus_be) != RSA2048_SIZE:
		raise ValueError("only RSA2048 IFWI signing keys are supported")

	text = run_openssl("rsa", "-in", private_key, "-noout", "-text")
	match = re.search(rb"publicExponent:\s+([0-9]+)", text)
	exponent = int(match.group(1)) if match else RSA_EXPONENT
	if exponent != RSA_EXPONENT:
		raise ValueError("only RSA exponent 65537 is supported")

	public_key = modulus_be[::-1]
	exponent_bytes = exponent.to_bytes(4, "little")
	public_hash = hashlib.sha256(public_key + exponent_bytes).digest()
	return public_key, exponent_bytes, public_hash


def rsa_sign_sha256(private_key, data):
	with tempfile.TemporaryDirectory() as tmpdir:
		input_file = Path(tmpdir) / "sign-data.bin"
		signature_file = Path(tmpdir) / "signature.bin"
		input_file.write_bytes(data)
		run_openssl("dgst", "-sha256", "-sign", private_key, "-out",
			    str(signature_file), str(input_file))
		signature = signature_file.read_bytes()

	if len(signature) != RSA2048_SIZE:
		raise ValueError("unexpected RSA signature size")
	return signature[::-1]


def mn2_date():
	source_date_epoch = os.environ.get("SOURCE_DATE_EPOCH")
	if source_date_epoch:
		date = time.strftime("%Y%m%d", time.gmtime(int(source_date_epoch)))
		return int(date, 16)
	return int(time.strftime("%Y%m%d", time.localtime()), 16)


def sign_mn2_manifest(manifest, private_key, public_key, exponent):
	out = bytearray(manifest)
	if len(out) < MN2_HEADER_SIZE:
		raise ValueError("MN2 manifest is too small")
	if out[0x1c:0x20] != b"$MN2":
		raise ValueError("missing MN2 manifest header")

	header_size = struct.unpack_from("<I", out, 4)[0] * 4
	manifest_size = struct.unpack_from("<I", out, 0x18)[0] * 4
	if header_size != MN2_HEADER_SIZE:
		raise ValueError("unsupported MN2 header size")
	if manifest_size > len(out):
		raise ValueError("MN2 manifest size exceeds buffer")

	out[0x78:0x7c] = struct.pack("<I", RSA2048_SIZE // 4)
	out[0x7c:0x80] = struct.pack("<I", len(exponent) // 4)
	out[0x80:0x180] = public_key
	out[0x180:0x184] = exponent
	out[MN2_SIGNATURE_OFFSET:MN2_SIGNATURE_OFFSET + RSA2048_SIZE] = (
		b"\x00" * RSA2048_SIZE)

	signed_data = bytes(out[:0x80] + out[header_size:manifest_size])
	signature = rsa_sign_sha256(private_key, signed_data)
	out[MN2_SIGNATURE_OFFSET:MN2_SIGNATURE_OFFSET + RSA2048_SIZE] = signature
	return bytes(out)


def create_mn2_manifest(private_key, public_key, exponent, vendor, extensions,
			flags=0, version=0, svn=0):
	size = MN2_HEADER_SIZE + len(extensions)
	if size % 4:
		raise ValueError("MN2 manifest size must be dword aligned")

	manifest = bytearray(size)
	struct.pack_into("<IIIIIIII", manifest, 0,
			 4, MN2_HEADER_DWORDS, 0x10000, flags,
			 vendor, mn2_date(), size // 4, 0x324e4d24)
	struct.pack_into("<IQI", manifest, 0x20, 0, version, svn)
	manifest[MN2_HEADER_SIZE:] = extensions
	return sign_mn2_manifest(manifest, private_key, public_key, exponent)


def padded(data, align=4 * KiB):
	size = align_up(len(data), align)
	return data + b"\xff" * (size - len(data))


def cpd_checksum(header):
	data = bytearray(header)
	data[11] = 0
	return (-sum(data)) & 0xff


def crc16_ccitt_false(data):
	crc = 0xffff
	for byte in data:
		crc ^= byte << 8
		for _ in range(8):
			if crc & 0x8000:
				crc = ((crc << 1) ^ 0x1021) & 0xffff
			else:
				crc = (crc << 1) & 0xffff
	return crc


def create_ifp_override():
	# Matches the FIT-generated APL/GLK IFP override sub-partition.
	return bytes.fromhex("b5000000000000000000000000000000")


def create_uep(public_hash, fpf_flags, boot_policy):
	if len(public_hash) != SHA256_SIZE:
		raise ValueError("UEP requires a SHA256 public key hash")

	uep = bytearray(0x108)
	uep[0:4] = b"$UEP"
	uep[6:8] = b"\0\x01"
	uep[0x18:0x1c] = b"TUE\0"
	struct.pack_into("<II", uep, 0x1c, 0x04400544, 0x00000440)

	# The UEP format is not public. These records match FIT 4.0.26 output:
	# two fuse-emulation records plus one OEM public-key-hash record.
	struct.pack_into("<II", uep, 0x2c, fpf_flags, 0)
	uep[0x34:0x3c] = public_hash[0:8]
	uep[0x40:0x58] = public_hash[8:32]

	struct.pack_into("<III", uep, 0x70, 0xc000, boot_policy, 0)
	uep[0x7c:0x84] = public_hash[0:8]
	uep[0x90:0xa8] = public_hash[8:32]

	struct.pack_into("<II", uep, 0xc4, fpf_flags, boot_policy)
	uep[0xcc:0xec] = public_hash

	struct.pack_into("<H", uep, 4, crc16_ccitt_false(uep[6:]))
	return bytes(uep)


def create_minimal_smip_iafw():
	return b"\xaf\xbe\xed\xde" + b"\x00" * 0x380 + b"\xaa\xcc\xff\xaa"


def create_minimal_smip_payload(smip_iafw):
	# FIT creates the same small SMIP payload when only a dummy IAFW SMIP is
	# supplied. The CSE validates/signs the full SMIP payload, even though
	# coreboot does not consume the IAFW SMIP data.
	prefix = bytearray(SMIP_IAFW_OFFSET)
	chunks = {
		0x000: "0400000000002400ce0000000100f200000400000200f2040000000003000000",
		0x060: "000000000000000000000000a10000000000ff00800000d86506000000000000",
		0x080: "0002600000005000120000000750000000000000200000000000000004000500",
		0x0a0: "004000000000000000400000000000000000000000f444001001000000000000",
		0x0c0: "0000000000000000ffffffffffffffffffffffffffffffffffffffffffffffff",
		0x0e0: "ffffffffffffffffffffffff0000000000000000000000000000000001010102",
		0x100: "0200c20100a0f4823809000000002bc1d9077d0000000000007d000000002bc1",
		0x120: "d9072bd18a06abaa2a6800000000000000000000000000000000000000000000",
		0x240: "0000000000000000000000000000000000000000000000000000010000000000",
		0x260: "000000000000000000000000000000000000924e924eb54fcb4f305e305e3c5e",
		0x280: "615e665e6b5eac5fad5f006f356fd06fdb6fdd6fe36f071a0a1a07120a120714",
		0x2a0: "0a14351c361c0000000000000000000000000000000000000000000000000000",
		0x2e0: "000000000000000000000000000000000000924e924eb54fcb4f165e185e225e",
		0x300: "235e305e305e3c5e615e665e6b5eac5fad5f006f356fd06fdb6fdd6fe36f071a",
		0x320: "0a1a07120a1207140a14351c361c000000000000000000000000000000000000",
		0x360: "000000000000000000000000000000000000005e275e00000000000000000000",
		0x3e0: "000000000000000000000000000000000000d34f0303245efdff000000000000",
		0x420: "000000000000000000000000000000000000d34f030300000000000000000000",
		0x460: "00000000000000000000000000000000000005000000215e3300a00100000000",
		0x480: "00000a2800000a280000215e215e205e205e000a3108380a310818180000205e",
		0x4a0: "0000010300000000000000000000000000000000000000000000000000000000",
	}
	for offset, chunk in chunks.items():
		data = bytes.fromhex(chunk)
		prefix[offset:offset + len(data)] = data

	total_size = len(prefix) + len(smip_iafw)
	struct.pack_into("<H", prefix, 0x02, total_size)
	struct.pack_into("<H", prefix, 0x18, len(smip_iafw))
	struct.pack_into("<H", prefix, 0x1e, total_size)
	return bytes(prefix) + smip_iafw


FITC_RTD_RECORDS = """
home           116d 0000 0000 0000 0000
RTFD           13ff 0007 0000 0000 0065
..             13ff 0000 0000 0000 0065
bios           13f8 0007 0000 0098 0180
..             13f8 0000 0000 0098 0180
bup            13f9 0007 0000 0003 015a
bup_sku        13e8 0007 0000 0003 00ee
crypto_dis     01a0 12af 0001 0003 00ee
emu_fuse_map   01c0 0001 0000 0003 00ce
fuse_ip_base   01c0 0001 0000 0003 00ce
plat_n_sku     01c0 0001 0004 0003 00ce
..             13e8 0000 0000 0003 00ee
alt_bpm_key    01a0 12af 0024 0003 00ce
ct             01e0 0001 0000 0003 015f
dataclear      01e0 1aaf 0004 0003 00ee
heci_guids     0100 1003 0050 0003 00ce
mbp            01e0 0001 0004 0003 00ce
oem_imr_data   03a0 12af 00a0 0003 00ce
si_features    01e0 0007 0014 0003 015f
..             13f9 0000 0000 0003 015a
cls            13c0 0007 0000 000a 00fb
..             13c0 0000 0000 000a 00fb
dal_ivm        13c0 0007 0000 0010 0000
app_data       13c0 0007 0000 0010 0000
..             13c0 0000 0000 0010 0000
app_files      13c0 0007 0000 0010 0000
..             13c0 0000 0000 0010 0000
sys            13c0 0007 0000 0010 0000
..             13c0 0000 0000 0010 0000
..             13c0 0000 0000 0010 0000
dal_sdm        13f8 0007 0000 0011 00b7
keys           13e8 0007 0000 0011 00b7
..             13e8 0000 0000 0011 00b7
..             13f8 0000 0000 0011 00b7
hotham         13f4 0007 0000 0036 00ce
SDTNonces      01f6 0001 0014 0036 00ce
..             13f4 0000 0000 0036 00ce
ish_srv        13c0 0007 0000 003d 00e1
INTC_pdt       01a0 17ef 03dd 003d 00ee
trace_config   01a0 17ef 0031 003d 00ee
..             13c0 0000 0000 003d 00e1
iunit          13c0 0007 0000 0002 0173
camera_mask    0380 0001 0008 0002 0000
port_cfg       0380 0001 0006 0002 0000
sec_ctl        0380 0001 0010 0002 0000
sec_touch      0380 0001 0001 0002 0000
..             13c0 0000 0000 0002 0173
loadmgr        13f8 0007 0000 0040 00bc
arben          07c0 12af 0010 0040 00bc
..             13f8 0000 0000 0040 00bc
manuf          13ff 0007 0000 0000 0065
..             13ff 0000 0000 0000 0065
mca            13fd 0007 0000 0046 00ee
attkb_mfg      07c0 12af 0000 0046 0000
eom            03a0 0aaf 0001 0000 00ee
manuf_lock     03e4 12af 0001 0046 00ee
spi_access     0180 000f 0004 0046 00ee
..             13fd 0000 0000 0046 00ee
mca_temp       13c0 0007 0000 0046 00ee
..             13c0 0000 0000 0046 00ee
mctp           13c0 0007 0000 0049 0117
device_ports   0180 0001 0004 0049 0117
eid_db         01c0 0001 0008 0049 0117
pg_info        0180 0001 0004 0049 0117
..             13c0 0000 0000 0049 0117
nvinf          13ff 0007 0000 0002 000c
..             13ff 0000 0000 0002 000c
pavp           13f8 0007 0000 0050 00ee
guc_hash       0130 1001 0024 0050 00ee
hdcp_ports     07a0 1aaf 0001 0050 00ee
lspcon_port    07a0 1aaf 0001 0050 00ee
vga_ports      07a0 1aaf 0001 0050 00ee
wv_cek_nvar    0180 1001 0100 0050 00ef
..             13f8 0000 0000 0050 00ee
policy         13ff 0007 0000 0055 0115
Bist           13e8 0007 0000 0055 00ee
auto_config    01a0 02af 0004 0055 00ee
..             13e8 0000 0000 0055 00ee
cfgmgr         13e8 0007 0000 0055 00ee
cfg_rules      01e0 0fff 0294 0055 00ee
..             13e8 0000 0000 0055 00ee
hci            13e8 0007 0000 0055 00ee
sysintid1      07a0 000f 0004 0055 00ee
sysintid2      07a0 000f 0004 0055 00ee
sysintid3      07a0 000f 0004 0055 00ee
..             13e8 0000 0000 0055 00ee
skumgr         13f8 0007 0000 0055 0115
ftpm_enable    01b8 000f 0001 0055 0115
..             13f8 0000 0000 0055 0115
DnxCblCo       01f8 000f 0000 0055 0115
NpkPtiDis      01f8 180f 0001 0055 00ee
..             13ff 0000 0000 0055 0115
ptt            13c0 0007 0000 0086 0000
dynamic        13c0 0007 0000 0086 0000
..             13c0 0000 0000 0086 0000
reserved       13c0 0007 0000 0086 0000
..             13c0 0000 0000 0086 0000
transact       13c0 0007 0000 0086 0000
..             13c0 0000 0000 0086 0000
PttProf        0338 1aaf 0004 0086 00ee
..             13c0 0000 0000 0086 0000
sigma          13c0 0007 0000 0063 00ca
..             13c0 0000 0000 0063 00ca
tcb            13c0 0007 0000 0080 012a
..             13c0 0000 0000 0080 012a
..             116d 0000 0000 0000 0000
"""


FITC_RTD_VALUES = {
	"plat_n_sku": bytes.fromhex("03000280"),
	"heci_guids": bytes.fromhex(
		"1f29392a51552f4899cb9e2274978ca815676a8ebc9a434088ef9e39c6f63e0f"
		"84352155299a1649badf0fb7ed682aeb5c39cc4fe5a94746bc6847bad7cc6b"
		"d3cb0e0f519dd1cd469c73af6180c6f58d"),
	"mbp": bytes.fromhex("01000000"),
	"si_features": bytes.fromhex("4b524f4414000000050000000000000000000000"),
	"SDTNonces": bytes.fromhex("010000000200000003000000040000000df0adde"),
	"INTC_pdt": bytes.fromhex(
		"0500dd03001400030302000000020700010000680100000001000001bf030200"
		"e400c20a878001000000001b7300010001000002410002000100000276000100"
		"040000420f020100010000427300010002000102310001000600000204020000"
		"000000003302000000000000730000000000004176000000000000010f020000"
		"0000000173000000000000007300000000000140410200000000000042020000"
		"00000000030200000000000034020000000000c08a0000000000000086000000"
		"0000000083000000000000000b0200000000000035020000000000c018020000"
		"0000000019020000000000001202000000000000100200000000000000030000"
		"000000004400c30a8780010000000007b400000000000000b100000000000000"
		"8f00000000000000c1000000000000008e000000000000007b00000000000000"
		"7c000000000000000a0073000100010000023e0000030000040000011d000100"
		"0600010000030502020028000000000000000000000000000000010000000000"
		"0000010000000000000001000000010101000000760001000400004234000002"
		"0000040000016b00020028000000000000000000000000000000010000000000"
		"00000100000000000000010000000101010000000f0201000100004234000002"
		"0000040000011d00020028000000000000000000000000000000010000000000"
		"0000010000000000000001000000010101000000410002000100000228000003"
		"000004000001480001000600010000010502020012000100010001031e000a00"
		"3c0064006400e80373000100020001023e000003000004000001290002002800"
		"0000000000000000000000000000010000000000000001000000000000000100"
		"0000010101000000010006000100000305040f02000000000001320000010200"
		"2e00010088130000000000000000000000000000000000000000000000000000"
		"0000000000000000000000000000000031000100060000020800000100000400"
		"00015d0076000000000000014b00000102004700020088130000000000010a05"
		"1464ff0100000000000000000000000000010001000100000000000000000000"
		"0000000000000000000000000000000000000000000000000000007300000000"
		"0000412100000102001d000100401f0000000000010000000000000000000000"
		"000001000100010000030000000000006900100202005b000000070100ffff00"
		"00ffff0000fa00000100000a0014000a00fa00000214000a00b4000a00fa0000"
		"03b4000a005e010a00fa000104b4000a005e010a00fa0002055e010a0068010a"
		"00fa00000646000a006e000a00fa000302000101000600010000000102"),
	"camera_mask": bytes.fromhex("3f0000003f000000"),
	"port_cfg": bytes.fromhex("100000100000"),
	"sec_ctl": bytes.fromhex("01000000010000000100000001000000"),
	"arben": b"\xff" * 16,
	"device_ports": bytes.fromhex("02000000"),
	"eid_db": bytes.fromhex("200f300092313233"),
	"pg_info": bytes.fromhex("02000000"),
	"hdcp_ports": bytes.fromhex("01"),
	"cfg_rules": bytes.fromhex(
		"000000000c0000040f000000010000000c00000400000000020000000c000004"
		"01000000030000000c00000400000000040000000c0000040100000005000000"
		"0c00000400000000060000000c00000401000000070000000c00000401000000"
		"080000000c00000401000000e70300000c00000401000000e70300000c000004"
		"00000000e70300000c000004000000000c0000000c000004000000000d000000"
		"0c000004000000000e0000000c000004000000000f0000000c00000403000000"
		"100000000c00000480000000110000000c000004ff000000120000000c000004"
		"00000000130000000c00000400000000140000000c000004ff00000015000000"
		"0c000004ff000000160000000c000004ff000000170000000c00000401000000"
		"180000000c00000400000000190000000c000004010000001a0000000c000004"
		"0a0000001b0000000c000004ff0000001c0000000c0000044750fe6d1d000000"
		"0c000004010000001e0000000c000004010000001f0000000c00000400000000"
		"20000000080000040000000021000000080000044750fea12200000008000004"
		"40000000230000000800000447445ea224000000080000040000000025000000"
		"0800000401000000260000000c00000401000000270000000800000400000000"
		"280000000800000400000000290000000c000004000000002a0000000c000004"
		"000000002b00000008000004000000002c00000008000004000000002d000000"
		"08000004000000002e0000000c000004010000002f0000000800000400000000"
		"300000000a000004010000003100000008000004000000003200000008000004"
		"0000000033000000080000040000000034000000080000040000000035000000"
		"0800000400000000360000000800000400000000"),
	"ftpm_enable": bytes.fromhex("01"),
	"PttProf": bytes.fromhex("01000000"),
}


def fitc_rtd_records(profile):
	if profile not in ("glk", "glkr"):
		raise ValueError(f"unknown fitc.cfg profile {profile!r}")

	records = []
	for line in FITC_RTD_RECORDS.splitlines():
		if not line.strip():
			continue
		name, *fields = line.split()
		records.append([name, [int(field, 16) for field in fields]])

	if profile == "glkr":
		records[0][1][0] = 0x11ff
		records[-1][1][0] = 0x11ff
		for name, fields in records:
			if name == "hotham" or fields == [0x13f4, 0, 0, 0x36, 0xce]:
				fields[0] = 0x13f6

	return records


def fitc_rtd_values(profile):
	values = dict(FITC_RTD_VALUES)
	if profile == "glkr":
		values["arben"] = bytes.fromhex("ebffffff00dfffffffffffffffffffff")
	return values


def create_fitc_config(profile):
	records = fitc_rtd_records(profile)
	values = fitc_rtd_values(profile)
	data_start = 4 + 14 + 14 + (len(records) - 1) * (14 + 14)
	data = bytearray()

	def record_payload(name, fields, data_offset):
		if len(name) > 14:
			raise ValueError(f"RTD record name {name!r} is too long")
		return (name.encode("ascii").ljust(14, b"\x00") +
			struct.pack("<HHHHHI", *fields, data_offset))

	out = bytearray()
	root_name, root_fields = records[0]
	out += struct.pack("<I", len(records))
	out += record_payload(root_name, root_fields, data_start)

	for name, fields in records[1:]:
		size = fields[2]
		if fields[0] & 0x1000:
			out += record_payload(name, fields, data_start)
			continue

		value = values.get(name, bytes(size))
		if len(value) != size:
			raise ValueError(f"RTD record {name!r} has invalid value size")
		out += record_payload(name, fields, data_start + len(data))
		data += value

	if len(out) != data_start:
		raise ValueError("invalid RTD record table size")
	return bytes(out + data)


def cpd_header(name, entries):
	header = bytearray()
	header += b"$CPD"
	header += struct.pack("<I", len(entries))
	header += bytes([1, 1, CPD_HEADER_SIZE, 0])
	header += name.encode("ascii")
	for entry_name, offset, length in entries:
		header += entry_name.encode("ascii").ljust(12, b"\x00")
		header += struct.pack("<III", offset, length, 0)
	header[11] = cpd_checksum(header)
	return bytes(header)


def create_cpd(name, payloads):
	offset = CPD_HEADER_SIZE + len(payloads) * CPD_ENTRY_SIZE
	entries = []
	for entry_name, payload in payloads:
		entries.append((entry_name, offset, len(payload)))
		offset += len(payload)

	out = bytearray(b"\xff" * align_up(offset, 4 * KiB))
	out[:CPD_HEADER_SIZE + len(payloads) * CPD_ENTRY_SIZE] = cpd_header(name, entries)

	for (_, payload), (_, offset, _) in zip(payloads, entries):
		out[offset:offset + len(payload)] = payload

	return bytes(out)


def create_cpd_with_offsets(name, payloads, min_size=0):
	entries = [(entry_name, offset, len(payload))
		   for entry_name, offset, payload in payloads]
	size = max([min_size, CPD_HEADER_SIZE + len(entries) * CPD_ENTRY_SIZE] +
		   [offset + len(payload) for _, offset, payload in payloads])

	out = bytearray(b"\xff" * align_up(size, 4 * KiB))
	header_size = CPD_HEADER_SIZE + len(entries) * CPD_ENTRY_SIZE
	out[:header_size] = cpd_header(name, entries)
	for _, offset, payload in payloads:
		out[offset:offset + len(payload)] = payload
	return bytes(out)


def find_cpd_entry(data, entry_name):
	for name, offset, length in parse_cpd(data, "CPD"):
		if name == entry_name:
			return offset, length
	raise ValueError(f"CPD: missing entry {entry_name}")


def boot_policy_metadata(ibbl, ibb, obb):
	metadata = bytearray()
	metadata += struct.pack("<III", EXT_BOOT_POLICY_MANIFEST,
				BPM_METADATA_SIZE, 0)
	for payload in (ibbl, ibb, obb):
		metadata += struct.pack("<II", HASH_ALG_SHA256, SHA256_SIZE)
		metadata += hashlib.sha256(payload).digest()

	if len(metadata) > BPM_METADATA_SIZE:
		raise ValueError("Boot Policy Manifest metadata is too large")
	metadata += b"\x00" * (BPM_METADATA_SIZE - len(metadata))
	return bytes(metadata)


def signed_package_extension(partition_name, usage_bitmap, module_name,
			     module_type, metadata):
	if len(partition_name) != 4:
		raise ValueError("partition name must be four characters")
	if len(usage_bitmap) != 16:
		raise ValueError("usage bitmap must be 16 bytes")

	extension = bytearray()
	extension += struct.pack("<II4sI", EXT_SIGNED_PACKAGE_INFO, EXT_SIZE,
				 partition_name.encode("ascii"), 0)
	extension += usage_bitmap
	extension += struct.pack("<I", 0)
	extension += b"\x00" * 16
	extension += module_name.encode("ascii").ljust(12, b"\x00")
	extension += struct.pack("<BBHI", module_type, HASH_ALG_SHA256,
				 SHA256_SIZE, len(metadata))
	extension += hashlib.sha256(metadata).digest()[::-1]

	if len(extension) != EXT_SIZE:
		raise ValueError("invalid signed package extension size")
	return bytes(extension)


def oem_key_manifest_extension(public_hash):
	extension = bytearray()
	extension += struct.pack("<IIIIHBB", EXT_KEY_MANIFEST, EXT_SIZE,
				 KEY_TYPE_OEM, 0, 0, OEM_KEY_ID, 0)
	extension += b"\x00" * 16
	extension += OEM_KEY_USAGE_BITMAP
	extension += b"\x00" * 16
	extension += struct.pack("<BBH", 0, HASH_ALG_SHA256, SHA256_SIZE)
	extension += public_hash

	if len(extension) != EXT_SIZE:
		raise ValueError("invalid OEM key manifest extension size")
	return bytes(extension)


def create_oem_key_manifest(private_key, public_key, exponent, public_hash):
	return create_mn2_manifest(
		private_key, public_key, exponent, 0x8086,
		oem_key_manifest_extension(public_hash))


def create_bios_manifest(private_key, public_key, exponent, bpm_metadata):
	extension = signed_package_extension("IBBP", IBBP_USAGE_BITMAP, "BPM.met",
					     MODULE_TYPE_METADATA, bpm_metadata)
	return create_mn2_manifest(private_key, public_key, exponent, 0x8086,
				   extension)


def create_bios(ibbl, ibb, obb, private_key, public_key, exponent):
	bpm_metadata = boot_policy_metadata(ibbl, ibb, obb)
	manifest = create_bios_manifest(private_key, public_key, exponent,
					bpm_metadata)
	ibb_offset = align_up(IBBL_OFFSET + len(ibbl), 4 * KiB)
	ibbp = create_cpd_with_offsets(
		"IBBP",
		[("IBBP.man", CPD_HEADER_SIZE + 4 * CPD_ENTRY_SIZE, manifest),
		 ("BPM.met", BPM_METADATA_OFFSET, bpm_metadata),
		 ("IBBL", IBBL_OFFSET, ibbl),
		 ("IBB", ibb_offset, ibb)])

	obbp = create_cpd_with_offsets(
		"OBBP",
		[("OBB", OBBP_DATA_OFFSET, obb),
		 ("NvStorage", OBBP_DATA_OFFSET + len(obb), b"")],
		min_size=OBBP_DATA_OFFSET + len(obb))

	obbp_offset = IBBP_OFFSET + IBB_SUBPARTITION_LENGTH
	size = obbp_offset + len(obbp)
	out = bytearray(b"\xff" * size)
	out[:0x10] = b"BIOS" + struct.pack("<I", 2) + b"\x00" * 8
	out[0x10:0x20] = (
		b"IBBP" + struct.pack("<III", IBBP_OFFSET, len(ibbp), 0))
	out[0x20:0x30] = (
		b"OBBP" + struct.pack("<III", obbp_offset, len(obbp), 0))
	out[IBBP_OFFSET:IBBP_OFFSET + len(ibbp)] = ibbp
	out[obbp_offset:obbp_offset + len(obbp)] = obbp
	return bytes(out)


def smip_metadata(smip):
	metadata = bytearray()
	metadata += struct.pack("<IIIIII", 0x0a, 0x38, 0, len(smip), len(smip), 0xf6)
	metadata += hashlib.sha256(smip).digest()[::-1]
	if len(metadata) != 0x38:
		raise ValueError("invalid SMIP metadata size")
	return bytes(metadata)


def create_smip_manifest(private_key, public_key, exponent, metadata, version):
	extension = signed_package_extension("SMIP", SMIP_USAGE_BITMAP, "smip",
					     MODULE_TYPE_SMIP, metadata)
	return create_mn2_manifest(private_key, public_key, exponent, 0,
				   extension, version=version)


def create_smip(smip, private_key, public_key, exponent, version):
	metadata = smip_metadata(smip)
	manifest = create_smip_manifest(private_key, public_key, exponent, metadata,
					version)

	return create_cpd_with_offsets(
		"SMIP",
		[("SMIP.man", CPD_HEADER_SIZE + 3 * CPD_ENTRY_SIZE, manifest),
		 ("smip", 0x380, smip),
		 ("smip.met", 0x344, metadata)])


def resign_smip(smip, private_key, public_key, exponent):
	smip = bytearray(smip)
	manifest_offset, manifest_size = find_cpd_entry(smip, "SMIP.man")
	manifest = sign_mn2_manifest(
		smip[manifest_offset:manifest_offset + manifest_size],
		private_key, public_key, exponent)
	smip[manifest_offset:manifest_offset + manifest_size] = manifest
	return bytes(smip)


def parse_cpd(data, name):
	if data[:4] != b"$CPD":
		raise ValueError(f"{name}: missing CPD header")

	count = struct.unpack_from("<I", data, 4)[0]
	header_version, entry_version, header_len = data[8], data[9], data[10]
	if header_version != 1 or entry_version != 1 or header_len != CPD_HEADER_SIZE:
		raise ValueError(f"{name}: unsupported CPD header")

	entries = []
	offset = header_len
	for _ in range(count):
		entry_name = data[offset:offset + 12].rstrip(b"\x00").decode("ascii")
		entry_offset, length, _ = struct.unpack_from("<III", data, offset + 12)
		entries.append([entry_name, entry_offset, length])
		offset += CPD_ENTRY_SIZE

	return entries


def patch_cpd_entries(data, replacements):
	out = bytearray(data)
	entries = parse_cpd(out, "FTPR")

	end = 0
	for _, offset, length in entries:
		if length:
			end = max(end, offset + length)
	end = align_up(end, 4 * KiB)

	for entry_name, payload in replacements:
		for entry in entries:
			if entry[0] == entry_name:
				entry[1] = end
				entry[2] = len(payload)
				break
		else:
			raise ValueError(f"FTPR: missing CPD entry {entry_name}")

		new_end = end + len(payload)
		if len(out) < new_end:
			out.extend(b"\xff" * (new_end - len(out)))
		out[end:new_end] = payload
		end = new_end

	out.extend(b"\xff" * (align_up(len(out), 4 * KiB) - len(out)))
	out[:CPD_HEADER_SIZE + len(entries) * CPD_ENTRY_SIZE] = cpd_header(
		"FTPR", [(name, offset, length) for name, offset, length in entries])
	return bytes(out)


def parse_fpt_partition(cse_image, name):
	for fpt_offset in (0x10, 0):
		if cse_image[fpt_offset:fpt_offset + 4] == b"$FPT":
			break
	else:
		raise ValueError("CSE image does not contain an FPT")

	count = struct.unpack_from("<I", cse_image, fpt_offset + 4)[0]
	header_len = cse_image[fpt_offset + 10]
	offset = fpt_offset + header_len

	for _ in range(count):
		entry_name = cse_image[offset:offset + 4].decode("ascii")
		entry_offset, length = struct.unpack_from("<II", cse_image, offset + 8)
		flags = struct.unpack_from("<I", cse_image, offset + 28)[0]
		valid = entry_offset != 0 and length != 0 and ((flags >> 24) & 0xff) != 0xff
		if entry_name == name:
			if not valid:
				raise ValueError(f"CSE FPT partition {name} is invalid")
			return cse_image[entry_offset:entry_offset + length]
		offset += 32

	raise ValueError(f"CSE FPT partition {name} not found")


def parse_bios_partition(bios, name):
	if bios[:4] != b"BIOS":
		raise ValueError("BIOS manifest does not start with BIOS")

	count = struct.unpack_from("<I", bios, 4)[0]
	offset = 0x10
	for _ in range(count):
		entry_name = bios[offset:offset + 4].decode("ascii")
		entry_offset, length, _ = struct.unpack_from("<III", bios, offset + 4)
		if entry_name == name:
			return bios[entry_offset:entry_offset + length]
		offset += 16

	raise ValueError(f"BIOS partition {name} not found")


def parse_ifd_region(descriptor, region):
	reg_offset = 0x40 + region * 4
	value = struct.unpack_from("<I", descriptor, reg_offset)[0]
	base = value & 0x7fff
	limit = (value >> 16) & 0x7fff
	if base > limit:
		return None
	return base * 4 * KiB, (limit - base + 1) * 4 * KiB


def bpdt_header(count, fit_tool_version):
	return struct.pack("<IHHIIQ", BPDT_SIGNATURE, count, 1, 0, 0,
			   fit_tool_version)


def bpdt_entry(entry_type, offset, size):
	return struct.pack("<HHII", entry_type, 0, offset, size)


def write_bpdt(entries, fit_tool_version, min_size=0x200):
	data = bytearray(bpdt_header(len(entries), fit_tool_version))
	for entry_type, offset, size in entries:
		data += bpdt_entry(entry_type, offset, size)
	if len(data) < min_size:
		data.extend(b"\xff" * (min_size - len(data)))
	return bytes(data)


def build_bp1(parts, debug_token_size, fit_tool_version):
	ifp = parts["IFP_OVERRIDE"]
	uep = parts["UEP"]
	subparts = {
		BPDT_SMIP: parts["SMIP"],
		BPDT_RBEP: parts["RBEP"],
		BPDT_PMCP: parts["PMCP"],
		BPDT_FTPR: parts["FTPR"],
		BPDT_UCOD: parts["UCOD"],
		BPDT_IBBP: parts["IBBP"],
		BPDT_DEBUG_TOKENS: b"\xff" * debug_token_size,
		BPDT_NFTP: parts["NFTP"],
	}

	offsets = {}
	offset = 0x200
	offsets[BPDT_IFP_OVERRIDE] = offset
	offset += len(ifp)
	offsets[BPDT_UEP] = offset
	offset += len(uep)
	offset = align_up(offset, 4 * KiB)

	for entry_type in (BPDT_SMIP, BPDT_RBEP, BPDT_PMCP, BPDT_FTPR, BPDT_UCOD,
			   BPDT_IBBP, BPDT_DEBUG_TOKENS):
		offsets[entry_type] = offset
		subparts[entry_type] = padded(subparts[entry_type])
		offset += len(subparts[entry_type])

	sbpdt_offset = offset
	nftp_offset = sbpdt_offset + 4 * KiB
	nftp = padded(subparts[BPDT_NFTP])
	sbpdt_size = 4 * KiB + len(nftp)

	bpdt_entries = [
		(BPDT_DLMP, 0, 0),
		(BPDT_IFP_OVERRIDE, offsets[BPDT_IFP_OVERRIDE], len(ifp)),
		(BPDT_S_BPDT, sbpdt_offset, sbpdt_size),
		(BPDT_RBEP, offsets[BPDT_RBEP], len(subparts[BPDT_RBEP])),
		(BPDT_UFS_PHY, 0, 0),
		(BPDT_UFS_GPP, 0, 0),
		(BPDT_FTPR, offsets[BPDT_FTPR], len(subparts[BPDT_FTPR])),
		(BPDT_UEP, offsets[BPDT_UEP], len(uep)),
		(BPDT_SMIP, offsets[BPDT_SMIP], len(subparts[BPDT_SMIP])),
		(BPDT_PMCP, offsets[BPDT_PMCP], len(subparts[BPDT_PMCP])),
		(BPDT_UCOD, offsets[BPDT_UCOD], len(subparts[BPDT_UCOD])),
		(BPDT_IBBP, offsets[BPDT_IBBP], len(subparts[BPDT_IBBP])),
		(BPDT_DEBUG_TOKENS, offsets[BPDT_DEBUG_TOKENS],
		 len(subparts[BPDT_DEBUG_TOKENS])),
	]

	sbpdt = write_bpdt([(BPDT_NFTP, nftp_offset, len(nftp))], fit_tool_version,
			   min_size=4 * KiB)

	total = nftp_offset + len(nftp)
	out = bytearray(b"\xff" * total)
	out[:0x200] = write_bpdt(bpdt_entries, fit_tool_version)
	out[offsets[BPDT_IFP_OVERRIDE]:offsets[BPDT_IFP_OVERRIDE] + len(ifp)] = ifp
	out[offsets[BPDT_UEP]:offsets[BPDT_UEP] + len(uep)] = uep
	for entry_type, payload in subparts.items():
		if entry_type == BPDT_NFTP:
			continue
		part_offset = offsets[entry_type]
		out[part_offset:part_offset + len(payload)] = payload
	out[sbpdt_offset:sbpdt_offset + len(sbpdt)] = sbpdt
	out[nftp_offset:nftp_offset + len(nftp)] = nftp
	return bytes(out)


def build_bp2(obbp, fit_tool_version):
	obbp = padded(obbp)
	sbpdt_offset = 0x200
	obbp_offset = 0x1000
	sbpdt_size = obbp_offset + len(obbp) - sbpdt_offset
	bpdt_entries = [
		(BPDT_DLMP, 0, 0),
		(BPDT_IFP_OVERRIDE, 0, 0),
		(BPDT_S_BPDT, sbpdt_offset, sbpdt_size),
		(BPDT_RBEP, 0, 0),
		(BPDT_UFS_PHY, 0, 0),
		(BPDT_UFS_GPP, 0, 0),
		(BPDT_FTPR, 0, 0),
		(BPDT_UEP, 0, 0),
	]
	sbpdt = write_bpdt([(BPDT_OBBP, obbp_offset, len(obbp))], fit_tool_version,
			   min_size=obbp_offset - sbpdt_offset)

	out = bytearray(b"\xff" * (obbp_offset + len(obbp)))
	out[:0x200] = write_bpdt(bpdt_entries, fit_tool_version)
	out[sbpdt_offset:sbpdt_offset + len(sbpdt)] = sbpdt
	out[obbp_offset:obbp_offset + len(obbp)] = obbp
	return bytes(out)


def parse_int(value):
	return int(value, 0)


def create_ifwi(args):
	descriptor = read_file(args.descriptor)
	if len(descriptor) != 4 * KiB:
		raise ValueError("descriptor must be 4 KiB")

	bios_region = parse_ifd_region(descriptor, 1)
	if bios_region is None:
		raise ValueError("descriptor has no BIOS region")
	bios_start, bios_size = bios_region
	bios_end = bios_start + bios_size

	if args.bp1_offset == 0:
		args.bp1_offset = bios_start
	if args.bp1_offset % (4 * KiB):
		raise ValueError("BP1 offset must be 4 KiB aligned")

	if args.bp2_offset == 0:
		args.bp2_offset = bios_start + bios_size // 2
	if args.bp2_offset % (4 * KiB):
		raise ValueError("BP2 offset must be 4 KiB aligned")

	ec_region = parse_ifd_region(descriptor, 8)

	cse_image = read_file(args.cse_image)
	public_key, exponent, public_hash = rsa_key_material(args.private_key)
	bios = create_bios(read_file(args.ibbl), read_file(args.ibb),
			   read_file(args.obb), args.private_key, public_key, exponent)
	if args.smip:
		smip = read_file(args.smip)
	else:
		smip_iafw = read_file(args.smip_iafw) if args.smip_iafw else create_minimal_smip_iafw()
		smip = create_minimal_smip_payload(smip_iafw)
	if smip[:4] == b"$CPD":
		smip = resign_smip(smip, args.private_key, public_key, exponent)
	else:
		smip = create_smip(smip, args.private_key, public_key, exponent,
				   args.fit_tool_version)
	oem_key = create_oem_key_manifest(args.private_key, public_key, exponent,
					  public_hash)

	ucod = create_cpd("UCOD", [(f"upatch{i + 1}", read_file(path))
				   for i, path in enumerate(args.microcode)])

	source_ftpr = parse_fpt_partition(cse_image, "FTPR")
	replacements = [("oem.key", oem_key)]
	if args.fitc_config:
		replacements.append(("fitc.cfg", read_file(args.fitc_config)))
	else:
		_, fitc_size = find_cpd_entry(source_ftpr, "fitc.cfg")
		if fitc_size == 0:
			if args.fitc_config_profile is None:
				raise ValueError("fitc.cfg profile is required when source "
						 "fitc.cfg is empty")
			replacements.append(("fitc.cfg",
					     create_fitc_config(args.fitc_config_profile)))
	ftpr = patch_cpd_entries(source_ftpr, replacements)

	ifp_override = read_file(args.ifp_override) if args.ifp_override else create_ifp_override()
	if args.uep:
		uep = read_file(args.uep)
	else:
		uep = create_uep(public_hash, args.uep_fpf_flags, args.uep_boot_policy)

	parts = {
		"IFP_OVERRIDE": ifp_override,
		"UEP": uep,
		"SMIP": smip,
		"RBEP": parse_fpt_partition(cse_image, "RBEP"),
		"PMCP": read_file(args.pmcp),
		"FTPR": ftpr,
		"UCOD": ucod,
		"IBBP": parse_bios_partition(bios, "IBBP"),
		"NFTP": parse_fpt_partition(cse_image, "NFTP"),
	}

	bp1 = build_bp1(parts, args.debug_token_size, args.fit_tool_version)
	bp2 = build_bp2(parse_bios_partition(bios, "OBBP"), args.fit_tool_version)

	if bios_start != args.bp1_offset:
		raise ValueError(f"BP1 offset {args.bp1_offset:#x} does not match BIOS "
				 f"region start {bios_start:#x}")

	if args.bp1_offset + len(bp1) > args.bp2_offset:
		raise ValueError("BP1 overlaps BP2")

	if args.bp2_offset + len(bp2) > bios_end:
		raise ValueError("BP2 exceeds BIOS region")

	out = bytearray(b"\xff" * args.flash_size)
	out[:len(descriptor)] = descriptor
	out[args.bp1_offset:args.bp1_offset + len(bp1)] = bp1
	out[args.bp2_offset:args.bp2_offset + len(bp2)] = bp2

	if args.ec:
		if ec_region is None:
			raise ValueError("EC image supplied, but descriptor has no EC region")
		ec_offset, ec_size = ec_region
		ec = read_file(args.ec)
		if len(ec) > ec_size:
			raise ValueError("EC image exceeds EC region")
		out[ec_offset:ec_offset + len(ec)] = ec

	Path(args.output).write_bytes(out)


def main(argv):
	parser = argparse.ArgumentParser()
	parser.add_argument("--output", required=True)
	parser.add_argument("--flash-size", type=parse_int, default=8 * 1024 * 1024)
	parser.add_argument("--bp1-offset", type=parse_int, default=0)
	parser.add_argument("--bp2-offset", type=parse_int, required=True)
	parser.add_argument("--debug-token-size", type=parse_int, default=0x2000)
	parser.add_argument("--fit-tool-version", type=parse_int, default=0)
	parser.add_argument("--descriptor", required=True)
	parser.add_argument("--cse-image", required=True)
	parser.add_argument("--ibbl", required=True)
	parser.add_argument("--ibb", required=True)
	parser.add_argument("--obb", required=True)
	parser.add_argument("--private-key", required=True)
	parser.add_argument("--pmcp", required=True)
	parser.add_argument("--smip")
	parser.add_argument("--smip-iafw")
	parser.add_argument("--ifp-override")
	parser.add_argument("--uep")
	parser.add_argument("--uep-fpf-flags", type=parse_int, default=0xc880)
	parser.add_argument("--uep-boot-policy", type=parse_int, default=0x11f)
	parser.add_argument("--fitc-config")
	parser.add_argument("--fitc-config-profile")
	parser.add_argument("--microcode", action="append", required=True)
	parser.add_argument("--ec")
	args = parser.parse_args(argv)

	create_ifwi(args)
	return 0


if __name__ == "__main__":
	try:
		sys.exit(main(sys.argv[1:]))
	except ValueError as err:
		print(f"apl_ifwi.py: {err}", file=sys.stderr)
		sys.exit(1)
