#!/bin/bash

########################################################################################################################
# Author: Siddharth Rathore 
# Version: 1.5
# Date : 10-Nov-2014
########################################################################################################################

export G8_HOME="/G8_DEV/Linux3288"
export ARCH=arm
export CROSS_COMPILE=../gcc-linaro-arm-linux-gnueabihf-4.9-2014.06_linux/bin/arm-linux-gnueabihf-
export version=V1.5
export HOSTNAME=$(hostname)
export DATE=$(date +"%d-%m-%Y")
export TIME=$(date +"%T")
export G8CORE=`cat /proc/cpuinfo | grep processor | wc -l`
cd $G8_HOME

function g8_bld_menu {
	cmd=(dialog --title "$G8_TITLE" --keep-tite --menu "Compiling Kernel " 25 96 16)
		options=(       1 "Install Necessary files and libraries"
				2 "Make backup of Previous config file"
				3 "Clean Kernel"
				4 "Make Menu Config"
				5 "Compile Kernel"
				m "Compile Modules"
				6 "Copy Resource and Boot Image to ../MK902II_Flash folder"
				7 "Open loader "
				x "Exit"
			)
		choices=$("${cmd[@]}" "${options[@]}" 2>&1 >/dev/tty)
		for choice in $choices
			do
			case $choice in
			1)
				clear
				sudo apt-get install gcc-arm-linux-gnueabihf build-essential lzop libncurses5-dev libssl-dev
				read -p "Press enter to continue..."
				g8_bld_menu 
				;;
			2)
				clear
				cp ${pwd}kernel/.config ../config/config.mk902II_working_"$HOSTNAME"_$(date +"%d-%m-%Y")_$(date +"%T").cfg
				read -p "Press enter to continue..."
				g8_bld_menu 
				;;
			3)
				clear
				cd ${pwd}kernel/
				make mrproper
				cd ..
				read -p "Press enter to continue..."
				g8_bld_menu 
				;;
			4)
				clear
				cd ${pwd}kernel/
				make menuconfig
				cd ..
				read -p "Press enter to continue..."
				g8_bld_menu 
				;;	
			5)
				clear
				cd ${pwd}kernel/
				make -j8 rk3288-box.img 
				read -p "Press enter to continue..."
				cd ..
				g8_bld_menu 
				;;
			m)
				clear
				cd ${pwd}kernel/
				make -j8  modules
				read -p "Press enter to continue..."
				cd ..
				g8_bld_menu 
				;;
			6)	
				clear
				cd ${pwd}kernel/
				cp ${pwd}resource.img ../../MK902II_Flash/ 
				cd ..
				mkbootimg --kernel ${pwd}kernel/kernel.img --ramdisk initramfs/initramfs-3.0.36+.cpio -o ../MK902II_Flash/boot.img
				read -p "Press enter to continue..."
				g8_bld_menu 
				;;
			7)	
				clear
				cd ../MK902II_Flash
				./g8_loader.sh
		                ;;
			x)
				clear 
				exit 0
				;;
		    esac
	done
}


g8_bld_menu 
