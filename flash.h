// Copyright 2018 Charles-Henri Mousset
// Very simple and unefficient FLASH memory driver

#ifndef __FLASH_H
#define __FLASH_H

#define FLASH_KEY1 ((uint32_t)0x45670123U)
#define FLASH_KEY2 ((uint32_t)0xCDEF89ABU)

void flash_unlock(void);
void flash_lock(void);
void flash_clear_err(void);
void flash_erase(uint32_t sector);
int flash_wait_done(int timeout);
void flash_prog_byte(uint32_t address, uint8_t data);

#endif
