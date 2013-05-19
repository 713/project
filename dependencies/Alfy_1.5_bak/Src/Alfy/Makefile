CC=gcc
# with profiling & debugging:
# CFLAGS= -m64 -O0 -g -Wall -Wshadow -pedantic   -pg -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 #-mcmodel=medium
# without profiling & debugging:
#CFLAGS= -g -Wall -Wshadow -pedantic -DSTRMAT -DSTATS
#CFLAGS= -O3 -DSTRMAT -DSTATS

#32 bit version
#when the option VER32 is set, the source will be compiled as the 32 bit version          
# preprocessor options: VER32 -> 32-bit version, DEBUG -> debug version, UNIX -> unix version (as opposed to windows version); m64 for unix/mac, exclude for linux!
CFLAGS= -O3 -Wall -Wshadow -pedantic -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -std=c99 -DVER32 -DUNIX -I/opt/local/include/ -L/opt/local/lib 


#64 bit version
# option VER32 is not set
CFLAGS64= -O3 -Wall -Wshadow -pedantic -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -std=c99 -DUNIX -I/opt/local/include/ -L/opt/local/lib 

################################################
#include directories
INCLUDE_COMMON_DIR = ../Common
INCLUDE_UTIL_DIR = ../Common/Util
INCLUDE_SEQUENCE_DIR = ../Common/Sequence
INCLUDE_INTERVAL_DIR = ../Common/Interval
INCLUDE_DIR32 = ../../Include/DeepShallow
INCLUDE_DIR64 = ../../Include/DeepShallow64

INCLUDES = -I$(INCLUDE_COMMON_DIR) -I$(INCLUDE_UTIL_DIR) -I$(INCLUDE_SEQUENCE_DIR) -I$(INCLUDE_INTERVAL_DIR)
INCLUDES32 = $(INCLUDES) -I$(INCLUDE_DIR32)
INCLUDES64 = $(INCLUDES) -I$(INCLUDE_DIR64)

#library directories
LIB_DIR = ../../Lib
LIB_DIR32 = ../../Lib/DeepShallow
LIB_DIR64 = ../../Lib/DeepShallow64

LIBS32 =  $(LIB_DIR)/libsequence.a $(LIB_DIR32)/ds_ssort.a $(LIB_DIR32)/bwtlcp.a $(LIB_DIR)/libutil.a $(LIB_DIR)/libinterval.a -lm #-lgsl -lgslcblas
LIBS64 =  $(LIB_DIR)/libsequence64.a $(LIB_DIR64)/ds_ssort.a $(LIB_DIR64)/bwtlcp.a $(LIB_DIR)/libutil64.a $(LIB_DIR)/libinterval64.a -lm #-lgsl -lgslcblas

#LIBS = libutil.a libutil64.a libsequence.a libsequence64.a
# just one library in LIBS; otherwise $(LIBS) would be executed as many numbers as there are libraries
LIBS = libutil.a -lm

################################################
#
# The source, object and library files
#
SRCFILES = mainAlfy.c interface.c auxiliaryLcpTree.c lcpTree.c analysis.c queryBTNode.c subjectNode.c annotation.c

OBJFILES = $(addsuffix 32.o, $(basename $(SRCFILES))) 
OBJFILES64 = $(addsuffix 64.o, $(basename $(SRCFILES))) 

EXECFILE = alfy
EXECFILE64 = alfy64

DIRECTORY = Alfy
################################################

VERSION = 1.0.2
#
# The dependencies for each of the *.o and *64.o files.
#
.PHONY : all
all : $(LIBS) $(EXECFILE) $(EXECFILE64) 
#make clean
#all : $(EXECFILE) $(EXECFILE64) 
#	make clean

# target dir = relative position from ../Common/Util
$(LIBS):
#cd ../../ExternSrc/DeepShallow/Src; make ds_ssort.a; make bwtlcp.a; mv *.a --target-directory=../$(LIB_DIR32); #rm -f *.o
#cd ../../ExternSrc/DeepShallow64/Src; make ds_ssort.a; make bwtlcp.a; mv *.a --target-directory=../$(LIB_DIR64); #rm -f *.o
	cd ../../ExternSrc/DeepShallow/Src; make ds_ssort.a; make bwtlcp.a; mv *.a ../$(LIB_DIR32); #rm -f *.o
	cd ../../ExternSrc/DeepShallow64/Src; make ds_ssort.a; make bwtlcp.a; mv *.a ../$(LIB_DIR64); #rm -f *.o
#cd ../Common/Util; make; mv *.a --target-directory=../$(LIB_DIR) 
	cd ../Common/Util; make; mv *.a ../$(LIB_DIR) 
#cd ../Common/Sequence; make; mv *.a --target-directory=../$(LIB_DIR)
	cd ../Common/Sequence; make; mv *.a ../$(LIB_DIR)
#cd ../Common/Interval; make; mv *.a --target-directory=../$(LIB_DIR)
	cd ../Common/Interval; make; mv *.a ../$(LIB_DIR)


# 32 bit version
$(EXECFILE) : $(OBJFILES)
	$(CC) $(CFLAGS) $(INCLUDES32) -o $(EXECFILE) $(OBJFILES) $(LIBS32)

# 64 bit version
$(EXECFILE64) : $(OBJFILES64)
	$(CC) $(CFLAGS64) $(INCLUDES64) -o $(EXECFILE64) $(OBJFILES64) $(LIBS64)

#$(OBJFILES): $(SRCFILES) # this doesn't work!
%32.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES32) -c -o $*32.o $*.c 

%64.o : %.c
	$(CC) $(CFLAGS64) $(INCLUDES64) -c -o $*64.o $*.c 

###############################################

#
# Other Standard make rules
#
lint : 
	lint $(SRCFILES) | more

clean:
	rm -f *.o

remove: 
	make clean
	rm -f $(EXECFILE)
	rm -f $(EXECFILE64)
