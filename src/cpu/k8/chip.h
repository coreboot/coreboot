extern struct chip_control cpu_k8_control;

struct ht_link {
	struct chip *chip;
	unsigned int ht_width, ht_speed;
};

struct cpu_k8_config {
	struct ht_link ldt0, ldt1, ldt2;
};
