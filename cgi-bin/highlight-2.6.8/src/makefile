# Simple Makefile for Highlight
# This file will compile the highlight binary.
# See INSTALL for instructions.

# Add -DHL_DATA_DIR=\"/your/path/\" to CFLAGS if you want to define a
# custom installation directory not listed in INSTALL.
# Copy *.conf, ./langDefs, ./themes amd ./indentSchemes to /your/path/.
# See ../makefile for the definition of ${data_dir}

# Add -DCONFIG_FILE_PATH=\"/your/path/.highlightrc\" if you want to define a
# custom path to the highlight configuration file (default: $HOME/.highlightrc)

# Add -DUSE_FN_MATCH to enable better wildcard support with the --batch-recursive option.
# See "man 3 fnmatch" for details.

CXX=c++

CFLAGS:=-O2 ${CFLAGS}

# Do not strip by default (Mac OS X lazy pointer issues)
# Add -static to avoid linking with shared libs (can cause trouble when highlight
# is run as service)
#LDFLAGS = 
#LDFLAGS = ${LDFLAGS} -s

CXX_COMPILE=${CXX} ${CFLAGS} -c
CXX_LINK=${CXX} ${LDFLAGS} -o highlight ${OBJECTFILES}

# Data directories (data dir, configuration file dir)
CXX_DIR=-DHL_DATA_DIR=\"${HL_DATA_DIR}\" -DHL_CONFIG_DIR=\"${HL_CONFIG_DIR}\"

OBJECTFILES:=configurationreader.o stylecolour.o stringtools.o \
	xhtmlgenerator.o latexgenerator.o texgenerator.o rtfgenerator.o \
	htmlgenerator.o ansigenerator.o xmlgenerator.o codegenerator.o xterm256generator.o \
	html32generator.o languagedefinition.o help.o elementstyle.o documentstyle.o \
	datadir.o cmdlineoptions.o preformatter.o \
	ASStreamIterator.o ASResource.o ASFormatter.o ASBeautifier.o ASEnhancer.o\
	dirstream.o dirstream_w32_posix.o platform_fs.o \
	Pattern.o Matcher.o

GUI_DIR=./gui/

# Third-Party software paths
ASTYLE_DIR=./astyle/
DIRSTREAM_DIR=./dirstream0.4/
REGEX_DIR=./re/

ifeq ($(USE_LOCAL_GETOPT),true)
	OBJECTFILES:=${OBJECTFILES} getopt.o
	CFLAGS:=${CFLAGS} -DUSE_LOCAL_GETOPT
endif

ifeq ($(BUILD_GUI),true)
	WX_FLAGS=`wx-config --cxxflags` -I.
	OBJECTFILES:=${OBJECTFILES} gui.o gui_failure_dlg.o gui_showfile_dlg.o
	CXX_LINK=${CXX} ${LDFLAGS} `wx-config --libs core,base,html,adv` -o highlight-gui ${OBJECTFILES}
else
	OBJECTFILES:=${OBJECTFILES} main.o
	CXX_LINK=${CXX} ${LDFLAGS} -o highlight ${OBJECTFILES}
endif

compile: ${OBJECTFILES}
	${CXX_LINK}

ansigenerator.o: ansigenerator.cpp ansigenerator.h codegenerator.h \
  languagedefinition.h configurationreader.h stringtools.h enums.h \
  documentstyle.h elementstyle.h stylecolour.h ${ASTYLE_DIR}astyle.h \
  preformatter.h charcodes.h version.h
	${CXX_COMPILE} ansigenerator.cpp

ASBeautifier.o: ${ASTYLE_DIR}ASBeautifier.cpp ${ASTYLE_DIR}astyle.h
	${CXX_COMPILE} ${ASTYLE_DIR}ASBeautifier.cpp

ASFormatter.o: ${ASTYLE_DIR}ASFormatter.cpp ${ASTYLE_DIR}astyle.h
	${CXX_COMPILE} ${ASTYLE_DIR}ASFormatter.cpp

