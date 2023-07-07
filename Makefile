all: run

TARGET_NAME := rpi_3d_renderer.elf
SSH_HOST := root@home-server.local
SSH_TARGET_DIR := /root/pico/dev/rpi-3d-renderer

run:
	mkdir -p build && cd build && cmake .. && make && \
	scp -i ~/.ssh/id_rsa $(TARGET_NAME) $(SSH_HOST):$(SSH_TARGET_DIR) && \
	ssh -i ~/.ssh/id_rsa root@home-server.local 'openocd -f interface/raspberrypi-swd.cfg \
	-f target/rp2040.cfg -c "program $(SSH_TARGET_DIR)/$(TARGET_NAME) verify reset exit"' && \
	ssh -t root@home-server.local 'sudo minicom -b 115200 -o -D /dev/serial0'

#run:
#	mkdir -p build && cd build && cmake .. && make && arm-none-eabi-gdb blink.elf \
#    -ex 'set pagination off' \
#    -ex 'target remote localhost:3333' \
#    -ex 'load' \
#    -ex 'continue' \
#    -ex 'monitor reset init' \
#    -ex 'set confirm off' -silent

clean:
	rm -rf build
