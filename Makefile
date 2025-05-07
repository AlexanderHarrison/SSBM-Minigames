.PHONY: clean iso all release

dats = build/eventMenu.dat build/monkey.dat build/items.dat build/jj.dat build/hitfall.dat build/meter.dat

# find all .asm and .s files in the ASM dir. We have the escape the spaces, so we pipe to sed
ASM_FILES := $(shell find ASM -type f \( -name '*.asm' -o -name '*.s' \) | sed 's/ /\\ /g')
SHELL := /bin/bash

MEX_BUILD=mono MexTK/MexTK.exe -ff -b "build" -q -ow -l "MexTK/melee.link" -op 2

ifndef iso
$(error Error: INVALID ISO - run `make iso=path/to/vanilla/melee iso`)
endif

HEADER := $(shell ./gc_fst get-header ${iso})
ifeq ($(HEADER), GALE01)
PATCH := patch.xdelta
else
ifeq ($(HEADER), GALJ01)
PATCH := patch_jp.xdelta
else
$(error Error: INVALID ISO - run `make iso=path/to/vanilla/melee iso`)
endif
endif

clean:
	rm -rf Minigames/patch.xdelta
	rm -rf Minigames.iso
	rm -rf ./build/

build/eventMenu.dat: src/events.c src/events.h
	cp "dats/eventMenu.dat" "build/eventMenu.dat" 
	$(MEX_BUILD) -i "src/events.c" -s "tmFunction" -dat "build/eventMenu.dat" -t "MexTK/tmFunction.txt"

build/monkey.dat: src/monkey.c src/events.h
	$(MEX_BUILD) -i "src/monkey.c" -s "evFunction" -dat "build/monkey.dat" -t "MexTK/evFunction.txt"

build/items.dat: src/items.c src/events.h
	$(MEX_BUILD) -i "src/items.c" -s "evFunction" -dat "build/items.dat" -t "MexTK/evFunction.txt"

build/jj.dat: src/jj.c src/events.h
	$(MEX_BUILD) -i "src/jj.c" -s "evFunction" -dat "build/jj.dat" -t "MexTK/evFunction.txt"

build/hitfall.dat: src/hitfall.c src/events.h
	$(MEX_BUILD) -i "src/hitfall.c" -s "evFunction" -dat "build/hitfall.dat" -t "MexTK/evFunction.txt"

build/meter.dat: src/meter.c src/events.h
	$(MEX_BUILD) -i "src/meter.c" -s "evFunction" -dat "build/meter.dat" -t "MexTK/evFunction.txt"

build/codes.gct: Additional\ ISO\ Files/opening.bnr $(ASM_FILES)
	cd "Build TM Codeset" && ./gecko build
	cp Additional\ ISO\ Files/* build/

build/Start.dol: | build
	./gc_fst read ${iso} Start.dol build/Start.dol
	xdelta3 -d -f -s build/Start.dol "Build TM Start.dol/$(PATCH)" build/Start.dol

Minigames.iso: build/Start.dol build/codes.gct $(dats)
	if [[ ! -f Minigames.iso ]]; then cp ${iso} Minigames.iso; fi
	./gc_fst fs Minigames.iso \
		delete MvHowto.mth \
		delete MvOmake15.mth \
		delete MvOpen.mth \
		insert TM/eventMenu.dat build/eventMenu.dat \
		insert TM/monkey.dat build/monkey.dat \
		insert TM/items.dat build/items.dat \
		insert TM/jj.dat build/jj.dat \
		insert TM/hitfall.dat build/hitfall.dat \
		insert TM/meter.dat build/meter.dat \
		insert PlPc.dat PlPc_bl.dat \
		insert codes.gct build/codes.gct \
		insert Start.dol build/Start.dol \
		insert opening.bnr build/opening.bnr
	./gc_fst set-header Minigames.iso "GTME01" "SSBM Minigames"

build:
	mkdir -p build

iso: Minigames.iso

Minigames.zip: Minigames.iso
	xdelta3 -f -s ${iso} -e Minigames.iso Minigames/patch.xdelta
	zip -r Minigames.zip Minigames/

release: Minigames.zip

all: iso release
