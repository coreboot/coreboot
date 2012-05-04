source=main.c pci.c final/intel_bios.c final/drm_modes.c final/i915_drv.c

all: probe

broken: video

video: $(source) final/intel_display.c
	cc -include video.h -Iinputs -static -g -o video $(source) \
		-lpci final/intel_display.c

probe: $(source)
	cc -include video.h -Iinputs -static -g -o probe $(source) -lpci
clean:
	rm -f *.o video probe

moreclean:  clean
	rm final/* per-file-changes/* tmp/*

superclean: moreclean
	rm inputs/*
