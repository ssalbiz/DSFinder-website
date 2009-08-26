# Installation script for highlight.
# See INSTALL for details.

# Installation directories:

# Destination directory for installation (intended for packagers)
DESTDIR = 

# Root directory for final installation
PREFIX = /var/www/localhost/htdocs/

# Location of the highlight data files:
data_dir = ${PREFIX}/cgi-bin/

# Location of the highlight binary:
bin_dir = ${PREFIX}/cgi-bin/

# Location of the highlight man page:
man_dir = ${PREFIX}/share/man/man1/

# Location of the highlight documentation:
doc_dir = ${PREFIX}/share/doc/highlight/

# Location of the highlight examples:
examples_dir = ${PREFIX}/share/doc/highlight/examples/

# Location of the highlight config files:
conf_dir = ${PREFIX}/etc/highlight/

# Commands:
INSTALL_DATA=install -m644
INSTALL_PROGRAM=install -m755
MKDIR=mkdir -p -m 755
RMDIR=rm -r -f

all:
	${MAKE} -C ./src -f ./makefile HL_DATA_DIR=${data_dir} HL_CONFIG_DIR=${conf_dir}

all-localgetopt:
	${MAKE} -C ./src -f ./makefile HL_DATA_DIR=${data_dir} HL_CONFIG_DIR=${conf_dir} USE_LOCAL_GETOPT='true'

all-gui:
	${MAKE} -C ./src -f ./makefile HL_DATA_DIR=${data_dir} HL_CONFIG_DIR=${conf_dir} BUILD_GUI='true'
	@echo
	@echo "You need to run 'make install' AND 'make install-gui' now!"

install:
	@echo "This script will install highlight in the following directories:"
	@echo "Data directory:	        ${DESTDIR}${data_dir}"
	@echo "Documentation directory: ${DESTDIR}${doc_dir}"
	@echo "Examples directory:      ${DESTDIR}${examples_dir}"
	@echo "Manual directory:        ${DESTDIR}${man_dir}"
	@echo "Binary directory:        ${DESTDIR}${bin_dir}"
	@echo "Configuration directory: ${DESTDIR}${conf_dir}"
	@echo

	${MKDIR} ${DESTDIR}${doc_dir}
	${MKDIR} ${DESTDIR}${conf_dir}
	${MKDIR} ${DESTDIR}${examples_dir} \
		${DESTDIR}${examples_dir}plugins \
		${DESTDIR}${examples_dir}plugins/dokuwiki \
		${DESTDIR}${examples_dir}plugins/movabletype \
		${DESTDIR}${examples_dir}plugins/wordpress \
		${DESTDIR}${examples_dir}swig
	${MKDIR} ${DESTDIR}${data_dir} \
		${DESTDIR}${data_dir}themes \
		${DESTDIR}${data_dir}langDefs \
		${DESTDIR}${data_dir}indentSchemes \
		${DESTDIR}${data_dir}helpmsg
	${MKDIR} ${DESTDIR}${man_dir}
	${MKDIR} ${DESTDIR}${bin_dir}

	${INSTALL_DATA} ./langDefs/*.lang ${DESTDIR}${data_dir}langDefs/
	${INSTALL_DATA} ./*.conf ${DESTDIR}${conf_dir}
	${INSTALL_DATA} ./themes/*.style ${DESTDIR}${data_dir}themes/
	${INSTALL_DATA} ./indentSchemes/*.indent ${DESTDIR}${data_dir}indentSchemes/
	${INSTALL_DATA} ./helpmsg/*.help ${DESTDIR}${data_dir}helpmsg/
	${INSTALL_DATA} ./man/highlight.1.gz ${DESTDIR}${man_dir}
	${INSTALL_DATA} ./AUTHORS ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./README ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./README_DE ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./README_INDENT ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./README_REGEX ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./README_LANGLIST ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./ChangeLog ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./COPYING ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./INSTALL ${DESTDIR}${doc_dir}
	${INSTALL_DATA} ./examples/plugins/dokuwiki/* ${DESTDIR}${examples_dir}plugins/dokuwiki/
	${INSTALL_DATA} ./examples/plugins/movabletype/* ${DESTDIR}${examples_dir}plugins/movabletype/
	${INSTALL_DATA} ./examples/plugins/wordpress/* ${DESTDIR}${examples_dir}plugins/wordpress/
	${INSTALL_DATA} ./examples/swig/*.py ./examples/swig/*.pl ${DESTDIR}${examples_dir}swig
	${INSTALL_DATA} ./examples/swig/*.i ./examples/swig/makefile ${DESTDIR}${examples_dir}swig
	${INSTALL_DATA} ./examples/swig/README_SWIG ${DESTDIR}${doc_dir}
	${INSTALL_PROGRAM} ./src/highlight ${DESTDIR}${bin_dir}

	@echo
	@echo "Done."
	@echo "Type highlight --help or man highlight for instructions."
	@echo "Highlight help is also available in Brazilian, Czech, German, French and Spanish"
	@echo "(highlight --help-lang [br|cs|de|en|es|fr])."
	@echo "Take a look at ${DESTDIR}${examples_dir} for plugins and SWIG language bindings."
	@echo "Execute 'make install-gui' to install the highlight GUI ('make all-gui')."
	@echo "Do not hesitate to report problems. Unknown bugs are hard to fix."

install-gui:
	@echo "Installing files for the GUI..."
	${MKDIR} ${DESTDIR}${data_dir} \
		${DESTDIR}${data_dir}gui_files \
		${DESTDIR}${data_dir}gui_files/ext \
		${DESTDIR}${data_dir}gui_files/i18n

	${INSTALL_DATA} ./gui_files/i18n/* ${DESTDIR}${data_dir}gui_files/i18n/
	${INSTALL_DATA} ./gui_files/ext/* ${DESTDIR}${data_dir}gui_files/ext/
	${INSTALL_PROGRAM} ./src/highlight-gui ${DESTDIR}${bin_dir}

uninstall:
	@echo "Removing highlight files from system..."
	${RMDIR} ${DESTDIR}${data_dir}
	${RMDIR} ${DESTDIR}${doc_dir}
	${RMDIR} ${DESTDIR}${conf_dir}
	rm ${DESTDIR}${man_dir}highlight.1.gz
	rm ${DESTDIR}${bin_dir}highlight
	rm ${DESTDIR}${bin_dir}highlight-gui
	@echo "Done. Have a nice day!"

clean:
	$(MAKE) -C ./src -f ./makefile clean

apidocs:
	doxygen Doxyfile

help:
	@echo "This makefile offers the following options:"
	@echo
	@echo "(all)            Compile for Linux systems."
	@echo "all-localgetopt  Compile using a local copy of getopt."
	@echo "all-gui          Compile the wxWidgets GUI."
	@echo "install*         Copy all data files to ${data_dir}."
	@echo "install-gui*     Copy GUI data files to ${data_dir}."
	@echo "clean            Remove object files and binary."
	@echo "apidocs          Generate HTML API documentation using doxygen."
	@echo "uninstall*       Remove highlight files from system."
	@echo
	@echo "* Command needs root privileges."
	@echo "See highlight/makefile for compilation and linking options."

# Target needed for redhat 9.0 rpmbuild
install-strip:

.PHONY: clean all all-localgetopt install apidocs help uninstall install-strip
