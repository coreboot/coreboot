@@
identifier f;
type T;
@@
T f(...){<...
-vga_get_uninterruptible(...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-vga_put(...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-intel_init_quirks(...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-drm_mode_config_init(...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-INIT_WORK(...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-setup_timer(...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-DRM_DEBUG_KMS(
+fprintf(stderr,
...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-DRM_ERROR(
+fprintf(stderr,
...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-DRM_DEBUG(
+fprintf(stderr,
...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-DRM_DEBUG_DRIVER(
+fprintf(stderr,
...);
...>}
@@
identifier f;
type T;
@@
T f(...){<...
-intel_init_display(...);
...>}
@ rulekz @
identifier t;
identifier f;
expression E1, E2;
type T;
@@
T f(...){<...
t = kzalloc(E1, E2);
...>}
@@
identifier rulekz.f;
expression E1, E2;
@@

- kzalloc(E1
+ calloc(1, E1
-  ,E2
   )
@@
identifier d;
@@
-static
const struct pci_device_id d[] = {...};
@@
expression E1;
@@
-WARN(
+if (
E1
-,
+) fprintf(stderr,
...);
@@
expression E1;
@@
-BUG_ON(
+assert(
E1);
@@
@@
-jiffies
+msecs()
@@
expression E1;
@@
-jiffies_to_msecs(
E1
-)
@@
expression E1;
@@
-msecs_to_jiffies(
E1
-)
@@
expression E1, E2;
@@
-time_after(
+(
E1
-,
+>
E2)
