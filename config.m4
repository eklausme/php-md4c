dnl config.m4 for php-md4c extension

PHP_ARG_WITH(md4c, [whether to enable MD4C support],
[  --with-md4c[[=DIR]]       Enable MD4C support.
                          DIR is the path to MD4C install prefix])

if test "$PHP_YAML" != "no"; then

	AC_MSG_CHECKING([for md4c headers])
	for i in "$PHP_MD4C" "$prefix" /usr /usr/local; do
		if test -r "$i/include/md4c-html.h"; then
			PHP_MD4C_DIR=$i
			AC_MSG_RESULT([found in $i])
			break
		fi
	done
	if test -z "$PHP_MD4C_DIR"; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([Please install md4c])
	fi

	PHP_ADD_INCLUDE($PHP_MD4C_DIR/include)
	dnl recommended flags for compilation with gcc
	dnl CFLAGS="$CFLAGS -Wall -fno-strict-aliasing"

	export OLD_CPPFLAGS="$CPPFLAGS"
	export CPPFLAGS="$CPPFLAGS $INCLUDES -DHAVE_MD4C"
	AC_CHECK_HEADERS([md4c.h md4c-html.h], [], AC_MSG_ERROR(['md4c.h' header not found]))
	#AC_CHECK_HEADER([md4c-html.h], [], AC_MSG_ERROR(['md4c-html.h' header not found]))
	PHP_SUBST(MD4C_SHARED_LIBADD)

	PHP_ADD_LIBRARY_WITH_PATH(md4c, $PHP_MD4C_DIR/$PHP_LIBDIR, MD4C_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(md4c-html, $PHP_MD4C_DIR/$PHP_LIBDIR, MD4C_SHARED_LIBADD)
	export CPPFLAGS="$OLD_CPPFLAGS"

	PHP_SUBST(MD4C_SHARED_LIBADD)
	AC_DEFINE(HAVE_MD4C, 1, [ ])
	PHP_NEW_EXTENSION(md4c, md4c.c, $ext_shared)
fi

