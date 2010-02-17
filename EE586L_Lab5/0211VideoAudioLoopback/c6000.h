/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : VideoAudioLoopback                Created  : 03-MAR-2009      */
/* File     : c6000.h                           Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */
#ifndef	_C6000_
#define _C6000_

#ifdef _C6701_
#define	CE0BASE		0x00400000	// CE0BASE Address register
#define	CE1BASE		0x01400000	// CE1BASE Address register
#define	CE2BASE		0x02000000	// CE2BASE Address register
#define	CE3BASE		0x03000000	// CE3BASE Address register
#endif

#ifdef _C6203_
#define	CE0BASE		0x00400000	// CE0BASE Address register
#define	CE1BASE		0x01400000	// CE1BASE Address register
#define	CE2BASE		0x02000000	// CE2BASE Address register
#define	CE3BASE		0x03000000	// CE3BASE Address register
#endif

#ifdef _C6711_
#define	CE0BASE		0x80000000	// CE0BASE Address register
#define	CE1BASE		0x90000000	// CE1BASE Address register
#define	CE2BASE		0xA0000000	// CE2BASE Address register
#define	CE3BASE		0xB0000000	// CE3BASE Address register
#endif

#ifdef _C6713_
#define	CE0BASE		0x80000000	// CE0BASE Address register
#define	CE1BASE		0x90000000	// CE1BASE Address register
#define	CE2BASE		0xA0000000	// CE2BASE Address register
#define	CE3BASE		0xB0000000	// CE3BASE Address register
#endif

#ifdef	_C6416_
#define	ACE0BASE	0x80000000	// ACE0BASE Address register
#define	ACE1BASE	0x90000000	// ACE1BASE Address register
#define	ACE2BASE	0xA0000000	// ACE2BASE Address register
#define	ACE3BASE	0xB0000000	// ACE3BASE Address register
#define	BCE0BASE	0x60000000	// BCE0BASE Address register
#define	BCE1BASE	0x64000000	// BCE1BASE Address register
#define	BCE2BASE	0x68000000	// BCE2BASE Address register
#define	BCE3BASE	0x6C000000	// BCE3BASE Address register
#endif

/* Phase Locked Loop Registers */
#define PLLCSR	 	*(volatile int *)0x01B7C100	// PLL Control/Status Register
#define PLLM	 	*(volatile int *)0x01B7C110	// PLL Multiplier control register
#define	PLLDIV0	 	*(volatile int *)0x01B7C114	// PLL Wrapper Divider 0 register
#define	PLLDIV1	 	*(volatile int *)0x01B7C118	// PLL Wrapper Divider 1 register
#define	PLLDIV2	 	*(volatile int *)0x01B7C11C	// PLL Wrapper Divider 2 register
#define	PLLDIV3	 	*(volatile int *)0x01B7C120	// PLL Wrapper Divider 3 register

/*  McBSP Registers & Address Map   */
#define DRR0     	*(volatile int *)0x018C0000	// Data Receive Register 0
#define DXR0     	*(volatile int *)0x018C0004	// Data Transmit Register 0
#define SPCR0    	*(volatile int *)0x018C0008	// Serial Port Control Register 0
#define RCR0     	*(volatile int *)0x018C000C	// Receive Control Register 0
#define XCR0     	*(volatile int *)0x018C0010	// Transmit Control Register 0
#define SRGR0    	*(volatile int *)0x018C0014	// Sample Rate Generator Register 0
#define MCR0     	*(volatile int *)0x018C0018	// Multichannel Control Register 0
#define RCER0    	*(volatile int *)0x018C001C	// Receive Channel Enable Register 0
#define XCER0    	*(volatile int *)0x018C0020	// Transmit Channel Enable Register 0
#define PCR0     	*(volatile int *)0x018C0024	// Pin Control Register 0

#define DRR1     	*(volatile int *)0x01900000	// Data Receive Register 1
#define DXR1     	*(volatile int *)0x01900004	// Data Transmit Register 1
#define SPCR1    	*(volatile int *)0x01900008	// Serial Port Control Register 1
#define RCR1     	*(volatile int *)0x0190000C	// Receive Control Register 1
#define XCR1     	*(volatile int *)0x01900010	// Transmit Control Register 1
#define SRGR1    	*(volatile int *)0x01900014	// Sample Rate Generator Register 1
#define MCR1     	*(volatile int *)0x01900018	// Multichannel Control Register 1
#define RCER1    	*(volatile int *)0x0190001C	// Receive Channel Enable Register 1
#define XCER1    	*(volatile int *)0x01900020	// Transmit Channel Enable Register 1
#define PCR1     	*(volatile int *)0x01900024	// Pin Control Register 1

