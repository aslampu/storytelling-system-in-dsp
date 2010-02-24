;------------------------------------------------------------------;
; DSP STAR Reset Vector for C6000                             	   ;
; Copyright (C) 2000-2006  Next Dimension Technology Co., Ltd.     ;
; All right reserved.                                              ;
;------------------------------------------------------------------;
			.global	_c_int00
			.global _c_int06
			.global _c_int15


			.sect	.vectors
reset:		B.S2	_c_int00
			NOP		5
			.space	0x18
NMI			B.S2	NRP
			NOP		5
			.space	0x58
INT4		B.S2	IRP
			NOP		5
			.space	0x18 
INT5		B.S2	IRP
			NOP		5
			.space	0x18 
INT6		B.S2	_c_int06
			NOP		5
			.space	0x18 
INT7		B.S2	IRP
			NOP		5
			.space	0x18 
INT8		B.S2	IRP
			NOP		5
			.space	0x18 
INT9		B.S2	IRP
			NOP		5
			.space	0x18 
INT10		B.S2	IRP
			NOP		5
			.space	0x18 
INT11		B.S2	IRP
			NOP		5
			.space	0x18 
INT12		B.S2	IRP
			NOP		5
			.space	0x18 
INT13		B.S2	IRP
			NOP		5
			.space	0x18 
INT14		B.S2	IRP
			NOP		5
			.space	0x18 
INT15		B.S2	_c_int15
			NOP		5
			.space	0x18 

			.end
