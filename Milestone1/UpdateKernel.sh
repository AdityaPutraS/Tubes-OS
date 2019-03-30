echo Mulai kompilasi kernel
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
echo Selesai kompilasi kernel
echo Copy kernel ke floppya img
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
echo Selesai kompilasi os
echo Load keyproc
./loadFile keyproc
echo Load logo
./loadFile logo.txt
echo Load bochs
bochs -f opsys.bxrc