#ifdef _C6203_
#define DRR2     	*(volatile int *)0x01A40000
#define DXR2     	*(volatile int *)0x01A40004
#define SPCR2    	*(volatile int *)0x01A40008
#define RCR2     	*(volatile int *)0x01A4000C
#define XCR2     	*(volatile int *)0x01A40010
#define SRGR2    	*(volatile int *)0x01A40014
#define MCR2     	*(volatile int *)0x01A40018
#define RCER2    	*(volatile int *)0x01A4001C
#define XCER2    	*(volatile int *)0x01A40020
#define PCR2     	*(volatile int *)0x01A40024
#endif

#ifdef _C6416_
#define DRR2     	*(volatile int *)0x01A40000
#define DXR2     	*(volatile int *)0x01A40004
#define SPCR2    	*(volatile int *)0x01A40008
#define RCR2     	*(volatile int *)0x01A4000C
#define XCR2     	*(volatile int *)0x01A40010
#define SRGR2    	*(volatile int *)0x01A40014
#define MCR2     	*(volatile int *)0x01A40018
#define RCER2    	*(volatile int *)0x01A4001C
#define XCER2    	*(volatile int *)0x01A40020
#define PCR2     	*(volatile int *)0x01A40024
#endif

/*	EMIF Registers	*/
#ifdef _C6416_
#define ACE1CTL 	*(volatile int *)0x01800004	// EMIF ACE1 space control register
#define ACE0CTL 	*(volatile int *)0x01800008	// EMIF ACE0 space control register
#define ACE2CTL 	*(volatile int *)0x01800010	// EMIF ACE2 space control register
#define ACE3CTL 	*(volatile int *)0x01800014	// EMIF ACE3 space control register
#define ASDCTL		*(volatile int *)0x01800018	// EMIF ASDRAM control register
#define ASDTIM		*(volatile int *)0x0180001C	// EMIF ASDRAM timing register
#define BCE1CTL 	*(volatile int *)0x01A80004	// EMIF BCE1 space control register
#define BCE0CTL 	*(volatile int *)0x01A80008	// EMIF BCE0 space control register
#define BCE2CTL 	*(volatile int *)0x01A80010	// EMIF BCE2 space control register
#define BCE3CTL 	*(volatile int *)0x01A80014	// EMIF BCE3 space control register
#define BSDCTL		*(volatile int *)0x01A80018	// EMIF BSDRAM control register
#else
#define CE1CTL 		*(volatile int *)0x01800004	// EMIF CE1 space control register
#define CE0CTL 		*(volatile int *)0x01800008	// EMIF CE0 space control register
#define CE2CTL	 	*(volatile int *)0x01800010	// EMIF CE2 space control register
#define CE3CTL	 	*(volatile int *)0x01800014	// EMIF CE3 space control register
#define SDCTL		*(volatile int *)0x01800018	// EMIF SDRAM control register
#define SDTIM		*(volatile int *)0x0180001C	// EMIF SDRAM timing register
#endif

/*	Timer Registers	*/
#define	CTL0		*(volatile int *)0x01940000	// Timer 0 control register
#define	PRD0		*(volatile int *)0x01940004	// Timer 0 period register
#define	CNT0		*(volatile int *)0x01940008	// Timer 0 counter register
#define	CTL1		*(volatile int *)0x01980000	// Timer 1 control register
#define	PRD1		*(volatile int *)0x01980004	// Timer 1 period register
#define	CNT1		*(volatile int *)0x01980008	// Timer 1 counter register
#ifdef _C6416_
#define	CTL2		*(volatile int *)0x01AC0000	// Timer 2 control register
#define	PRD2		*(volatile int *)0x01AC0004	// Timer 2 period register
#define	CNT2		*(volatile int *)0x01AC0008	// Timer 2 counter register
#endif

/*	Interrupt Selector Registers	*/
#define MUXH	 	*(volatile int *)0x019c0000	// Interrupt multiplexer high register
#define MUXL	 	*(volatile int *)0x019c0004	// Interrupt multiplexer low register
#define	EXTPOL		*(volatile int *)0x019c0008	// External interrupt polarity register

