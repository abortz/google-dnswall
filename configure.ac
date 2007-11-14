## Process this file with autoconf to produce configure.
## In general, the safest way to proceed is to run the following:
##    % aclocal -I . -I `pwd`/../autoconf && autoheader && autoconf && automake

# make sure we're interpreted by some minimal autoconf
AC_PREREQ(2.57)

AC_INIT([dnswall], [0.1.4], [opensource@google.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/dnswall.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h stdlib.h string.h strings.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([bzero socket])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
