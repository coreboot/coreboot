package fields

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields/cb"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields/fsp"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields/raw"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
)

// InterfaceSet - set the interface for decoding configuration
// registers DW0 and DW1.
func Get() common.FieldsIf {
	var fldstylemap = map[p2m.FieldType]common.FieldsIf{
		p2m.NoFlds:  cb.FieldCollection{}, // analyze fields using cb macros
		p2m.CbFlds:  cb.FieldCollection{},
		p2m.FspFlds: fsp.FieldCollection{},
		p2m.RawFlds: raw.FieldCollection{},
	}
	return fldstylemap[p2m.Config.Field]
}
