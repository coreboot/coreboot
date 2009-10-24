void i82801er_hard_reset(void);

/* FIXME: There's another hard_reset() in cache_as_ram_auto.c. Why? */
void hard_reset(void)
{
	i82801er_hard_reset();
}
