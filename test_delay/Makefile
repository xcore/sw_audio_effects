# ansi C compile

# get Operating System
OS = $(shell uname)

CFLAGS = $(OPT)

MAIN =	main

# Base-names of Test harness modules
TST_NAMS =	$(MAIN) \
						test_harness \

# Base-names of modules for Application under test
APP_NAMS =	delay_line \

# Common include files (NB triggers recompilation of all source)

TST_DIR =	src
APP_DIR =	../module_delay/src
INC_DIR = src  ../module_delay/src ../module_utils/src

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

ALL_OBJS = $(TST_OBJS) $(APP_OBJS) 

FLIBS   = -lm


$(EXE):	$(ALL_OBJS)
	$(LINK.c) $(ALL_OBJS) $(FLIBS) -o $(EXE)

$(TST_OBJS) : $(OBJ_DIR)/%.o: $(TST_DIR)/%.c $(TST_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

$(APP_OBJS) : $(OBJ_DIR)/%.o: $(APP_DIR)/%.c $(APP_DIR)/%.h Makefile
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	\rm $(OBJ_DIR)/*.o
	\rm $(EXE)
#
