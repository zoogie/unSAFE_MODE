/*
*   main.c
*/
#include <string.h>
#include "types.h"
#include "fs.h"
#include "crypto.h"
#include "draw.h"
#include "screen.h"
#include "b9s.h"
#include "cache.h"
#include "i2c.h"
#include "utils.h"
#include "fatfs/sdmmc/sdmmc.h"

//#include "firm.h"
//#include "boot9strap.h"
//#include "boot.h"
//#include "install.h"

#define CFG_UNITINFO        (*(vu8  *)0x10010010)
#define ISDEVUNIT    (CFG_UNITINFO != 0)
#define CRC32_RETAIL 0xb01d8055
#define CRC32_DEV 0x6f61cc0f

u32 crc32(u8 *data, int size)
{
  u32 r = ~0; u8 *end = data + size;
 
  while(data < end)
  {
    r ^= *data++;
 
    for(int i = 0; i < 8; i++)
    {
      u32 t = ~((r&1) - 1); r = (r>>1) ^ (0xEDB88320 & t); 
    }
  }
 
  return ~r;
}

int selfcheck(){
	u32 minib9s=0x23F00000;
	if(crc32((u8*)minib9s, 0x10000-4) != *(u32*)(minib9s+0x10000-4)) return 1;
	return 0;
}

void messageScreen(u32 duration, char *message){
	for(u32 i=0x80000;i<0x180000;i+=4){
			*(u32*)(0x20000000 + i)=0xffffffff;

	}
	u32 charskip = 0x2000;
	for(u32 j=0; j<duration; j++){
		for(u32 i=0x80000;i<0x180000;i+=charskip){
			*(u32*)0x23FFFE00=0x20000000 + i;							
			drawString(message, 0, 0, 0);
		}
	}
	
}

void colorScreen(u8 shade){
	u8 *framebuff=fb->top_left;
	u32 size=(240*400*3)*8; //trying to slow screendraw down a bit to look more dramatic
	for(u32 i=0; i<size;i++){
		framebuff[i/8]=shade;
	}
}

void drawStringCenter(char *str, u8 row){
	int x=(400-(strlen(str)*8))/2;
	if(x<0 || x>=200) {drawString("WHAT IS WRONG WITH THE ELF", 0,30,0); return;}
	drawString(str, x, row*8, 0);
}

int writeB9S(bool isFirm1, u32 CRC32){
	u32 boot9strap_size=0x7800;
	u32 checkedsum=0;
	u8 *backup=(u8*)0x22000000;
	u8 *scratch=(u8*)0x22100000;
	u32 fail=0;
	isFirm1 ? readFirm1(backup, boot9strap_size) : readFirm0(backup, boot9strap_size);
		
	if(crc32(backup, boot9strap_size) != CRC32){
		writeFirm(boot9strap, isFirm1, boot9strap_size);  //'false' in this case means firm0, true would mean firm1.
		
		memset(scratch, 0, boot9strap_size);
		isFirm1 ? readFirm1(scratch,  boot9strap_size) : readFirm0(scratch,  boot9strap_size);
		checkedsum=crc32(scratch, boot9strap_size);    //verify that the read-back firm0 has the expected CRC32 of the valid boot9strap, confirming installation success.
		
		if(checkedsum!=CRC32){
			writeFirm(backup, isFirm1, boot9strap_size);  //attempt to restore old data should the b9s NAND write fail - still should be safe given firm1 backup even if this doesn't work
			fail=5;
		}
	}
	else{
		fail=1;
	}
	
	return fail;
}

void main(void)
{
	u32 boot9strap_size=0x7800;
	u32 CRC32=CRC32_RETAIL;
	u32 fail=0;
	u32 fail_f0=0;
	u32 fail_f1=0;
	
	fail=selfcheck();
	
	bool isdev=ISDEVUNIT;
	if(isdev && fail==0){
		memcpy(boot9strap+0x100, boot9strap+boot9strap_size, 0x100); //copy appended dev sig to the header location. retail/dev boot9strap only differ in this 0x100 signature.
		CRC32=CRC32_DEV;
		fail=41;     //no sense supporting dev without a public method, avoid (extremely) small brick risk for retail as well - comment out and compile if you think otherwise
	}
	
	u32 checkedsum=crc32(boot9strap, boot9strap_size);

	if(checkedsum==CRC32 && fail==0){
		sdmmc_sdcard_init();
		ctrNandInit();
		mountSd();

		fail_f0=writeB9S(false, CRC32);
		if(fail_f0 != 5) fail_f1=writeB9S(true, CRC32);
	}
	else{
		fail+=1;
	}

	colorScreen(0xC0);
	drawStringCenter("mini_b9s_installer",1);
	drawStringCenter("zoogie",2);
	isdev ? drawStringCenter("DEVELOPER UNIT",4) : drawStringCenter("RETAIL UNIT",4);
	ISN3DS ? drawStringCenter("NEW 3DS",5) : drawStringCenter("OLD 3DS",5);
	
	drawStringCenter("Greets from ARM9!",10);
	//fail=2;
	drawStringCenter("SAFEHAX completed",12);
	if	(fail==0)  while(0);
	else if	(fail==1)  drawStringCenter("RAM  error",13);  
	else if	(fail==2)  drawStringCenter("RAM2 error",13); 
	else if (fail==42) drawStringCenter("Dev not supported",13);  
	else               drawStringCenter("WHAT IS WRONG WITH THE ELF?",13);
	
	if(fail==0){
		if	(fail_f0==5)  drawStringCenter("NAND error (FIRM0)",13);
		else if	(fail_f0==1)  drawStringCenter("You already have B9S! (FIRM0)",13); 
		else if (fail_f0==0)  drawStringCenter("B9S install SUCCESS! (FIRM0)",13); 
		else                  drawStringCenter("WHAT IS WRONG WITH THE ELF? (FIRM0)",13);
		
		if	(fail_f0==5)  drawStringCenter("Skipped (FIRM1)",14); 
		else if	(fail_f1==5)  drawStringCenter("NAND error (FIRM1)",14);  	
		else if (fail_f1==1)  drawStringCenter("You already have B9S! (FIRM1)",14);  
		else if (fail_f1==0)  drawStringCenter("B9S install SUCCESS! (FIRM1)",14); 
		else                  drawStringCenter("WHAT IS WRONG WITH THE ELF? (FIRM1)",14);
	}	
	
	drawStringCenter("Press any key to reboot",16);
			
	waitInput();

	flushEntireDCache();
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);    //reboot system
	//i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 0);  //power off system
	while(true); 
}