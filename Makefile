PROGS = fileproviderctl_internal haxx login
CC ?= clang
STRIP ?= strip
CFLAGS += -Os -isysroot $(shell xcrun -sdk iphoneos --show-sdk-path) -miphoneos-version-min=14.0
LDLFAGS += -lSystem

all: $(PROGS)

clean:
	rm -f $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
	$(STRIP) $@
	codesign -s "Worth Doing Badly iPhone OS Application Signing" --entitlements entitlements.xml --force --deep $*

.PHONY: all clean