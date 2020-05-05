import os,sys,struct,binascii

payload="\x00"*0x20000

with open("stage2/LoadCodeMset.dat","rb") as f:
	rop=f.read()
	
with open("bb3_installer/payload/rop_payload.bin","rb") as f:
	bb3=f.read()	

with open("stage1/slot1.bin","rb") as f:
	slot1=f.read()

with open("stage3_pre9otherapp/otherapp.bin","rb") as f:
	code=f.read()
if(len(code) > 0x8000):
	print("Error: codebin too large")
	sys.exit(0)

with open("mini_b9s_installer/mini_b9s_installer.bin","rb") as f:
	b9s=f.read()
if(len(b9s) > 0x10000):
	print("Error: b9s_installer too large")
	sys.exit(0)
crc=binascii.crc32(b9s+("\x00"*(0x10000-len(b9s)-4))) & 0xffffffff

with open("usm.bin","wb") as f:
	f.write(payload)
	f.seek(0x0)
	f.write(rop)
	f.seek(0x4000)
	f.write(bb3)
	f.seek(0x5000)
	f.write(slot1)
	f.seek(0x8000)
	f.write(code)
	f.seek(0x10000)
	f.write(b9s)
	f.seek(0x20000-4)
	f.write(struct.pack("<I",crc))
