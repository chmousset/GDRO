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

#include "ch.h"
#include "flash.h"

int flash_wait_done(int timeout)
{ 	
	while(FLASH->SR & FLASH_SR_BSY)
	{
		if(timeout-- <= 0)
			return -1;
		chThdSleepMilliseconds(1);
	}

	if(FLASH->SR & (FLASH_SR_OPERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | \
		FLASH_SR_PGPERR | FLASH_SR_ERSERR ))
		return -1;
	return 0;
}

void flash_erase(uint32_t sector)
{
	FLASH->CR &= ~FLASH_CR_PSIZE;
	CLEAR_BIT(FLASH->CR, FLASH_CR_SNB);
	FLASH->CR |= FLASH_CR_SER | ((sector & 0xF) << 3);
	FLASH->CR |= FLASH_CR_STRT;
	
	__DSB();	// wait until the flash operation is done
}

void flash_unlock(void)
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2; 
}

void flash_lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;	
}

void flash_clear_err(void)
{
	FLASH->SR &= ~(FLASH_SR_OPERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | \
		FLASH_SR_PGPERR | FLASH_SR_ERSERR );
}

void flash_prog_byte(uint32_t address, uint8_t data)
{
	FLASH->CR &= ~FLASH_CR_PSIZE;
	FLASH->CR |= FLASH_CR_PG;

	*(__IO uint8_t*)address = data;

	__DSB();
}
