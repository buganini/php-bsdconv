/*
 * Copyright (c) 2009-2011 Kuan-Chung Chiu <buganini@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_bsdconv.h"

/* True global resources - no need for thread safety here */
static int le_bsdconv;

#include <bsdconv.h>

#define LE_BSDCONV_DESC "bsdconv instance"

#define IBUFLEN 1024

static void bsdconv_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC){
	bsdconv_destroy((struct bsdconv_instance *) rsrc->ptr);
}

/* {{{ proto resource bsdconv_create(string conversion)
  create bsdconv instance */
PHP_FUNCTION(bsdconv_create){
	char *c;
	int l;
	struct bsdconv_instance *r;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &c, &l) == FAILURE){
		return;
	}
	r=bsdconv_create(c);
	if(r==NULL) RETURN_BOOL(0);
	ZEND_REGISTER_RESOURCE(return_value, r, le_bsdconv);
}
/* }}} */

/* {{{ proto int bsdconv_insert_phase(resource ins, string conversion, int phase_type, int phasen)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_insert_phase){
	char *c;
	int l;
	long phase_type;
	long phasen;
	zval *r=NULL;
	struct bsdconv_instance *ins;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsll", &r, &c, &l, &phase_type, &phasen) == FAILURE){
		RETURN_LONG(-1);
	}
	if(r==NULL) RETURN_LONG(-1);
	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);
	RETURN_LONG(bsdconv_insert_phase(ins, c, phase_type, phasen));
}
/* }}} */

/* {{{ proto int bsdconv_insert_codec(resource ins, string conversion, int phasen, int codecn)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_insert_codec){
	char *c;
	int l;
	long phasen;
	long codecn;
	zval *r=NULL;
	struct bsdconv_instance *ins;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsll", &r, &c, &l, &phasen, &codecn) == FAILURE){
		RETURN_LONG(-1);
	}
	if(r==NULL) RETURN_LONG(-1);
	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);
	RETURN_LONG(bsdconv_insert_codec(ins, c, phasen, codecn));
}
/* }}} */

/* {{{ proto int bsdconv_replace_phase(resource ins, string conversion, int phase_type, int phasen)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_replace_phase){
	char *c;
	int l;
	long phase_type;
	long phasen;
	zval *r=NULL;
	struct bsdconv_instance *ins;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsll", &r, &c, &l, &phase_type, &phasen) == FAILURE){
		RETURN_LONG(-1);
	}
	if(r==NULL) RETURN_LONG(-1);
	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);
	RETURN_LONG(bsdconv_replace_phase(ins, c, phase_type, phasen));
}
/* }}} */

/* {{{ proto int bsdconv_replace_codec(resource ins, string conversion, int phasen, int codecn)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_replace_codec){
	char *c;
	int l;
	long phasen;
	long codecn;
	zval *r=NULL;
	struct bsdconv_instance *ins;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsll", &r, &c, &l, &phasen, &codecn) == FAILURE){
		RETURN_LONG(-1);
	}
	if(r==NULL) RETURN_LONG(-1);
	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);
	RETURN_LONG(bsdconv_replace_codec(ins, c, phasen, codecn));
}
/* }}} */

/* {{{ proto void bsdconv_init(resource ins)
  initialize/reset bsdconv instance */
PHP_FUNCTION(bsdconv_init){
	zval *r=NULL;
	struct bsdconv_instance *ins;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &r) == FAILURE){
		RETURN_BOOL(0);
	}

	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);
	bsdconv_init(ins);
}
/* }}} */

/* {{{ proto bool bsdconv_destroy(resource ins)
  destroy bsdconv instance */
PHP_FUNCTION(bsdconv_destroy){
	zval *p=NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &p) == FAILURE){
		RETURN_BOOL(0);
	}
	if(zend_list_delete(Z_RESVAL_P(p)) == FAILURE){
		RETURN_BOOL(0);
	}
	RETURN_BOOL(1);
}
/* }}} */

/* {{{ proto mixed bsdconv(resource ins, string str)
  bsdconv main function
*/
PHP_FUNCTION(bsdconv){
	zval *r=NULL;
	struct bsdconv_instance *ins;
	char *c;
	int l;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &r, &c, &l) == FAILURE){
		RETURN_BOOL(0);
	}

	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);

	if(ins==NULL){
		RETURN_BOOL(0);
	}
	bsdconv_init(ins);
	ins->output_mode=BSDCONV_PREMALLOCED;
	ins->input.data=c;
	ins->input.len=l;
	ins->input.flags=0;
	ins->output.data=NULL;
	ins->flush=1;
	bsdconv(ins);

	ins->output.data=emalloc(ins->output.len);
	bsdconv(ins);

	RETURN_STRINGL(ins->output.data, ins->output.len, 0);
}
/* }}} */

/* {{{ proto mixed bsdconv_chunk(resource ins, string str)
  bsdconv converting function without initializing and flushing
*/
PHP_FUNCTION(bsdconv_chunk){
	zval *r=NULL;
	struct bsdconv_instance *ins;
	char *c;
	int l;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &r, &c, &l) == FAILURE){
		RETURN_BOOL(0);
	}

	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);

	if(ins==NULL){
		RETURN_BOOL(0);
	}
	ins->output_mode=BSDCONV_PREMALLOCED;
	ins->input.data=c;
	ins->input.len=l;
	ins->input.flags=0;
	ins->output.data=NULL;
	bsdconv(ins);

	ins->output.data=emalloc(ins->output.len);
	bsdconv(ins);

	RETURN_STRINGL(ins->output.data, ins->output.len, 0);
}
/* }}} */

