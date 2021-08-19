/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <crc.h>
#include <crc_table.h>
#include <export.h>

uint8_t crc4(uint8_t *src, int len, uint8_t crc)
{
    len *= 2;
    while (len--)
        crc = crc4_table[(crc & 0xf) ^ *src++];
    return crc;
}
EXPORT_SYMBOL(crc4);

uint16_t crc16(uint8_t *src, int len, uint16_t crc)
{
    while (len--)
        crc = (crc >> 8) ^ crc16_table[(crc & 0xff) ^ *src++];
    return crc;
}
EXPORT_SYMBOL(crc16);

uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    while (len--)
        crc = (crc >> 8) ^ crc32_table[(crc & 0xff ) ^ *src++];
    return crc;
} 
EXPORT_SYMBOL(crc32);