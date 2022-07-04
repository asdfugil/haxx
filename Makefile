PROGS = fileproviderctl_internal haxx login
CC ?= clang
CFLAGS += -Os -isysroot $(shell xcrun -sdk iphoneos --show-sdk-path) -miphoneos-version-min=14.0
LDLFAGS += -lSystem

all: $(PROGS) sign

sign: $(PROGS)
	codesign -s "Worth Doing Badly iPhone OS Application Signing" --entitlements entitlements.xml --force --deep $(PROGS)

clean:
	rm -f $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

.PHONY: all clean