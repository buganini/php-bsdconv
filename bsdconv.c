/*
 * Copyright (c) 2009-2014 Kuan-Chung Chiu <buganini@gmail.com>
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

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <bsdconv.h>

/* True global resources - no need for thread safety here */
static int le_bsdconv_fp;

static void bsdconv_fp_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC){
	fclose((FILE *) rsrc->ptr);
}

zend_class_entry *bsdconv_ce;

zend_object_handlers bsdconv_object_handlers;

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
	RETURN_BOOL(1);
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

/* {{{ proto mixed ctl(int ctl, resource res, int num)
  bsdconv ctl
*/
PHP_METHOD(Bsdconv, ctl){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	zval *res=NULL;
	long ctl;
	long num;
	void *ptr;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lrl", &ctl, &res, &num) == FAILURE){
		RETURN_BOOL(0);
	}

	switch(ctl){
		case BSDCONV_CTL_ATTACH_SCORE:
		case BSDCONV_CTL_ATTACH_OUTPUT_FILE:
			ZEND_FETCH_RESOURCE(ptr, void *, &res, -1, "bsdconv fp", le_bsdconv_fp);
			if(ptr==NULL){
				RETURN_BOOL(0);
			}
			break;
	}

	bsdconv_ctl(ins, ctl, ptr, num);
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
	ins->input.next=NULL;
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
	ins->input.next=NULL;
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
	ins->input.next=NULL;
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

#ifndef WIN32
	struct stat stat;
	fstat(fileno(inf), &stat);
	fchown(fileno(otf), stat.st_uid, stat.st_gid);
	fchmod(fileno(otf), stat.st_mode);
#endif

	bsdconv_init(ins);
	do{
		in=bsdconv_malloc(IBUFLEN);
		ins->input.data=in;
		ins->input.len=fread(in, 1, IBUFLEN, inf);
		ins->input.flags|=F_FREE;
		ins->input.next=NULL;
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

/* {{{ proto mixed counter([counter])
  get counter(s) value
*/
PHP_METHOD(Bsdconv, counter){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *key=NULL;
	int l;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &l) == FAILURE){
		RETURN_BOOL(0);
	}

	if(key){
		bsdconv_counter_t *v=bsdconv_counter(ins, key);
		RETURN_LONG((long)(*v));
	}else{
		array_init(return_value);
		struct bsdconv_counter_entry *p=ins->counter;
		while(p){
			add_assoc_long(return_value, p->key, (long)(p->val));
			p=p->next;
		}
	}
}
/* }}} */

/* {{{ proto void counter_reset()
  reset all counters
*/
PHP_METHOD(Bsdconv, counter_reset){
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *key=NULL;
	int l;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &l) == FAILURE){
		RETURN_BOOL(0);
	}

	bsdconv_counter_reset(ins, key);
}
/* }}} */

/* {{{ proto array __toString()
  bsdconv conversion
*/
PHP_METHOD(Bsdconv, __toString){
#define TEMPLATE "Bsdconv(\"%s\")"
	struct bsdconv_object *obj=(struct bsdconv_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	struct bsdconv_instance *ins=obj->ins;
	char *s;
	char *s2;
	int len=sizeof(TEMPLATE);
	s=bsdconv_pack(ins);
	len+=strlen(s);
	s2=malloc(len);
	sprintf(s2, TEMPLATE, s);
	bsdconv_free(s);
	RETVAL_STRING(s2, 1);
	free(s2);
}
/* }}} */

/* {{{ proto int bsdconv_insert_phase(string conversion, string codecs, int phase_type, int phasen)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_insert_phase){
	char *conv;
	char *c;
	int l;
	long phase_type;
	long phasen;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &conv, &l, &c, &l, &phase_type, &phasen) == FAILURE){
		RETURN_LONG(-1);
	}
	char *r=bsdconv_insert_phase(conv, c, phase_type, phasen);
	RETVAL_STRING(r, 1);
	bsdconv_free(r);
}
/* }}} */

/* {{{ proto int bsdconv_insert_codec(string conversion, string codec, int phasen, int codecn)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_insert_codec){
	char *conv;
	char *c;
	int l;
	long phasen;
	long codecn;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &conv, &l, &c, &l, &phasen, &codecn) == FAILURE){
		RETURN_LONG(-1);
	}
	char *r=bsdconv_insert_codec(conv, c, phasen, codecn);
	RETVAL_STRING(r, 1);
	bsdconv_free(r);
}
/* }}} */

/* {{{ proto int bsdconv_replace_phase(string conversion, string codecs, int phase_type, int phasen)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_replace_phase){
	char *conv;
	char *c;
	int l;
	long phase_type;
	long phasen;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &conv, &l, &c, &l, &phase_type, &phasen) == FAILURE){
		RETURN_LONG(-1);
	}
	char *r=bsdconv_replace_phase(conv, c, phase_type, phasen);
	RETVAL_STRING(r, 1);
	bsdconv_free(r);
}
/* }}} */

/* {{{ proto int bsdconv_replace_codec(string conversion, string codec, int phasen, int codecn)
  alter bsdconv instance */
PHP_FUNCTION(bsdconv_replace_codec){
	char *conv;
	char *c;
	int l;
	long phasen;
	long codecn;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll", &conv, &l, &c, &l, &phasen, &codecn) == FAILURE){
		RETURN_LONG(-1);
	}
	char *r=bsdconv_replace_codec(conv, c, phasen, codecn);
	RETVAL_STRING(r, 1);
	bsdconv_free(r);
}
/* }}} */

