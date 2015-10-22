#include "bench.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include <msgpuck.h>

enum { REQUEST_BODY_MAXLEN = 1024 };

/* {{{ Utils */

double
nowtime(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec * 1e-6;
}

void
randstr(char *out, size_t len)
{
	static const char alphanum[] =
			"0123456789-_"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
	static const size_t alen = sizeof(alphanum);
	for (size_t i = 0; i < len; ++i)
		out[i] = alphanum[rand() % (alen - 1)];
	out[len] = '\0';
}

/* }}} */

/* {{{ Generators */

char *
gen_num(char *r, const struct keygen_params *params)
{
	(void) params;
	r = mp_encode_array(r, 1);
	r = mp_encode_uint(r, rand());
	return r;
}

API_EXPORT char *
gen_str(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
	r = mp_encode_array(r, 1);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	return r;
}

char *
gen_num_num(char *r, const struct keygen_params *params)
{
	(void) params;
	r = mp_encode_array(r, 2);
	r = mp_encode_uint(r, rand());
	r = mp_encode_uint(r, rand());
	return r;
}

API_EXPORT char *
gen_str_str(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
	r = mp_encode_array(r, 2);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	return r;
}

char *
gen_num_str(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
	r = mp_encode_array(r, 2);
	r = mp_encode_uint(r, rand());
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	return r;
}

char *
gen_str_num(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
	r = mp_encode_array(r, 2);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	r = mp_encode_uint(r, rand());
	return r;
}

/* }}} */

/* {{{ Tests */

void
test_keys(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		params->keygen(r, params->keygen_params);
	}
}

void
test_selects(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		char *key_end = params->keygen(r, params->keygen_params);
		box_tuple_t *result;
		box_index_get(params->space_id, 0, r, key_end, &result);
		//fiber_gc();
	}
}

void
test_replaces(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		char *tuple_end = params->keygen(r, params->keygen_params);
		box_replace(params->space_id, r, tuple_end, 0);
	}
}

void
test_selrepl(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		char *rend = params->keygen(r, params->keygen_params);
		box_tuple_t *result;
		box_index_get(params->space_id, 0, r, rend, &result);
		box_replace(params->space_id, r, rend, 0);
	}
}

void
test_updates(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];
	char reqdata2[REQUEST_BODY_MAXLEN];

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		char *rend = params->keygen(r, params->keygen_params);
		char *r2 = reqdata2;
		r2 = mp_encode_array(r2, 1);
		r2 = mp_encode_array(r2, 3);
		r2 = mp_encode_str(r2, "!", 1);
		r2 = mp_encode_int(r2, -1);
		r2 = mp_encode_uint(r2, 0);
		box_update(params->space_id, 0, r, rend, reqdata2, r2, 0, 0);
	}

}

void
test_deletes(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		const char *key_end = params->keygen(r, params->keygen_params);
		box_delete(params->space_id, 0, r, key_end, 0);
	}
}

/* }}} */

#include <lua.h>

LUA_API
int
luaopen_cbench_bench(lua_State *L)
{
	 return 0;
}
