#include <device/device.h>

/* Dummy chip_operations, so every chip has one for sure.
 * Temporary work-around before total chip.h removal.
 */

struct chip_operations cpu_via_c3_ops = {};
struct chip_operations cpu_via_c7_ops = {};
struct chip_operations cpu_amd_geode_lx_ops = {};
struct chip_operations cpu_amd_geode_gx1_ops = {};
struct chip_operations cpu_amd_geode_gx2_ops = {};
struct chip_operations drivers_ati_ragexl_ops = {};
struct chip_operations drivers_dec_21143_ops = {};
struct chip_operations drivers_generic_generic_ops = {};
struct chip_operations drivers_oxford_oxpcie_ops = {};
struct chip_operations drivers_realtek_ops = {};
struct chip_operations drivers_sil_3114_ops = {};
struct chip_operations drivers_trident_blade3d_ops = {};
struct chip_operations southbridge_amd_amd8131_ops = {};
struct chip_operations southbridge_amd_amd8132_ops = {};
struct chip_operations southbridge_amd_amd8151_ops = {};
struct chip_operations southbridge_broadcom_bcm21000_ops = {};
struct chip_operations southbridge_broadcom_bcm5780_ops = {};
struct chip_operations southbridge_intel_i82870_ops = {};
struct chip_operations southbridge_rdc_r8610_ops = {};
struct chip_operations southbridge_via_k8t890_ops = {};
struct chip_operations superio_serverengines_pilot_ops = {};
struct chip_operations superio_smsc_lpc47n207_ops = {};
struct chip_operations superio_smsc_sio1007_ops = {};

