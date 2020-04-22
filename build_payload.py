import os,sys,struct

payload="\x00"*0x20000

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

with open("stage2/LoadCodeMset.dat","rb") as f:
	rop=f.read()
	
with open("bb3_installer/payload/rop_payload.bin","rb") as f:
	bb3=f.read()	

with open("slot1.bin","rb") as f:
	slot1=f.read()

with open("stage3_pre9otherapp/otherapp.bin","rb") as f:
	code=f.read()
if(len(code) > 0x8000):
	print("Error: codebin too large")
	sys.exit(0)

with open("mini_b9s_installer/out/mini_b9s_installer.bin","rb") as f:
	b9s=f.read()
if(len(b9s) > 0x10000):
	print("Error: b9s_installer too large")
	sys.exit(0)

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

print("Payload built")