// interdevice CAN communication
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
#include "cancom.h"

/*
 * 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 */
static const CANConfig cancfg = {
	.mcr = CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP | CAN_MCR_NART,
	.btr = CAN_BTR_SJW(2) | CAN_BTR_TS2(1) |
			CAN_BTR_TS1(12) | CAN_BTR_BRP(5)
};

/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx_wa, 256);
static THD_FUNCTION(can_rx, p) {
	event_listener_t el;
	CANRxFrame rxmsg;

	palSetPadMode(GPIOI, 1, PAL_MODE_OUTPUT_PUSHPULL);

	(void)p;
	chRegSetThreadName("CANCom receiver");
	chEvtRegister(&CAND1.rxfull_event, &el, 0);
	while (true) {
		if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
			continue;
		while (canReceive(&CAND1, CAN_ANY_MAILBOX,
											&rxmsg, TIME_IMMEDIATE) == MSG_OK) {
			/* Process message.*/
			palTogglePad(GPIOI, 1);
		}
	}
	chEvtUnregister(&CAND1.rxfull_event, &el);
}

/*
 * Transmitter thread.
 */
static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p) {
	CANTxFrame txmsg;

	(void)p;
	chRegSetThreadName("CANCom transmitter");
	txmsg.IDE = CAN_IDE_EXT;
	txmsg.EID = 0x01234568;
	txmsg.RTR = CAN_RTR_DATA;
	txmsg.DLC = 8;
	txmsg.data32[0] = 0xDEADBEEF;
	txmsg.data32[1] = 0x00FF00FF;

	while (true) {
		canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
		chThdSleepMilliseconds(500);
	}
}

void cancom_init(void)
{
	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(9));
	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(9));
	canStart(&CAND1, &cancfg);

	chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 7,
										can_rx, NULL);
	chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7,
										can_tx, NULL);
}
