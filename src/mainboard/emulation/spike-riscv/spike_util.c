#include <spike_util.h>

uintptr_t htif_interrupt(uintptr_t mcause, uintptr_t* regs) {
        uintptr_t fromhost = swap_csr(mfromhost, 0);
        if (!fromhost)
        return 0;

        uintptr_t dev = FROMHOST_DEV(fromhost);
        uintptr_t cmd = FROMHOST_CMD(fromhost);
        uintptr_t data = FROMHOST_DATA(fromhost);

        sbi_device_message* m = HLS()->device_request_queue_head;
        sbi_device_message* prev = 0x0;
        unsigned long i, n;
        for (i = 0, n = HLS()->device_request_queue_size; i < n; i++) {
                /*
                if (!supervisor_paddr_valid(m, sizeof(*m))
                && EXTRACT_FIELD(read_csr(mstatus), MSTATUS_PRV1) != PRV_M)
                panic("htif: page fault");
                */

                sbi_device_message* next = (void*)m->sbi_private_data;
                if (m->dev == dev && m->cmd == cmd) {
                        m->data = data;

                        // dequeue from request queue
                        if (prev)
                        prev->sbi_private_data = (uintptr_t)next;
                        else
                        HLS()->device_request_queue_head = next;
                        HLS()->device_request_queue_size = n-1;
                        m->sbi_private_data = 0;

                        // enqueue to response queue
                        if (HLS()->device_response_queue_tail)
                        {
                                HLS()->device_response_queue_tail->sbi_private_data = (uintptr_t)m;
                        }
                        else
                        {
                                HLS()->device_response_queue_head = m;
                        }
                        HLS()->device_response_queue_tail = m;

                        // signal software interrupt
                        set_csr(mip, MIP_SSIP);
                        return 0;
                }

                prev = m;
                m = (void*)atomic_read(&m->sbi_private_data);
        }
        //HLT();
        return 0;
        //panic("htif: no record");
}

uintptr_t mcall_console_putchar(uint8_t ch)
{
        while (swap_csr(mtohost, TOHOST_CMD(1, 1, ch)) != 0);
        while (1) {
                uintptr_t fromhost = read_csr(mfromhost);
                if (FROMHOST_DEV(fromhost) != 1 || FROMHOST_CMD(fromhost) != 1) {
                if (fromhost)
                htif_interrupt(0, 0);
                continue;
        }
        write_csr(mfromhost, 0);
        break;
        }
        return 0;
}

void testPrint(void) {
        /* Print a test command to check Spike console output */
        mcall_console_putchar('h');
        mcall_console_putchar('e');
        mcall_console_putchar('l');
        mcall_console_putchar('l');
        mcall_console_putchar('o');
        mcall_console_putchar('\n');
}
