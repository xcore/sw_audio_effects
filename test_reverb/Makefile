# ansi C compile

# get Operating System
OS = $(shell uname)

CFLAGS = $(OPT)

MAIN =	main

# Base-names of Test harness modules
TST_NAMS =	$(MAIN) \
						test_harness \

# Base-names of modules for Application under test
APP_NAMS =	reverb \

# Full path + Base-names of other source modules ...
# WARNING: Every module directory require is own list, and separate build lines below!-(

# Base-names of bi_quad modules
BQ_NAMS =	biquad_simple \

# Base-names of delay-line modules
DL_NAMS =	delay_line \

# Base-names of Loudness modules
NG_NAMS =	non_linear_gain \

# Common include files (NB triggers recompilation of all source)

ROOT_DIR = ../../export.dir/sw_audio_effects
TST_DIR =	src
APP_DIR =	$(ROOT_DIR)/module_reverb/src
BQ_DIR =	$(ROOT_DIR)/module_biquad/src
DL_DIR =	$(ROOT_DIR)/module_delay/src
NG_DIR =	$(ROOT_DIR)/module_loudness/src
INC_DIR = $(TST_DIR) $(APP_DIR) $(BQ_DIR) $(DL_DIR) $(NG_DIR) $(ROOT_DIR)/module_utils/src

CFLAGS += -g -Wall
CFLAGS += $(INC_DIR:%=-I$(PWD)/%)

# Select compiler for current target
ifdef MK_TARG
	CC = xcc
	EXT = xe
	CFLAGS += -target=$(MK_TARG)
else
	CC = gcc
	EXT = x
endif

OBJ_DIR = $(CC).dir
EXE_DIR = $(OBJ_DIR)

EXE = $(MAIN:%=$(EXE_DIR)/%.$(EXT))

ALL_NAMS = $(TST_NAMS) $(APP_NAMS)

TST_MODS = $(TST_NAMS:%=$(TST_DIR)/%.c)
TST_INCS = $(TST_NAMS:%=$(TST_DIR)/%.h)
TST_OBJS = $(TST_NAMS:%=$(OBJ_DIR)/%.o)

APP_MODS = $(APP_NAMS:%=$(APP_DIR)/%.c)
APP_INCS = $(APP_NAMS:%=$(APP_DIR)/%.h)
APP_OBJS = $(APP_NAMS:%=$(OBJ_DIR)/%.o)

BQ_MODS = $(BQ_NAMS:%=$(BQ_DIR)/%.c)
BQ_INCS = $(BQ_NAMS:%=$(BQ_DIR)/%.h)
BQ_OBJS = $(BQ_NAMS:%=$(OBJ_DIR)/%.o)

DL_MODS = $(DL_NAMS:%=$(DL_DIR)/%.c)
DL_INCS = $(DL_NAMS:%=$(DL_DIR)/%.h)
DL_OBJS = $(DL_NAMS:%=$(OBJ_DIR)/%.o)

NG_MODS = $(NG_NAMS:%=$(NG_DIR)/%.c)
NG_INCS = $(NG_NAMS:%=$(NG_DIR)/%.h)
NG_OBJS = $(NG_NAMS:%=$(OBJ_DIR)/%.o)


ALL_OBJS = $(TST_OBJS) $(APP_OBJS) $(BQ_OBJS) $(DL_OBJS) $(NG_OBJS) 

FLIBS   = -lm


$(EXE):	$(ALL_OBJS)
	$(LINK.c) $(ALL_OBJS) $(FLIBS) -o $(EXE)

$(TST_OBJS) : $(OBJ_DIR)/%.o: $(TST_DIR)/%.c $(TST_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

$(APP_OBJS) : $(OBJ_DIR)/%.o: $(APP_DIR)/%.c $(APP_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

$(BQ_OBJS) : $(OBJ_DIR)/%.o: $(BQ_DIR)/%.c $(BQ_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

$(DL_OBJS) : $(OBJ_DIR)/%.o: $(DL_DIR)/%.c $(DL_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

$(NG_OBJS) : $(OBJ_DIR)/%.o: $(NG_DIR)/%.c $(NG_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	\rm $(OBJ_DIR)/*.o
	\rm $(EXE)
#
