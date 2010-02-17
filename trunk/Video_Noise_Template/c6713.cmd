-q 
-cr 
-m cam2yuv.map 
-o cam2yuv.out 
-stack 0x2000
-x

/* Memory for TMS320C6713 */
MEMORY   { IDRAM  : origin = 0x00000000,len = 0x0040000
		 SDRAM  : origin = 0x80000000,len = 0x01000000 }

SECTIONS { .vectors:    {} > 0h
           .text:       {} > IDRAM
           .const:      {} > IDRAM
           .cinit:      {} > IDRAM
           .bss:        {} > IDRAM
           .sysmem:     {} > IDRAM
           .cio:        {} > IDRAM
           .far:        {} > IDRAM
           .stack:      {} > IDRAM
           .sdram:		{} > SDRAM }






