cd stage1 && make && cd ..
cd stage2 && make && cd ..

cd stage3_pre9otherapp/usr2arm9ldr && make clean && make && cd ../..
cp stage3_pre9otherapp/usr2arm9ldr/build/arm9.bin stage3_pre9otherapp/build/
cd stage3_pre9otherapp && make clean && make && cd ..
cd mini_b9s_installer && make && cd ..
cd bb3_installer && make
cd payload && make && cd ../..

python stage0_unsafemode.py
python build_payload.py
cp usm.bin h:/
cp usm.bin e:/
cp slot1.bin slotTool/data
cd slotTool && make
pause