/* {{{ proto mixed bsdconv_last(resource ins, string str)
  bsdconv converting function without initializing
*/
PHP_FUNCTION(bsdconv_chunk_last){
	zval *r=NULL;
	struct bsdconv_instance *ins;
	char *c;
	int l;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &r, &c, &l) == FAILURE){
		RETURN_BOOL(0);
	}

	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);

	if(ins==NULL){
		RETURN_BOOL(0);
	}
	ins->output_mode=BSDCONV_PREMALLOCED;
	ins->input.data=c;
	ins->input.len=l;
	ins->input.flags=0;
	ins->output.data=NULL;
	ins->flush=1;
	bsdconv(ins);

	ins->output.data=emalloc(ins->output.len);
	bsdconv(ins);

	RETURN_STRINGL(ins->output.data, ins->output.len, 0);
}
/* }}} */

/* {{{ proto mixed bsdconv_file(resource ins, string infile, string outfile)
  bsdconv_file function
*/
PHP_FUNCTION(bsdconv_file){
	zval *r=NULL;
	struct bsdconv_instance *ins;
	char *s1, *s2;
	int l,l2;
	FILE *inf, *otf;
	char *in;
	char *tmp;
	int fd;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &r, &s1, &l, &s2, &l2) == FAILURE){
		RETURN_BOOL(0);
	}

	ZEND_FETCH_RESOURCE(ins, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);

	if(ins==NULL){
		RETURN_BOOL(0);
	}

	inf=fopen(s1,"r");
	if(!inf) RETURN_BOOL(0);
	tmp=malloc(l2+8);
	strcpy(tmp, s2);
	strcat(tmp, ".XXXXXX");
	if((fd=mkstemp(tmp))==-1){
		free(tmp);
		RETURN_BOOL(0);
	}
	otf=fdopen(fd,"w");
	if(!otf){
		free(tmp);
		RETURN_BOOL(0);
	}

	bsdconv_init(ins);
	do{
		in=malloc(IBUFLEN);
		ins->input.data=in;
		ins->input.len=fread(in, 1, IBUFLEN, inf);
		ins->input.flags|=F_FREE;
		if(ins->input.len==0){
			ins->flush=1;
		}
		ins->output_mode=BSDCONV_FILE;
		ins->output.data=otf;
		bsdconv(ins);
	}while(ins->flush==0);

	fclose(inf);
	fclose(otf);
	unlink(s2);
	rename(tmp,s2);
	free(tmp);

	RETURN_BOOL(1);
}
/* }}} */

/* {{{ proto array bsdconv_info(resource ins)
  bsdconv conversion info function
*/
PHP_FUNCTION(bsdconv_info){
	zval *r=NULL;
	struct bsdconv_instance *p;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &r) == FAILURE){
		RETURN_BOOL(0);
	}

	ZEND_FETCH_RESOURCE(p, struct bsdconv_instance *, &r, -1, LE_BSDCONV_DESC, le_bsdconv);

	array_init(return_value);
	add_assoc_long(return_value, "ierr", p->ierr);
	add_assoc_long(return_value, "oerr", p->oerr);
	add_assoc_long(return_value, "score", p->score);
}
/* }}} */

/* {{{ proto string bsdconv(void)
  bsdconv error message
*/
PHP_FUNCTION(bsdconv_error){
	char *c;
	c=bsdconv_error();
	RETVAL_STRING(c, 1);
	free(c);
}
/* }}} */

/* {{{ bsdconv_functions[]
 *
 * Every user visible function must have an entry in bsdconv_functions[].
 */
zend_function_entry bsdconv_functions[] = {
	PHP_FE(bsdconv_create,	NULL)
	PHP_FE(bsdconv_destroy,	NULL)
	PHP_FE(bsdconv_info,	NULL)
	PHP_FE(bsdconv,		NULL)
	PHP_FE(bsdconv_init,	NULL)
	PHP_FE(bsdconv_chunk,	NULL)
	PHP_FE(bsdconv_chunk_last,NULL)
	PHP_FE(bsdconv_file,	NULL)
	PHP_FE(bsdconv_insert_phase,NULL)
	PHP_FE(bsdconv_insert_codec,NULL)
	PHP_FE(bsdconv_replace_phase,NULL)
	PHP_FE(bsdconv_replace_codec,NULL)
	PHP_FE(bsdconv_error,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in bsdconv_functions[] */
};
/* }}} */

/* {{{ bsdconv_module_entry
 */
zend_module_entry bsdconv_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"bsdconv",
	bsdconv_functions,
	PHP_MINIT(bsdconv),
	PHP_MSHUTDOWN(bsdconv),
	NULL,
	NULL,
	PHP_MINFO(bsdconv),
#if ZEND_MODULE_API_NO >= 20010901
	"5.0", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BSDCONV
ZEND_GET_MODULE(bsdconv)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(bsdconv)
{
	le_bsdconv = zend_register_list_destructors_ex(bsdconv_dtor, NULL, LE_BSDCONV_DESC, module_number);
	REGISTER_LONG_CONSTANT("BSDCONV_FROM", FROM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BSDCONV_INTER", INTER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BSDCONV_TO", TO, CONST_CS|CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(bsdconv)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(bsdconv)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "bsdconv support", "enabled");
	php_info_print_table_end();
}
/* }}} */
