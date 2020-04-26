package snr

import "strings"

// GroupNameExtract - This function extracts the group ID, if it exists in a row
// line      : string from the configuration file
// return
//     bool   : true if the string contains a group identifier
//     string : group identifier
func (PlatformSpecific) GroupNameExtract(line string) (bool, string) {
	for _, groupKeyword := range []string{
		"GPP_A", "GPP_B", "GPP_F",
		"GPP_C", "GPP_D", "GPP_E",
		"GPD",   "GPP_I",
		"GPP_J", "GPP_K",
		"GPP_G", "GPP_H", "GPP_L",
	} {
		if strings.Contains(line, groupKeyword) {
			return true, groupKeyword
		}
	}
	return false, ""
}

// KeywordCheck - This function is used to filter parsed lines of the configuration file and
//                returns true if the keyword is contained in the line.
// line      : string from the configuration file
func (PlatformSpecific) KeywordCheck(line string) bool {
	for _, keyword := range []string{
		"GPP_", "GPD",
	} {
		if strings.Contains(line, keyword) {
			return true
		}
	}
	return false
}
