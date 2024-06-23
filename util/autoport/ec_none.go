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

	si := Create(ctx, "acpi/superio.asl")
	defer si.Close()

	ec := Create(ctx, "acpi/ec.asl")
	defer ec.Close()
}
