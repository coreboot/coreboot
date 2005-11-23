//it takes the ENABLE_APIC_EXT_ID and APIC_ID_OFFSET and LIFT_BSP_APIC_ID
static unsigned init_cpus(unsigned cpu_init_detectedx, int controllers, const struct mem_controller *ctrl)
{
		unsigned cpu_reset;
		unsigned bsp_apicid = 0;
                struct node_core_id id;

	#if CONFIG_LOGICAL_CPUS == 1
		/* if dual core is not enabled, we don't need reorder the apicid */
                set_apicid_cpuid_lo();
	#endif	

                id = get_node_core_id_x(); // that is initid
        #if ENABLE_APIC_EXT_ID == 1
                if(id.coreid == 0) {
                        enable_apic_ext_id(id.nodeid);
                }
        #endif

                #if (ENABLE_APIC_EXT_ID == 1)
	                #if LIFT_BSP_APIC_ID == 1
                        bsp_apicid += APIC_ID_OFFSET;
			#endif	
		#endif


                enable_lapic();

		init_timer(); 


        #if (ENABLE_APIC_EXT_ID == 1)
            #if LIFT_BSP_APIC_ID == 0
                if( id.nodeid != 0 ) //all except cores in node0
            #endif
			{
				//get initial apic id and lift it
				uint32_t dword = lapic_read(LAPIC_ID);
				dword &= ~(0xff<<24);
				dword |= ((get_initial_apicid() + APIC_ID_OFFSET)<<24);
	                        lapic_write(LAPIC_ID, dword );
			}
        #endif

                if (cpu_init_detectedx) {
//                       __asm__ volatile ("jmp __cpu_reset");
                        soft_reset(); //  avoid soft reset? , I don't want to reinit ram again, make sure bsp get get INIT, So need to send one INIT to BSP ....
                                        /*
                                                1. check if it is BSP
                                                2. if not send INIT to BSP and get out
                                                3. if it is BSP, check if the mem is inited or not
                                                4. if not inited, issue soft reset
                                                5. if it is inited, call post_cache_as_ram with cpu_reset ==0. --- need to clear first 1M ram
                                        */

#if 0
                        if(!mem_inited(controllers, ctrl)) {
                                print_debug("mem is not initialized properly, need to hard reset\r\n");
                                hard_reset();
                        }
                        cpu_reset = 1;
                        post_cache_as_ram(cpu_reset);
#endif
                        //no return;
                }
                distinguish_cpu_resets(id.nodeid);

                if (!boot_cpu()) {
                        // We need stop the CACHE as RAM for this CPU too
                        #include "cpu/amd/car/disable_cache_as_ram.c"
                        stop_this_cpu(); // it will stop all cores except core0 of cpu0
                }

		return bsp_apicid;
}
