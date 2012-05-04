@@
identifier d;
@@
(
-struct drm_i915_fence_reg{...};
|
-struct drm_i915_error_state{...};
|
-struct drm_i915_gem_object{...};
|
-struct drm_i915_display_funcs {...};
|
-struct drm_i915_gem_phys_object{...};
|
-struct mem_block{...};
|
-struct drm_i915_gem_request{...};
|
-struct drm_i915_file_private{...};
|
-struct drm_i915_master_private{...};
|
-struct intel_gmbus{...};
|
-struct work_struct d;
|
-struct mem_*d;
|
-struct timer_list d;
|
-struct intel_fbc_work *d;
|
-struct drm_i915_error_state *d;
|
-struct i2c_adapter d;
|
-struct i2c_adapter *d;
|
-struct intel_hw_status_page{...};
|
-struct intel_hw_status_page d;
)
@@
@@
-#include<...>
@@
identifier d;
@@
struct s {...
- struct drm_i915_display_funcs d;
...
};
@@
identifier d;
@@
struct s {...
-struct notifier_block d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-spinlock_t d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-atomic_t d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-drm_dma_handle_t *d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-drm_local_map_t d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-spinlock_t *d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-struct i2c_adapter d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-struct i2c_adapter *d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
(
-struct inteL_gmbus {...} *d;
|
-struct drm_i915_gem_object *d;
)
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-struct work_struct d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-struct resource d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-struct timer_list d;
...
};
@@
identifier s;
identifier d;
@@
struct s {...
-wait_queue_head_t d;
...
};
@@
identifier s;
identifier d;
constant c;
@@
struct s {...
-struct intel_ring_buffer d[c];
...
};
@@
identifier s;
identifier d;
constant c;
@@
struct s {...
-struct drm_i915_fence_reg d[c];
...
};

