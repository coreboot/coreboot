package mtl

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

// Group : "GPP_V", "GPP_C", "GPP_A", "GPP_E", "GPP_H", "GPP_F", "GPP_S", "GPP_B", "GPP_D",
//         "GPD", "VGPIO_USB", "VGPIO_PCIE"

// KeywordCheck - This function is used to filter parsed lines of the configuration file and
// returns true if the keyword is contained in the line.
// line : string from the configuration file
func (PlatformSpecific) KeywordCheck(line string) bool {
	isIncluded, _ := common.KeywordsCheck(line, "GPP_", "GPD", "VGPIO")
	return isIncluded
}
