##
## This file is part of the coreboot project.
##
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

define libgnat-template
# $1 arch

additional-dirs += $$(obj)/libgnat-$(1)/adainclude
additional-dirs += $$(obj)/libgnat-$(1)/adalib

ADAFLAGS_libgnat-$(1) := \
	--RTS=$$(obj)/libgnat-$(1)/ \
	-gnatg \
	-gnatpg \
	-I$$(src)/lib/gnat/ \
	$$(GCC_ADAFLAGS_$(1)) \
	-Werror \

libgnat-$(1)-y += a-unccon.ads
libgnat-$(1)-y += ada.ads
libgnat-$(1)-y += g-souinf.ads
libgnat-$(1)-y += gnat.ads
libgnat-$(1)-y += i-c.adb
libgnat-$(1)-y += i-c.ads
libgnat-$(1)-y += interfac.ads
libgnat-$(1)-y += s-atacco.ads
libgnat-$(1)-y += s-imenne.adb
libgnat-$(1)-y += s-imenne.ads
libgnat-$(1)-y += s-maccod.ads
libgnat-$(1)-y += s-parame.ads
libgnat-$(1)-y += s-stoele.adb
libgnat-$(1)-y += s-stoele.ads
libgnat-$(1)-y += s-unstyp.ads
libgnat-$(1)-y += system.ads

# Copy bodies too for inlining
$$(obj)/libgnat-$(1)/libgnat.a: $$$$(libgnat-$(1)-objs)
	cp $$(libgnat-$(1)-srcs) $$(libgnat-$(1)-extra-specs) \
	   $$(obj)/libgnat-$(1)/adainclude/
	cp $$(libgnat-$(1)-alis) \
	   $$(obj)/libgnat-$(1)/adalib/
	rm -f $$@
	@printf "    AR         $$(subst $$(obj)/,,$$(@))\n"
	$$(AR_libgnat-$(1)) cr $$@ $$^

endef


$(foreach arch,$(standard-archs), \
	$(eval $(call define_class,libgnat-$(arch),$(arch))))

$(foreach arch,$(standard-archs), \
	$(eval $(call libgnat-template,$(arch))))

ifeq ($(CONFIG_RAMSTAGE_ADA),y)
ramstage-libs += $(obj)/libgnat-$(ARCH-ramstage-y)/libgnat.a
endif
