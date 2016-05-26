##### http://autoconf-archive.cryp.to/ax_lib_php.html
#
# SYNOPSIS
#
#   AX_LIB_PHP([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of PHP Embeded library
#   of particular version or newer.
#
#   AX_LIB_PHP macro takes only one argument which is optional. If
#   there is no required version passed, then macro does not run
#   version test.
#
#   The --with-php option takes one of three possible values:
#
#   no - do not check for PHP embeded library
#
#   yes - do check for PHP library in standard locations
#   (php_config should be in the PATH)
#
#   path - complete path to php_config utility, use this option if
#   php_config can't be found in the PATH
#
#   This macro calls:
#
#     AC_SUBST(PHP_CFLAGS)
#     AC_SUBST(PHP_LDFLAGS)
#     AC_SUBST(PHP_VERSION)
#
#   And sets:
#
#     HAVE_PHP
#
# LAST MODIFICATION
#
#   2006-09-29
#
# COPYLEFT
#
#   Copyright (c) 2007 Mathieu CARBONNEAUX
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_LIB_PHP],
[
    PHP_CONFIG="no"

AC_ARG_WITH(php,
[
To add PHP embeded poller:
AC_HELP_STRING([--with-php@<:@=ARG@:>@], [use PHP Embeded library @<:@default=no@:>@, optionally specify path to php-config ]) ],[
        if test "$withval" = "no"; then
            want_php="no"
        elif test "$withval" = "yes"; then
            want_php="yes"
        else
            want_php="yes"
            PHP_CONFIG_PATH="$withval"
        fi
        ],
        [want_php="no"]
    )

    PHP_CFLAGS=""
    PHP_LDFLAGS=""
    PHP_LIBS=""
    PHP_VERSION=""

    dnl
    dnl Check PHP Embeded libraries (libphp5)
    dnl

    if test "$want_php" = "yes"; then

        if test -z "$PHP_CONFIG" -o test; then
            AC_PATH_PROG([PHP_CONFIG], [php-config], [no] , [$PHP_CONFIG_PATH:$PATH])
        fi

        if test -f "$PHP_CONFIG"; then

            PHP_CFLAGS="`$PHP_CONFIG --includes` -I/product/php/moteur/php-5.2.4/include/php/sapi/embed"
            PHP_PREFIX="`$PHP_CONFIG --prefix`"
            PHP_LDFLAGS="`$PHP_CONFIG --libs` -L$PHP_PREFIX/lib"

	    _save_php_libs="${LIBS}"
	    _save_php_ldflags="${LDFLAGS}"
	    _save_php_cflags="${CFLAGS}"
	    LDFLAGS="${LDFLAGS} ${PHP_LDFLAGS}"
	    CFLAGS="${CFLAGS} ${PHP_CFLAGS}"

	    AC_CHECK_LIB(php5 , php_embed_init,[ PHP_LIBS="-lphp5" ],[ AC_MSG_ERROR([Not found php embeded library]) ])

	    LIBS="${_save_php_libs}"
	    LDFLAGS="${_save_php_ldflags}"
	    CFLAGS="${_save_php_cflags}"
	    unset _save_php_libs
	    unset _save_php_ldflags
	    unset _save_php_cflags

	    PHP_VERSION=`$PHP_CONFIG --version`

	    AC_DEFINE(HAVE_PHP,1,[Define to 1 if PHP libraries are available])

            CFLAGS="${CFLAGS} ${PHP_CFLAGS}"
            found_php="yes"
        else
            found_php="no"
        fi
    fi

    dnl
    dnl Check if required version of PHP is available
    dnl


    php_version_req=ifelse([$1], [], [], [$1])

    if test "$found_php" = "yes" -a -n "$php_version_req"; then

        AC_MSG_CHECKING([if PHP version is >= $php_version_req])

        dnl Decompose required version string of PHP
        dnl and calculate its number representation
        php_version_req_major=`expr $php_version_req : '\([[0-9]]*\)'`
        php_version_req_minor=`expr $php_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        php_version_req_micro=`expr $php_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$php_version_req_micro" = "x"; then
            php_version_req_micro="0"
        fi

        php_version_req_number=`expr $php_version_req_major \* 1000000 \
                                   \+ $php_version_req_minor \* 1000 \
                                   \+ $php_version_req_micro`

        dnl Decompose version string of installed PHP
        dnl and calculate its number representation
        php_version_major=`expr $PHP_VERSION : '\([[0-9]]*\)'`
        php_version_minor=`expr $PHP_VERSION : '[[0-9]]*\.\([[0-9]]*\)'`
        php_version_micro=`expr $PHP_VERSION : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$php_version_micro" = "x"; then
            php_version_micro="0"
        fi

        php_version_number=`expr $php_version_major \* 1000000 \
                                   \+ $php_version_minor \* 1000 \
                                   \+ $php_version_micro`

        php_version_check=`expr $php_version_number \>\= $php_version_req_number`
        if test "$php_version_check" = "1"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
        fi
    fi

    AC_SUBST([PHP_VERSION])
    AC_SUBST([PHP_CFLAGS])
    AC_SUBST([PHP_LDFLAGS])
    AC_SUBST([PHP_LIBS])
])
