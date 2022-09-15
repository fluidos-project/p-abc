#!/bin/sh
#Read options, e.g. with https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash

# A POSIX variable
OPTIND=1         # Reset in case getopts has been used previously in the shell.

export_dir=""
build="false"
test="false"
scriptdir=$(pwd)

while getopts "btd:" opt; do
  case "$opt" in
    b)  build="true"
      ;;
    d)  export_dir=$OPTARG
      ;;
    t)  test="true"
      ;;
  esac
done

shift $((OPTIND-1))

[ "${1:-}" = "--" ] && shift

#echo "build=$build, export_dir='$export_dir', Leftovers: $@"

if [ $(ls "/usr/local/lib" | grep cmocka | wc -l)  -eq 0 ] ; then 
# Install cmocka, either through cmake+make install or through apt-get install (cmocka lib dev package)
    #Method 1, with the cmocka/make install
    cd lib/pfecCwrapper/test/cmocka
    mkdir build
    cd build
    cmake ..
    sudo make install
    cd scriptdir 
    #Method 2, install package
    #sudo apt install libcmocka-dev
fi

#(cd lib/pfecCwrapper/lib/miracl-core-c && python config32_mod.py -o 31 && cp core.a ../Miracl_BLS12381_32b/core_unix.a)
(cd lib/pfecCwrapper/lib/miracl-core-c && python config64.py -o 31 && cp core.a ../Miracl_BLS12381_64b/core_unix.a)


if [ "$build" = "false" -a ! -z "$export_dir" ] ; then
    echo "Export directory can only be set if build is set"
    exit 1
fi

elif [ "$build" = "false" -a "$dotest" = "true" ] ; then
    echo "Testing can only be set if build is set"
    exit 1
fi

if [ "$build" = "true" ] ; then
	rm -rf build
    mkdir build
    cd build
    cmake ..
    make
    if [ "$dotest" = "true" ] ; then
        CMOCKA_MESSAGE_OUTPUT="XML"
        export CMOCKA_MESSAGE_OUTPUT
        CMOCKA_XML_FILE="%g_cmtestresult.xml"
        export CMOCKA_XML_FILE
        ctest
    fi
    if [ ! -z "$export_dir" ] ; then
    	cd $scriptdir
    	if [ -d "$export_dir" ]; then
	        cp libdpabc_psms_bundled.a $export_dir
        else
            echo "D option must be a directory for exporting, received $export_dir"
        fi
    fi
    cd $scriptdir
fi




