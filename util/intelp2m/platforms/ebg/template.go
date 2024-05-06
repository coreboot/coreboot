package ebg

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

type InheritanceTemplate interface {
	GroupNameExtract(line string) (bool, string)
	KeywordCheck(line string) bool
}

// GroupNameExtract - This function extracts the group ID, if it exists in a row
// line      : string from the configuration file
// return
//     bool   : true if the string contains a group identifier
//     string : group identifier
func (platform PlatformSpecific) GroupNameExtract(line string) (bool, string) {
	return common.KeywordsCheck(line,
		"GPPC_A", "GPPC_B", "GPPC_S", "GPPC_C", "GPP_D", "GPP_E", "GPPC_H", "GPP_J",
		"GPP_I", "GPP_L", "GPP_M", "GPP_N")
}

// KeywordCheck - This function is used to filter parsed lines of the configuration file and
//                returns true if the keyword is contained in the line.
// line      : string from the configuration file
func (platform PlatformSpecific) KeywordCheck(line string) bool {
	isIncluded, _ := common.KeywordsCheck(line, "GPP_", "GPPC_")
	return isIncluded
}
