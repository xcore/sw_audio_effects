# ansi C compile

# get Operating System
OS = $(shell uname)

CFLAGS = $(OPT)

MAIN =	main

# Base-names of Test harness modules
TST_NAMS =	$(MAIN) \
						test_harness \

# Base-names of modules for Application under test
MOD_NAMS =	non_linear_gain \

ROOT_DIR = ..
TST_DIR =	src
MOD_DIR =	$(ROOT_DIR)/module_dsp_loudness/src
INC_DIR = $(TST_DIR) $(MOD_DIR) $(ROOT_DIR)/module_dsp_utils/src

CFLAGS += -g -Wall
CFLAGS += $(INC_DIR:%=-I$(PWD)/%)

# Select compiler for current target
ifdef MK_TARG
	CC = xcc
	EXT = xe
	CFLAGS += -target=$(MK_TARG) -report
else
	CC = gcc
	EXT = x
endif

OBJ_DIR = $(CC).dir
EXE_DIR = $(OBJ_DIR)

EXE = $(MAIN:%=$(EXE_DIR)/%.$(EXT))

ALL_NAMS = $(TST_NAMS) $(MOD_NAMS)

TST_MODS = $(TST_NAMS:%=$(TST_DIR)/%.c)
TST_INCS = $(TST_NAMS:%=$(TST_DIR)/%.h)
TST_OBJS = $(TST_NAMS:%=$(OBJ_DIR)/%.o)

MOD_MODS = $(MOD_NAMS:%=$(MOD_DIR)/%.c)
MOD_INCS = $(MOD_NAMS:%=$(MOD_DIR)/%.h)
MOD_OBJS = $(MOD_NAMS:%=$(OBJ_DIR)/%.o)

ALL_OBJS = $(TST_OBJS) $(MOD_OBJS) 

FLIBS   = -lm


$(EXE):	$(ALL_OBJS)
	$(LINK.c) $(ALL_OBJS) $(FLIBS) -o $(EXE)

$(TST_OBJS) : $(OBJ_DIR)/%.o: $(TST_DIR)/%.c $(TST_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

$(MOD_OBJS) : $(OBJ_DIR)/%.o: $(MOD_DIR)/%.c $(MOD_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	\rm $(OBJ_DIR)/*.o
	\rm $(EXE)
#