/* {{{ proto string bsdconv_error(void)
  bsdconv error message
*/
PHP_FUNCTION(bsdconv_error){
	char *c;
	c=bsdconv_error();
	RETVAL_STRING(c, 1);
	bsdconv_free(c);
}
/* }}} */

/* {{{ proto array bsdconv_modules_list(integer)
  list codecs
*/
PHP_FUNCTION(bsdconv_modules_list){
	array_init(return_value);
	char **list;
	char **p;
	long phase_type;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &phase_type) == FAILURE){
		RETURN_BOOL(0);
	}

	list=bsdconv_modules_list(phase_type);
	p=list;
	while(*p!=NULL){
		add_next_index_string(return_value, *p, 1);
		bsdconv_free(*p);
		p+=1;
	}
	bsdconv_free(list);
}
/* }}} */

PHP_FUNCTION(bsdconv_codecs_list){
	array_init(return_value);
	char **list;
	char **p;
	long phase_type;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &phase_type) == FAILURE){
		RETURN_BOOL(0);
	}

	list=bsdconv_modules_list(phase_type);
	p=list;
	while(*p!=NULL){
		add_next_index_string(return_value, *p, 1);
		bsdconv_free(*p);
		p+=1;
	}
	bsdconv_free(list);
}

/* {{{ proto bool bsdconv_module_check(int type, string codec)
  check if a codec is available
*/
PHP_FUNCTION(bsdconv_module_check){
	char *c;
	int l;
	long phase_type;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &phase_type, &c, &l) == FAILURE){
		RETURN_LONG(-1);
	}
	if(bsdconv_module_check(phase_type, c)){
		RETURN_BOOL(1);
	}
	RETURN_BOOL(0);
}
/* }}} */

PHP_FUNCTION(bsdconv_codec_check){
	char *c;
	int l;
	long phase_type;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &phase_type, &c, &l) == FAILURE){
		RETURN_LONG(-1);
	}
	if(bsdconv_module_check(phase_type, c)){
		RETURN_BOOL(1);
	}
	RETURN_BOOL(0);
}

/* {{{ proto resource bsdconv_fopen(string path, string mode)
  fopen
*/
PHP_FUNCTION(bsdconv_fopen){
	char *pc, *mc;
	int pl, ml;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &pc, &pl, &mc, &ml) == FAILURE){
		RETURN_LONG(-1);
	}
	FILE *r=fopen(pc, mc);
	if(r==NULL) RETURN_BOOL(0);
	ZEND_REGISTER_RESOURCE(return_value, r, le_bsdconv_fp);
}
/* }}} */

