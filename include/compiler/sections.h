/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SECTIONS_H_
#define _SECTIONS_H_

#define __init      __section(".init.text") __cold
#define __initdata  __section(".init.data")
#define __initconst __section(".init.rodata")
#define __exitdata  __section(".exit.data")
#define __exit_call __section(".exit.exitcall")

#endif   /* _SECTIONS_H_ */
