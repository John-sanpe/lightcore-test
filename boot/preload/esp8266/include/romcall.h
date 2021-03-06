#ifndef _ROMCALL_H_
#define _ROMCALL_H_

#include <types.h>
#include <stdarg.h>

typedef struct {
    uint32_t device_id;
    uint32_t chip_size;    // chip size in bytes
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;
    uint32_t status_mask;
} esp_rom_spiflash_chip_t;

void uart_div_modify(uint32_t uart_no, uint32_t baud_div);
int ets_io_vprintf(int (*putc)(int), const char* fmt, va_list ap);
void system_soft_wdt_feed();

int SPI_page_program(esp_rom_spiflash_chip_t *chip,  uint32_t dst_addr, void *pbuf, uint32_t len);
int SPI_read_data(esp_rom_spiflash_chip_t *chip,  uint32_t dst_addr, void *pbuf, uint32_t len);
int SPI_write_enable(esp_rom_spiflash_chip_t *chip);
int SPI_sector_erase(esp_rom_spiflash_chip_t *chip, uint32_t sect_addr);
int SPI_write_status(esp_rom_spiflash_chip_t *chip, uint32_t status);
int SPI_read_status(esp_rom_spiflash_chip_t *chip, uint32_t *status);
int Enable_QMode(esp_rom_spiflash_chip_t *chip);

int SPIWrite(uint32_t addr, const uint8_t *src, uint32_t size);
int SPIRead(uint32_t addr, void *dst, uint32_t size);
int SPIEraseSector(uint32_t sector_num);
uint32_t Wait_SPI_Idle();

void Cache_Read_Disable();
void Cache_Read_Enable(uint8_t map, uint8_t p, uint8_t v);
void Cache_Read_Enable_New();

#endif /* _ROMCALL_H_ */
