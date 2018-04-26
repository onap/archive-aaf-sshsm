MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS =			-I$(srcdir)/.. \
				-I$(srcdir)/../common \
				-I$(srcdir)/../crypto \
				-I$(srcdir)/../data_mgr \
				-I$(srcdir)/../object_store \
				-I$(srcdir)/../pkcs11 \
				-I$(srcdir)/../session_mgr

noinst_LTLIBRARIES =		libsofthsm_hwinfra.la
libsofthsm_hwinfra_la_SOURCES =	HwInfra.cpp

SUBDIRS =

EXTRA_DIST =			$(srcdir)/*.h
