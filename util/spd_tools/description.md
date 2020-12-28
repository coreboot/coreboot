Tools for generating SPD files for DDR4 memory used in platforms with
memory down configuration.



* _gen_spd.go_ - Generates de-duplicated SPD files using a
                 global memory part list provided by the
                 mainboard in JSON format. `Go`

* _gen_part_id.go_ - Allocates DRAM strap IDs for different
                     DDR4 memory parts used by the board. `Go`
