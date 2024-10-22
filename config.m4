dnl config.m4 for php-md4c extension

PHP_ARG_WITH(md4c, [whether to enable MD4C support],
[  --with-md4c[[=DIR]]       Enable MD4C support.
                          DIR is the path to MD4C install prefix])

if test "$PHP_MD4C" != "no"; then
	PHP_SUBST(MD4C_SHARED_LIBADD)
	AC_DEFINE(HAVE_MD4C, 1, [ ])
	PHP_NEW_EXTENSION(md4c, md4c.c, $ext_shared)
fi
