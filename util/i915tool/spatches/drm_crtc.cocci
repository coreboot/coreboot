@@
identifier s;
identifier head;
@@
struct s {...
- struct list_head head;
...};
@@
@@
-#include <...>
@@
@@
(
-struct drm_crtc_funcs{...};
|
-struct drm_crtc{...};
|
-struct drm_connector_funcs{...};
|
-struct drm_encoder_funcs{...};
|
-struct drm_encoder{...};
|
-struct drm_connector{...};
|
-struct drm_mode_config_funcs{...};
|
-struct drm_mode_config{...};
|
-struct drm_framebuffer_funcs{...};
)

