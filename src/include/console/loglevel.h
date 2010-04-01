#ifndef LOGLEVEL_H
#define LOGLEVEL_H

/* Safe for inclusion in assembly */
#define BIOS_EMERG      0   /* system is unusable                   */
#define BIOS_ALERT      1   /* action must be taken immediately     */
#define BIOS_CRIT       2   /* critical conditions                  */
#define BIOS_ERR        3   /* error conditions                     */
#define BIOS_WARNING    4   /* warning conditions                   */
#define BIOS_NOTICE     5   /* normal but significant condition     */
#define BIOS_INFO       6   /* informational                        */
#define BIOS_DEBUG      7   /* debug-level messages                 */
#define BIOS_SPEW       8   /* way too many details                 */
#define BIOS_NEVER	9   /* these messages are never printed     */

#endif /* LOGLEVEL_H */
