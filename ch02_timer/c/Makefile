.PONEY : build run clean

CC = i686-elf-gcc
CFLAGS = -m16 -mregparm=3 -mno-push-args -fcall-used-eax -fcall-used-edx -ffreestanding -fno-pie \
         -nostdlib -nostdinc -fno-asynchronous-unwind-tables \
         -fno-builtin -fno-stack-protector -mno-mmx -mno-sse
#		 -fno-inline -O0
ASMFLAGS = -m16 -mregparm=3 -mno-push-args -fcall-used-eax -fcall-used-edx -ffreestanding -fno-pie \
		-nostdlib -nostdinc -fno-asynchronous-unwind-tables \
		-fno-builtin -fno-stack-protector -mno-mmx -mno-sse -O0 -S 
         
bootsector.o: bootsector.c bootsector.h ../../ch01_bootsector/c/bootsector.h Makefile
	$(CC) $(CFLAGS) -c bootsector.c -o bootsector.o
	$(CC) $(ASMFLAGS) -o bootsector.s bootsector.c

bootsector.bin: bootsector.o
	i686-elf-ld -T linker.ld --oformat binary -s -o bootsector.bin bootsector.o

build: bootsector.bin

clean:
	rm -f *.o *.bin *.elf

# bootsector.o : bootsector.c
# 	x86_64-elf-gcc -ffreestanding -nostdlib -nostdinc -fno-asynchronous-unwind-tables -c bootsector.c -o bootsector.o

# bootsector.bin : bootsector.o linker.ld
# 	../.pipe/pb_holder/target/release/pb_holder.exe p2f tinyos_bootsector_holder bootsector.pz
# 	#x86_64-elf-ld -T linker.ld --oformat binary -s -o bootsector.bin bootsector.o 	
# 	x86_64-elf-ld -T linker.ld --oformat binary -s -o \\\\.\\pipe\\tinyosbootsector bootsector.o 	

# build: bootsector.bin

# ../.pipe/pb_holder/target/release/pb_holder.exe : ../.pipe/pb_holder/src/main.rs
# 	cd ../.pipe/pb_holder && cargo b --release

#pb_holder: ../.pipe/pb_holder/target/release/pb_holder.exe

run: build
	#../.pipe/pb_holder/target/release/pb_holder.exe f2p bootsector.pz tinyos_bootsector_provider
	echo "run qemu load bootsector.bin as disk image for boot device"
	qemu-system-x86_64 -drive file=bootsector.bin,format=raw
	#qemu-system-x86_64 -drive file=\\\\.\\pipe\\tinyos_bootsector_provider,format=raw

# clean:
# 	rm bootsector.o bootsector.bin