#define INT4		0
#define INT5		5
#define INT6		10
#define INT7		16
#define INT8		21
#define INT9		26
#define INT10		32
#define INT11		37
#define INT12		42
#define INT13		48
#define INT14		53
#define INT15		58

/*	Available Interrupt	Source	*/
#ifdef _C6701_
#define	DSPINT		0x0		// Host processor to DSP interrupt
#define	TINT0		0x1		// Timer 0 interrupt
#define	TINT1		0x2		// Timer 1 interrupt
#define	SD_INT		0x3		// EMIF SDRAM timer interrupt
#define EXT_INT4	0x4		// External interrupt pin 4
#define EXT_INT5	0x5		// External interrupt pin 5
#define EXT_INT6	0x6		// External interrupt pin 6
#define EXT_INT7	0x7		// External interrupt pin 7
#define DMA_INT0	0x8		// DMA channel 0 interrupt
#define DMA_INT1	0x9		// DMA channel 1 interrupt
#define DMA_INT2	0xa		// DMA channel 2 interrupt
#define DMA_INT3	0xb		// DMA channel 3 interrupt
#define	XINT0		0xc		// McBSP 0 transmit interrupt
#define	RINT0		0xd		// McBSP 0 receive interrupt
#define	XINT1		0xe		// McBSP 1 transmit interrupt
#define	RINT1		0xf		// McBSP 1 receive interrupt
#endif

#ifdef _C6203_
#define	DSPINT		0x0		// Host processor to DSP interrupt
#define	TINT0		0x1		// Timer 0 interrupt
#define	TINT1		0x2		// Timer 1 interrupt
#define	SD_INT		0x3		// EMIF SDRAM timer interrupt
#define EXT_INT4	0x4		// External interrupt pin 4
#define EXT_INT5	0x5		// External interrupt pin 5
#define EXT_INT6	0x6		// External interrupt pin 6
#define EXT_INT7	0x7		// External interrupt pin 7
#define DMA_INT0	0x8		// DMA channel 0 interrupt
#define DMA_INT1	0x9		// DMA channel 1 interrupt
#define DMA_INT2	0xa		// DMA channel 2 interrupt
#define DMA_INT3	0xb		// DMA channel 3 interrupt
#define	XINT0		0xc		// McBSP 0 transmit interrupt
#define	RINT0		0xd		// McBSP 0 receive interrupt
#define	XINT1		0xe		// McBSP 1 transmit interrupt
#define	RINT1		0xf		// McBSP 1 receive interrupt
#define	XINT2		0x11	// McBSP 2 transmit interrupt
#define	RINT2		0x12	// McBSP 2 receive interrupt
#endif

#ifdef _C6711_
#define	DSPINT		0x0		// Host Processer to DSP interrupt
#define	TINT0		0x1		// Timer 0 interrupt
#define	TINT1		0x2		// Timer 1 interrupt
#define	SD_INT		0x3		// EMIF SDRAM timer interrupt
#define EXT_INT4	0x4		// External interrupt pin 4
#define EXT_INT5	0x5		// External interrupt pin 5
#define EXT_INT6	0x6		// External interrupt pin 6
#define EXT_INT7	0x7		// External interrupt pin 7
#define EDMA_INT	0x8		// EDMA channel (0 through 15) interrupt
#define	XINT0		0xc		// McBSP 0 transmit interrupt
#define	RINT0		0xd		// McBSP 0 receive interrupt
#define	XINT1		0xe		// McBSP 1 transmit interrupt
#define	RINT1		0xf		// McBSP 1 receive interrupt
#endif

#ifdef _C6713_
#define	DSPINT		0x0		// Host Processer to DSP interrupt
#define	TINT0		0x1		// Timer 0 interrupt
#define	TINT1		0x2		// Timer 1 interrupt
#define	SD_INT		0x3		// EMIF SDRAM timer interrupt
#define EXT_INT4	0x4		// External interrupt pin 4
#define EXT_INT5	0x5		// External interrupt pin 5
#define EXT_INT6	0x6		// External interrupt pin 6
#define EXT_INT7	0x7		// External interrupt pin 7
#define EDMA_INT	0x8		// EDMA channel (0 through 15) interrupt
#define	XINT0		0xc		// McBSP 0 transmit interrupt
#define	RINT0		0xd		// McBSP 0 receive interrupt
#define	XINT1		0xe		// McBSP 1 transmit interrupt
#define	RINT1		0xf		// McBSP 1 receive interrupt
#endif

