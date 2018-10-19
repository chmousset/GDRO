// Copyright 2018 Charles-Henri Mousset
// interdevice CAN communication

#include "hal.h"
#include "cancom.h"

/*
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 */
static const CANConfig cancfg = {
	.mcr = CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
	.btr = CAN_BTR_LBKM | CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
			CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
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
	txmsg.EID = 0x01234567;
	txmsg.RTR = CAN_RTR_DATA;
	txmsg.DLC = 8;
	txmsg.data32[0] = 0x55AA55AA;
	txmsg.data32[1] = 0x00FF00FF;

	while (true) {
		canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
		chThdSleepMilliseconds(500);
	}
}

void cancom_init(void)
{
	canStart(&CAND1, &cancfg);

	chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 7,
										can_rx, NULL);
	chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7,
										can_tx, NULL);
}