ASResource.o: ${ASTYLE_DIR}ASResource.cpp ${ASTYLE_DIR}astyle.h
	${CXX_COMPILE} ${ASTYLE_DIR}ASResource.cpp

ASEnhancer.o: ${ASTYLE_DIR}ASResource.cpp ${ASTYLE_DIR}astyle.h
	${CXX_COMPILE} ${ASTYLE_DIR}ASEnhancer.cpp

ASStreamIterator.o: ${ASTYLE_DIR}ASStreamIterator.cpp ${ASTYLE_DIR}astyle.h
	${CXX_COMPILE} ${ASTYLE_DIR}ASStreamIterator.cpp

cmdlineoptions.o: cmdlineoptions.cpp cmdlineoptions.h stringtools.h \
  configurationreader.h datadir.h platform_fs.h enums.h
	${CXX_COMPILE} cmdlineoptions.cpp

codegenerator.o: codegenerator.cpp codegenerator.h languagedefinition.h \
  configurationreader.h stringtools.h enums.h documentstyle.h \
  elementstyle.h stylecolour.h ${ASTYLE_DIR}astyle.h preformatter.h \
  htmlgenerator.h version.h charcodes.h xhtmlgenerator.h rtfgenerator.h \
  latexgenerator.h texgenerator.h ansigenerator.h html32generator.h
	${CXX_COMPILE} codegenerator.cpp

configurationreader.o: configurationreader.cpp configurationreader.h \
  stringtools.h
	${CXX_COMPILE} configurationreader.cpp

datadir.o: datadir.cpp datadir.h platform_fs.h
	${CXX_COMPILE} datadir.cpp ${CXX_DIR}

dirstream.o:  ${DIRSTREAM_DIR}dirstream.cpp ${DIRSTREAM_DIR}dirstream.h
	${CXX_COMPILE} ${DIRSTREAM_DIR}dirstream.cpp

dirstream_w32_posix.o:  ${DIRSTREAM_DIR}dirstream_w32_posix.cpp ${DIRSTREAM_DIR}dirstream.h
	${CXX_COMPILE} ${DIRSTREAM_DIR}dirstream_w32_posix.cpp

platform_fs.o: platform_fs.cpp platform_fs.h ${DIRSTREAM_DIR}dirstream.h
	${CXX_COMPILE} platform_fs.cpp

documentstyle.o: documentstyle.cpp documentstyle.h configurationreader.h \
  stringtools.h elementstyle.h stylecolour.h
	${CXX_COMPILE} documentstyle.cpp

elementstyle.o: elementstyle.cpp elementstyle.h stylecolour.h
	${CXX_COMPILE} elementstyle.cpp

getopt.o: getopt.cpp
	${CXX_COMPILE} getopt.cpp

help.o: help.cpp help.h
	${CXX_COMPILE} help.cpp

htmlgenerator.o: htmlgenerator.cpp htmlgenerator.h codegenerator.h \
  languagedefinition.h configurationreader.h platform_fs.h enums.h \
  documentstyle.h elementstyle.h stylecolour.h \
  ${ASTYLE_DIR}astyle.h preformatter.h version.h charcodes.h
	${CXX_COMPILE} htmlgenerator.cpp

languagedefinition.o: languagedefinition.cpp languagedefinition.h \
  configurationreader.h platform_fs.h enums.h
	${CXX_COMPILE} languagedefinition.cpp

latexgenerator.o: latexgenerator.cpp latexgenerator.h codegenerator.h \
  languagedefinition.h configurationreader.h stringtools.h enums.h \
  documentstyle.h elementstyle.h stylecolour.h ${ASTYLE_DIR}astyle.h \
  preformatter.h version.h charcodes.h
	${CXX_COMPILE} latexgenerator.cpp

preformatter.o: preformatter.cpp preformatter.h stringtools.h
	${CXX_COMPILE} preformatter.cpp

