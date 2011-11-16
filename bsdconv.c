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


#include <bsdconv.h>


struct bsdconv_object {
    zend_object std;
    struct bsdconv_instance *ins;
};

#define IBUFLEN 1024

static void bsdconv_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC){
	bsdconv_destroy((struct bsdconv_instance *) rsrc->ptr);
}

/* {{{ proto resource __construct(string conversion)
  create bsdconv instance */
PHP_METHOD(Bsdconv,  __construct){
	char *c;
	int l;
	struct bsdconv_instance *ins;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &c, &l) == FAILURE){
		return;
	}
	ins=bsdconv_create(c);
	if(ins==NULL)  RETURN_NULL();
	struct bsdconv_object *obj = (struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	obj->ins = ins;
}
/* }}} */

/* {{{ proto int __destruct()
  destroy bsdconv instance */
PHP_METHOD(Bsdconv, __destruct){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	bsdconv_destroy(ins);
	obj->ins=NULL;
}

/* }}} */
/* {{{ proto int insert_phase(string conversion, int phase_type, int phasen)
  alter bsdconv instance */
PHP_METHOD(Bsdconv, insert_phase){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *c;
	int l;
	long phase_type;
	long phasen;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &c, &l, &phase_type, &phasen) == FAILURE){
		RETURN_LONG(-1);
	}
	RETURN_LONG(bsdconv_insert_phase(ins, c, phase_type, phasen));
}
/* }}} */

/* {{{ proto int insert_codec(resource ins, string conversion, int phasen, int codecn)
  alter bsdconv instance */
PHP_METHOD(Bsdconv, insert_codec){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;	char *c;
	int l;
	long phasen;
	long codecn;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &c, &l, &phasen, &codecn) == FAILURE){
		RETURN_LONG(-1);
	}
	RETURN_LONG(bsdconv_insert_codec(ins, c, phasen, codecn));
}
/* }}} */

/* {{{ proto int replace_phase(resource ins, string conversion, int phase_type, int phasen)
  alter bsdconv instance */
PHP_METHOD(Bsdconv, replace_phase){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *c;
	int l;
	long phase_type;
	long phasen;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &c, &l, &phase_type, &phasen) == FAILURE){
		RETURN_LONG(-1);
	}
	RETURN_LONG(bsdconv_replace_phase(ins, c, phase_type, phasen));
}
/* }}} */

/* {{{ proto int replace_codec(resource ins, string conversion, int phasen, int codecn)
  alter bsdconv instance */
PHP_METHOD(Bsdconv, replace_codec){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *c;
	int l;
	long phasen;
	long codecn;
	zval *r=NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &c, &l, &phasen, &codecn) == FAILURE){
		RETURN_LONG(-1);
	}
	RETURN_LONG(bsdconv_replace_codec(ins, c, phasen, codecn));
}
/* }}} */

/* {{{ proto void init()
  initialize/reset bsdconv instance */
PHP_METHOD(Bsdconv, init){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	bsdconv_init(ins);
}
/* }}} */

/* {{{ proto mixed conv(string str)
  bsdconv main function
*/
PHP_METHOD(Bsdconv, conv){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *c;
	int l;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &c, &l) == FAILURE){
		RETURN_BOOL(0);
	}

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

/* {{{ proto mixed conv_chunk(string str)
  bsdconv converting function without initializing and flushing
*/
PHP_METHOD(Bsdconv, conv_chunk){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *c;
	int l;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &c, &l) == FAILURE){
		RETURN_BOOL(0);
	}

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

/* {{{ proto mixed conv_chunk_last(string str)
  bsdconv converting function without initializing
*/
PHP_METHOD(Bsdconv, conv_chunk_last){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *c;
	int l;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &c, &l) == FAILURE){
		RETURN_BOOL(0);
	}

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

/* {{{ proto mixed conv_file(string infile, string outfile)
  bsdconv_file function
*/
PHP_METHOD(Bsdconv, conv_file){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *s1, *s2;
	int l,l2;
	FILE *inf, *otf;
	char *in;
	char *tmp;
	int fd;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &s1, &l, &s2, &l2) == FAILURE){
		RETURN_BOOL(0);
	}

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

/* {{{ proto array info()
  bsdconv conversion info function
*/
PHP_METHOD(Bsdconv, info){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;

	array_init(return_value);
	add_assoc_long(return_value, "ierr", ins->ierr);
	add_assoc_long(return_value, "oerr", ins->oerr);
	add_assoc_long(return_value, "score", ins->score);
}
/* }}} */

/* {{{ proto string bsdconv_error(void)
  bsdconv error message
*/
PHP_FUNCTION(bsdconv_error){
	char *c;
	c=bsdconv_error();
	RETVAL_STRING(c, 1);
	free(c);
}
/* }}} */

function_entry bsdconv_methods[] = {
	PHP_ME(Bsdconv,  __construct,	NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Bsdconv,  __destruct,	NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	PHP_ME(Bsdconv, insert_phase,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, insert_codec,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, replace_phase,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, replace_codec,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, init,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv_chunk,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv_chunk_last,NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv_file,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, info,		NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ bsdconv_functions[]
 *
 * Every user visible function must have an entry in bsdconv_functions[].
 */
zend_function_entry bsdconv_functions[] = {
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
	"6.5", /* Replace with version number for your extension */
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
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Bsdconv", bsdconv_methods);
	zend_register_internal_class(&ce TSRMLS_CC);
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
