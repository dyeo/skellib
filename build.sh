#!/bin/bash

#
# This build script was tested with mingw-w64-x86_64-gcc
# It should also work on linux, but you will need to replace the libs
#

set -xe

APPEXE="$1"
CONFIG="$2"
CMPCMD="gcc"
INCDIR="./include"
LIBDIR="./lib/$CONFIG"
SRCDIR="./src"
OUTDIR="./out/$CONFIG"
OUTPUT="$OUTDIR/$APPEXE.exe"

shift
shift

SOURCE=""
for srcFile in "$@"; do
    SOURCE="$SOURCE $SRCDIR/$srcFile"
done

if [ "$CONFIG" == "Debug" ]; then
    SRCARG="-g"
    DEBARG="-DDEBUG"
else
    SRCARG=""
    DEBARG="-DNDEBUG"
fi

DEPLIBS="-lraylib"

case "$(uname -s)" in
CYGWIN* | MINGW32* | MSYS* | MINGW*)
    LIBDIR="$LIBDIR/mingw"
    DEPLIBS="$DEPLIBS -lopengl32 -lgdi32 -lwinmm"
    ;;
*)
    LIBDIR="$LIBDIR/linux"
    DEPLIBS="$DEPLIBS -lGL -lm -lpthread -ldl -lrt -lX11"
    ;;
esac

mkdir -p "$SRCDIR"
mkdir -p "$INCDIR"
mkdir -p "$LIBDIR"
mkdir -p "$OUTDIR"
$CMPCMD -o "$OUTPUT" $SRCARG $SOURCE $DEBARG -I "$INCDIR" -L "$LIBDIR" $DEPLIBS
