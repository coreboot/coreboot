/*
	_cache_ram_seg_base = DEFINED(CACHE_RAM_BASE)? CACHE_RAM_BASE - _rodata : 0;
	_cache_ram_seg_base_low    = (_cache_ram_seg_base) & 0xffff;
	_cache_ram_seg_base_middle = (_cache_ram_seg_base >> 16) & 0xff;
	_cache_ram_seg_base_high   = (_cache_ram_seg_base >> 24) & 0xff;

	_rom_code_seg_base =  _ltext - _text;
	_rom_code_seg_base_low    = (_rom_code_seg_base) & 0xffff;
	_rom_code_seg_base_middle = (_rom_code_seg_base >> 16) & 0xff;
	_rom_code_seg_base_high   = (_rom_code_seg_base >> 24) & 0xff;
*/