#ifdef _C6416_
#define	DSPINT		0x0		// Host Processer to DSP interrupt
#define	TINT0		0x1		// Timer 0 interrupt
#define	TINT1		0x2		// Timer 1 interrupt
#define	SD_INTA		0x3		// EMIFA SDRAM timer interrupt
#define EXT_INT4	0x4		// GPIO interrupt 4/External interrupt pin 4
#define EXT_INT5	0x5		// GPIO interrupt 5/External interrupt pin 5
#define EXT_INT6	0x6		// GPIO interrupt 6/External interrupt pin 6
#define EXT_INT7	0x7		// GPIO interrupt 7/External interrupt pin 7
#define EDMA_INT	0x8		// EDMA channel (0 through 63) interrupt
#define	XINT0		0xc		// McBSP 0 transmit interrupt
#define	RINT0		0xd		// McBSP 0 receive interrupt
#define	XINT1		0xe		// McBSP 1 transmit interrupt
#define	RINT1		0xf		// McBSP 1 receive interrupt
#define	GPINT0		0x10	// GPIO interrupt 0
#define	XINT2		0x11	// McBSP 2 transmit interrupt
#define	RINT2		0x12	// McBSP 2 receive interrupt
#define	TINT2		0x13	// Timer 2 interrupt
#define	SD_INTB		0x14	// EMIFB SDRAM timer interrupt
#define PCI_WAKEUP	0x15	// PCI wakeup interrupt
#define UINT		0x17	// UTOPIA interrupt
#endif


/*	DMA Registers	*/
#ifdef _C6701_
#define DMA_PRICTL0		*(volatile int *)0x01840000	// DMA channel 0 priority control register
#define DMA_SECCTL0		*(volatile int *)0x01840008	// DMA channel 0 secondary control register
#define DMA_SRC0		*(volatile int *)0x01840010	// DMA channel 0 source address
#define DMA_DST0		*(volatile int *)0x01840018	// DMA channel 0 destination address
#define DMA_XFRCNT0		*(volatile int *)0x01840020	// DMA channel 0 transfer counter
#define DMA_PRICTL1		*(volatile int *)0x01840040	// DMA channel 1 priority control register
#define DMA_SECCTL1		*(volatile int *)0x01840048	// DMA channel 1 secondary control register
#define DMA_SRC1		*(volatile int *)0x01840050	// DMA channel 1 source address
#define DMA_DST1		*(volatile int *)0x01840058	// DMA channel 1 destination address
#define DMA_XFRCNT1		*(volatile int *)0x01840060	// DMA channel 1 transfer counter
#define DMA_PRICTL2		*(volatile int *)0x01840004	// DMA channel 2 priority control register
#define DMA_SECCTL2		*(volatile int *)0x0184000C	// DMA channel 2 secondary control register
#define DMA_SRC2		*(volatile int *)0x01840014	// DMA channel 2 source address
#define DMA_DST2		*(volatile int *)0x0184001C	// DMA channel 2 destination address
#define DMA_XFRCNT2		*(volatile int *)0x01840024	// DMA channel 2 transfer counter
#define DMA_PRICTL3		*(volatile int *)0x01840044	// DMA channel 3 priority control register
#define DMA_SECCTL3		*(volatile int *)0x0184004C	// DMA channel 3 secondary control register
#define DMA_SRC3		*(volatile int *)0x01840054	// DMA channel 3 source address
#define DMA_DST3		*(volatile int *)0x0184005C	// DMA channel 3 destination address
#define DMA_XFRCNT3		*(volatile int *)0x01840064	// DMA channel 3 transfer counter
#define	DMA_AUXCTL		*(volatile int *)0x01840070	// DMA auxiliary control register
#define	DMA_GBLADDRA	*(volatile int *)0x01840038	// DMA global address register A
#define	DMA_GBLADDRB	*(volatile int *)0x0184003C	// DMA global address register B
#define	DMA_GBLADDRC	*(volatile int *)0x01840068	// DMA global address register C
#define	DMA_GBLADDRD	*(volatile int *)0x0184006C	// DMA global address register D
#define	DMA_GBLCNTA		*(volatile int *)0x01840028	// DMA global count reload register A
#define	DMA_GBLCNTB		*(volatile int *)0x0184002C	// DMA global count reload register B
#define	DMA_GBLIDXA		*(volatile int *)0x01840030	// DMA global index register A
#define	DMA_GBLIDXB		*(volatile int *)0x01840034	// DMA global index register B
#endif

