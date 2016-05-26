AC_INIT

AC_DEFUN([APACHE_DIR],[

  AC_ARG_WITH(
    apxs,
    [  --with-apxs[=EXE]     Apache2 apxs script],
    ,
    [with_apxs="no"]
  )

  AC_MSG_CHECKING(for Apache apxs script)

  if test "$with_apxs" = "no"; then
    AC_MSG_ERROR( Specify where is the apache apxs script using --with-apxs)
  else
    AP_INCLUDE_DIR=$($with_apxs -q "includedir")
    # make sure that a well known include file exists
    if test -e $AP_INCLUDE_DIR/httpd.h; then
      AC_MSG_RESULT( found!)
    else
      AC_MSG_ERROR( $with_apxs not found. )
    fi
    APXS=$with_apxs
  fi

])

dnl Checking for PHP support
AX_LIB_PHP()
if test "x$want_php" = "xyes"; then
	if test "x$found_php" = "xyes"; then
		have_php="yes"
                echo " Embeded PHP:        ${have_php}"
	else
		AC_MSG_ERROR([Not found php library])
	fi
fi

CFLAGS="${CFLAGS} ${PHP_CPPFLAGS}"
LDFLAGS="${SERVER_LDFLAGS} ${PHP_LDFLAGS}"
LIBS="${SERVER_LIBS} ${PHP_LIBS}"

dnl APACHE_DIR
dnl AC_SUBST(APXS)

AC_OUTPUT(Makefile)

