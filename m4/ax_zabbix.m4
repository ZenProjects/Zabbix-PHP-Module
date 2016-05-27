AC_DEFUN([ZABBIX_DIR],[

  AC_ARG_WITH(
    zabbix-include,
    [
To add Zabbix loadable module header:
AC_HELP_STRING([--with-zabbix-include@<:@=DIR@:>@], [Zabbix include directory]) ],
    ,
    [with_zabbix_include="no"]
  )

  AC_MSG_CHECKING(for Zabbix include directory)

  if test "$with_zabbix_include" = "no"; then
    AC_MSG_ERROR( Specify where Zabbix include directory are --with-zabbix-include)
  else
    # make sure that a well known include file exists
    if test -e $with_zabbix_include/sysinc.h; then
      AC_MSG_RESULT( found!)
    else
      AC_MSG_ERROR( $with_zabbix_include not found. )
    fi
    ZABBIX_INC_DIR=$with_zabbix_include
  fi

  AC_SUBST([ZABBIX_INC_DIR])

])

