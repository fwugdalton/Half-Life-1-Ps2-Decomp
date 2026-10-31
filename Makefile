BASENAME  := SLUS_200.66
CROSS     ?= mips-linux-gnu-
AS        := $(CROSS)as
LD        := $(CROSS)ld
OBJCOPY   := $(CROSS)objcopy
PYTHON    ?= python3
WIBO      := tools/compilers/wibo
CC        := $(WIBO) tools/compilers/ee-gcc2.95.2-274/bin/ee-gcc.exe

BUILD_DIR := build
ROM_SIZE  := 2723716 # 0x298F84
ASFLAGS   := -EL -march=r5900 -mabi=eabi -G0 -no-pad-sections -I include -I .
LDFLAGS   := -EL --no-check-sections -T undefined_syms_auto.txt -T undefined_funcs_auto.txt -T linker_script_extra.ld -Map $(BUILD_DIR)/$(BASENAME).map

CFLAGS    := -O2 -G0 -Iinclude -Isrc
# ajustes para o gas moderno se comportar como o ee-as original (ver tools/fixasm.py)
FIX_ASM   := $(PYTHON) tools/fixasm.py

S_FILES   := $(shell find asm -name '*.s' -not -path 'asm/nonmatchings/*')
C_FILES   := $(shell find src -name '*.c')
CXX_FILES := $(shell find src -name '*.cpp')
O_FILES   := $(S_FILES:%.s=$(BUILD_DIR)/%.o) $(C_FILES:%.c=$(BUILD_DIR)/%.o) $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)

ELF       := $(BUILD_DIR)/$(BASENAME).elf
ROM       := $(BUILD_DIR)/$(BASENAME).rom

all: $(ROM)
	@sha1sum -c checksum.sha1

split:
	# apaga o asm antigo para nao sobrarem arquivos de segmentos que viraram C
	rm -rf asm
	$(PYTHON) -m splat split $(BASENAME).yaml
	# o gas não aceita a sintaxe do splat para as macros do VU0
	$(PYTHON) tools/vu0words.py
	# .sbss/.bss do original ficam em enderecos fixos (alinhamento que o splat nao reproduz)
	sed -i -e 's/^\(\s*\)\.cod_bss (NOLOAD) :/\1.cod_bss 0x00399000 (NOLOAD) :/' \
	       -e 's/^\(\s*\)cod_BSS_START = \.;/\1. = 0x900;\n\1cod_BSS_START = .;/' $(BASENAME).ld

setup: $(BASENAME).rom split

$(BASENAME).rom: $(BASENAME)
	$(OBJCOPY) -O binary --gap-fill=0x00 $< $@

# o gas alinha .text/.data/.rodata em 16; o original tem fronteiras de arquivo em 8 (texto) e 4 (dados)
SECTALIGN := --set-section-alignment .text=8 --set-section-alignment .data=4 --set-section-alignment .rodata=8 \
             --set-section-alignment .bss=4 --set-section-alignment .sbss=4
SECTALIGN_C := --set-section-alignment .text=8 --set-section-alignment .data=4 --set-section-alignment .rodata=8

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<
	$(OBJCOPY) $(SECTALIGN) $@

# o zlib (inflate 1.1.3) foi compilado sem strict aliasing (o ee-gcc liga por padrão no -O2)
$(BUILD_DIR)/src/zlib/%.o: CFLAGS += -fno-strict-aliasing
$(BUILD_DIR)/src/zlib/adler32.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/gbx/SystemSaveInitialize.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/gbx/MemCardSlot.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/gbx/GlobalPak.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/gbx/GamePak.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/dlls/teamplay_gamerules.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/dlls/util.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/dlls/monsters.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/dlls/healthkit.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/dlls/h_ai.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/dlls/h_battery.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/gbx/StreamZip.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/gbx/HL1Read.o: CFLAGS += -fmove-all-movables
# as dlls do jogo (HLSDK) usam sqrt.s direto
$(BUILD_DIR)/src/dlls/%.o: CFLAGS += -ffast-math
# o objeto estático do LINK_ENTITY_TO_CLASS (1 byte de .bss) já está no .bss em asm;
# o .data das dlls é alinhado em 8
$(BUILD_DIR)/src/dlls/%.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8
# o .rodata do tentacle_decay começa alinhado em 16 (o tentacle.cpp termina com 0xC de padding)
$(BUILD_DIR)/src/dlls/tentacle_decay.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/doors.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/effects.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/rosenberg.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/islave.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/ichthyosaur.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/houndeye.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/hgrunt.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/func_break.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/gamerules.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=4
$(BUILD_DIR)/src/dlls/hassassin.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/roach.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/controller.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/bmodels.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/bullsquid.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/combat.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/talkmonster.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/plats.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/monsters.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/nihilanth.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=8 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/dlls/teamplay_gamerules.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=4
$(BUILD_DIR)/src/dlls/spectator.o: OBJCOPY_EXTRA := -R .bss --set-section-alignment .data=4

