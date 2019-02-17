# Paths to TMS9900 compilation tools 
# ( Set in environment to override paths )
TMS9900_DIR?=/cygdrive/d/tms9900/bin
ELF2EA5_DIR?=/usr/local/bin
EA5_SPLIT_DIR?=/usr/local/bin
CLASSIC99_DSK1?=/cygdrive/d/classic99/DSK1

# Full paths to the executables used
GAS=$(TMS9900_DIR)/tms9900-as
LD=$(TMS9900_DIR)/tms9900-ld
CC=$(TMS9900_DIR)/tms9900-gcc
AR=$(TMS9900_DIR)/tms9900-ar
ELF2EA5=$(ELF2EA5_DIR)/elf2ea5
EA5_SPLIT=$(EA5_SPLIT_DIR)/ea5split
LIBTI99_DIR=/home/thomc/Workspace/libti99
LIBGCC_DIR=/cygdrive/d/tms9900/lib
LDFLAGS_EA5=\
  --section-start .text=a000 --section-start .data=2080 -M

# output file
NAME=libti99.a

CFLAGS=\
  -O2 -std=c99 -s --save-temp -I$(TMS9900_DIR)/lib/gcc/tms9900/4.4.0/include -I$(LIBTI99_DIR) -fno-builtin

# List of compiled objects used in executable
OBJECT_LIST=\
	crt0_ea5.o \
	copying.o

# Recipe to compile the library
all: copying

copying: $(OBJECT_LIST)
	$(LD) $(OBJECT_LIST_EA5) $(OBJECT_LIST) $(LDFLAGS_EA5) -L$(LIBGCC_DIR) -lgcc -L$(LIBTI99_DIR) -lti99 -o copying.ea5.elf > ea5.map
	$(ELF2EA5) copying.ea5.elf copying.ea5.bin
	$(EA5_SPLIT) copying.ea5.bin
	scp COPYING irata.online:/ssd/ti99/
	scp gpl-3.0-40col.txt irata.online:/ssd/ti99/COPYING.TXT

# Recipe to clean all compiled objects
.phony clean:
	-rm -f *.o
	-rm -f *.a
	-rm -f *.s
	-rm -f *.i
	-rm -f *.elf
	-rm -f *.map
	-rm -f *.bin
	-rm -f COPY*

# Recipe to compile all assembly files
%.o: %.asm
	$(GAS) $< -o $@

# Recipe to compile all C files
%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@
