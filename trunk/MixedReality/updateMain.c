//few part need to be updated for 6416
//just below main
ACE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPEA);
//No PLL
//just below while loop
for(i=0;i<1000000;i++) if(EDMA_CIPRL&0x200) break;		
VM3224ADDH = 0x08000;		
for(i=0;i<200;i++);		
EDMA_CIPRL = 0x200;
//in the output part
for(i=0;i<1000000;i++) if(EDMA_CIPRL&0x200) break;
VM3224ADDH = 0x0000;		
EDMA_CIPRL = 0x200;	

