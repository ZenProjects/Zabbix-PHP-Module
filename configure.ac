# Autotool M4 Script to add compilation with libphp Embed SAPI
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

m4_include([m4/ax_lib_php.m4])
m4_include([m4/ax_zabbix.m4])

AC_INIT

dnl Checking for PHP support
AX_LIB_PHP("5.5")
if test "x$found_php" = "xyes"; then
	have_php="yes"
else
	AC_MSG_ERROR([Specify where php installation are with --with-php])
fi

CFLAGS="${CFLAGS} ${PHP_CPPFLAGS}"
LDFLAGS="${SERVER_LDFLAGS} ${PHP_LDFLAGS}"
LIBS="${SERVER_LIBS} ${PHP_LIBS}"
MODULE_NAME=zbx_php

dnl Checking for Zabbix directory
ZABBIX_DIR

AC_SUBST([MODULE_NAME])
AC_SUBST([CFLAGS])
AC_SUBST([LDFLAGS])
AC_SUBST([LIBS])
AC_SUBST([prefix])

AC_CONFIG_HEADER(zbx_php_config.h)
AC_OUTPUT(Makefile)

echo ""
echo " Zabbix PHP laodable module install diretory   = ${prefix}"
echo " Embeded PHP base directory                    = ${PHP_PREFIX}"
echo " Zabbix include directory                      = ${ZABBIX_INC_DIR}"
