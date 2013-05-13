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

INCLUDES = -I$(INCLUDE_COMMON_DIR) 
INCLUDES64 = -I$(INCLUDE_COMMON_DIR) 

#library directories; at the moment no libraries
LIB_DIR = ../../../Lib/  

LIBS = 
LIBS64 = 

################################################
#
# The source, object and library files
#
SRCFILES = eprintf.c fileListUnix.c stringUtil.c ran.c subjectUtil.c

# object files for 32- and 64-bit version have to have different suffixes as to avoid syntax problem with rules
OBJFILES = $(addsuffix 32.o, $(basename $(SRCFILES)))
OBJFILES64 = $(addsuffix 64.o, $(basename $(SRCFILES)))

COMMON_LIB = libutil.a 
COMMON64_LIB = libutil64.a
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
	# mv $(COMMON_LIB) $(LIB_DIR)
#	rm -f $(OBJFILES)

$(COMMON64_LIB) : $(OBJFILES64)
	ar rcs $(COMMON64_LIB) $(OBJFILES64)
	# mv $(COMMON64_LIB) $(LIB_DIR)
#	rm -f $(OBJFILES64)

#$(OBJFILES): $(SRCFILES) # this rule doesn't work!
%32.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $*32.o $*.c $(LIBS)

%64.o : %.c
	$(CC) $(CFLAGS64) $(INCLUDES64) -c -o $*64.o $*.c $(LIBS64)

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


