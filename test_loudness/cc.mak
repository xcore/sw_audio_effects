# ansi C compile

# get Operating System
OS = $(shell uname)

MAIN =	main

# Base-names of Test harness modules
TST_NAMS =	$(MAIN) \
						test_harness \

# Base-names of modules for Application under test
APP_NAMS =	biquad_simple \

# Common include files (NB triggers recompilation of all source)

TST_DIR =	src
APP_DIR =	../module_biquad/src
OBJ_DIR = Linux.dir
EXE_DIR = $(OBJ_DIR)

EXE = $(MAIN:%=$(EXE_DIR)/%.x)

ALL_NAMS = $(TST_NAMS) $(APP_NAMS)

TST_MODS = $(TST_NAMS:%=$(TST_DIR)/%.c)
TST_INCS = $(TST_NAMS:%=$(TST_DIR)/%.h)
TST_OBJS = $(TST_NAMS:%=$(OBJ_DIR)/%.o)

APP_MODS = $(APP_NAMS:%=$(APP_DIR)/%.c)
APP_INCS = $(APP_NAMS:%=$(APP_DIR)/%.h)
APP_OBJS = $(APP_NAMS:%=$(OBJ_DIR)/%.o)

ALL_OBJS = $(TST_OBJS) $(APP_OBJS) 

FLIBS   = -lm

CC = xcc -target=XC-1

# This section assigns CFLAGS ...

CFLAGS = $(OPT)


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
