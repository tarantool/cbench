#include "bench.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "request.h"
#include "pickle.h"
#include "lib/msgpuck/msgpuck.h"
#include "box/box.h"
#include "port.h"
#include "fiber.h"

enum { REQUEST_BODY_MAXLEN = 1024 };

/* {{{ Utils */

void
assert_fail(const char *assertion, const char *file, unsigned int line,
	    const char *function)
{
	(void)function;
	fflush(NULL);
	printf("assert: %s:%d %s\n", file, line, assertion);
	exit(1);
}

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
gen_uint(char *r, const struct keygen_params *params)
{
	(void) params;
#if defined(MSGPACK)
	r = mp_encode_array(r, 1);
	r = mp_encode_uint(r, rand());
#else
	r = pack_u32(r, 1);
	r = pack_varint32(r, sizeof(uint32_t));
	r = pack_u32(r, rand());
#endif
	return r;
}

API_EXPORT char *
gen_str(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
#if defined(MSGPACK)
	r = mp_encode_array(r, 1);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
#else
	r = pack_u32(r, 1);
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
#endif
	return r;
}

char *
gen_uint_uint(char *r, const struct keygen_params *params)
{
	(void) params;
#if defined(MSGPACK)
	r = mp_encode_array(r, 2);
	r = mp_encode_uint(r, rand());
	r = mp_encode_uint(r, rand());
#else
	r = pack_u32(r, 2);
	r = pack_varint32(r, sizeof(uint32_t));
	r = pack_u32(r, rand());
	r = pack_varint32(r, sizeof(uint32_t));
	r = pack_u32(r, rand());
#endif
	return r;
}

API_EXPORT char *
gen_str_str(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
#if defined(MSGPACK)
	r = mp_encode_array(r, 2);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
#else
	r = pack_u32(r, 2);
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
#endif
	return r;
}

char *
gen_uint_str(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
#if defined(MSGPACK)
	r = mp_encode_array(r, 2);
	r = mp_encode_uint(r, rand());
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
#else
	r = pack_u32(r, 2);
	r = pack_varint32(r, sizeof(uint32_t));
	r = pack_u32(r, rand());
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
#endif
	return r;
}

char *
gen_str_uint(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
#if defined(MSGPACK)
	r = mp_encode_array(r, 2);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	r = mp_encode_uint(r, rand());
#else
	r = pack_u32(r, 2);
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
	r = pack_varint32(r, sizeof(uint32_t));
	r = pack_u32(r, rand());
#endif
	return r;
}

/* }}} */

/* {{{ Tests */

static void
port_bench_add_tuple(struct port *port, struct tuple *tuple, uint32_t flags);

struct port_bench
{
	struct port_vtab *vtab;
	size_t count;
};

static struct port_vtab port_bench_vtab = {
	port_bench_add_tuple,
	null_port_eof,
};

static void
port_bench_add_tuple(struct port *port, struct tuple *tuple, uint32_t flags)
{
	(void) tuple;
	(void) flags;
	struct port_bench *port_bench = (struct port_bench *) port;
	port_bench->count++;
}

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
	struct request request;
	struct port_bench port = { &port_bench_vtab, 0 };

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		r = pack_u32(r, params->space_id); // space
		r = pack_u32(r, 0); // index
		r = pack_u32(r, 0); // offset
		r = pack_u32(r, 4294967295); // limit
		r = pack_u32(r, 1); // key count
		r = params->keygen(r, params->keygen_params);
		request_create(&request, SELECT, reqdata, r - reqdata);
		port.count = 0;
		box_process((struct port *) &port, &request);
		assert(port.count == 1);
		fiber_gc();
	}
}

void
test_replaces(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];
	struct request request;
	struct port_bench port = { &port_bench_vtab, 0 };

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		r = pack_u32(r, params->space_id); // space
		r = pack_u32(r, 0); // flags
		r = params->keygen(r, params->keygen_params);
		request_create(&request, REPLACE, reqdata, r - reqdata);
		port.count = 0;
		box_process((struct port *) &port, &request);
		fiber_gc();
	}
}

void
test_deletes(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];
	struct request request;
	struct port_bench port = { &port_bench_vtab, 0 };

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		r = pack_u32(r, params->space_id);
		r = pack_u32(r, 0); /* flags */
		r = params->keygen(r, params->keygen_params);
		request_create(&request, DELETE, reqdata, r - reqdata);
		port.count = 0;
		box_process((struct port *) &port, &request);
		fiber_gc();
	}
}

/* }}} */

