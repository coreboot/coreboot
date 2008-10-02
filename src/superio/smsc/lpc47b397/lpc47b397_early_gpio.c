static void lpc47b397_gpio_offset_out(unsigned iobase, unsigned offset, unsigned value)
{
	outb(value,iobase+offset);
}

static unsigned lpc47b397_gpio_offset_in(unsigned iobase, unsigned offset)
{
	return inb(iobase+offset);
}

//for GP60-GP64, GP66-GP85
#define LPC47B397_GPIO_CNTL_INDEX 0x70
#define LPC47B397_GPIO_CNTL_DATA 0x71

static void lpc47b397_gpio_index_out(unsigned iobase, unsigned index, unsigned value)
{
	outb(index,iobase+LPC47B397_GPIO_CNTL_INDEX);
	outb(value, iobase+LPC47B397_GPIO_CNTL_DATA);
}

static unsigned lpc47b397_gpio_index_in(unsigned iobase, unsigned index)
{
	outb(index,iobase+LPC47B397_GPIO_CNTL_INDEX);
	return inb(iobase+LPC47B397_GPIO_CNTL_DATA);
}

