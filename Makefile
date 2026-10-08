BASENAME  := SLUS_200.66
CROSS     ?= mips-linux-gnu-
AS        := $(CROSS)as
LD        := $(CROSS)ld
OBJCOPY   := $(CROSS)objcopy
PYTHON    ?= python3

BUILD_DIR := build
ROM_SIZE  := 2723716 # 0x298F84
ASFLAGS   := -EL -march=r5900 -mabi=eabi -G0 -no-pad-sections -I include -I .
LDFLAGS   := -EL --no-check-sections -T undefined_syms_auto.txt -T undefined_funcs_auto.txt -T linker_script_extra.ld -Map $(BUILD_DIR)/$(BASENAME).map

S_FILES   := $(shell find asm -name '*.s')
O_FILES   := $(S_FILES:%.s=$(BUILD_DIR)/%.o)

ELF       := $(BUILD_DIR)/$(BASENAME).elf
ROM       := $(BUILD_DIR)/$(BASENAME).rom

all: $(ROM)
	@sha1sum -c checksum.sha1

split:
	$(PYTHON) -m splat split $(BASENAME).yaml
	# .sbss/.bss do original ficam em enderecos fixos (alinhamento que o splat nao reproduz)
	sed -i -e 's/^\(\s*\)\.cod_bss (NOLOAD) :/\1.cod_bss 0x00399000 (NOLOAD) :/' \
	       -e 's/^\(\s*\)cod_BSS_START = \.;/\1. = 0x900;\n\1cod_BSS_START = .;/' $(BASENAME).ld

setup: $(BASENAME).rom split

$(BASENAME).rom: $(BASENAME)
	$(OBJCOPY) -O binary --gap-fill=0x00 $< $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

$(ELF): $(O_FILES) $(BASENAME).ld
	$(LD) $(LDFLAGS) -T $(BASENAME).ld -o $@

$(ROM): $(ELF)
	$(OBJCOPY) -O binary --gap-fill=0x00 $< $@
	truncate -s $(ROM_SIZE) $@

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -rf asm assets $(BASENAME).ld $(BASENAME).d $(BASENAME).rom

.PHONY: all clean distclean split setup
