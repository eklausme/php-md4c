dnl config.m4 for php-md4c extension

PHP_ARG_ENABLE(md4c, [whether to enable MD4C support],
[  --enable-md4c           Enable MD4C support.])

if test "$PHP_MD4C" != "no"; then
	PHP_SUBST(MD4C_SHARED_LIBADD)
	AC_DEFINE(HAVE_MD4C, 1, [ ])
	PHP_NEW_EXTENSION(md4c, md4c.c, $ext_shared)
fi
