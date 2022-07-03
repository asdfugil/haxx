PROGS = fileproviderctl_internal haxx
CC ?= clang
CFLAGS += -Os -isysroot $(shell xcrun -sdk iphoneos --show-sdk-path) -miphoneos-version-min=14.0
LDLFAGS += -lSystem

all: $(PROGS) sign

fileproviderctl_internal:
	$(CC) $(CFLAGS) $(LDFLAGS) fileproviderctl_internal.c -o fileproviderctl_internal

haxx:
	$(CC) $(CFLAGS) $(LDFLAGS) haxx.c -o haxx

sign: $(PROGS)
	codesign -s "Worth Doing Badly iPhone OS Application Signing" --entitlements entitlements.xml --force --deep $(PROGS)

clean:
	rm -f $(PROGS)

.PHONY: all clean install uninstall