#ifdef _C6203_
#define DMA_PRICTL0		*(volatile int *)0x01840000	// DMA channel 0 priority control register
#define DMA_SECCTL0		*(volatile int *)0x01840008	// DMA channel 0 secondary control register
#define DMA_SRC0		*(volatile int *)0x01840010	// DMA channel 0 source address
#define DMA_DST0		*(volatile int *)0x01840018	// DMA channel 0 destination address
#define DMA_XFRCNT0		*(volatile int *)0x01840020	// DMA channel 0 transfer counter
#define DMA_PRICTL1		*(volatile int *)0x01840040	// DMA channel 1 priority control register
#define DMA_SECCTL1		*(volatile int *)0x01840048	// DMA channel 1 secondary control register
#define DMA_SRC1		*(volatile int *)0x01840050	// DMA channel 1 source address
#define DMA_DST1		*(volatile int *)0x01840058	// DMA channel 1 destination address
#define DMA_XFRCNT1		*(volatile int *)0x01840060	// DMA channel 1 transfer counter
#define DMA_PRICTL2		*(volatile int *)0x01840004	// DMA channel 2 priority control register
#define DMA_SECCTL2		*(volatile int *)0x0184000C	// DMA channel 2 secondary control register
#define DMA_SRC2		*(volatile int *)0x01840014	// DMA channel 2 source address
#define DMA_DST2		*(volatile int *)0x0184001C	// DMA channel 2 destination address
#define DMA_XFRCNT2		*(volatile int *)0x01840024	// DMA channel 2 transfer counter
#define DMA_PRICTL3		*(volatile int *)0x01840044	// DMA channel 3 priority control register
#define DMA_SECCTL3		*(volatile int *)0x0184004C	// DMA channel 3 secondary control register
#define DMA_SRC3		*(volatile int *)0x01840054	// DMA channel 3 source address
#define DMA_DST3		*(volatile int *)0x0184005C	// DMA channel 3 destination address
#define DMA_XFRCNT3		*(volatile int *)0x01840064	// DMA channel 3 transfer counter
#define	DMA_AUXCTL		*(volatile int *)0x01840070	// DMA auxiliary control register
#define	DMA_GBLADDRA	*(volatile int *)0x01840038	// DMA global address register A
#define	DMA_GBLADDRB	*(volatile int *)0x0184003C	// DMA global address register B
#define	DMA_GBLADDRC	*(volatile int *)0x01840068	// DMA global address register C
#define	DMA_GBLADDRD	*(volatile int *)0x0184006C	// DMA global address register D
#define	DMA_GBLCNTA		*(volatile int *)0x01840028	// DMA global count reload register A
#define	DMA_GBLCNTB		*(volatile int *)0x0184002C	// DMA global count reload register B
#define	DMA_GBLIDXA		*(volatile int *)0x01840030	// DMA global index register A
#define	DMA_GBLIDXB		*(volatile int *)0x01840034	// DMA global index register B
#endif

/* EDMA Registers */
#define PaRAM_OPT	0	// Options
#define PaRAM_SRC	1	// Source Address
#define PaRAM_CNT	2	// Frame count, Element count
#define PaRAM_DST	3	// Destination Address
#define PaRAM_IDX	4	// Frame index, Element index
#define PaRAM_RDL	5	// Element count reload, Link address

#ifdef _C6711_
#define	EDMA_PQSR	*(volatile int *)0x01A0FFE0	// EDMA Priority queue status register
#define	EDMA_CIPR	*(volatile int *)0x01A0FFE4	// EDMA Channel interrupt pending register
#define	EDMA_CIER	*(volatile int *)0x01A0FFE8	// EDMA Channel interrupt enable register
#define	EDMA_CCER	*(volatile int *)0x01A0FFEC	// EDMA Channel chain enable register
#define	EDMA_ER		*(volatile int *)0x01A0FFF0	// EDMA Event register
#define	EDMA_EER	*(volatile int *)0x01A0FFF4	// EDMA Event enable register
#define	EDMA_ECR	*(volatile int *)0x01A0FFF8	// EDMA Event clear register
#define	EDMA_ESR	*(volatile int *)0x01A0FFFC	// EDMA Event set register
#endif