main.o: main.cpp main.h cmdlineoptions.h platform_fs.h \
  configurationreader.h datadir.h enums.h codegenerator.h \
  languagedefinition.h documentstyle.h elementstyle.h \
  stylecolour.h ${ASTYLE_DIR}astyle.h preformatter.h \
  help.h version.h
	${CXX_COMPILE} main.cpp ${CXX_DIR}

rtfgenerator.o: rtfgenerator.cpp rtfgenerator.h codegenerator.h \
  languagedefinition.h configurationreader.h stringtools.h enums.h \
  documentstyle.h elementstyle.h stylecolour.h ${ASTYLE_DIR}astyle.h \
  preformatter.h charcodes.h version.h
	${CXX_COMPILE} rtfgenerator.cpp

stringtools.o: stringtools.cpp stringtools.h
	${CXX_COMPILE} stringtools.cpp

stylecolour.o: stylecolour.cpp stylecolour.h
	${CXX_COMPILE} stylecolour.cpp

texgenerator.o: texgenerator.cpp texgenerator.h charcodes.h version.h \
  codegenerator.h languagedefinition.h configurationreader.h \
  stringtools.h enums.h documentstyle.h elementstyle.h stylecolour.h \
  ${ASTYLE_DIR}astyle.h preformatter.h
	${CXX_COMPILE} texgenerator.cpp

xhtmlgenerator.o: xhtmlgenerator.cpp xhtmlgenerator.h htmlgenerator.h \
  codegenerator.h languagedefinition.h configurationreader.h \
  stringtools.h enums.h documentstyle.h elementstyle.h stylecolour.h \
  ${ASTYLE_DIR}astyle.h preformatter.h version.h \
  charcodes.h
	${CXX_COMPILE} xhtmlgenerator.cpp

xmlgenerator.o: xmlgenerator.cpp xmlgenerator.h codegenerator.h \
  languagedefinition.h configurationreader.h stringtools.h enums.h \
  documentstyle.h elementstyle.h stylecolour.h ${ASTYLE_DIR}astyle.h \
  preformatter.h version.h
	${CXX_COMPILE} xmlgenerator.cpp

xterm256generator.o: xterm256generator.cpp xterm256generator.h codegenerator.h
	${CXX_COMPILE} xterm256generator.cpp

html32generator.o: html32generator.cpp html32generator.h
	${CXX_COMPILE} html32generator.cpp

Matcher.o:  ${REGEX_DIR}Matcher.cpp ${REGEX_DIR}Matcher.h
	${CXX_COMPILE} ${REGEX_DIR}Matcher.cpp

Pattern.o:  ${REGEX_DIR}Pattern.cpp ${REGEX_DIR}Pattern.h
	${CXX_COMPILE} ${REGEX_DIR}Pattern.cpp

# WX GUI Targets ###################

gui.o: ${GUI_DIR}gui.cpp ${GUI_DIR}gui.h
	${CXX_COMPILE} ${GUI_DIR}gui.cpp ${WX_FLAGS} 

gui_failure_dlg.o: ${GUI_DIR}gui_failure_dlg.cpp ${GUI_DIR}gui_failure_dlg.h
	${CXX_COMPILE} ${GUI_DIR}gui_failure_dlg.cpp ${WX_FLAGS} 

gui_showfile_dlg.o: ${GUI_DIR}gui_showfile_dlg.cpp ${GUI_DIR}gui_showfile_dlg.h
	${CXX_COMPILE} ${GUI_DIR}gui_showfile_dlg.cpp ${WX_FLAGS}

#gui_options_dlg.o: ${GUI_DIR}gui_options_dlg.cpp ${GUI_DIR}gui_options_dlg.h
#	${CXX_COMPILE} ${GUI_DIR}gui_options_dlg.cpp ${WX_FLAGS} 

clean:
	@rm -f *.o
	@rm -f ./highlight
	@rm -f ./highlight_gui
	@rm -f ./.deps/*
