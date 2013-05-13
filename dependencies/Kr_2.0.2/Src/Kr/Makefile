CC=gcc
# with profiling & debugging:
# CFLAGS= -m64 -O0 -g -Wall -Wshadow -pedantic   -pg -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 #-mcmodel=medium
# without profiling & debugging:
#CFLAGS= -g -Wall -Wshadow -pedantic -DSTRMAT -DSTATS
#CFLAGS= -O3 -DSTRMAT -DSTATS

#32 bit version
#when the option VER32 is set, the source will be compiled as the 32 bit version          
# preprocessor options: VER32 -> 32-bit version, DEBUG -> debug version, UNIX -> unix version (as opposed to windows version); m64 for unix/mac, exclude for linux!

CFLAGS= -O3 -Wall -Wshadow -pedantic -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -std=c99 -DVER32 -DUNIX -I/opt/local/include/ -L/opt/local/lib #-m64

#64 bit version: option VER32 is not set
CFLAGS64= -O3 -Wall -Wshadow -pedantic -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -std=c99 -DUNIX -I/opt/local/include/ -L/opt/local/lib #-m64

FLATS = ACKNOWLEDGMENTS COPYRIGHT.GPL.txt COPYRIGHT INSTALL LICENSE README ChangeLog #
FLATS_DIR = ../../

################################################
#include directories
INCLUDE_COMMON_DIR = ../Common
INCLUDE_UTIL_DIR = ../Common/Util
INCLUDE_SEQUENCE_DIR = ../Common/Sequence
INCLUDE_INTERVAL_DIR = ../Common/Interval
# INCLUDE_DIR32 = ../../Include/DeepShallow
# INCLUDE_DIR64 = ../../Include/DeepShallow64
INCLUDE_DIR32 = ../../ExternSrc/DeepShallow/Src
INCLUDE_DIR64 = ../../ExternSrc/DeepShallow64/Src

INCLUDES = -I$(INCLUDE_COMMON_DIR) -I$(INCLUDE_UTIL_DIR) -I$(INCLUDE_SEQUENCE_DIR) -I$(INCLUDE_INTERVAL_DIR)
INCLUDES32 = $(INCLUDES) -I$(INCLUDE_DIR32)
INCLUDES64 = $(INCLUDES) -I$(INCLUDE_DIR64)

#library directories
LIB_DIR = ../../Lib
LIB_DIR32 = ../../Lib/DeepShallow
LIB_DIR64 = ../../Lib/DeepShallow64

LIBS32 = -lm $(LIB_DIR)/libsequence.a $(LIB_DIR32)/ds_ssort.a $(LIB_DIR32)/bwtlcp.a $(LIB_DIR)/libinterval.a $(LIB_DIR)/libutil.a -lgsl -lgslcblas
LIBS64 = -lm $(LIB_DIR)/libsequence64.a $(LIB_DIR64)/ds_ssort.a $(LIB_DIR64)/bwtlcp.a $(LIB_DIR)/libinterval64.a $(LIB_DIR)/libutil64.a -lgsl -lgslcblas

#LIBS = libutil.a libutil64.a libsequence.a libsequence64.a
# just one library in LIBS; otherwise $(LIBS) would be executed as many numbers as there are libraries
LIBS = libutil.a 

################################################
#
# The source, object and library files
#
SRCFILES = mainKr.c analysis.c interface.c lcpSubjectTree.c subjectUnion.c divergence.c

OBJFILES = $(addsuffix 32.o, $(basename $(SRCFILES))) 
OBJFILES64 = $(addsuffix 64.o, $(basename $(SRCFILES))) 

EXECFILE = kr
EXECFILE64 = kr64

VERSION = 2.0.2
DIRECTORY = Kr_$(VERSION)
################################################
#
# The dependencies for each of the *.o and *64.o files.
#
.PHONY : all
all : $(LIBS) $(EXECFILE) $(EXECFILE64) 

$(LIBS):
#	cd ../Common/Util; make; mv *.a --target-directory=../$(LIB_DIR) # target dir = relative position from ../Common/Util
	cd ../Common/Util; make; mv libutil.a libutil64.a ../$(LIB_DIR) # target dir = relative position from ../Common/Util
	cd ../Common/Sequence; make; mv libsequence.a libsequence64.a ../$(LIB_DIR)
	cd ../Common/Interval; make; mv libinterval.a libinterval64.a ../$(LIB_DIR)
#	cd ../../ExternSrc/DeepShallow/Src; make ds_ssort.a; make bwtlcp.a; mv *.a --target-directory=../$(LIB_DIR32); rm -f *.o
#	cd ../../ExternSrc/DeepShallow64/Src; make ds_ssort.a; make bwtlcp.a; mv *.a --target-directory=../$(LIB_DIR64); rm -f *.o
	cd ../../ExternSrc/DeepShallow/Src; make ds_ssort.a; make bwtlcp.a; mv ds_ssort.a bwtlcp.a ../$(LIB_DIR32); rm -f *.o
	cd ../../ExternSrc/DeepShallow64/Src; make ds_ssort.a; make bwtlcp.a; mv ds_ssort.a bwtlcp.a ../$(LIB_DIR64); rm -f *.o

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
	cd ../../Src/Common/Interval; make clean
	cd ../../Src/Common/Util; make clean
	cd ../../Src/Common/Sequence; make clean
	cd ../../ExternSrc/DeepShallow/Src; make clean
	cd ../../ExternSrc/DeepShallow64/Src; make clean

remove: 
	make clean
	rm -f $(EXECFILE)
	rm -f $(EXECFILE64)

tarfile:
	make clean
	mkdir $(DIRECTORY)
	cd $(DIRECTORY); mkdir Src; cd Src; mkdir Common; mkdir Kr; cd ../; mkdir Lib; mkdir Lib/DeepShallow; mkdir Lib/DeepShallow64
	cp -rf ../../Src/Common $(DIRECTORY)/Src
	cp -rf ../../ExternSrc $(DIRECTORY)
	cp -rf ../../Data $(DIRECTORY)
	cp -rf ../../Doc $(DIRECTORY)
	cp -f $(SRCFILES) *.h Makefile $(DIRECTORY)/Src/Kr
	cd ../..; cp -f $(FLATS) Src/Kr/$(DIRECTORY); cd Src/Kr
	tar cvzfh $(EXECFILE)_$(VERSION).tgz $(DIRECTORY)
	mv $(EXECFILE)_$(VERSION).tgz ../../../
	rm -rf $(DIRECTORY)

