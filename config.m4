dnl config.m4 for php-md4c extension

PHP_ARG_ENABLE(md4c, [whether to enable MD4C support],
	[AS_HELP_STRING([--enable-md4c], [Enable MD4C support])], [yes], [yes])

PHP_ARG_ENABLE(system-libmd4c, [whether to enable system libmd4c library],
	[AS_HELP_STRING([--enable-system-md4c], [Enable system libmd4c])], [no], [no])

if test "$PHP_MD4C" != "no"; then
	if test "$PHP_SYSTEM_LIBMD4C" = "yes"; then
		AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

		PKG_CHECK_MODULES([LIBMD4C_HTML], [md4c-html])
		PHP_EVAL_INCLINE($LIBMD4C_HTML_CFLAGS)
		PHP_EVAL_LIBLINE($LIBMD4C_HTML_LIBS, MD4C_SHARED_LIBADD)
		AC_DEFINE([HAVE_LIBMD4C], 1, [With system libmd4c library])

		LIBMD4C_VERSION=`$PKG_CONFIG md4c-html --modversion`
		AC_DEFINE_UNQUOTED([PHP_LIBMD4C_VERSION], ["$LIBMD4C_VERSION"], [With system libmd4c library])
	fi
	PHP_SUBST(MD4C_SHARED_LIBADD)
	AC_DEFINE(HAVE_MD4C, 1, [ ])
	PHP_NEW_EXTENSION(md4c, md4c.c, $ext_shared)
fi
