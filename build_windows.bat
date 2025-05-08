@echo off
setlocal

REM set directory to location of this file. Needed when drag n dropping across directories.
cd /d %~dp0

set ISO="%~1"

if %ISO%=="" (
    echo ERROR: iso path not passed
    echo USAGE: build_windows.bat vanilla_melee.iso
    goto end
) else (
    echo iso: %ISO%
)

if not exist %ISO% (
    echo ERROR: iso does not exist
    echo USAGE: build_windows.bat vanilla_melee.iso
    goto end
)

if not exist "C:/devkitPro/devkitPPC" (
    echo ERROR: devkitPro not found at "C:/devkitPro/devkitPPC"
    echo Please install devkitPro with the GameCube package
    goto cleanup
) else (
    echo found devkitPro
)

if not exist "Minigames.iso" (
    copy %ISO% "Minigames.iso"
)

echo BUILD C FILES --------------------------------------------------------

mkdir build

echo build event menu
copy "dats\eventMenu.dat" "build\eventMenu.dat"
"MexTK/MexTK.exe" -ff -i "src/events.c" -b "build" -s tmFunction -dat "build/eventMenu.dat" -t "MexTK/tmFunction.txt" -q -ow -l "MexTK/melee.link" -op 2 || ( echo ERROR: Failed to compile 'events.c' & goto cleanup )
"MexTK/MexTK.exe" -trim "build/eventMenu.dat" || ( echo ERROR: Dat file trimming failed & goto cleanup )

echo build monkey event
"MexTK/MexTK.exe" -ff -i "src/monkey.c" -b "build" -s evFunction -dat "build/monkey.dat" -t "MexTK/evFunction.txt" -q -ow -l "MexTK/melee.link" -op 2 || ( echo ERROR: Failed to compile 'monkey.c' & goto cleanup)
"MexTK/MexTK.exe" -trim "build/monkey.dat" || ( echo ERROR: Dat file trimming failed & goto cleanup )

echo build item event
"MexTK/MexTK.exe" -ff -i "src/items.c" -b "build" -s evFunction -dat "build/items.dat" -t "MexTK/evFunction.txt" -q -ow -l "MexTK/melee.link" -op 2 || ( echo ERROR: Failed to compile 'items.c' & goto cleanup)
"MexTK/MexTK.exe" -trim "build/items.dat" || ( echo ERROR: Dat file trimming failed & goto cleanup )

echo build jungle jam event
"MexTK/MexTK.exe" -ff -i "src/jj.c" -b "build" -s evFunction -dat "build/jj.dat" -t "MexTK/evFunction.txt" -q -ow -l "MexTK/melee.link" -op 2 || ( echo ERROR: Failed to compile 'jj.c' & goto cleanup)
"MexTK/MexTK.exe" -trim "build/jj.dat" || ( echo ERROR: Dat file trimming failed & goto cleanup )

echo build hitfall event
"MexTK/MexTK.exe" -ff -i "src/hitfall.c" -b "build" -s evFunction -dat "build/hitfall.dat" -t "MexTK/evFunction.txt" -q -ow -l "MexTK/melee.link" -op 2 || ( echo ERROR: Failed to compile 'hitfall.c' & goto cleanup)
"MexTK/MexTK.exe" -trim "build/hitfall.dat" || ( echo ERROR: Dat file trimming failed & goto cleanup )

echo build meter event
"MexTK/MexTK.exe" -ff -i "src/meter.c" -b "build" -s evFunction -dat "build/meter.dat" -t "MexTK/evFunction.txt" -q -ow -l "MexTK/melee.link" -op 2 || ( echo ERROR: Failed to compile 'meter.c' & goto cleanup)
"MexTK/MexTK.exe" -trim "build/meter.dat" || ( echo ERROR: Dat file trimming failed & goto cleanup )

echo BUILD ASM FILES --------------------------------------------------------

del "Additional ISO Files\codes.gct"
cd "Build TM Codeset"
echo gecko.exe build

REM gecko always returns success, so we check that the file exists after building as a replacement
gecko.exe build
cd ..
if not exist "Additional ISO Files\codes.gct" (
    echo ERROR: gecko.exe build failed
    goto cleanup
)

copy "Additional ISO Files\codes.gct" "build\"
copy "Additional ISO Files\opening.bnr" "build\"


echo BUILD START.DOL --------------------------------------------------------

for /f "delims=" %%A in ('gc_fst get-header "%ISO%"') do set "GAME_ID=%%A"
if "%GAME_ID%" == "GALJ01" (
	set "PATCH_FILENAME=patch_jp.xdelta"
) else (
	set "PATCH_FILENAME=patch.xdelta"
)

gc_fst read %ISO% "Start.dol" "Start.dol"
"Build TM Start.dol\xdelta.exe" -d -f -s "Start.dol" "Build TM Start.dol\%PATCH_FILENAME%" "build\Start.dol" || ( echo ERROR: Failed to patch Start.dol & goto cleanup )
del Start.dol

echo BUILD ISO --------------------------------------------------------
gc_fst fs Minigames.iso ^
	delete MvHowto.mth ^
	delete MvOmake15.mth ^
	delete MvOpen.mth ^
	insert TM\eventMenu.dat build\eventMenu.dat ^
	insert TM\monkey.dat build\monkey.dat ^
	insert TM\items.dat build\items.dat ^
	insert TM\jj.dat build\jj.dat ^
	insert TM\hitfall.dat build\hitfall.dat ^
	insert TM\meter.dat build\meter.dat ^
	insert PlPc.dat PlPc_bl.dat ^
	insert codes.gct build\codes.gct ^
	insert Start.dol build\Start.dol ^
	insert opening.bnr build\opening.bnr
gc_fst set-header Minigames.iso "GTME01" "SSBM Minigames"

echo ############ Minigames.iso has been created ######################

:cleanup

echo CLEANUP -----------------------------------------------------------------

if exist "build" (
    echo deleting build dir...
    rmdir /s /q build\
)

:end

REM pause if not run from command line
echo %CMDCMDLINE% | findstr /C:"/c">nul && pause
