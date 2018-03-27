/* Copyright 2018 Intel Corporation, Inc

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

MAINTAINERCLEANFILES = 		$(srcdir)/Makefile.in

AM_CPPFLAGS =			-I$(srcdir)/.. \
				-I$(srcdir)/../common \
				-I$(srcdir)/../crypto \
				-I$(srcdir)/../data_mgr \
				-I$(srcdir)/../object_store \
				-I$(srcdir)/../pkcs11 \
				-I$(srcdir)/../session_mgr

noinst_LTLIBRARIES =		libsofthsm_hwinfra.la
libsofthsm_hwinfra_la_SOURCES =	HwInfra.c

SUBDIRS =

EXTRA_DIST =			$(srcdir)/*.h
