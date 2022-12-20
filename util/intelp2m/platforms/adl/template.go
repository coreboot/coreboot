package adl

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

// CheckKeyword() parses lines of the configuration file and returns true if the keyword is
// contained in the line
// "GPP_A", "GPP_B", "GPP_C", "GPP_D", "GPP_E", "GPP_F", "GPP_G",
// "GPP_H", "GPP_I", "GPP_J", "GPP_K", "GPP_R", "GPP_S", "GPP_T",
// "GPD", "VGPIO_USB", "VGPIO_PCIE"
func CheckKeyword(line string) bool {
	isIncluded, _ := common.KeywordsCheck(line, "GPP_", "GPD", "VGPIO")
	return isIncluded
}