#ifdef _C6713_
#define	EDMA_PQSR	*(volatile int *)0x01A0FFE0	// EDMA Priority queue status register
#define	EDMA_CIPR	*(volatile int *)0x01A0FFE4	// EDMA Channel interrupt pending register 
#define	EDMA_CIER	*(volatile int *)0x01A0FFE8	// EDMA Channel interrupt enable register
#define	EDMA_CCER	*(volatile int *)0x01A0FFEC	// EDMA Channel chain enable register
#define	EDMA_ER		*(volatile int *)0x01A0FFF0	// EDMA Event register
#define	EDMA_EER	*(volatile int *)0x01A0FFF4	// EDMA Event enable register
#define	EDMA_ECR	*(volatile int *)0x01A0FFF8	// EDMA Event clear register
#define	EDMA_ESR	*(volatile int *)0x01A0FFFC	// EDMA Event set register
#endif

#ifdef _C6416_
#define	EDMA_EPRL	*(volatile int *)0x01A0FFDC	// EDMA Polarity low register
#define	EDMA_EPRH	*(volatile int *)0x01A0FF9C	// EDMA Polarity high register
#define	EDMA_PQAR0	*(volatile int *)0x01A0FFC0	// EDMA Priority queue allocation register 0
#define	EDMA_PQAR1	*(volatile int *)0x01A0FFC4	// EDMA Priority queue allocation register 1
#define	EDMA_PQAR2	*(volatile int *)0x01A0FFC8	// EDMA Priority queue allocation register 2
#define	EDMA_PQAR3	*(volatile int *)0x01A0FFCC	// EDMA Priority queue allocation register 3
#define	EDMA_PQSR	*(volatile int *)0x01A0FFE0	// EDMA Priority queue status register
#define	EDMA_CIPRL	*(volatile int *)0x01A0FFE4	// EDMA Channel interrupt pending low register
#define	EDMA_CIPRH	*(volatile int *)0x01A0FFA4	// EDMA Channel interrupt pending high register
#define	EDMA_CIERL	*(volatile int *)0x01A0FFE8	// EDMA Channel interrupt enable low register
#define	EDMA_CIERH	*(volatile int *)0x01A0FFA8	// EDMA Channel interrupt enable high register
#define	EDMA_CCERL	*(volatile int *)0x01A0FFEC	// EDMA Channel chain enable low register
#define	EDMA_CCERH	*(volatile int *)0x01A0FFAC	// EDMA Channel chain enable high register
#define	EDMA_ERL	*(volatile int *)0x01A0FFF0	// EDMA Event low register
#define	EDMA_ERH	*(volatile int *)0x01A0FFB0	// EDMA Event high register
#define	EDMA_EERL	*(volatile int *)0x01A0FFF4	// EDMA Event enable low register
#define	EDMA_EERH	*(volatile int *)0x01A0FFB4	// EDMA Event enable high register
#define	EDMA_ECRL	*(volatile int *)0x01A0FFF8	// EDMA Event clear low register
#define	EDMA_ECRH	*(volatile int *)0x01A0FFB8	// EDMA Event clear high register
#define	EDMA_ESRL	*(volatile int *)0x01A0FFFC	// EDMA Event set low register
#define	EDMA_ESRH	*(volatile int *)0x01A0FFBC	// EDMA Event set high register
#endif

/*	DMA Channel Synchronization Events	*/
#ifdef _C6701_
#define	XEVT0		0xc		// McBSP 0 transmit interrupt
#define	REVT0		0xd		// McBSP 0 receive interrupt
#define	XEVT1		0xe		// McBSP 1 transmit interrupt
#define	REVT1		0xf		// McBSP 1 receive interrupt
#endif

#ifdef _C6203_
#define	XEVT0		0xc		// McBSP 0 transmit interrupt
#define	REVT0		0xd		// McBSP 0 receive interrupt
#define	XEVT1		0xe		// McBSP 1 transmit interrupt
#define	REVT1		0xf		// McBSP 1 receive interrupt
#define	XEVT2		0x11	// McBSP 2 transmit interrupt
#define	REVT2		0x12	// McBSP 2 receive interrupt
#endif
/*	EDMA Channel Synchronization Events	*/
#ifdef _C6711_
#define	XEVT0		0xc		// McBSP 0 transmit interrupt
#define	REVT0		0xd		// McBSP 0 receive interrupt
#define	XEVT1		0xe		// McBSP 1 transmit interrupt
#define	REVT1		0xf		// McBSP 1 receive interrupt
#endif

