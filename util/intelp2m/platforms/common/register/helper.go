package register

func SetVal[T DW0 | DW1](value uint32, ro uint32) T {
	return T{Register: Register{Value: value, ReadOnly: ro}}
}
