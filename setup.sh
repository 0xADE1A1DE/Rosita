#! /bin/bash
echo -n "Generating random value file (may take a few minutes)..."
cd ./ELMO/test/elmotestbinaries
if [[ ! -f randdata100000.txt ]]; then
python3 genrand.py -f randdata100000.txt 500000000
fi
cd - > /dev/null
echo "Done"
echo -n "Building ELMO..."
cd ./ELMO ; make clean &> /dev/null ; make &> elmobuild.log ; cd - > /dev/null
echo "Done"
echo -n "Extracting toolchain..."
cd TOOLCHAIN
OS=$(uname -s)
MACH=$(uname -m)
XFILE="gnu-mcu-eclipse-$OS-$MACH.tar.xz"
if [ -e $XFILE ]; then
    tar xf gnu-mcu-eclipse-$OS-$MACH.tar.xz
else
    echo -e "\nERROR: An ARM toolchain was not bundled for your operating system, please try on Linux x86_64 or manually add toolchain to TOOLCHAIN directory."
    exit -1
fi
cd - > /dev/null
echo "Done."
