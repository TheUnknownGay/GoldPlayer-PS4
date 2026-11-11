# =====================================================================
#  GoldPlayer - Lecteur multimedia homebrew pour PS4 (OpenOrbis)
#  Build autonome (aucune dependance sur les samples du SDK).
#  Usage :  make          (necessite la variable OO_PS4_TOOLCHAIN)
#  Sortie : IV0000-GOLD00001_00-GOLDPLAYER000000.pkg
# =====================================================================

# ---- Metadonnees du paquet ----
TITLE       := GoldPlayer
VERSION     := 1.00
TITLE_ID    := GOLD00001
CONTENT_ID  := IV0000-GOLD00001_00-GOLDPLAYER000000

# ---- Bibliotheques liees a l'ELF ----
LIBS        := -lc -lkernel -lc++ \
               -lSceUserService -lSceVideoOut -lSceAudioOut -lScePad \
               -lSceSysmodule -lSceFreeType \
               -lSDL2

# ---- Flags supplementaires ----
# gnu++14 (et non c++14) : active les extensions GNU/POSIX requises par les
# en-tetes musl du toolchain (ex. nanosleep), sinon compilation impossible.
EXTRAFLAGS  := -std=gnu++14

# ---- Repertoire des sources (.cpp compiles automatiquement) ----
PROJDIR     := goldplayer

# ---- Fichiers de ressources et modules ----
ASSETS      := $(shell find assets -type f 2>/dev/null)
LIBMODULES  := $(wildcard sce_module/*)

# =====================================================================
#  Interne au toolchain - ne pas modifier en dessous
# =====================================================================
TOOLCHAIN   := $(OO_PS4_TOOLCHAIN)
INTDIR      := $(PROJDIR)/x64/Debug

CFILES      := $(wildcard $(PROJDIR)/*.c)
CPPFILES    := $(wildcard $(PROJDIR)/*.cpp)
OBJS        := $(patsubst $(PROJDIR)/%.c, $(INTDIR)/%.o, $(CFILES)) \
               $(patsubst $(PROJDIR)/%.cpp, $(INTDIR)/%.o, $(CPPFILES))

CFLAGS      := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c $(EXTRAFLAGS) \
               -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include
CXXFLAGS    := $(CFLAGS) -isystem $(TOOLCHAIN)/include/c++/v1
LDFLAGS     := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr \
               -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crt1.o

_unused     := $(shell mkdir -p $(INTDIR))

UNAME_S     := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CC      := clang
    CCX     := clang++
    LD      := ld.lld
    CDIR    := linux
endif
ifeq ($(UNAME_S),Darwin)
    CC      := /usr/local/opt/llvm/bin/clang
    CCX     := /usr/local/opt/llvm/bin/clang++
    LD      := /usr/local/opt/llvm/bin/ld.lld
    CDIR    := macos
endif

all: $(CONTENT_ID).pkg

$(CONTENT_ID).pkg: pkg.gp4
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core pkg_build $< .

pkg.gp4: eboot.bin sce_sys/about/right.sprx sce_sys/param.sfo \
         sce_sys/icon0.png sce_sys/pic0.png sce_sys/pic1.png \
         $(LIBMODULES) $(ASSETS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-gp4 -out $@ --content-id=$(CONTENT_ID) --files "$^"

sce_sys/param.sfo: Makefile
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_new $@
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ APP_TYPE --type Integer --maxsize 4 --value 1
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ APP_VER --type Utf8 --maxsize 8 --value '$(VERSION)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ ATTRIBUTE --type Integer --maxsize 4 --value 0
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ CATEGORY --type Utf8 --maxsize 4 --value 'gd'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ CONTENT_ID --type Utf8 --maxsize 48 --value '$(CONTENT_ID)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ SYSTEM_VER --type Integer --maxsize 4 --value 0
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ TITLE --type Utf8 --maxsize 128 --value '$(TITLE)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ TITLE_ID --type Utf8 --maxsize 12 --value '$(TITLE_ID)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ VERSION --type Utf8 --maxsize 8 --value '$(VERSION)'

eboot.bin: $(INTDIR) $(OBJS)
	$(LD) $(INTDIR)/*.o -o $(INTDIR)/$(PROJDIR).elf $(LDFLAGS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -in=$(INTDIR)/$(PROJDIR).elf -out=$(INTDIR)/$(PROJDIR).oelf --eboot "eboot.bin" --paid 0x3800000000000011

$(INTDIR)/%.o: $(PROJDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(INTDIR)/%.o: $(PROJDIR)/%.cpp
	$(CCX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(CONTENT_ID).pkg pkg.gp4 sce_sys/param.sfo eboot.bin \
		$(INTDIR)/$(PROJDIR).elf $(INTDIR)/$(PROJDIR).oelf $(OBJS)

.PHONY: all clean
