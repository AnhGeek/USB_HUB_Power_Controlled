# Compile each .c file into .rel files
hex: main.rel UsbDescriptor.rel
	sdcc main.rel UsbDescriptor.rel

# Compile main.c to main.rel
main.rel: main.c
	sdcc -c main.c

# Compile UsbDescriptor.c to UsbDescriptor.rel
 UsbDescriptor.rel: UsbDescriptor.c
	sdcc -c UsbDescriptor.c

all: main.rel
	packihx main.ihx > main.hex
	
clean:
	git clean -dfx