#!/bin/bash
set -e

echo "***********************"
echo "make kernel.img"
echo "***********************"

if [ "$1" = "t034" ]; then
echo "make rk3288-t034.dts"

sed -i "s~CONFIG_SND_RK_SOC_RK1000=.*~CONFIG_SND_RK_SOC_RK1000=n~" .config

make rk3288-t034.img -j8
elif [ "$1" = "t023a" ]; then
echo "rk3288-t023a.dts"

sed -i "s~CONFIG_SND_RK_SOC_RK1000=.*~CONFIG_SND_RK_SOC_RK1000=n~" .config

make rk3288-t023a.img -j8
else
echo "make rk3288-box.dts"

make rk3288-box.img -j8
fi

sz kernel.img resource.img

echo "***********************"
echo "return origin files"
echo "***********************"
git checkout -- .config

echo "return origin files success!!!"
