EXECFILE = alfy
EXECFILE64 = alfy64

DIRECTORY = Alfy
################################################

VERSION = 1.5
.PHONY : all
all : $(EXECFILE) $(EXECFILE64)

# 32 bit version
$(EXECFILE):
	cd Src/Alfy; make; cp alfy ../../
# 64 bit version
$(EXECFILE64):
	cd Src/Alfy; make; cp alfy64 ../../

###############################################
#
# Other Standard make rules
#
clean:
	rm $(EXECFILE) $(EXECFILE64)
	cd ExternSrc/DeepShallow/Src/; make clean;
	cd ExternSrc/DeepShallow64/Src; make clean;
	cd Src/Common/Interval; make clean;
	cd Src/Common/Sequence/; make clean;
	cd Src/Common/Util/; make clean;
	cd Src/Alfy; make clean;
tarfile:
	mkdir $(DIRECTORY)_$(VERSION)
	cd Doc; make pdf 
	cp -r Doc $(DIRECTORY)_$(VERSION)
	cp -r Data $(DIRECTORY)_$(VERSION)
	cp -r Scripts $(DIRECTORY)_$(VERSION)
	cp ACKNOWLEDGMENTS COPYRIGHT.GPL.txt COPYRIGHT ChangeLog INSTALL README Makefile $(DIRECTORY)_$(VERSION)
	mkdir $(DIRECTORY)_$(VERSION)/ExternSrc $(DIRECTORY)_$(VERSION)/Lib \
	$(DIRECTORY)_$(VERSION)/Lib/DeepShallow $(DIRECTORY)_$(VERSION)/Lib/DeepShallow64
	mkdir $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow64
	mkdir $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow/Src $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow64/Src
	cp ExternSrc/DeepShallow/COPYRIGHT.* ExternSrc/DeepShallow/README $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow/
	cp -r ExternSrc/DeepShallow/Doc $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow/
	cp ExternSrc/DeepShallow64/COPYRIGHT.* ExternSrc/DeepShallow64/README $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow64/
	cp -r ExternSrc/DeepShallow64/Doc $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow64/
	cp ExternSrc/DeepShallow/Src/Makefile ExternSrc/DeepShallow/Src/*.c ExternSrc/DeepShallow/Src/*.h $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow/Src/
	cp ExternSrc/DeepShallow64/Src/Makefile ExternSrc/DeepShallow64/Src/*.c ExternSrc/DeepShallow64/Src/*.h $(DIRECTORY)_$(VERSION)/ExternSrc/DeepShallow64/Src/
	mkdir $(DIRECTORY)_$(VERSION)/Src $(DIRECTORY)_$(VERSION)/Src/Alfy \
	$(DIRECTORY)_$(VERSION)/Src/Common $(DIRECTORY)_$(VERSION)/Src/Common/Interval \
	$(DIRECTORY)_$(VERSION)/Src/Common/Sequence $(DIRECTORY)_$(VERSION)/Src/Common/Util
	cp Src/Common/*.h $(DIRECTORY)_$(VERSION)/Src/Common/
	cp Src/Common/Interval/Makefile Src/Common/Interval/*.c Src/Common/Interval/*.h $(DIRECTORY)_$(VERSION)/Src/Common/Interval
	cp Src/Common/Sequence/Makefile Src/Common/Sequence/*.c Src/Common/Sequence/*.h $(DIRECTORY)_$(VERSION)/Src/Common/Sequence
	cp Src/Common/Util/Makefile Src/Common/Util/*.c Src/Common/Util/*.h $(DIRECTORY)_$(VERSION)/Src/Common/Util
	cp Src/Alfy/Makefile Src/Alfy/*.c Src/Alfy/*.h $(DIRECTORY)_$(VERSION)/Src/Alfy
	tar cvzfh $(EXECFILE)_$(VERSION).tgz $(DIRECTORY)_$(VERSION)
	mv $(EXECFILE)_$(VERSION).tgz ../
	rm -r $(DIRECTORY)_$(VERSION)