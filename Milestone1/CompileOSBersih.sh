echo Hapus bersih floppya img
dd if=/dev/zero of=floppya.img bs=512 count=2880
echo Membuat bootloader
nasm bootload.asm
echo Copy bootloader ke floppya img
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
echo Copy map.img ke floppya img
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
echo Copy dir.img ke floppya img
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
echo Mulai kompilasi kernel
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
echo Selesai kompilasi kernel
echo Copy kernel ke floppya img
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
echo Selesai kompilasi os
echo Load logo
./loadFile logo.txt
echo Load keyproc
./loadFile keyproc
echo Load bochs
bochs -f opsys.bxrc
