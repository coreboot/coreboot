package fields

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/config"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields/fsp"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields/cb"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields/raw"
)

// InterfaceSet - set the interface for decoding configuration
// registers DW0 and DW1.
func InterfaceGet() common.Fields {
	var fldstylemap = map[uint8]common.Fields{
		config.NoFlds  : cb.FieldMacros{}, // analyze fields using cb macros
		config.CbFlds  : cb.FieldMacros{},
		config.FspFlds : fsp.FieldMacros{},
		config.RawFlds : raw.FieldMacros{},
	}
	return fldstylemap[config.FldStyleGet()]
}
