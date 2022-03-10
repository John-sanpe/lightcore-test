# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# System top
# ==========================================================================

include scripts/top.mk

#####################################
# Global Config                     #
#####################################

include $(srctree)/platform.mk

sys-include-y += include/ include/kconfig.h             \
                 include/compiler/compiler-attributes.h \
                 include/compiler/compiler-type.h       \
                 include/compiler/compiler-gcc.h        \
                 include/compiler/compiler.h            \
                 include/compiler/pointer.h             \
                 include/compiler/sections.h            \
                 include/compiler/stringify.h
sys-include-y += arch/$(arch)/                          \
                 arch/$(arch)/include/                  \
                 arch/$(arch)/include/generated/

sys-ccflags-$(CONFIG_CC_OPTIMIZE_FOR_DEBUG)         += -O0
sys-ccflags-$(CONFIG_CC_OPTIMIZE_FOR_PERFORMANCE)   += -O2
sys-ccflags-$(CONFIG_CC_OPTIMIZE_FOR_SIZE)          += -Os
sys-ccflags-$(CONFIG_STRICT) += -Werror

sys-ccflags-y  += -nostdinc -fno-builtin
sys-ccflags-y  += -static -fno-pic
sys-ccflags-y  += -fno-common -fno-stack-protector
sys-ccflags-y  += -ffreestanding -std=gnu11

sys-asflags-y  += -D__ASSEMBLY__
sys-ldsflags-y += -D__ASSEMBLY__

asflags-y  := $(strip $(sys-asflags-y) $(platform-asflags-y))
ccflags-y  := $(strip $(sys-ccflags-y) $(platform-ccflags-y))
acflags-y  := $(strip $(sys-acflags-y) $(platform-acflags-y))
ldsflags-y := $(strip $(sys-ldsflags-y) $(platform-ldsflags-y))
symflags-y := $(strip $(sys-symflags-y) $(platform-symflags-y))
ldflags-y  := $(strip $(sys-ldflags-y) $(platform-ldflags-y))
include-direct-y := $(strip $(sys-include-y) $(platform-include-y))

lightcore-flags-$(CONFIG_KERNEL_MAP) += -Wl,--cref,-Map=$@.map
lightcore-flags-y += -e boot_head -nostdlib -T arch/$(arch)/kernel.lds
lightcore-flags-y += $(platform-elfflags-y) -Wl,--build-id=sha1

ifdef CONFIG_KERNEL_DEBUG
acflags-y += -g
lightcore-flags-y += -g
endif

ifdef CONFIG_LD_DEAD_CODE_DATA_ELIMINATION
acflags-y += -ffunction-sections -fdata-sections
lightcore-flags-y += -Wl,--gc-sections
endif

export CROSS_COMPILE include-direct-y
export asflags-y ccflags-y cppflags-y acflags-y
export symflags-y ldsflags-y ldflags-y

#####################################
# Generic headers                   #
#####################################

asm-generic: FORCE
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/asm \
	generic=include/asm-generic
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/lightcore/asm \
	generic=include/lightcore/asm-generic

#####################################
# Devicetree files                  #
#####################################

dtstree = boot/dts

%dtb: scripts_dtc
	$(Q)$(MAKE) $(build)=$(dtstree) $(dtstree)/$@
dtbs: scripts_dtc
	$(Q)$(MAKE) $(build)=$(dtstree)

ifdef CONFIG_BUILTIN_DTB
build: dtbs
endif

#####################################
# Subproject                        #
#####################################

obj-y += arch/
obj-y += doc/logo/
obj-y += drivers/
obj-y += fs/
obj-y += init/
obj-y += ipc/
obj-y += kernel/
obj-y += lib/
obj-y += mm/
obj-y += net/
obj-$(CONFIG_KUSR) += usr/

#####################################
# Generate romdisk                  #
#####################################

$(obj)/boot/romdisk.cpio: $(src)/boot/romdisk/ FORCE
	$(call if_changed,cpio)

ifdef CONFIG_ROMDISK
$(obj)/drivers: $(obj)/boot/romdisk.cpio FORCE
targets += boot/romdisk.cpio
endif

#####################################
# Compiler                          #
#####################################

lightcore-objs += built-in.o
elf-always-y += lightcore

lightcore.dump-obj += lightcore
dump-always-$(CONFIG_KERNEL_DUMP) += lightcore.dump

build/boot/kboot: build
build/boot/preload: build
disk: preload

kboot:      build/boot/kboot FORCE
preload:    build/boot/preload FORCE
tools:      build/tools/kernelcrc build/tools/mkincbin FORCE

build: asm-generic scripts_basic tools FORCE
	$(Q)$(MAKE) $(build)=$(srctree)

disk uboot: kboot FORCE
	$(Q)$(MAKE) $(build)=$(srctree)/boot $@

ifdef CONFIG_PRELOAD
start: disk
else
start: uboot
endif

#####################################
# Run & Install                     #
#####################################

include $(srctree)/boot/run/config.mk
ckfile := $(objtree)/boot/kboot/boot.bin

ifeq ($(wildcard $(ckfile)),)
run install uninsall: FORCE
	$(Q)$(ECHO) "Please run make first"
else
run: FORCE
	$(Q)$(SHELL) $(srctree)/boot/run/$(run_shell)
%install: FORCE
	$(Q)$(SHELL) $(srctree)/boot/install $@
endif

clean-subdir-y += boot/
