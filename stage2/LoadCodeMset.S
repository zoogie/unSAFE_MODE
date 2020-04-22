	.arm
	.text
	
#include "includes.h"

#define GARBAGE 0xdeadb0b0
#define ROP_POPR3_ADDSPR3_POPPC 0x001000bc
#define STACK_PIVOT ROP_POPR3_ADDSPR3_POPPC

#define ROPBUF 0x00600000     //bss location of rop payload (ropkit_boototherapp.s) that launches otherapp
#define ROPKIT_LINEARMEM_REGIONBASE 0x14000000
#define ROPKIT_LINEARMEM_BUF (ROPKIT_LINEARMEM_REGIONBASE+0x200000)

#define ROPKIT_BINPAYLOAD_PATH "YS:/usm.bin"
#define ROPKIT_BINPAYLOAD_FILEOFFSET 0x8000  //put pre9otherapp inside usm.bin

//#define ROPKIT_MOUNTSD        
#define ROPKIT_TMPDATA 0x0FFFc000
#define ROPKIT_BINLOAD_TEXTOFFSET 0x1000
#define ROPKIT_ENABLETERMINATE_GSPTHREAD
#define ROPKIT_BEFOREJUMP_CACHEBUFADDR ROPKIT_LINEARMEM_BUF
#define ROPKIT_BEFOREJUMP_CACHEBUFSIZE 0x100000  //large gsgpu flush fixes our new3ds L2 cache issues - and increases stability for old3ds


#include "ropkit_ropinclude.s" 

_start:
ropstackstart:

#include "ropkit_boototherapp.s"   

ropkit_cmpobject:
.word (ROPBUFLOC(ropkit_cmpobject) + 0x4) @ Vtable-ptr
.fill (0x40 / 4), 4, STACK_PIVOT @ Vtable