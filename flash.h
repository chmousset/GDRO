// Very simple and unefficient FLASH memory driver
// Copyright (C) 2018 Charles-Henri Mousset - ch.mousset@gmail.com
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3 of the License
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
