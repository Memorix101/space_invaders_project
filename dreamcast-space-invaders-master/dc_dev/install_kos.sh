#!/usr/bin/env bash
###############################################################################
# KOS VM Script
###############################################################################
# This script represents a machine where KOS is built.

set -e
set -o xtrace

export DC_ROOT=~/Tools/dreamcast
export DC_TOOLCHAIN_ROOT=$DC_ROOT/kallistios/utils/dc-chain


function pkg_install() {
    sudo -E DEBIAN_FRONTEND=noninteractive apt-get install -y $@
}


function install_gcc_prerequisites() {
    sudo apt-get update
    # Thanks to kenws on #dreamcastdev for providing this list.
    # Not sure if libjpeg62-dev and libpng-dev are correct- they're recent
    # additions since jpeg and png headers and dev libraries are now required.
    pkg_install build-essential gcc-4.7 make bison flex   \
         libelf-dev texinfo latex2html git wget sed lyx \
         libjpeg62-dev libpng-dev
}

function download_kos_source() {
    mkdir -p $DC_ROOT
    pushd $DC_ROOT
    git clone git://git.code.sf.net/p/cadcdev/kallistios
    git clone --recursive git://git.code.sf.net/p/cadcdev/kos-ports
    popd

    # FUTURE
    # # Replace GCC 4.7 with 5.2
    # pushd /tmp
    # git clone https://github.com/DC-SWAT/DreamShell.git
    # cp -rf /tmp/DreamShell/sdk/toolchain/*  "${DC_TOOLCHAIN_ROOT}/"
    # popd
}

function prepare_gcc_source() {
    pushd $DC_TOOLCHAIN_ROOT

    # FUTURE:
    # Ok, maybe use this?
    # https://github.com/DC-SWAT/DreamShell/tree/master/sdk/toolchain


    # Replace in download.sh

    # export GCC_VER=4.7.3
    # export BINUTILS_VER=2.23.2
    # export NEWLIB_VER=2.0.0
    # export GMP_VER=4.3.2
    # export MPFR_VER=2.4.2
    # export MPC_VER=0.8.1

    # with

    # export GCC_VER=5.2.0
    # export BINUTILS_VER=2.25
    # export NEWLIB_VER=2.2.0
    # export GMP_VER=4.3.2
    # export MPFR_VER=2.4.2
    # export MPC_VER=0.8.1

    # Ditto for unpack.sh
    # END FUTURE

    # This downloads the GCC source code.
    ./download.sh
    # Unzips it.
    ./unpack.sh
    pushd gcc-4.7.3
    # FUTURE pushd gcc-5.2.0

    # This next script downloads the current versions of mp, mpfr, and mpc.
    # If it fails, an alternative is to instal them via the package manager.
    ./contrib/download_prerequisites
    popd
    # There are numerous problems where the build process will not have access
    # to various directories. This preemptively creates those directories and
    # chowns them so we have access.
    if [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ];then
        mkdir -p /opt/toolchains/dc
    else
        sudo mkdir -p /opt/toolchains/dc
        sudo chown $USER /opt/toolchains/dc
    fi
    mkdir -p /opt/toolchains/dc/sh-elf/sh-elf/include
    mkdir -p /opt/toolchains/dc/sh-elf/share



    # The last step uses the KOS provided make file to patch GCC to
    # work with the Dreamcast processors.

    # FUTURE
    # Have to change
    #kos_base=/usr/local/dc/kos/kos
    # to
    # $DC_ROOT/kallistios
    # set -i -e 's/\/usr\/local\/dc\/kos\/kos/'"$DC_ROOT"'\/kallistios/g' Makefile
    # END FUTURE
    make patch
    popd
}

function build_sh4_tools() {
    pushd $DC_TOOLCHAIN_ROOT
    make build-sh4-binutils
    make build-sh4-gcc-pass1
    make build-sh4-newlib-only
    make fixup-sh4-newlib
    make build-sh4-gcc-pass2
    popd
}

