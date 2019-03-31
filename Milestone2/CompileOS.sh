echo Hapus bersih floppya img
dd if=/dev/zero of=floppya.img bs=512 count=2880
echo Membuat bootloader
nasm bootload.asm
echo Copy bootloader ke floppya img
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
echo Copy map.img ke floppya img
dd if=map.img of=floppya.img bs=512 count=1 seek=256 conv=notrunc
echo Copy files.img ke floppya img
dd if=files.img of=floppya.img bs=512 count=1 seek=258 conv=notrunc
echo Copy sectors.img ke floppya img
dd if=sectors.img of=floppya.img bs=512 count=1 seek=259 conv=notrunc
echo Mulai kompilasi kernel
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
echo Selesai kompilasi kernel
echo Copy kernel ke floppya img
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=1
echo Selesai kompilasi os
echo load shell
as86 lib.asm -o lib_asm.o
bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o lib_asm.o
./loadFile shell
echo load utilitas
bcc -ansi -c -o echoUtil.o echoUtil.c
ld86 -o echoUtil -d echoUtil.o lib_asm.o
./loadFile echoUtil
bcc -ansi -c -o mkdir.o mkdir.c
ld86 -o mkdir -d mkdir.o lib_asm.o
./loadFile mkdir
bcc -ansi -c -o ls.o ls.c
ld86 -o ls -d ls.o lib_asm.o
./loadFile ls
bcc -ansi -c -o cat.o cat.c
ld86 -o cat -d cat.o lib_asm.o
./loadFile cat
echo load keyproc
./loadFile keyproc2
echo Load bochs
bochs -f opsys.bxrc