/* {{{ proto bool bsdconv_fclose(resource fp)
  fclose
*/
PHP_FUNCTION(bsdconv_fclose){
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

/* {{{ proto array(fp, path) bsdconv_mktemp(string template)
  mkstemp
*/
PHP_FUNCTION(bsdconv_mktemp){
	char *c;
	int l;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &c, &l) == FAILURE){
		RETURN_LONG(-1);
	}
	char *t=strdup(c);
	int fd=bsdconv_mkstemp(t);
	if(fd==-1) RETURN_BOOL(0);
	FILE *r=fdopen(fd, "wb+");
	if(r==NULL) RETURN_BOOL(0);
	zval *res;
	MAKE_STD_ZVAL(res);
	ZEND_REGISTER_RESOURCE(res, r, le_bsdconv_fp);
	array_init(return_value);
	add_next_index_resource(return_value, Z_RESVAL_P(res));
	add_next_index_string(return_value, t, 1);
	free(t);
}
/* }}} */

zend_function_entry bsdconv_methods[] = {
	PHP_ME(Bsdconv,  __construct,	NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Bsdconv,  __destruct,	NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	PHP_ME(Bsdconv,  __toString,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, init,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, ctl,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv_chunk,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv_chunk_last,NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, conv_file,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, counter,	NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Bsdconv, counter_reset,	NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ bsdconv_functions[]
 *
 * Every user visible function must have an entry in bsdconv_functions[].
 */
zend_function_entry bsdconv_functions[] = {
	PHP_FE(bsdconv_insert_phase,	NULL)
	PHP_FE(bsdconv_insert_codec,	NULL)
	PHP_FE(bsdconv_replace_phase,	NULL)
	PHP_FE(bsdconv_replace_codec,	NULL)
	PHP_FE(bsdconv_error,		NULL)
	PHP_FE(bsdconv_codecs_list,	NULL)
	PHP_FE(bsdconv_codec_check,	NULL)
	PHP_FE(bsdconv_modules_list,	NULL)
	PHP_FE(bsdconv_module_check,	NULL)
	PHP_FE(bsdconv_fopen,		NULL)
	PHP_FE(bsdconv_fclose,		NULL)
	PHP_FE(bsdconv_mktemp,		NULL)
	{NULL, NULL, NULL}	/* Must be the last line in bsdconv_functions[] */
};
/* }}} */

/* {{{ bsdconv_module_entry
 */
zend_module_entry bsdconv_module_entry = {
	STANDARD_MODULE_HEADER,
	"bsdconv",
	bsdconv_functions,
	PHP_MINIT(bsdconv),
	PHP_MSHUTDOWN(bsdconv),
	NULL,
	NULL,
	PHP_MINFO(bsdconv),
	"11.3.0", /* Replace with version number for your extension */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

void bsdconv_free_storage(void *object TSRMLS_DC)
{
	efree(object);
}

zend_object_value bsdconv_create_handler(zend_class_entry *type TSRMLS_DC)
{
	zval *tmp;
	zend_object_value retval;

	struct bsdconv_object *obj = (struct bsdconv_object *)emalloc(sizeof(struct bsdconv_object));
	memset(obj, 0, sizeof(struct bsdconv_object));
	obj->std.ce = type;


	retval.handle = zend_objects_store_put(obj, NULL, bsdconv_free_storage, NULL TSRMLS_CC);
	retval.handlers = &bsdconv_object_handlers;

	return retval;
}

#ifdef COMPILE_DL_BSDCONV
ZEND_GET_MODULE(bsdconv)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(bsdconv)
{
	le_bsdconv_fp = zend_register_list_destructors_ex(bsdconv_fp_dtor, NULL, "bsdconv_fopen resource", module_number);
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Bsdconv", bsdconv_methods);
	bsdconv_ce = zend_register_internal_class(&ce TSRMLS_CC);
	bsdconv_ce->create_object = bsdconv_create_handler;
	memcpy(&bsdconv_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	bsdconv_object_handlers.clone_obj = NULL;
	REGISTER_LONG_CONSTANT("BSDCONV_FILTER", FILTER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BSDCONV_FROM", FROM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BSDCONV_INTER", INTER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BSDCONV_TO", TO, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("BSDCONV_CTL_ATTACH_SCORE", BSDCONV_CTL_ATTACH_SCORE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BSDCONV_CTL_ATTACH_OUTPUT_FILE", BSDCONV_CTL_ATTACH_OUTPUT_FILE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BSDCONV_CTL_AMBIGUOUS_PAD", BSDCONV_CTL_AMBIGUOUS_PAD, CONST_CS|CONST_PERSISTENT);
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
