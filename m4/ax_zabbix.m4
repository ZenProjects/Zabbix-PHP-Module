# Autotool M4 Script to add compilation of Zabbix Module with zabbix header
#
# Copyright (C) 2007-2016 Mathieu CARBONNEAUX
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

AC_DEFUN([AC_CHECK_ZABBIX_DIR],[

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
    if test -e $with_zabbix_include/module.h; then
      AC_MSG_RESULT( found!)
    else
      AC_MSG_ERROR( $with_zabbix_include not found. )
    fi
    ZABBIX_INC_DIR=$with_zabbix_include
    CPPFLAGS="-I$ZABBIX_INC_DIR"
  fi

  AC_SUBST([ZABBIX_INC_DIR])

])

