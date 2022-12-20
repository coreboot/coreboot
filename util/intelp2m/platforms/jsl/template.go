package jsl

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

// Group : "GPP_A", "GPP_B", "GPP_C", "GPP_D", "GPP_E", "GPP_F", "GPP_G", "GPP_H", "GPP_R",
//         "GPP_S", "GPP_T", "GPD", "HVMOS", "VGPIO5"

// CheckKeyword - This function is used to filter parsed lines of the configuration file and
// returns true if the keyword is contained in the line.
// line : string from the configuration file
func CheckKeyword(line string) bool {
	isIncluded, _ := common.KeywordsCheck(line, "GPP_", "GPD", "VGPIO")
	return isIncluded
}
