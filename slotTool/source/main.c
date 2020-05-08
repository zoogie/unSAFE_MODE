#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <3ds.h>
#include "checksum.h"
#include "slot1_bin.h"
#include "cfgu.h"

u8 workbuf[0xC00]={0};
const char *yellow="\x1b[33;1m";
const char *blue="\x1b[34;1m";
const char *dblue="\x1b[34;0m";
const char *white="\x1b[37;1m";
#define HAXX 0x58584148

void fixCRC(u8 *buff){
	u16 crc16=crc_16(buff+4, 0x410);
	memcpy(buff+2, &crc16, 2);
}

Result check_slots(){
	printf("%sSlot Status:\n", blue);
	for(int i=0; i<3; i++){
		printf("%d)", i+1);
		_CFG_GetConfigInfoBlk4(0xC00, 0x80000+i, workbuf);
		if(*(u32*)(workbuf+0x420) == HAXX){
			printf(" Haxx\n");
		}
		else{
			printf(" User\n");
		}
	}
	printf("%s\n", white);
	
	return 0;
}

Result restore_slots(){
	Result res;
	for(int i=0; i<3; i++){
		printf("Restoring slot %d... ", i+1);
		memset(workbuf, 0, 0xC00);
		_CFG_GetConfigInfoBlk4(0xC00, 0x80000+i, workbuf);
		if(*(u32*)(workbuf+0x420) == HAXX){
			memcpy(workbuf, workbuf+0x500, 0x500); //restore backup slot to wifi slot
			memset(workbuf+0x500, 0, 0x500);       //clear slot backup to zeros
			res = _CFG_SetConfigInfoBlk4(0xC00, 0x80000+i, workbuf); //commit workbuf to slot
		}
		else{
			res = 1;
		}
		if(res) printf(" FAIL\n");
		else    printf(" GOOD\n");
	}
	_CFG_UpdateConfigSavegame();
	
	return 0;
}

Result inject_slots(){
	Result res;
	for(int i=0; i<3; i++){
		printf("Injecting slot %d... ", i+1);
		memset(workbuf, 0, 0xC00);
		_CFG_GetConfigInfoBlk4(0xC00, 0x80000+i, workbuf);
		if(*(u32*)(workbuf+0x420) == HAXX){
			res = 1;
		}
		else{
			memcpy(workbuf+0x500, workbuf, 0x500); //backup user slot to slot+0x500
			memcpy(workbuf, slot1_bin, 0x500);         //write slot1 to workbuf
			res = _CFG_SetConfigInfoBlk4(0xC00, 0x80000+i, workbuf); //commit workbuf to slot
		}
		if(res) printf(" FAIL\n");
		else    printf(" GOOD\n");
	}
	_CFG_UpdateConfigSavegame();
	 
	 return 0;
}


int cursor=0;
int menu(u32 n){
	consoleClear();
	printf("slotTool v1.3 - zoogie\n\n");
	
	check_slots();

	char *choices[]={
		"INSTALL exploit to wifi slots 1,2,3 & shutdown",
		"RESTORE original wifi slots 1,2,3",
		"EXIT    to menu"
	};
	
	int maxchoices=sizeof(choices)/4; //each array element is a 32 bit pointer so numElements is sizeof/4 (this is a bad practice but whatever).
	
	if(n & KEY_UP) cursor--;
	else if (n & KEY_DOWN) cursor++;
	if (cursor >= maxchoices) cursor=0;
	else if (cursor < 0) cursor=maxchoices-1;
	
	
	for(int i=0; i<maxchoices; i++){
		printf("%s%s%s\n", cursor==i ? yellow:white, choices[i], white);
	}
	
	printf("--------------------------------------------------");
	printf(" \n");
	
	if(n & KEY_A) {
		
		switch(cursor){
			case 0:
			inject_slots();
			printf("powering down now...");
			printf("\n");
			svcSleepThread(1000*1000*1000);
			PTMSYSM_ShutdownAsync(1000*1000*1000);
			while(1) svcSleepThread(100*1000*1000);
			break;
			
			case 1:
			restore_slots();
			printf("rebooting now...");
			printf("\n");
			svcSleepThread(1000*1000*1000);
			APT_HardwareResetAsync();
			while(1) svcSleepThread(100*1000*1000);
			break;
			
			case 2:
			return 1;   //exit app
			break;
			
			default:;
		};
		svcSleepThread(500*1000*1000);
	}
	
	return 0;
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	printf("slotTool v1.3 - zoogie\n");

	Result res;
	//u32 SIZE=0xC00;
	res = ptmSysmInit();
	res = nsInit();
	res = _cfguInit();
	printf("cfgInit: %08X\n", (int)res);
	res = _CFG_GetConfigInfoBlk4(0xC00, 0x80000, workbuf);
	printf("cfgTest: %08X\n", (int)res);
	if(res){
		printf("WHAT IS WRONG WITH THE ELF?\nPlease hold power to turn off :(\n");
		while(1) gspWaitForVBlank();
	}

	menu(0);

	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();
		u32 kDown = hidKeysDown();
		
		if(kDown){
			res = menu(kDown);
			if(res) break;
		}
	}

	gfxExit();
	return 0;
}