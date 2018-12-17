// DRO display for iGaging scales
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

#include "hal.h"
#include "scale_qei.h"

void rst_tim(GPTDriver *drv)
{
	if (drv == &GPTD8)
	{
		rccEnableTIM8(true);
		rccResetTIM8();
	}
}

void init_qei(GPTDriver *drv, uint8_t filter)
{
	rst_tim(drv);
	drv->tim->CR1  = 0;                      // Initially stopped
	// drv->tim->CR2  = TIM_CR2_CCDS;           // DMA on UE (if any)
	drv->tim->PSC  = 0;
	drv->tim->DIER = 0;
	drv->tim->EGR  = TIM_EGR_UG;             // Re-initialize the counter
	drv->tim->CNT  = 0;                      // Reset counter
	drv->tim->CCMR1 = (TIM_CCMR1_IC1F & (filter << 4));
	drv->tim->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
	drv->tim->SR   = 0;                      // Clear pending IRQs (if any)
	drv->tim->CCER = 0;
	// drv->tim->CCMR2 = TIM_CCMR2_CC3S_0;		// capture CC3 on CH3, 2 samples filter
	// drv->tim->CCER = TIM_CCER_CC3E;			// rising edge
	drv->tim->CR1  = TIM_CR1_URS | TIM_CR1_CEN;		// start the counter
}

void scale_qei_init(SCALEDriver *drvs)
{
	(void) drvs;
	palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(3) | PAL_MODE_INPUT_PULLUP);
	palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(3) | PAL_MODE_INPUT_PULLUP);
	init_qei(&GPTD8, 3);
}
