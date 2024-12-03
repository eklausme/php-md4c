dnl config.m4 for php-md4c extension

PHP_ARG_ENABLE(md4c, [whether to enable MD4C support],
	[AS_HELP_STRING([--enable-md4c], [Enable MD4C support])], [yes], [yes])

PHP_ARG_ENABLE(system-libmd4c, [whether to enable system libmd4c library],
	[AS_HELP_STRING([--disable-system-libmd4c], [Disable system libmd4c])], [auto], [no])

if test "$PHP_MD4C" != "no"; then
	AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

	dnl by default (auto mode) use system library if available
	if test "$PHP_SYSTEM_LIBMD4C" = "auto"; then
		AC_MSG_CHECKING([libmd4c])
		if pkg-config --exists md4c-html; then
			AC_MSG_RESULT([use available system library])
			PHP_SYSTEM_LIBMD4C="yes"
		else
			AC_MSG_RESULT([use bundled library])
		fi
	fi

	if test "$PHP_SYSTEM_LIBMD4C" = "yes"; then
		dnl use system library
		PKG_CHECK_MODULES([LIBMD4C_HTML], [md4c-html])
		PHP_EVAL_INCLINE($LIBMD4C_HTML_CFLAGS)
		PHP_EVAL_LIBLINE($LIBMD4C_HTML_LIBS, MD4C_SHARED_LIBADD)
		AC_DEFINE([HAVE_LIBMD4C], 1, [With system libmd4c library])

		LIBMD4C_VERSION=`$PKG_CONFIG md4c-html --modversion`
		AC_DEFINE_UNQUOTED([PHP_LIBMD4C_VERSION], ["$LIBMD4C_VERSION"], [With system libmd4c library])

		PHP_NEW_EXTENSION(md4c, php_md4c.c, $ext_shared)
	else
		dnl use bundled library
		LIBMD4C_SOURCES="$EXT_SOURCES \
			libmd4c/src/entity.c \
			libmd4c/src/md4c.c \
			libmd4c/src/md4c-html.c \
		"
		PHP_NEW_EXTENSION(md4c, php_md4c.c $LIBMD4C_SOURCES, $ext_shared)
		PHP_ADD_INCLUDE([$ext_srcdir/libmd4c/src])
		PHP_ADD_BUILD_DIR([$ext_builddir/libmd4c/src])
	fi
	PHP_SUBST(MD4C_SHARED_LIBADD)
	AC_DEFINE(HAVE_MD4C, 1, [ ])
fi
