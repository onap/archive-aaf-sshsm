ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = \
        config.log config.status softhsm2.module \
        $(srcdir)/Makefile.in \
        $(srcdir)/config.h.in $(srcdir)/config.h.in~ \
        $(srcdir)/configure \
        $(srcdir)/install-sh $(srcdir)/ltmain.sh $(srcdir)/missing \
        $(srcdir)/depcomp $(srcdir)/aclocal.m4 $(srcdir)/compile \
        $(srcdir)/config.guess $(srcdir)/config.sub

SUBDIRS = src

if WITH_P11KIT
p11moddir = @P11KIT_PATH@
p11mod_DATA = softhsm2.module
endif

EXTRA_DIST =	$(srcdir)/aes_wrap_key_with_pad/botan-diff \
		$(srcdir)/aes_wrap_key_with_pad/README \
		$(srcdir)/CMakeLists.txt \
		$(srcdir)/CMAKE-NOTES.md \
		$(srcdir)/config.h.in.cmake \
		$(srcdir)/FIPS-NOTES.md \
		$(srcdir)/LICENSE \
		$(srcdir)/m4/*.m4 \
		$(srcdir)/modules/*.cmake \
		$(srcdir)/modules/tests/*.c \
		$(srcdir)/modules/tests/*.cpp \
		$(srcdir)/OSX-NOTES.md \
		$(srcdir)/README.md
