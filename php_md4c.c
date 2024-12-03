/* MD4C extension for PHP: Markdown to HTML conversion

   Info on PHP extension writing:
   1. Sara Golemon: Extending and Embedding PHP, Sams Publishing, 2006, xx+410 p.
   2. https://www.phpinternalsbook.com/php7/extensions_design/zend_extensions.html
   3. For globals: https://www.phpinternalsbook.com/php7/extensions_design/globals_management.html#managing-request-globals
   4. https://github.com/dstogov/php-extension

   Elmar Klausmeier, 19-Feb-2024: Started with FFI C module and converted memory allocation
   Elmar Klausmeier, 19-Oct-2024: Amalgamates md4c/src files and inserted it here
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/info.h>

#include <md4c-html.h>


struct membuffer {
	char* data;
	size_t asize;	// allocated size = max usable size
	size_t size;	// current size
};


//static struct membuffer mbuf = { NULL, 0, 0 };
ZEND_BEGIN_MODULE_GLOBALS(md4c)
	struct membuffer mbuf;
ZEND_END_MODULE_GLOBALS(md4c)


ZEND_DECLARE_MODULE_GLOBALS(md4c)

#ifdef ZTS
#define MD4C_GET(v)	ZEND_MODULE_GLOBALS_ACCESSOR(md4c,v)
#else
#define MD4C_GET(v)	(md4c_globals.v)
#endif



static void membuf_init(struct membuffer* buf, MD_SIZE new_asize) {
	buf->size = 0;
	buf->asize = new_asize;
	if ((buf->data = safe_pemalloc(buf->asize,sizeof(char),0,1)) == NULL)
		php_error_docref(NULL, E_ERROR, "php-md4c.c: membuf_init: safe_pemalloc() failed with asize=%ld.\n",(long)buf->asize);
}



static void membuf_grow(struct membuffer* buf, size_t new_asize) {
	buf->data = safe_perealloc(buf->data, sizeof(char*), new_asize, 0, 1);
	if (buf->data == NULL)
		php_error_docref(NULL, E_ERROR, "php-md4c.c: membuf_grow: realloc() failed, new_asize=%ld.\n",(long)new_asize);
	buf->asize = new_asize;
}



static void membuf_append(struct membuffer* buf, const char* data, MD_SIZE size) {
	if (buf->asize < buf->size + size)
		membuf_grow(buf, buf->size + buf->size / 2 + size);
	memcpy(buf->data + buf->size, data, size);
	buf->size += size;
}



static void process_output(const MD_CHAR* text, MD_SIZE size, void* userdata) {
	membuf_append((struct membuffer*) userdata, text, size);
}



/* {{{ string md4c_toHtml( string $markdown, [ int $flag ] )
 */
PHP_FUNCTION(md4c_toHtml) {	// return HTML string
	char *markdown;
	size_t markdown_len;
	int ret;
	zend_long flag = MD_DIALECT_GITHUB | MD_FLAG_NOINDENTEDCODEBLOCKS;

	struct membuffer* buf = &( MD4C_GET(mbuf) );
	if (buf == NULL)
		php_error_docref(NULL, E_ERROR, "php-md4c.c: PHP_FUNCTION(md4c_toHtml): MD4C_GET(md4c) is NULL\n");

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(markdown, markdown_len)
		Z_PARAM_OPTIONAL Z_PARAM_LONG(flag)
	ZEND_PARSE_PARAMETERS_END();

	if (buf->asize == 0) membuf_init(buf,16777216);	// =16MB

	buf->size = 0;	// prepare for next call
	ret = md_html(markdown, markdown_len, process_output,
		buf, (MD_SIZE)flag, 0);
	membuf_append(buf,"\0",1); // make it a null-terminated C string, so PHP can deduce length
	if (ret < 0) {
		RETVAL_STRINGL("<br>- - - Error in Markdown - - -<br>\n",sizeof("<br>- - - Error in Markdown - - -<br>\n"));
	} else {
		RETVAL_STRING(estrndup(buf->data,buf->size));
	}
}
/* }}}*/



//static PHP_GINIT_FUNCTION(md4c) {
//#if defined(COMPILE_DL_BCMATH) && defined(ZTS)
//	ZEND_TSRMLS_CACHE_UPDATE();
//#endif
	// do absolutely nothing
//}



/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(md4c) {	// module initialization
	//REGISTER_INI_ENTRIES();
	//php_printf("In PHP_MINIT_FUNCTION(md4c): module initialization\n");

	REGISTER_LONG_CONSTANT("MD4C_DIALECT_GITHUB", MD_DIALECT_GITHUB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MD4C_FLAG_NOINDENTEDCODEBLOCKS", MD_FLAG_NOINDENTEDCODEBLOCKS, CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */



/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(md4c) {	// module shutdown
	struct membuffer* buf = &( MD4C_GET(mbuf) );
	if (buf == NULL)
		php_error_docref(NULL, E_ERROR, "php-md4c.c: PHP_MSHUTDOWN_FUNCTION(md4c) MD4C_GET(md4c) is NULL\n");

	if (buf->data) pefree(buf->data,1);
	return SUCCESS;
}
/* }}} */



PHP_GSHUTDOWN_FUNCTION(md4c) { }

PHP_GINIT_FUNCTION(md4c) {
	md4c_globals->mbuf.data = NULL;
	md4c_globals->mbuf.asize = 0;
	md4c_globals->mbuf.size = 0;
}


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(md4c) {
	php_info_print_table_start();
	php_info_print_table_row(2, "MD4C", "enabled");
	php_info_print_table_row(2, "PHP-MD4C version", "1.1");
#ifdef HAVE_LIBMD4C
	php_info_print_table_row(2, "MD4C system library version", PHP_LIBMD4C_VERSION);
#else
	php_info_print_table_row(2, "MD4C bundled library version", "0.5.2");
#endif
	php_info_print_table_end();
}
/* }}} */



/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_md4c_toHtml, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, flag, "MD_DIALECT_GITHUB | MD_FLAG_NOINDENTEDCODEBLOCKS")
ZEND_END_ARG_INFO()
/* }}} */



/* {{{ test_functions[]
 */
static const zend_function_entry php_md4c_functions[] = {
	PHP_FE(md4c_toHtml,	arginfo_md4c_toHtml)
	PHP_FE_END
};
/* }}} */



/* {{{ md4c_module_entry
 */
zend_module_entry md4c_module_entry = {
	STANDARD_MODULE_HEADER,
	"md4c",				// Extension name
	php_md4c_functions,		// zend_function_entry
	PHP_MINIT(md4c),		// PHP_MINIT - Module initialization
	PHP_MSHUTDOWN(md4c),		// PHP_MSHUTDOWN - Module shutdown
	NULL,				// PHP_RINIT - Request initialization
	NULL,				// PHP_RSHUTDOWN - Request shutdown
	PHP_MINFO(md4c),		// PHP_MINFO - Module info
	"1.1",				// Version
	PHP_MODULE_GLOBALS(md4c),
	PHP_GINIT(md4c),
	PHP_GSHUTDOWN(md4c),
	NULL, /* PRSHUTDOWN() */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */



#ifdef COMPILE_DL_MD4C
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
#endif
ZEND_GET_MODULE(md4c)

