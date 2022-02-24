AC_PREREQ(2.57)
AC_INIT(alexeictrl,0.7.1,bytex64@yahoo.com)
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(alexeictrl,0.7.1)
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(ncurses,initscr)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h ncurses.h linux/joystick.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 client/Makefile
                 libalexeictrl/Makefile
                 server/Makefile
		 tools/Makefile
                 tests/Makefile])
AC_OUTPUT
