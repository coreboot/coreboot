package main

func NoEC(ctx Context) {
	ap := Create(ctx, "acpi/platform.asl")
	defer ap.Close()

	Add_SPDX(ap, ASL, GPL2_only)
	ap.WriteString(
		`Method(_WAK, 1)
{
	Return(Package() {0, 0})
}

Method(_PTS, 1)
{
}
`)

	Create_Empty(ctx, "acpi/superio.asl", ASL)
	Create_Empty(ctx, "acpi/ec.asl", ASL)
}
