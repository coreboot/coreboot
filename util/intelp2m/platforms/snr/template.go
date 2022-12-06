package snr

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

// Group: "GPP_A", "GPP_B", "GPP_F", "GPP_C", "GPP_D", "GPP_E", "GPD", "GPP_I", "GPP_J",
//        "GPP_K", "GPP_G", "GPP_H", "GPP_L"

// KeywordCheck - This function is used to filter parsed lines of the configuration file and
// returns true if the keyword is contained in the line.
// line : string from the configuration file
// Returns false if no word was found, or true otherwise
func (PlatformSpecific) KeywordCheck(line string) bool {
	included, _ := common.KeywordsCheck(line, "GPP_", "GPD")
	return included
}