#ifdef _C6713_
#define	XEVT0		0xc		// McBSP 0 transmit interrupt
#define	REVT0		0xd		// McBSP 0 receive interrupt
#define	XEVT1		0xe		// McBSP 1 transmit interrupt
#define	REVT1		0xf		// McBSP 1 receive interrupt
#endif

#ifdef _C6416_
#define GPEVT0		0x8		// GPIO event 0
#define GPEVT1		0x9		// GPIO event 1
#define GPEVT2		0xa		// GPIO event 2
#define GPEVT3		0xb		// GPIO event 3
#define	XEVT0		0xc		// McBSP 0 transmit interrupt
#define	REVT0		0xd		// McBSP 0 receive interrupt
#define	XEVT1		0xe		// McBSP 1 transmit interrupt
#define	REVT1		0xf		// McBSP 1 receive interrupt
#define	XEVT2		0x11	// McBSP 2 transmit interrupt
#define	REVT2		0x12	// McBSP 2 receive interrupt
#define PCI			0x15	// PCI wakeup interrupt
#define UREVT		0x20	// Utopia receive event
#define UXEVT		0x28	// Utopia transmit event
#define GPEVT8		0x30	// GPIO event 8
#define GPEVT9		0x31	// GPIO event 9
#define GPEVT10		0x32	// GPIO event 10
#define GPEVT11		0x33	// GPIO event 11
#define GPEVT12		0x34	// GPIO event 12
#define GPEVT13		0x35	// GPIO event 13
#define GPEVT14		0x36	// GPIO event 14
#define GPEVT15		0x37	// GPIO event 15
#endif

/*	QDMA Registers	*/
#ifdef	_C6711_
#define	QDMA_OPT	*(volatile int *)0x02000000	// QDMA Options
#define	QDMA_SRC	*(volatile int *)0x02000004	// Source address
#define	QDMA_CNT	*(volatile int *)0x02000008	// Array/frame, Element count
#define	QDMA_DST	*(volatile int *)0x0200000c // Destinaton address
#define	QDMA_IDX	*(volatile int *)0x02000010 // Array/frame, Element index
#define	QDMA_S_OPT	*(volatile int *)0x02000020	// QDMA Options
#define	QDMA_S_SRC	*(volatile int *)0x02000024	// Source address
#define	QDMA_S_CNT	*(volatile int *)0x02000028	// Array/frame, Element count
#define	QDMA_S_DST	*(volatile int *)0x0200002c // Destinaton address
#define	QDMA_S_IDX	*(volatile int *)0x02000030 // Array/frame, Element index
#endif

#ifdef	_C6713_
#define	QDMA_OPT	*(volatile int *)0x02000000
#define	QDMA_SRC	*(volatile int *)0x02000004
#define	QDMA_CNT	*(volatile int *)0x02000008
#define	QDMA_DST	*(volatile int *)0x0200000c
#define	QDMA_IDX	*(volatile int *)0x02000010
#define	QDMA_S_OPT	*(volatile int *)0x02000020
#define	QDMA_S_SRC	*(volatile int *)0x02000024
#define	QDMA_S_CNT	*(volatile int *)0x02000028
#define	QDMA_S_DST	*(volatile int *)0x0200002c
#define	QDMA_S_IDX	*(volatile int *)0x02000030
#endif

#ifdef	_C6416_
#define	QDMA_OPT	*(volatile int *)0x02000000
#define	QDMA_SRC	*(volatile int *)0x02000004
#define	QDMA_CNT	*(volatile int *)0x02000008
#define	QDMA_DST	*(volatile int *)0x0200000c
#define	QDMA_IDX	*(volatile int *)0x02000010
#define	QDMA_S_OPT	*(volatile int *)0x02000020
#define	QDMA_S_SRC	*(volatile int *)0x02000024
#define	QDMA_S_CNT	*(volatile int *)0x02000028
#define	QDMA_S_DST	*(volatile int *)0x0200002c
#define	QDMA_S_IDX	*(volatile int *)0x02000030
#endif

#endif	//_C6000_
