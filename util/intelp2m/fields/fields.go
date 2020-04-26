package fields

import "../config"
import "../platforms/common"

import "./fsp"
import "./cb"
import "./raw"

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