# mathlib.c foi compilado com -ffast-math (sqrt.s direto, sem o teste de errno)
$(BUILD_DIR)/src/engine/mathlib.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/net_chan.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/pmove.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/pmovetst.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/pr_cmds.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/r_trans.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/sv_main.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/sv_move.o: CFLAGS += -ffast-math -fmove-all-movables
$(BUILD_DIR)/src/engine/sv_phys.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/gl_screen.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/cl_tent.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/cl_cam.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/cl_input.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/sv_upld.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/sv_user.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/tmessage.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/view.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/wad.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/world.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/zone.o: CFLAGS += -ffast-math -fmove-all-movables
$(BUILD_DIR)/src/engine/cl_parse.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/Host.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/engine/cmodel.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/engine/Host_cmd.o: CFLAGS += -ffast-math -fmove-all-movables
$(BUILD_DIR)/src/engine/Gl_model.o: CFLAGS += -ffast-math -fmove-all-movables
$(BUILD_DIR)/src/engine/qgl.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/r_efrag.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/tri.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/gl_warp.o: CFLAGS += -ffast-math -fmove-all-movables
$(BUILD_DIR)/src/engine/gl_rsurf.o: CFLAGS += -ffast-math -fmove-all-movables
$(BUILD_DIR)/src/engine/gl_rlight.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/engine/r_part.o: CFLAGS += -fmove-all-movables
$(BUILD_DIR)/src/engine/r_studio.o: CFLAGS += -fmove-all-movables -ffast-math
$(BUILD_DIR)/src/engine/gl_studio.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/snd_dma.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/snd_mix_gbx.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/cdll_int.o: CFLAGS += -ffast-math
$(BUILD_DIR)/src/engine/common.o: CFLAGS += -ffast-math -fmove-all-movables
$(BUILD_DIR)/src/engine/snd_win_gbx.o: CFLAGS += -fmove-all-movables
# infblock/infcodes/inflate têm tabelas de salto (.align 4): a .rodata deles começa alinhada em 16
$(BUILD_DIR)/src/zlib/infblock.o $(BUILD_DIR)/src/zlib/infcodes.o $(BUILD_DIR)/src/zlib/inflate.o: SECTALIGN_C := --set-section-alignment .text=8 --set-section-alignment .data=4 --set-section-alignment .rodata=16
# a .data do Zutil.c (z_errmsg) começa alinhada em 8
$(BUILD_DIR)/src/zlib/zutil.o: SECTALIGN_C := --set-section-alignment .text=8 --set-section-alignment .data=8 --set-section-alignment .rodata=8
# MemFileSystem.cpp tem tabelas de salto: .rodata alinhada em 16
$(BUILD_DIR)/src/gbx/MemFileSystem.o: SECTALIGN_C := --set-section-alignment .text=8 --set-section-alignment .data=4 --set-section-alignment .rodata=16
# a .rodata do gl_warp começa alinhada em 16 no original
# o .text do gl.o começa alinhado a 256 bytes (0x23AA00); o fim do GsEnv.o é preenchido com nops
$(BUILD_DIR)/src/gbx/gl.o: SECTALIGN_C := --set-section-alignment .text=256 --set-section-alignment .data=4 --set-section-alignment .rodata=16
$(BUILD_DIR)/src/gbx/TexConv.o: SECTALIGN_C := --set-section-alignment .text=256 --set-section-alignment .data=4 --set-section-alignment .rodata=8
$(BUILD_DIR)/src/engine/gl_warp.o: SECTALIGN_C := --set-section-alignment .text=8 --set-section-alignment .data=4 --set-section-alignment .rodata=16

$(BUILD_DIR)/src/%.o: src/%.c $(shell find include -name "*.h")
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -S -o $(BUILD_DIR)/src/$*.s0 $<
	$(FIX_ASM) $(BUILD_DIR)/src/$*.s0 $(BUILD_DIR)/src/$*.s
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/src/$*.s
	$(OBJCOPY) $(SECTALIGN_C) $@

# código C++ da Gearbox (Gbx/*.cpp): o mesmo ee-gcc chama o cc1plus
$(BUILD_DIR)/src/%.o: src/%.cpp $(shell find include -name "*.h")
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -S -o $(BUILD_DIR)/src/$*.s0 $<
	$(FIX_ASM) $(BUILD_DIR)/src/$*.s0 $(BUILD_DIR)/src/$*.s
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/src/$*.s
	$(OBJCOPY) $(SECTALIGN_C) $(OBJCOPY_EXTRA) $@

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
