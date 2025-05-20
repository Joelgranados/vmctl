CC := $(shell command -v musl-clang 2>/dev/null || echo gcc)
CFLAGS := -Os -static -ffunction-sections -fdata-sections
LDFLAGS := -Wl,--gc-sections
BUILDDIR ?= .
SRC := vmctl_init.c
OUT := $(BUILDDIR)/init
IMG := $(BUILDDIR)/init.img

.PHONY: clean

$(OUT): $(SRC)
	@echo "Compiler: $(CC)"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(IMG): $(OUT)
	fd "^init$$" | cpio -ov -H newc > $(IMG)

all: $(IMG)

clean:
	rm -f $(OUT) $(IMG)
