/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to default visibility of PKCS#11 entry points */
#define CRYPTOKI_VISIBILITY 1

/* The default log level */
#define DEFAULT_LOG_LEVEL "INFO"

/* Default storage backend for token objects */
#define DEFAULT_OBJECTSTORE_BACKEND "file"

/* The default PKCS#11 library */
#define DEFAULT_PKCS11_LIB "/usr/local/lib/softhsm/libsofthsm2.so"

/* The default location of softhsm2.conf */
#define DEFAULT_SOFTHSM2_CONF "/etc/softhsm2.conf"

/* The default location of the token directory */
#define DEFAULT_TOKENDIR "/var/lib/softhsm/tokens/"

/* Define if advanced AES key wrap without pad is supported */
#define HAVE_AES_KEY_WRAP 1

/* Define if advanced AES key wrap with pad is supported */
/* #undef HAVE_AES_KEY_WRAP_PAD */

/* define if the compiler supports basic C++11 syntax */
#define HAVE_CXX11 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have dlopen */
#define HAVE_DLOPEN 1

/* Define to 1 if you have the `getpwuid_r' function. */
#define HAVE_GETPWUID_R 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `crypto' library (-lcrypto). */
#define HAVE_LIBCRYPTO 1

/* Define to 1 if you have the `sqlite3' library (-lsqlite3). */
/* #undef HAVE_LIBSQLITE3 */

/* Whether LoadLibrary is available */
/* #undef HAVE_LOADLIBRARY */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Build with object store database backend. */
/* #undef HAVE_OBJECTSTORE_BACKEND_DB */

/* Define to 1 if you have the <openssl/ssl.h> header file. */
#define HAVE_OPENSSL_SSL_H 1

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* Define to 1 if you have the <sqlite3.h> header file. */
/* #undef HAVE_SQLITE3_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Maximum PIN length */
#define MAX_PIN_LEN 255

/* Minimum PIN length */
#define MIN_PIN_LEN 4

/* Name of package */
#define PACKAGE "softhsm"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "SoftHSM"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "SoftHSM 2.4.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "softhsm"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.4.0"

/* Non-paged memory for secure storage */
#define SENSITIVE_NON_PAGE /**/

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.4.0"

/* SoftHSM major version number via PKCS#11 */
#define VERSION_MAJOR 2

/* SoftHSM minor version number via PKCS#11 */
#define VERSION_MINOR 4

/* Compile with AES GCM */
#define WITH_AES_GCM 1

/* Compile with Botan support */
/* #undef WITH_BOTAN */

/* Compile with ECC support */
#define WITH_ECC /**/

/* Compile with FIPS 140-2 mode */
/* #undef WITH_FIPS */

/* Compile with GOST support */
#define WITH_GOST /**/

/* Compile with OpenSSL support */
#define WITH_OPENSSL /**/

/* Compile with raw RSA PKCS PSS */
#define WITH_RAW_PSS 1
