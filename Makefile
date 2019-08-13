# Makefile of xsvt
.PHONY: all xsvt_checker xsvt_onehot xsvt_transfer

CC := gcc -std=gnu99 -D_GNU_SOURCE -D__USE_STRING_INLINES

# for debug version
CFLAGSd = -I. -I.. -Wall -static-libgcc -m64 -O0 -DDEBUG  -gdwarf-3 -march=x86-64

# for release version
CFLAGS0 = -I. -I.. -static -static-libgcc -m64 -Ofast -DNDEBUG -g0 -march=native -funroll-loops

all: xsvt_checker xsvt_onehot xsvt_transfer
	strip -s xsvt.*.x64.*

xsvt_checker:
	$(CC) $(CFLAGSd) checker/*.c -o xsvt.checker.gcc.x64.d
	$(CC) $(CFLAGS0) checker/*.c -o xsvt.checker.gcc.x64.0

xsvt_onehot:
	$(CC) $(CFLAGSd) checker/checker.c onehot/*.c -o xsvt.onehot.gcc.x64.d
	$(CC) $(CFLAGS0) checker/checker.c onehot/*.c -o xsvt.onehot.gcc.x64.0

xsvt_transfer:
	$(CC) $(CFLAGSd) checker/checker.c transfer/*.c -o xsvt.transfer.gcc.x64.d
	$(CC) $(CFLAGS0) checker/checker.c transfer/*.c -o xsvt.transfer.gcc.x64.0

clean:
	rm -rf xsvt.*.x64.*



