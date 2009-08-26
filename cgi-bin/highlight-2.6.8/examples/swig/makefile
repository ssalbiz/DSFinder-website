
CXX=g++ -fPIC

HL_SRC=../../src/

PYTHON_INC=/usr/include/python2.4/

PERL_INC=`perl -MExtUtils::Embed -eperl_inc`

OBJECTFILES :=  ${HL_SRC}configurationreader.o ${HL_SRC}stylecolour.o ${HL_SRC}stringtools.o \
	${HL_SRC}xhtmlgenerator.o ${HL_SRC}latexgenerator.o ${HL_SRC}texgenerator.o ${HL_SRC}rtfgenerator.o \
	${HL_SRC}htmlgenerator.o ${HL_SRC}html32generator.o ${HL_SRC}ansigenerator.o ${HL_SRC}xmlgenerator.o ${HL_SRC}codegenerator.o \
	${HL_SRC}xterm256generator.o ${HL_SRC}languagedefinition.o ${HL_SRC}elementstyle.o ${HL_SRC}documentstyle.o \
	${HL_SRC}preformatter.o ${HL_SRC}platform_fs.o  ${HL_SRC}dirstream.o ${HL_SRC}dirstream_w32_posix.o\
	${HL_SRC}ASStreamIterator.o ${HL_SRC}ASResource.o ${HL_SRC}ASFormatter.o ${HL_SRC}ASBeautifier.o ${HL_SRC}ASEnhancer.o \
	${HL_SRC}Pattern.o ${HL_SRC}Matcher.o

python:
	${MAKE} -C ../../ -f ./makefile
	swig -c++ -python -o highlight_wrap.cpp  highlight.i
	${CXX} -c highlight_wrap.cpp  -I${PYTHON_INC} -I${HL_SRC}
	${CXX} -shared -s highlight_wrap.o  ${OBJECTFILES}   -o _highlight.so

perl:
	${MAKE} -C ../../ -f ./makefile
	swig -c++ -perl -o highlight_wrap.cpp  highlight.i
	${CXX} -c highlight_wrap.cpp ${PERL_INC} -I${HL_SRC}
	${CXX} -shared -s highlight_wrap.o  ${OBJECTFILES}   -o highlight.so


python-clean:
	rm -f _highlight.so highlight_wrap.cpp highlight.py* *.o

perl-clean:
	rm -f highlight.so highlight_wrap.cpp highlight.pm *.o


.PHONY: python python-clean perl perl-clean
