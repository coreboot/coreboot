#include "linux_syscall.h"
#include "linux_console.h"

struct socket_desc {
	short up;
	short down;
	short across;
};

static void main(void)
{
	static const struct socket_desc cpu_socketsA[] = {
		{  .up = 2,  .down = -1, .across = 1 }, /* Node 0 */
		{  .up = 3,  .down = -1, .across = 0 }, /* Node 1 */
		{  .up = -1, .down =  0, .across = 3 }, /* Node 2 */
		{  .up = -1, .down =  1, .across = 2 }  /* Node 3 */
	};
	static const struct socket_desc cpu_socketsB[4] = {
		{  2,  -1, 1 }, /* Node 0 */
		{  3,  -1, 0 }, /* Node 1 */
		{  -1,  0, 3 }, /* Node 2 */
		{  -1,  1, 2 }  /* Node 3 */
	};
	int i;
	print_debug("cpu_socketA\n");
	for(i = 0; i < sizeof(cpu_socketsA)/sizeof(cpu_socketsA[0]); i++) {
		print_debug(".up=");
		print_debug_hex16(cpu_socketsA[i].up);
		print_debug(" .down=");
		print_debug_hex16(cpu_socketsA[i].down);
		print_debug(" .across=");
		print_debug_hex16(cpu_socketsA[i].across);
		print_debug("\n");
	}
	print_debug("\ncpu_socketB\n");
	for(i = 0; i < sizeof(cpu_socketsB)/sizeof(cpu_socketsB[0]); i++) {
		print_debug(".up=");
		print_debug_hex16(cpu_socketsB[i].up);
		print_debug(" .down=");
		print_debug_hex16(cpu_socketsB[i].down);
		print_debug(" .across=");
		print_debug_hex16(cpu_socketsB[i].across);
		print_debug("\n");
	}
	_exit(0);
}
