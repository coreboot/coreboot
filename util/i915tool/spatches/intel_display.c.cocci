@@
@@
-struct intel_limit {... };
@@
identifier d;
@@
- intel_limit_t d = {...};
@@
@@
- intel_connector_attach_encoder(...){...}
@@
@@
- intel_modeset_cleanup(...){...}
@@
@@
- intel_modeset_gem_init(...){...}
@@
@@
- intel_modeset_init(...){...}
@@
@@
- i915_disable_vga(...){...}
@@
@@
- intel_init_quirks(...){...}
@@
@@
- quirk_ssc_force_disable(...){...}
@@
@@
- quirk_pipea_force (...){...}
@@
@@
- intel_init_display(...){...}
@@
@@
- intel_init_clock_gating(...){...}
@@
@@
- ironlake_enable_rc6(...){...}
@@
@@
- ironlake_disable_rc6(...){...}
@@
@@
- ironlake_teardown_rc6(...){...}
@@
@@
- cpt_init_clock_gating(...){...}
@@
@@
- ibx_init_clock_gating(...){...}
@@
@@
- i830_init_clock_gating(...){...}
@@
@@
- i85x_init_clock_gating(...){...}
@@
@@
- gen3_init_clock_gating(...){...}
@@
@@
- broadwater_init_clock_gating(...){...}
@@
@@
- crestline_init_clock_gating(...){...}
@@
@@
- g4x_init_clock_gating(...){...}
@@
@@
- ivybridge_init_clock_gating(...){...}
@@
@@
- gen6_init_clock_gating(...){...}
@@
@@
- ironlake_init_clock_gating(...){...}
@@
@@
- gen6_update_ring_freq(...){...}
@@
@@
- gen6_enable_rps(...){...}
@@
@@
- intel_init_emon(...){...}
@@
@@
- gen6_disable_rps(...){...}
@@
@@
- gen6_set_rps(...){...}
@@
@@
- ironlake_disable_drps(...){...}
@@
@@
- ironlake_enable_drps(...){...}
@@
@@
- intel_user_framebuffer_destroy(...){...}
@@
@@
- intel_setup_outputs(...){...}
@@
@@
- intel_crtc_init(...){...}
@@
@@
- intel_crtc_reset(...){...}
@@
@@
- intel_prepare_page_flip(...){...}
@@
@@
- intel_finish_page_flip_plane(...){...}
@@
@@
- intel_finish_page_flip(...){...}
@@
@@
- intel_unpin_work_fn(...){...}
@@
@@
- intel_crtc_destroy(...){...}
@@
@@
- intel_mark_busy(...){...}
@@
@@
- intel_idle_update(...){...}
@@
@@
- intel_decrease_pllclock(...){...}
@@
@@
- intel_increase_pllclock(...){...}
@@
@@
- intel_crtc_idle_timer(...){...}
@@
@@
- intel_gpu_idle_timer(...){...}
@@
@@
- ivb_update_cursor(...){...}
@@
@@
- i9xx_update_cursor(...){...}
@@
@@
- i845_update_cursor(...){...}
@@
@@
- intel_crtc_load_lut(...){...}
@@
@@
- ironlake_update_pch_refclk(...){...}
@@
@@
- intel_update_watermarks(...){...}
@@
@@
- sandybridge_update_wm(...){...}
@@
@@
- ironlake_update_wm(...){...}
@@
@@
- i830_update_wm(...){...}
@@
@@
- i9xx_update_wm(...){...}
@@
@@
- i965_update_wm(...){...}
@@
@@
- g4x_update_wm(...){...}
@@
@@
- pineview_update_wm(...){...}
@@
@@
- pineview_disable_cxsr(...){...}
@@
@@
- intel_encoder_destroy(...){...}
@@
@@
- intel_encoder_commit (...){...}
@@
@@
- intel_encoder_prepare (...){...}
@@
@@
- ironlake_crtc_commit(...){...}
@@
@@
- ironlake_crtc_prepare(...){...}
@@
@@
- i9xx_crtc_commit(...){...}
@@
@@
- i9xx_crtc_prepare(...){...}
@@
@@
- intel_crtc_disable(...){...}
@@
@@
- intel_crtc_dpms(...){...}
@@
@@
- i9xx_crtc_dpms(...){...}
@@
@@
- i9xx_crtc_disable(...){...}
@@
@@
- i9xx_crtc_enable(...){...}
@@
@@
- intel_crtc_dpms_overlay(...){...}
@@
@@
- ironlake_crtc_dpms(...){...}
@@
@@
- ironlake_crtc_disable(...){...}
@@
@@
- ironlake_crtc_enable(...){...}
@@
@@
- intel_cpt_verify_modeset(...){...}
@@
@@
- ironlake_pch_enable(...){...}
@@
@@
- intel_crtc_wait_for_pending_flips(...){...}
@@
@@
- intel_clear_scanline_wait(...){...}
@@
@@
- ironlake_fdi_disable(...){...}
@@
@@
- cpt_phase_pointer_disable(...){...}
@@
@@
- ironlake_fdi_pll_enable(...){...}
@@
@@
- ivb_manual_fdi_link_train(...){...}
@@
@@
- gen6_fdi_link_train(...){...}
@@
@@
- ironlake_fdi_link_train(...){...}
@@
@@
- cpt_phase_pointer_enable(...){...}
@@
@@
- intel_fdi_normal_train(...){...}
@@
@@
- ironlake_set_pll_edp(...){...}
@@
@@
- intel_update_fbc(...){...}
@@
@@
- intel_disable_fbc(...){...}
@@
@@
- intel_enable_fbc(...){...}
@@
@@
- intel_cancel_fbc_work(...){...}
@@
@@
- intel_fbc_work_fn(...){...}
@@
@@
- ironlake_disable_fbc(...){...}
@@
@@
- ironlake_enable_fbc(...){...}
@@
@@
- sandybridge_blit_fbc_update(...){...}
@@
@@
- g4x_disable_fbc(...){...}
@@
@@
- g4x_enable_fbc(...){...}
@@
@@
- i8xx_enable_fbc(...){...}
@@
@@
- i8xx_disable_fbc(...){...}
@@
@@
- intel_disable_pll(...){...}
@@
@@
- intel_enable_pll(...){...}
@@
@@
- intel_clock(...){...}
@@
@@
- pineview_clock(...){...}
@@
@@
- intel_ironlake_limit(...){...}
@@
@@
- intel_g4x_limit(...){...}
@@
@@
- intel_limit(...){...}
@@
@@
- intel_pipe_has_type(...){...}
@@
@@
- intel_PLL_is_valid(...){...}
@@
@@
- intel_find_best_PLL(...){...}
@@
@@
- intel_g4x_find_best_PLL(...){...}
@@
@@
- intel_find_pll_ironlake_dp(...){...}
@@
@@
- intel_fbc_enabled(...){...}
@@
@@
- intel_pin_and_fence_fb_obj(...){...}
@@
@@
- i9xx_update_plane(...){...}
@@
@@
- ironlake_update_plane(...){...}
@@
@@
- intel_pipe_set_base_atomic(...){...}
@@
@@
- intel_pipe_set_base(...){...}
@@
@@
- intel_crtc_driving_pch(...){...}
@@
@@
- intel_crtc_mode_fixup(...){...}
@@
@@
- single_enabled_crtc(...){...}
@@
@@
- g4x_compute_wm0(...){...}
@@
@@
- g4x_compute_srwm(...){...}
@@
@@
- ironlake_compute_srwm(...){...}
@@
@@
- intel_panel_use_ssc(...){...}
@@
@@
- intel_choose_pipe_bpp_dither(...){...}
@@
@@
- i9xx_crtc_mode_set(...){...}
@@
@@
- ironlake_crtc_mode_set(...){...}
@@
@@
- intel_crtc_mode_set(...){...}
@@
@@
- intel_crtc_update_cursor(...){...}
@@
@@
- intel_crtc_cursor_set(...){...}
@@
@@
- intel_crtc_cursor_move(...){...}
@@
@@
- intel_crtc_fb_gamma_set(...){...}
@@
@@
- intel_crtc_fb_gamma_get(...){...}
@@
@@
- intel_crtc_gamma_set(...){...}
@@
@@
- intel_framebuffer_create(...){...}
@@
@@
- intel_framebuffer_size_for_mode(...){...}
@@
@@
- intel_framebuffer_create_for_mode(...){...}
@@
@@
- mode_fits_in_fbdev(...){...}
@@
@@
- intel_get_load_detect_pipe(...){...}
@@
@@
- intel_release_load_detect_pipe(...){...}
@@
@@
- intel_crtc_clock_get(...){...}
@@
@@
- intel_crtc_mode_get(...){...}
@@
@@
- do_intel_finish_page_flip(...){...}
@@
@@
- intel_gen2_queue_flip(...){...}
@@
@@
- intel_gen3_queue_flip(...){...}
@@
@@
- intel_gen4_queue_flip(...){...}
@@
@@
- intel_gen6_queue_flip(...){...}
@@
@@
- intel_gen7_queue_flip(...){...}
@@
@@
- intel_default_queue_flip(...){...}
@@
@@
- intel_crtc_page_flip(...){...}
@@
@@
- intel_get_pipe_from_crtc_id(...){...}
@@
@@
- intel_encoder_clones(...){...}
@@
@@
- intel_user_framebuffer_create_handle(...){...}
@@
@@
- intel_framebuffer_init(...){...}
@@
@@
- intel_user_framebuffer_create(...){...}
@@
@@
- intel_alloc_context_page(...){...}
@@
@@
- ironlake_setup_rc6(...){...}
@@
@@
- intel_best_encoder(...){...}
@@
identifier d;
@@
- struct drm_framebuffer_funcs d = {...};
@@
identifier d;
@@
- struct drm_mode_config_funcs d = {...};
@@
identifier d;
@@
-static struct drm_crtc_helper_funcs d = {...};
@@
identifier d;
@@
-static struct drm_crtc_funcs d = {...};
@@
identifier d;
@@
- struct intel_quirk d[] = {...};
