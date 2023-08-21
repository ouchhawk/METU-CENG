all: build

mount:
	sudo mount -o rw ./cmake-build-debug/example.img fs-root/

umount:
	sudo umount fs-root/

build:
	g++ main.cpp helper_printer.cpp helper_reader.cpp helper_writer.cpp -o hw3

create:
	dd if=/dev/zero of=./cmake-build-debug/example.img bs=1024 count=102400

format:
	mkfs.vfat -F 32 -S 512 -s 2 ./cmake-build-debug/example.img

run:
	./hw3 ./cmake-build-debug/example.img
