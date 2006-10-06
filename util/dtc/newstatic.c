struct	root {
	u32 model;
	u32 Haddress_cells;
	u32 Hsize_cells;
	u32 compatible;
}; /*root*/
struct	cpus {
	u32 Haddress_cells;
	u32 Hsize_cells;
}; /*cpus*/
struct	emulation_qemu_i386 {
	u32 name;
	u32 device_type;
	u32 clock_frequency;
	u32 timebase_frequency;
	u32 linux_boot_cpu;
	u32 reg;
	u32 i_cache_size;
	u32 d_cache_size;
	u32 linux_phandle;
}; /*emulation_qemu_i386*/
struct	memory {
	u32 device_type;
	u32 reg;
}; /*memory*/
struct	northbridge_intel_440bx {
	u32 associated_cpu;
}; /*northbridge_intel_440bx*/
struct	southbridge_intel_piix4 {
}; /*southbridge_intel_piix4*/
struct	superio_nsc_sucks {
}; /*superio_nsc_sucks*/
struct	uart {
	u32 enabled;
}; /*uart*/
struct	chosen {
	u32 bootargs;
	u32 linux_platform;
	u32 linux_stdout_path;
}; /*chosen*/
struct	options {
	u32 normal;
	u32 fallback;
}; /*options*/
struct root root = {
	u8 model = {71,65,6d,75,00,};
	u8 Haddress_cells = {00,00,00,01,};
	u8 Hsize_cells = {00,00,00,01,};
	u8 compatible = {65,6d,75,6c,61,74,69,6f,6e,2d,69,33,38,36,2c,71,65,6d,75,00,};
}; /*root*/
struct device dev_root = {
	.children = dev_cpus
}
struct cpus cpus = {
	u8 Haddress_cells = {00,00,00,01,};
	u8 Hsize_cells = {00,00,00,00,};
}; /*cpus*/
struct device dev_cpus = {
	.sibling = dev_memory_0
	.children = dev_emulation_qemu_i386_0
}
struct emulation_qemu_i386 emulation_qemu_i386_0 = {
	u8 name = {65,6d,75,6c,61,74,69,6f,6e,2c,71,65,6d,75,2d,69,33,38,36,00,};
	u8 device_type = {63,70,75,00,};
	u8 clock_frequency = {5f,5e,10,00,};
	u8 timebase_frequency = {01,fffffffc,ffffffa0,55,};
	u8 reg = {00,00,00,00,};
	u8 i_cache_size = {00,00,20,00,};
	u8 d_cache_size = {00,00,20,00,};
	u8 linux_phandle = {00,00,00,01,};
}; /*emulation_qemu_i386_0*/
struct device dev_emulation_qemu_i386_0 = {
}
struct memory memory_0 = {
	u8 device_type = {6d,65,6d,6f,72,79,00,};
	u8 reg = {00,00,00,00,20,00,00,00,};
}; /*memory_0*/
struct device dev_memory_0 = {
	.sibling = dev_northbridge_intel_440bx
}
struct northbridge_intel_440bx northbridge_intel_440bx = {
	u8 associated_cpu = {00,00,00,01,};
}; /*northbridge_intel_440bx*/
struct device dev_northbridge_intel_440bx = {
	.sibling = dev_chosen
	.children = dev_southbridge_intel_piix4
}
struct southbridge_intel_piix4 southbridge_intel_piix4 = {
}; /*southbridge_intel_piix4*/
struct device dev_southbridge_intel_piix4 = {
	.children = dev_superio_nsc_sucks
}
struct superio_nsc_sucks superio_nsc_sucks = {
}; /*superio_nsc_sucks*/
struct device dev_superio_nsc_sucks = {
	.children = dev_uart_0
}
struct uart uart_0 = {
	u8 enabled = {00,00,00,01,};
}; /*uart_0*/
struct device dev_uart_0 = {
}
struct chosen chosen = {
	u8 bootargs = {72,6f,6f,74,3d,2f,64,65,76,2f,73,64,61,32,00,};
	u8 linux_platform = {00,00,06,00,};
	u8 linux_stdout_path = {2f,64,65,76,2f,74,74,79,53,30,00,};
}; /*chosen*/
struct device dev_chosen = {
	.sibling = dev_options
}
struct options options = {
	u8 normal = {6e,6f,72,6d,61,6c,00,};
	u8 fallback = {66,61,6c,6c,62,61,63,6b,00,};
}; /*options*/
struct device dev_options = {
}
