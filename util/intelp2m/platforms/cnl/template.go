package cnl

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

// CheckKeyword() parses lines of the configuration file and returns true if the keyword is
// contained in the line
// "GPP_A", "GPP_B", "GPP_G", "GPP_D", "GPP_F", "GPP_H", "GPD", "GPP_C", "GPP_E"
func CheckKeyword(line string) bool {
	included, _ := common.KeywordsCheck(line, "GPP_", "GPD")
	return included
}
