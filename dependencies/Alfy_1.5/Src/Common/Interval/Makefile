CC=gcc
# with profiling & debugging:
# CFLAGS= -m64 -O0 -g -Wall -Wshadow -pedantic   -pg -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 #-mcmodel=medium
# without profiling & debugging:
#CFLAGS= -g -Wall -Wshadow -pedantic -DSTRMAT -DSTATS
#CFLAGS= -O3 -DSTRMAT -DSTATS

#32 bit version
#when the option VER32 is set, the source will be compiled as the 32 bit version          
# preprocessor options: VER32 -> 32-bit version, DEBUG -> debug version, UNIX -> unix version (as opposed to windows version); m64 for unix/mac, exclude for linux!
CFLAGS= -O3 -Wall -Wshadow -pedantic -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -std=c99 -DVER32 -DUNIX #-m64

#64 bit version
# option VER32 is not set
CFLAGS64= -O3 -Wall -Wshadow -pedantic -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -std=c99 -DUNIX #-m64

################################################
#include directories
INCLUDE_COMMON_DIR = ..
INCLUDE_UTIL_DIR = ../Util
INCLUDE_SEQUENCE_DIR = ../Sequence
# INCLUDE_DIR32 = ../../../Include/DeepShallow
# INCLUDE_DIR64 = ../../../Include/DeepShallow64
INCLUDE_DIR32 = ../../../ExternSrc/DeepShallow/Src
INCLUDE_DIR64 = ../../../ExternSrc/DeepShallow64/Src

INCLUDES = -I$(INCLUDE_COMMON_DIR) -I$(INCLUDE_UTIL_DIR) -I$(INCLUDE_SEQUENCE_DIR) 
INCLUDES32 = $(INCLUDES) -I$(INCLUDE_DIR32)
INCLUDES64 = $(INCLUDES) -I$(INCLUDE_DIR64)

#library directories
LIB_DIR = ../../../Lib
LIB_DIR32 = ../../../Lib/DeepShallow/
LIB_DIR64 = ../../../Lib/DeepShallow64/

LIBS32 = -lm -L$(LIB_DIR) -lutil -lsequence
LIBS64 = -lm -L$(LIB_DIR) -lutil64 -lsequence64


################################################
#
# The source, object and library files
#
SRCFILES = interval.c intervalKr.c intervalStack.c queryInterval.c

# object files for 32- and 64-bit version have to have different suffixes as to avoid syntax problem with rules
OBJFILES = $(addsuffix 32.o, $(basename $(SRCFILES)))
OBJFILES64 = $(addsuffix 64.o, $(basename $(SRCFILES)))

COMMON_LIB = libinterval.a 
COMMON64_LIB = libinterval64.a
################################################

VERSION = 2.0.0
#
# The dependencies for each of the *.o and *64.o files.
#
.PHONY : all
all: $(COMMON_LIB) $(COMMON64_LIB)

# library is created and moved to Lib directory and object files are removed
$(COMMON_LIB) : $(OBJFILES)
	ar rcs $(COMMON_LIB) $(OBJFILES)

$(COMMON64_LIB) : $(OBJFILES64)
	ar rcs $(COMMON64_LIB) $(OBJFILES64)

#$(OBJFILES): $(SRCFILES) # this rule doesn't work!
%32.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $*32.o $*.c

%64.o : %.c
	$(CC) $(CFLAGS64) $(INCLUDES64) -c -o $*64.o $*.c

###############################################

#
# Other Standard make rules
#
lint : 
	lint $(SRCFILES) | more

clean:
	rm -f *.o *.a

remove: 
	make clean


