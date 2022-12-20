package ebg

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

// Group : "GPPC_A", "GPPC_B", "GPPC_S", "GPPC_C", "GPP_D", "GPP_E", "GPPC_H", "GPP_J",
//         "GPP_I", "GPP_L", "GPP_M", "GPP_N"

// CheckKeyword - This function is used to filter parsed lines of the configuration file and
// returns true if the keyword is contained in the line.
// line : string from the configuration file
func CheckKeyword(line string) bool {
	isIncluded, _ := common.KeywordsCheck(line, "GPP_", "GPPC_")
	return isIncluded
}
