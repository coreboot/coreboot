package register

import (
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

type Register struct {
	Value    uint32
	Mask     uint32
	ReadOnly uint32
}

// MaskCheck() checks the mask of the new macro
// Returns true if the macro is generated correctly
func (r Register) MaskCheck() bool {
	mask := ^(r.Mask | r.ReadOnly)
	return (r.Value & mask) == 0
}

// GetFieldVal() gets the value of the register bit field
func (r *Register) GetFieldVal(bitfields bits.Fields, offset bits.Offset) uint32 {
	mask := bitfields[offset]
	r.Mask |= mask
	return (r.Value & mask) >> offset
}

// CntrMaskFieldsClear() clears filed in control mask
// fieldMask : bitfields mask to be cleared
func (r *Register) CntrMaskFieldsClear(fieldMask uint32) {
	r.Mask &= ^fieldMask
}

// IgnoredFieldsGet() returns mask of unchecked (ignored) fields.
// These bit fields were not read when the macro was generated.
// return : mask of ignored bit field
func (r Register) IgnoredFieldsGet() uint32 {
	mask := r.Mask | r.ReadOnly
	return r.Value & ^mask
}

func (r Register) String() string {
	return fmt.Sprintf("0x%0.8x", r.Value)
}
