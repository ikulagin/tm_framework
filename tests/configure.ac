#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([TM framework tests], [1.0], [ivan.i.kulagin@gmail.com])

# Checks for programs.
AC_PROG_CC([gcc])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile thread_pool/Makefile stm/Makefile mutex/Makefile])
AC_OUTPUT
