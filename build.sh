#!/bin/bash

# ReWritten by Caio Oliveira aka Caio99BR <caiooliveirafarias0@gmail.com>
# credits to Rashed for the base of zip making
# credits to the internet for filling in else where

echo "This is an open source script, feel free to use and share it"
echo "Caio99BR says: I did it!"

location=.
custom_kernel=SKernel
version=alpha

if [ -z $target ]; then
	echo "Choose to which you will build"
	echo "1) Single"
	echo "2) Dual"
	read -p "L3 II " choice
	case "$choice" in
		1 ) export target=Single ; export defconfig=cyanogenmod_vee3s_defconfig;;
		2 ) export target=Dual ; export defconfig=cyanogenmod_vee3_defconfig;;
		* ) echo "Invalid choice"; sleep 2 ; $0;;
	esac
fi

if [ -z $compiler ]; then
	if [ -f ../arm-eabi-4.6/bin/arm-eabi-* ]; then
		export compiler=../arm-eabi-4.6/bin/arm-eabi-
	elif [ -f arm-eabi-4.6/bin/arm-eabi-* ]; then
		export compiler=arm-eabi-4.6/bin/arm-eabi-
	else
		echo "Please specify a location and the prefix of the chosen toolchain (ex. '/bin/arm-eabi-') at the end"
        read compiler
	fi
fi

cd $location
export ARCH=arm
export CROSS_COMPILE=$compiler
if [ -z "$clean" ]; then
	read -p "Do 'make clean mrproper'?(Y/N)" clean
fi
case "$clean" in
	y|Y ) echo "Cleaning..."; make clean mrproper;;
	n|N ) echo "Continuing...";;
	* ) echo "Invalid option"; sleep 2 ; build.sh;;
esac

echo "Now, building the $custom_kernel for L3 II $target $version!"

START=$(date +%s)

make $defconfig
make -j `cat /proc/cpuinfo | grep "^processor" | wc -l` "$@"

# The zip creation
if [ -f arch/arm/boot/zImage ]; then
	rm -f zip-creator/kernel/zImage
	rm -rf zip-creator/system/

	# antdking: "clean up mkdir commands" - 04/02/13
	mkdir -p zip-creator/system/lib/modules

	cp arch/arm/boot/zImage zip-creator/kernel

	# antdking: "now copy all created modules" - 04/02/13
	find . -name *.ko | xargs cp -a --target-directory=zip-creator/system/lib/modules/

	# caio99br: "Use Name of Kernel, Device and Version to create .zip" - 31/03/15
	zipfile="$custom_kernel-L3-II-$target-$version.zip"
	cd zip-creator
	rm -f *.zip
	zip -r $zipfile * -x *kernel/.gitignore*

	echo "Saved on zip-creator/$zipfile"
else
	echo "The build failed so a zip won't be created"
fi

END=$(date +%s)
BUILDTIME=$((END - START))
B_MIN=$((BUILDTIME / 60))
B_SEC=$((BUILDTIME - E_MIN * 60))
echo -ne "\033[32mBuildtime: "
[ $B_MIN != 0 ] && echo -ne "$B_MIN min(s) "
echo -e "$B_SEC sec(s)\033[0m"
