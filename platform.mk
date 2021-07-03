#############################################################
# Platform Config
#############################################################

ifdef CONFIG_ARCH_ARM
CROSS_COMPILE       := arm-none-eabi-
arch                := arm

platform-acflags-y  += -D__ARCH_BIT32__
kernel-flags-y      += -nostdlib

ifdef CONFIG_ARCH_ARM_926EJS
platform-acflags-y  += -march=armv5tej -mcpu=arm926ej-s -mfloat-abi=soft -marm
endif

ifdef CONFIG_ARCH_ARM_V7A
endif

endif # CONFIG_ARCH_ARM

ifdef CONFIG_ARCH_ARME
CROSS_COMPILE       := arm-none-eabi-
arch                := arme

platform-acflags-y  += -march=armv7-m -mthumb
kernel-flags-y      += -nostdlib -mthumb 
endif

ifdef CONFIG_ARCH_CSKY
include arch/csky/config.mk
CROSS_COMPILE       := /disk/d/project/buildroot/output/host/bin/csky-linux-
arch                := csky

kernel-flags-y      += -nostdlib
endif

ifdef CONFIG_ARCH_M68K
CROSS_COMPILE       := m68k-elf-
arch                := m68k

kernel-flags-y      += -nostdlib
endif

ifdef CONFIG_ARCH_RISCV64
CROSS_COMPILE       := riscv64-linux-gnu-
arch                := riscv64

platform-acflags-y  := -march=rv64imafdc -mabi=lp64d -mcmodel=medany
kernel-flags-y      += -nostdlib
endif

ifdef CONFIG_ARCH_X86
CROSS_COMPILE       := 
arch                := x86

ifneq ($(call cc-option, -mpreferred-stack-boundary=2),)
      cc_stack_align4 := -mpreferred-stack-boundary=2
else ifneq ($(call cc-option, -mstack-alignment=4),)
      cc_stack_align4 := -mstack-alignment=4
endif

biarch := $(call cc-option,-m32)

platform-acflags-y  += -m32

platform-ccflags-y  += $(cc_stack_align4)

# Try to use registers to pass parameters
# platform-ccflags-y  += -mregparm=3

# Prevent GCC from generating any FP code by mistake.
platform-ccflags-y  += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow 
platform-ccflags-y  += $(call cc-option,-mno-avx)

platform-ldflags-y  += -m elf_i386
kernel-flags-y      += -m32 -nostdlib
endif

ifdef CONFIG_ARCH_XTENSA
include arch/xtensa/config.mk
arch                := xtensa

platform-ccflags-y  += -mtext-section-literals
platform-acflags-y  += -pipe -mlongcalls 
kernel-flags-y      += -nostdlib
endif