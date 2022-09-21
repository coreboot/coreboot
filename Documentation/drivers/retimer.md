# USB4 Retimers

## Introduction
As USB speeds continue to increase (up to 5G, 10G, and even 20G or higher in
newer revisions of the spec), it becomes more difficult to maintain signal
integrity for longer traces. Devices such as retimers and redrivers can be used
to help signals maintain their integrity over long distances.

A redriver is a device that boosts the high-frequency content of a signal in
order to compensate for the attenuation typically caused by travelling through
various circuit components (PCB, connectors, CPU, etc.). Redrivers are not
protocol-aware, which makes them relatively simple. However, their effectiveness
is limited, and may not work at all in some scenarios.

A retimer is a device that retransmits a fresh copy of the signal it receives,
by doing CDR and retransmitting the data (i.e., it is protocol-aware). Since
this is a digital component, it may have firmware.


## Driver Usage

Some operating systems may have the ability to update firmware on USB4 retimers,
and ultimately will need some way to power the device on and off so that its new
firmware can be loaded. This is achieved by providing a GPIO signal that can be
used for this purpose; its active state must be the one in which power is
applied to the retimer. This driver will generate the required ACPI AML code
which will toggle the GPIO in response to the kernel's request (through the
`_DSM` ACPI method). Simply put something like the following in your devicetree:

```
device pci 0.0 on
	chip drivers/intel/usb4/retimer
		register "power_gpio" = "ACPI_GPIO_OUTPUT_ACTIVE_HIGH(GPP_A0)"
		device generic 0 on end
	end
end
```

replacing the GPIO with the appropriate pin and polarity.

