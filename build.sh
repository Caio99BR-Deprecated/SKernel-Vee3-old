#!/bin/bash

# Written by Caio Oliveira aka Caio99BR <caiooliveirafarias0@gmail.com>
# credits to Rashed for the base of zip making
# credits to the internet for filling in else where

echo ""
echo "Script says: This is an open source script, feel free to use and share it"; sleep 2
echo "Caio99BR says: I did it!"; sleep 2

location=.
custom_kernel=SKernel
version=v0.2

if [ -z $target ]; then
	echo ""
	echo "Script says: Choose to which you will build"; sleep 2
	echo "Caio99BR says: 1) L3 II Single or 2) L3 II Dual"
	read -p "Choice: " choice
	case "$choice" in
		1 ) export target="L3 II Single"; echo "$target"; sleep 2; export defconfig=cyanogenmod_vee3s_defconfig;;
		2 ) export target="L3 II Dual"; echo "$target"; sleep 2; export defconfig=cyanogenmod_vee3_defconfig;;
		* ) echo "This option is not valid"; sleep 2; $0;;
	esac
fi

cd $location

echo ""
echo "Script says: Choose the place of the toolchain"; sleep 2
echo "Google GCC - 1) 4.4.3 | 2) 4.7 | 3) 4.8"
echo "Linaro GCC - 4) 4.6.2 | 5) 4.7.4"
echo "or any key to Choose the place"
read -p "Choice: " toolchain
case "$toolchain" in
	1 ) export CROSS_COMPILE="../android_prebuilt_toolchains/arm-eabi-4.4.3/bin/arm-eabi-"; echo "../android_prebuilt_toolchains/arm-eabi-4.4.3/bin/arm-eabi-";;
	2 ) export CROSS_COMPILE="../android_prebuilt_toolchains/arm-eabi-4.7/bin/arm-eabi-"; echo "../android_prebuilt_toolchains/arm-eabi-4.7/bin/arm-eabi-";;
	3 ) export CROSS_COMPILE="../android_prebuilt_toolchains/arm-eabi-4.8/bin/arm-eabi-"; echo "../android_prebuilt_toolchains/arm-eabi-4.8/bin/arm-eabi-";;
	4 ) export CROSS_COMPILE="../android_prebuilt_toolchains/arm-eabi-linaro-4.6.2/bin/arm-eabi-"; echo "../android_prebuilt_toolchains/arm-eabi-linaro-4.6.2/bin/arm-eabi-";;
	5 ) export CROSS_COMPILE="../android_prebuilt_toolchains/arm-unknown-linux-gnueabi-linaro_4.7.4-2013.12/bin/arm-unknown-linux-gnueabi-"; echo "../android_prebuilt_toolchains/arm-unknown-linux-gnueabi-linaro_4.7.4-2013.12/bin/arm-unknown-linux-gnueabi-";;
	* ) echo "Script says: Please specify a location"; sleep 1;
		echo "Script says: and the prefix of the chosen toolchain at the end"; sleep 1
		echo ""
		echo "Caio99BR says: 4.6 ex. ../arm-eabi-4.6/bin/arm-eabi-"; sleep 2
		read compiler; export CROSS_COMPILE=$compiler;;
esac

echo ""
read -p "Script says: Enter any key for "Clean" or N for "Continue": " clean
case $clean in
	*) echo "Script says: Cleaning..."; echo ""; make clean mrproper;;
	n|N) echo "Script says: Continuing...";;
esac

echo ""
echo "Script says: Now, building the Kernel!"; sleep 2
echo "$custom_kernel for $target $version Edition!"
echo ""; sleep 3

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
	if [ "$target" == "L3 II Single" ]; then
		zipfile="$custom_kernel-L3-II-Single-$version.zip"
	else
		# caio99br: "Update the updater-script to Dual Device" - 02/04/15
		sed 's/Single/Dual/' zip-creator/META-INF/com/google/android/updater-script > zip-creator/META-INF/com/google/android/updater-script-temp
		rm zip-creator/META-INF/com/google/android/updater-script
		mv zip-creator/META-INF/com/google/android/updater-script-temp zip-creator/META-INF/com/google/android/updater-script
		zipfile="$custom_kernel-L3-II-Dual-$version.zip"
	fi
	cd zip-creator
	rm -f *.zip
	zip -r $zipfile * -x *kernel/.gitignore*

	echo "Script says: Saved on zip-creator/$zipfile"

	cd ..
	# caio99br: "Back to Stock updater-script if building to Dual" - 02/04/15
	if [ "$target" == "L3 II Dual" ]; then
		sed 's/Dual/Single/' zip-creator/META-INF/com/google/android/updater-script > zip-creator/META-INF/com/google/android/updater-script-temp
		rm zip-creator/META-INF/com/google/android/updater-script
		mv zip-creator/META-INF/com/google/android/updater-script-temp zip-creator/META-INF/com/google/android/updater-script
	fi
else
	echo "Script says: The build failed so a zip won't be created"
fi

END=$(date +%s)
BUILDTIME=$((END - START))
B_MIN=$((BUILDTIME / 60))
B_SEC=$((BUILDTIME - E_MIN * 60))
echo -ne "\033[32mBuildtime: "
[ $B_MIN != 0 ] && echo -ne "$B_MIN min(s) "
echo -e "$B_SEC sec(s)\033[0m"
