#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <3ds.h>
#include "checksum.h"
#include "slot1_bin.h"
#include "cfgu.h"

u8 cfgData[0xC00]={0};
const char *yellow="\x1b[33;1m";
const char *blue="\x1b[34;1m";
const char *dblue="\x1b[34;0m";
const char *white="\x1b[37;1m";

void fixCRC(u8 *buff){
	u16 crc16=crc_16(buff+4, 0x410);
	memcpy(buff+2, &crc16, 2);
}

void dumpSlot(u32 block, u32 size){
	char path[0x100];
	sprintf(path, "/wifi/slot%ld.bin", (block & 0x3)+1);
	printf("dumping %s...\n", path);
	Result res=0;
	memset(cfgData, 0, 0xC00);
	res = _CFG_GetConfigInfoBlk4(size, block, cfgData);
	printf("getConfig: %08X\n", (int)res);

	if(!res && memcmp(cfgData + 0x420, "HAXX", 4)){
		printf("writing file to %s...\n", path);
		FILE *f=fopen(path,"wb");
		if(f){
			fwrite(cfgData, 1, size, f);
			fclose(f);
		}
		else{
			printf("file write failed!\n");
		}
	}
	else{
		printf("slot dump error!\n");
	}

	printf("done\n\n");
}

void writeSlot(u32 block, u32 size){
	char path[0x100];
	memset(path, 0, 0x100);
	sprintf(path, "/wifi/slot%ld.bin", (block & 0x3)+1);
	
	printf("writing %s...\n", path);
	Result res=0;
	FILE *f=fopen(path,"rb");
	if(f){
		fread(cfgData, 1, size, f);
		fclose(f);
		
		fixCRC(cfgData);
		
		res = _CFG_SetConfigInfoBlk4(size, block, cfgData); //update the nand config savegame to skip setup which fixes a format bricked 2ds
		printf("setConfig: %08X\n", (int)res);
		res = _CFG_UpdateConfigSavegame();                         //confirm our changes
		printf("updateConfig: %08X\n", (int)res);
	}
	
	printf("done\n\n");
}

void writeBlock(u32 block, u32 size){

	Result res;
	//memset(cfgData, 0x61, size);
	
	printf("writing cfg...\n");
	res = _CFG_SetConfigInfoBlk4(size, block, cfgData); //update the nand config savegame 
	printf("setConfig: %08X\n", (int)res);
	res = _CFG_UpdateConfigSavegame();                         //confirm our changes
	printf("updateConfig: %08X\n", (int)res);
	printf("done\n\n");
}


int cursor=0;
int menu(u32 n){
	
	consoleClear();
	printf("slotTool - zoogie\n\n");

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
			memcpy(cfgData, slot1_bin, slot1_bin_size);
			fixCRC(cfgData);
			writeBlock(0x00080000, 0xC00); 
			writeBlock(0x00080001, 0xC00); 
			writeBlock(0x00080002, 0xC00); 
			printf("powering down now...");
			printf("\n");
			svcSleepThread(500*1000*1000);
			PTMSYSM_ShutdownAsync(1000*1000*1000);
			svcSleepThread(0x1000000000LL);
			break;
			
			case 1:
			writeSlot(0x00080000, 0xC00); 
			writeSlot(0x00080001, 0xC00); 
			writeSlot(0x00080002, 0xC00); 
			APT_HardwareResetAsync();
			printf("rebooting now...");
			printf("\n");
			svcSleepThread(500*1000*1000);
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
	printf("slotTool - zoogie\n");
	
	mkdir("/wifi",0777);
	srand(time(0));

	Result res;
	//u32 SIZE=0xC00;
	res = ptmSysmInit();
	res = nsInit();
	res = _cfguInit();
	printf("configInit: %08X\n", (int)res);
	
	FILE *t=fopen("/wifi/slot1.bin", "rb"); //quick lazy pre-test to check if backups need to be made
	if(!t){
		printf("creating wifi slot backups...\n");
		fclose(t);
		for(int i=0;i<3;i++){
			dumpSlot(0x00080000 + i, 0xC00);
		}
		svcSleepThread(2*1000*1000*1000);
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
