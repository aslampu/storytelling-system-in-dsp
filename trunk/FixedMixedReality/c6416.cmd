-q 
-c 
-m FixedMixedReality.map 
-o FixedMixedReality.out 
-stack 0x4000
-x

/* Memory for TMS320C641x */
MEMORY   { IDRAM  : origin = 0x00000000,len = 0x00100000 
		   SDRAM  : origin = 0x80000000,len = 0x02000000}
		   
SECTIONS { .vectors:    {} > IDRAM
           .text:       {} > IDRAM
           .const:      {} > IDRAM
           .cinit:      {} > IDRAM
           .bss:        {} > IDRAM
           .sysmem:     {} > IDRAM
           .cio:        {} > IDRAM
           .far:        {} > SDRAM
           .stack:      {} > IDRAM
           .sdram:      {} > SDRAM }






