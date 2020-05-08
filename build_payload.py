import os,sys,struct,binascii

payload=b"\x00"*0x20000

with open("stage2/LoadCodeMset.dat","rb") as f:
	rop=f.read()
	
with open("bb3_installer/payload/rop_payload.bin","rb") as f:
	bb3_rop=f.read()

with open("bb3_installer/payload/otherapp_template/otherapp.bin","rb") as f:
	bb3_code=f.read()	

with open("stage1/slot1.bin","rb") as f:
	slot1=f.read()

with open("stage3_universal_otherapp/otherapp.bin","rb") as f:
	code=f.read()
if(len(code) > 0x8000):
	print("Error: codebin too large")
	sys.exit(0)

with open("mini_b9s_installer/mini_b9s_installer.bin","rb") as f:
	b9s=f.read()
if(len(b9s) > 0xF000):
	print("Error: b9s_installer too large")
	sys.exit(0)

with open("usm.bin","wb+") as f:
	f.write(payload)
	f.seek(0x0)
	f.write(rop)
	f.seek(0x4000)
	f.write(bb3_rop)
	f.seek(0x6000)
	f.write(bb3_code)
	f.seek(0x8000)
	f.write(code)
	f.seek(0x10000)
	f.write(b9s)
	f.seek(0x1F000)
	f.write(slot1)
	f.seek(0x10000)
	crc=binascii.crc32(f.read(0x10000-4)) & 0xffffffff
	f.seek(0x20000-4)
	f.write(struct.pack("<I",crc))