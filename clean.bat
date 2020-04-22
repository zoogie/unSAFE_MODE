rm usm.bin
rm slot1.bin
cd stage1 && make clean && cd ..
cd stage2 && make clean && cd ..
cd stage3_safehax2 && make clean && cd ..
cd stage3_pre9otherapp && make clean
cd usr2arm9ldr && make clean && cd ../..
cd mini_b9s_installer && make clean && cd ..
cd slotTool && make clean && cd ..
cd bb3_installer && make clean
cd payload && make clean 
pause