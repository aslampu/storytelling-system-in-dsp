#ifndef INC_QDMA2_H
#define INC_QDMA2_H

//#define		CE2CTL		*(volatile int *)(0x01800010)
#define		ACE2CTL		*(volatile int *)(0x01800010)
// Definitions for async access(change as you wish)
#define		WSU			(2<<28)	 // Write Setup : 0-15
#define		WST			(8<<22)  // Write Strobe: 0-63
#define		WHD			(2<<20)	 // Write Hold  : 0-3
#define		RSU			(2<<16)	 // Read Setup  : 0-15
#define		TA			(3<<14)	 // Turn Around : 0-3
#define		RST			(8<<8)	 // Read Strobe : 0-63
#define		RHD			(2<<0)	 // Read Hold   : 0-3
#define		MTYPE		(2<<4)

/* QDMA Registers */
#define		QDMA_OPT	*(volatile int *)(0x02000000)
#define		QDMA_SRC	*(volatile int *)(0x02000004)
#define		QDMA_CNT	*(volatile int *)(0x02000008)
#define		QDMA_DST	*(volatile int *)(0x0200000c)
#define		QDMA_IDX	*(volatile int *)(0x02000010)
#define		QDMA_S_OPT	*(volatile int *)(0x02000020)
#define		QDMA_S_SRC	*(volatile int *)(0x02000024)
#define		QDMA_S_CNT	*(volatile int *)(0x02000028)
#define		QDMA_S_DST	*(volatile int *)(0x0200002c)
#define		QDMA_S_IDX	*(volatile int *)(0x02000030)

/* EDMA Registers */
#define		PaRAM_OPT	0	// Options
#define		PaRAM_SRC	1	// Source Address
#define		PaRAM_CNT	2	// Frame count, Element count
#define		PaRAM_DST	3	// Destination Address
#define		PaRAM_IDX	4	// Frame index, Element index
#define		PaRAM_RDL	5	// Element count reload, Link address
#define		EDMA_CIPRL	*(volatile int *)0x01A0FFE4	// EDMA Channel interrupt pending low register
#define		EDMA_CIPRH	*(volatile int *)0x01A0FFA4	// EDMA Channel interrupt pending high register
#define		EDMA_CIERL	*(volatile int *)0x01A0FFE8	// EDMA Channel interrupt enable low register
#define		EDMA_CIERH	*(volatile int *)0x01A0FFA8	// EDMA Channel interrupt enable high register
#define		EDMA_ERL	*(volatile int *)0x01A0FFF0	// EDMA Event low register
#define		EDMA_ERH	*(volatile int *)0x01A0FFB0	// EDMA Event high register
#define		EDMA_EERL	*(volatile int *)0x01A0FFF4	// EDMA Event enable low register
#define		EDMA_EERH	*(volatile int *)0x01A0FFB4	// EDMA Event enable high register
#define		EDMA_ECRL	*(volatile int *)0x01A0FFF8	// EDMA Event clear low register
#define		EDMA_ECRH	*(volatile int *)0x01A0FFB8	// EDMA Event clear high register
#define		EDMA_ESRL	*(volatile int *)0x01A0FFFC	// EDMA Event set low register
#define		EDMA_ESRH	*(volatile int *)0x01A0FFBC	// EDMA Event set high register
//#define		EDMA_CIPR	*(volatile int *)0x01A0FFE4	// EDMA Channel interrupt pending low register
//#define		EDMA_CIER	*(volatile int *)0x01A0FFE8	// EDMA Channel interrupt enable low register
//#define		EDMA_ER		*(volatile int *)0x01A0FFF0	// EDMA Event low register
//#define		EDMA_EER	*(volatile int *)0x01A0FFF4	// EDMA Event enable low register
//#define		EDMA_ECR	*(volatile int *)0x01A0FFF8	// EDMA Event clear low register
//#define		EDMA_ESR	*(volatile int *)0x01A0FFFC	// EDMA Event set low register

#define 	PRI			(2<<29)	// 1:High priority, 2:Low priority
#define 	ESIZE		(1<<27)	// 0:32bit, 1:16bit, 2:8bit, 3:reserved
#define 	DS2			(0<<26)	// 0:1-Dimensional
#define 	SUM			(0<<24)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	DD2			(1<<23)	// 0:1-Dimensional
#define 	DUM			(1<<21)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	TCINT		(1<<20)	// 1:enable
#define 	TCC			(9<<16)	// 4 bit code
#define 	LINK		(0<<1)	// 0:disable, 1:enable
#define 	FS			(1<<0)	// 0:disable, 1:enable

#define 	DS2_1		(1<<26)	// 0:1-Dimensional
#define 	SUM_1		(1<<24)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	DD2_1		(0<<23)	// 0:1-Dimensional
#define 	DUM_1		(0<<21)	// 0:no update, 1:increment, 2:decrement, 3:by index

#define 	OptionField_0 (PRI|ESIZE|DS2|SUM|DD2|DUM|TCINT|TCC|LINK|FS)
#define 	OptionField_1 (PRI|ESIZE|DS2_1|SUM_1|DD2_1|DUM_1|TCINT|TCC|LINK|FS)

#endif /* INC_QDMA2_H */

