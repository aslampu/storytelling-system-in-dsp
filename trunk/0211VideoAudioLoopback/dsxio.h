/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : VideoAudioLoopback                Created  : 03-MAR-2009      */
/* File     : dsxio.h                           Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */
#ifndef _DSXIO_
#define _DSXIO_

#define	RXRDY		0x10		// RS232 Receive ready bit set
#define	TXRDY		0x20		// RS232 Transmitter ready bit set
#define PAGE 		0xB8		// LCD Row PAGE bit set
#define Column 		0x40		// LCD Column bit set
#define R4K 		0x2			// Sample rate bit set to 4KHz
#define R8K 		0x0			// Sample rate bit set to 8KHz
#define R16K		0x1			// Sample rate bit set to 16KHz

#ifdef _C6416_
#define	FLASHBASE	(BCE1BASE+0x00000000)	// FLASHBASE Address register
#define IOBASE		(BCE2BASE+0x00000000)	// IO Base Address register
#define	UIOBASE		(ACE1BASE+0x00380000)	// User IO Base Address register
#define EIOBASE		(ACE1BASE+0x003A0000)	// External IO Base Address register
#else
#define	FLASHBASE	(CE1BASE+0x00000000)
#define IOBASE		(CE1BASE+0x00200000)
#define	UIOBASE		(CE1BASE+0x00380000)
#define EIOBASE		(CE1BASE+0x003A0000)
#endif

#define RS232C		*(volatile short *)(IOBASE+0x00000000)	// RS232 Control register
#define RS232D		*(volatile short *)(IOBASE+0x00000004)	// RS232 Data register

#define ADC 		*(volatile short *)(UIOBASE+0x0000000C)	// ADC control register
#define LED 		*(volatile short *)(UIOBASE+0x00000014)	// LED control register
#define LCDLD		*(volatile short *)(UIOBASE+0x0000001C)	// LCD left date register
#define LCDLI		*(volatile short *)(UIOBASE+0x00000024)	// LCD left instruction register
#define LCDRD		*(volatile short *)(UIOBASE+0x0000002C)	// LCD right date register
#define LCDRI		*(volatile short *)(UIOBASE+0x00000030)	// LCD right instruction register
#define PWM 		*(volatile short *)(UIOBASE+0x00000034)	// PWM conttrol register
#define MOTOR 		*(volatile short *)(UIOBASE+0x0000003C)	// MOTOR control register
#define SW			*(volatile short *)(UIOBASE+0x00000040)	// Switch control register 
#define DIN			*(volatile short *)(UIOBASE+0x00000040)	// Digital IO Input register
#define DOUT 		*(volatile short *)(UIOBASE+0x00000040)	// Digital IO Output register
#define CODECRATE 	*(volatile short *)(UIOBASE+0x00000044)	// Codec sample rate register

#define	VM480CNTL	*(volatile short *)(EIOBASE+0x00000000) // VM480 Control register
#define	VM480DATA	*(volatile short *)(EIOBASE+0x00000004) // VM480 Data register
#define	VM480ADDL	*(volatile short *)(EIOBASE+0x00000008) // VM480 Low address register
#define	VM480ADDH	*(volatile short *)(EIOBASE+0x0000000C) // VM480 High address register

#define CHIP_6713 1
#endif //_DSXIO_
