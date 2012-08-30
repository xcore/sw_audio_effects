# The top level directory of the application.

BASE_DIR = .

# The TARGET variable determines what target system the application is 
# compiled for. It either refers to an XN file in the source directories
# or a valid argument for the --target option when compiling.

TARGET = L1_AUDIO


# The flags passed to xcc when building the application

BUILD_FLAGS     += -DAUDIO_2 -DFLASH_MAX_UPGRADE_SIZE=64*1024
XCC_FLAGS       = $(BUILD_FLAGS) -fcomment-asm -Xmapper --map -Xmapper MAPFILE -Wall -O3 -report -lflash -fsubword-select -save-temps -g -DUSB_CORE=0
#-fschedule


# The USED_MODULES variable lists other module used by the application. These
# modules will extend the SOURCE_DIRS, INCLUDE_DIRS and LIB_DIRS variables. 
# Modules are expected to be in the directory above the BASE_DIR directory.

USED_MODULES = module_utils.0v1 module_delay.0v1 module_loudness.0v1 module_biquad.0v1 module_usb_shared.1v3 module_xud.0v60 module_usb_aud_shared.2v4 module_spdif_tx.1v37 module_spdif_rx.1v25 module_usb_midi.1v42 module_dfu.1v5

# The following part of the Makefile includes the common build infrastructure
# for compiling XMOS applications. You should not need to edit this apart from
# changing the version of module_xmos_common.

include ../module_xmos_common/build/Makefile.common

# You can change the flags of a set of files using the SET_XCC_[lang]_FLAGS
# functions. The first argument is a list of directories and the
# second argument is the value to set the compile flags to. e.g.
#
# $(call SET_XCC_C_FLAGS, mydir1 mydir2, $(XCC_FLAGS) -g -O3)


# You can change the flags of an individual file by setting the 
# XCC_FLAGS_[filename] variable. e.g.
#
# XCC_FLAGS_myfile.xc = $(XCC_FLAGS) -fsubword-select

clean: clean_common

# The final target of the build. This is usually dependent on a binary file
# in the $(BIN_DIR) directory e.g.
#
#  all : $(BIN_DIR)/my_app.xe

all: $(BIN_DIR)/usb_audio.xe
