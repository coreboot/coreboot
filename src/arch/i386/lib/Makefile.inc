obj-y += c_start.o
obj-y += cpu.o
obj-y += pci_ops_conf1.o
obj-y += pci_ops_conf2.o
obj-y += pci_ops_mmconf.o
obj-y += pci_ops_auto.o
obj-y += exception.o

initobj-y += printk_init.o
initobj-y += cbfs_and_run.o
