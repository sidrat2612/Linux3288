#!/bin/bash
set -e

KEHU_FILE=kehu_T032_ruikemei

echo "*************"
echo "rm logo_linux_clut224"
echo "*************"
find drivers/video/logo/ -name logo_linux_clut224.o -exec rm -f {} \;
find drivers/video/logo/ -name logo_linux_clut224.c -exec rm -f {} \;
find drivers/video/logo/ -name logo_linux_clut224.ppm -exec rm -f {} \;

echo "***********************"
echo "copy logo_linux_clut224.ppm"
echo "***********************"

cp ../$KEHU_FILE/logo_linux_clut224.ppm drivers/video/logo/


echo "***********************"
echo "make kernel.img"
echo "***********************"
make rk3288-box.img -j8
sz kernel.img resource.img

echo "***********************"
echo "return origin files"
echo "***********************"
find drivers/video/logo/ -name logo_linux_clut224.o -exec rm -f {} \;
find drivers/video/logo/ -name logo_linux_clut224.c -exec rm -f {} \;
git checkout -- drivers/video/logo/logo_linux_clut224.ppm

echo "return origin files success!!!"
