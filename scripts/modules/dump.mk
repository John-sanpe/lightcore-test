# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Make dump
# ==========================================================================

# Include dump rule
include $(build_home)/modules/dump_rule.mk

dump-flags-y ?= --source --demangle --disassemble \
                --line-numbers --reloc --syms --wide

quiet_cmd_build_dump = $(ECHO_DUMP) $@
      cmd_build_dump = $(OBJDUMP)                           \
                       $(dump-flags-y)                      \
                       $($(@F)-flags-y)                     \
                       $(addprefix $(obj)/,$($(@F)-obj))    \
                       > $@
$(dump): FORCE
	$(call if_changed,build_dump)
$(call multi_depend, $(dump), , -obj)
