#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/viacoin.png
ICON_DST=../../src/qt/res/icons/viacoin.ico
convert ${ICON_SRC} -resize 16x16 viacoin-16.png
convert ${ICON_SRC} -resize 32x32 viacoin-32.png
convert ${ICON_SRC} -resize 48x48 viacoin-48.png
convert viacoin-16.png viacoin-32.png viacoin-48.png ${ICON_DST}

