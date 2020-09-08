package common

import "strings"

// KeywordsCheck - check if one of the keyword from the <keywords> group is included in the
// <line> string. Returns false if no word was found, or true otherwise and also this word
// itself
func KeywordsCheck(line string, keywords ...string) (bool, string) {
	for _, key := range keywords {
		if strings.Contains(line, key) {
			return true, key
		}
	}
	return false, ""
}
