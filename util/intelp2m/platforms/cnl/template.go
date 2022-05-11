package cnl

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

type InheritanceTemplate interface {
	KeywordCheck(line string) bool
}

// GroupNameExtract - This function extracts the group ID, if it exists in a row
// line      : string from the configuration file
// return
//     bool   : true if the string contains a group identifier
//     string : group identifier
func (PlatformSpecific) GroupNameExtract(line string) (bool, string) {
	return common.KeywordsCheck(line,
		"GPP_A", "GPP_B", "GPP_G", "GPP_D", "GPP_F", "GPP_H", "GPD", "GPP_C", "GPP_E")
}

// KeywordCheck - This function is used to filter parsed lines of the configuration file and
//                returns true if the keyword is contained in the line.
// line      : string from the configuration file
func (platform PlatformSpecific) KeywordCheck(line string) bool {
	return platform.InheritanceTemplate.KeywordCheck(line)
}