function build_arm_tools() {
    pushd $DC_TOOLCHAIN_ROOT
    make build-arm-binutils
    make build-arm-gcc
    popd
}

function create_environ_sh_script() {
    echo "
# kallistios environment variable settings
export KOS_ARCH='dreamcast'

export KOS_SUBARCH='pristine'

export KOS_BASE='$DC_ROOT/kallistios'
" >  $DC_ROOT/environ.sh
    echo '
# Make utility
export KOS_MAKE="make"

# Load utility
export KOS_LOADER="dc-tool -x"              # dcload, preconfigured

# Genromfs utility
export KOS_GENROMFS="${KOS_BASE}/utils/genromfs/genromfs"

# Compiler prefixes
export KOS_CC_BASE="/opt/toolchains/dc/sh-elf"      # DC
export KOS_CC_PREFIX="sh-elf"

export DC_ARM_BASE="/opt/toolchains/dc/arm-eabi"
export DC_ARM_PREFIX="arm-eabi"

export PATH="${PATH}:${KOS_CC_BASE}/bin:/usr/local/dc/bin"

export KOS_INC_PATHS="-I${KOS_BASE}/../kos-ports/include"

export KOS_CFLAGS=""
export KOS_CPPFLAGS=""
export KOS_LDFLAGS=""
export KOS_AFLAGS=""

export KOS_CFLAGS="-O2 -fomit-frame-pointer"

. ${KOS_BASE}/environ_base.sh
    ' >> $DC_ROOT/environ.sh
}

function build_kos() {
    pushd $DC_ROOT/kallistios
    make
    popd
}

function build_kos_ports() {
    pushd $DC_ROOT/kos-ports

    export KOS_PORTS=$DC_ROOT/kos-ports
    for f in *; do
        if [[ -d "$f" && ! -L "$f" ]]; then
            echo $f
            if [[ "$f" == "opus" || "$f" == "opusfile" || "$f" == "scripts" || "$f" == "utils" ]]; then
                echo "Skipping opus"
            else
                echo "Buidling $f".
                pushd "$f"
                make
                popd
            fi
        fi
    done

    popd
}

function build_dc_tool() {
    pushd $DC_ROOT
    git clone git://cadcdev.git.sourceforge.net/gitroot/cadcdev/dcload-ip
    cd dcload-ip
    if [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ];then
        echo 'Modifying Makefile.cfg for Windows...'
        sed -i 's/\#HOSTCFLAGS/HOSTCFLAGS/' Makefile.cfg
        sed -i 's/\#EXECUTABLEEXTENSION/EXECUTABLEEXTENSION/' Makefile.cfg
    fi
    make
    cd host-src
    popd
    if [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ];then
        echo "alias dc-tool='$DC_ROOT/dcload-ip/host-src/tool/dc-tool.exe'" >> $DC_ROOT/environ.sh
    else
        echo "alias dc-tool='$DC_ROOT/dcload-ip/host-src/tool/dc-tool'" >> $DC_ROOT/environ.sh
    fi
}

function windows_instructions() {
    echo "
    Before continuing, please make sure you've installed the necessary
    dependencies as follows:

    1. Create a temporary directory, i.e. C:\KosTemp.
    2. Download the Cygwin installer, 'setup-x86.exe', to that directory.
    3. Open the Windows command prompt, then run
        > cd C:\KosTemp
        > setup-x86.exe --quiet-mode --packages gcc-core,gcc-g++,make,bison,flex,libelf0-devel,ELFIO,texinfo,git,wget,sed,lyx,patch,patchutils,libiconv

    Once this is done, press enter to continue...
    "
    read -rsp $'Press enter to continue...\n'
}

function cmd_install() {
    if [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ];then
        windows_instructions
    else
        install_gcc_prerequisites
    fi
    download_kos_source
    prepare_gcc_source
    build_sh4_tools
    build_arm_tools
    create_environ_sh_script
    source $DC_ROOT/environ.sh
    build_kos
    source "${DC_ROOT}/environ.sh"
    build_kos_ports
    build_dc_tool
}

cmd_install
