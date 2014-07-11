#include "bench.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "box/box.h"
#include "request.h"
#include "tuple.h"
#include "port.h"
#include "fiber.h"

#if defined(MSGPUCK_H_INCLUDED)
#define MASTER 1
#include "iproto_constants.h"
#endif

enum { REQUEST_BODY_MAXLEN = 1024 };

/* {{{ Utils */

#if !defined(MASTER)
void
assert_fail(const char *assertion, const char *file, unsigned int line,
	    const char *function)
{
	(void)function;
	fflush(NULL);
	printf("assert: %s:%d %s\n", file, line, assertion);
	exit(1);
}
#endif /* STABLE */

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
#if defined(MASTER)
	r = mp_encode_array(r, 1);
	r = mp_encode_uint(r, rand());
#else /* STABLE */
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
#if defined(MASTER)
	r = mp_encode_array(r, 1);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
#else /* STABLE */
	r = pack_u32(r, 1);
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
#endif
	return r;
}

char *
gen_num_num(char *r, const struct keygen_params *params)
{
	(void) params;
#if defined(MASTER)
	r = mp_encode_array(r, 2);
	r = mp_encode_uint(r, rand());
	r = mp_encode_uint(r, rand());
#else /* STABLE */
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
#if defined(MASTER)
	r = mp_encode_array(r, 2);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
#else /* STABLE */
	r = pack_u32(r, 2);
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
#endif
	return r;
}

char *
gen_num_str(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
#if defined(MASTER)
	r = mp_encode_array(r, 2);
	r = mp_encode_uint(r, rand());
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
#else /* STABLE */
	r = pack_u32(r, 2);
	r = pack_varint32(r, sizeof(uint32_t));
	r = pack_u32(r, rand());
	randstr(buf, params->len);
	r = pack_lstr(r, buf, params->len);
#endif
	return r;
}

char *
gen_str_num(char *r, const struct keygen_params *params)
{
	char buf[params->len + 1];
#if defined(MASTER)
	r = mp_encode_array(r, 2);
	randstr(buf, params->len);
	r = mp_encode_str(r, buf, params->len);
	r = mp_encode_uint(r, rand());
#else /* STABLE */
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

struct port_bench
{
	struct port_vtab *vtab;
	size_t count;
};

#if defined(MASTER)
static void
port_bench_add_tuple(struct port *port, struct tuple *tuple)
{
	(void) tuple;
	struct port_bench *port_bench = (struct port_bench *) port;
	port_bench->count++;
}
#else /* STABLE */
static void
port_bench_add_tuple(struct port *port, struct tuple *tuple, uint32_t flags)
{
	(void) tuple;
	(void) flags;
	struct port_bench *port_bench = (struct port_bench *) port;
	port_bench->count++;
}
#endif

static struct port_vtab port_bench_vtab = {
	port_bench_add_tuple,
	null_port_eof,
};

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
	struct port_bench port = { &port_bench_vtab, 0 };

	for (uint32_t i = 0; i < params->count; i++) {
		port.count = 0;
		char *r = reqdata;
#if defined(MASTER)
		struct request request;
		request_create(&request, IPROTO_SELECT);
		request.space_id = params->space_id;
		request.limit = 1;
		request.key = r;
		request.key_end = params->keygen(r, params->keygen_params);
		box_process((struct port *) &port, &request);
#else /* STABLE */
		r = pack_u32(r, params->space_id); // space
		r = pack_u32(r, 0); // index
		r = pack_u32(r, 0); // offset
		r = pack_u32(r, 4294967295); // limit
		r = pack_u32(r, 1); // key count
		r = params->keygen(r, params->keygen_params);
		box_process((struct port *) &port, SELECT, reqdata, r - reqdata);
#endif
		assert(port.count == 1);
		fiber_gc();
	}
}

void
test_replaces(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];
	struct port_bench port = { &port_bench_vtab, 0 };

	for (uint32_t i = 0; i < params->count; i++) {
		port.count = 0;
		char *r = reqdata;
#if defined(MASTER)
		struct request request;
		request_create(&request, IPROTO_REPLACE);
		request.space_id = params->space_id;
		request.tuple = r;
		request.tuple_end = params->keygen(r, params->keygen_params);
		box_process((struct port *) &port, &request);
#else /* STABLE */
		r = pack_u32(r, params->space_id); // space
		r = pack_u32(r, 0); // flags
		r = params->keygen(r, params->keygen_params);
		box_process((struct port *) &port, REPLACE, reqdata, r - reqdata);
#endif
		fiber_gc();
	}
}

void
test_updates(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];
	struct port_bench port = { &port_bench_vtab, 0 };

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		port.count = 0;
#if defined(MASTER)
		struct request request;
		request_create(&request, IPROTO_UPDATE);
		request.space_id = params->space_id;
		request.key = r;
		r = params->keygen(r, params->keygen_params);
		request.key_end = r;
		request.tuple = r;
		r = mp_encode_array(r, 1);
		r = mp_encode_array(r, 3);
		r = mp_encode_str(r, "!", 1);
		r = mp_encode_int(r, -1);
		r = mp_encode_uint(r, 0);
		request.tuple_end = r;
		box_process((struct port *) &port, &request);
#else /* STABLE */
		r = pack_u32(r, params->space_id);
		r = pack_u32(r, 0); /* flags */
		r = params->keygen(r, params->keygen_params);
		r = pack_u32(r, 1); /* op_count */
		r = pack_u32(r, -1); /* field_no */
		r = pack_u8(r, 7); /* UPDATE_OP_INSERT */
		r = pack_varint32(r, sizeof(uint32_t));
		r = pack_u32(r, 0);
		box_process((struct port *) &port, UPDATE, reqdata, r - reqdata);
#endif
		fiber_gc();
	}

}

void
test_deletes(const struct test_params *params)
{
	char reqdata[REQUEST_BODY_MAXLEN];
	struct port_bench port = { &port_bench_vtab, 0 };

	for (uint32_t i = 0; i < params->count; i++) {
		char *r = reqdata;
		port.count = 0;
#if defined(MASTER)
		struct request request;
		request_create(&request, IPROTO_DELETE);
		request.space_id = params->space_id;
		request.key = r;
		request.key_end = params->keygen(r, params->keygen_params);
		box_process((struct port *) &port, &request);
#else /* STABLE */
		r = pack_u32(r, params->space_id);
		r = pack_u32(r, 0); /* flags */
		r = params->keygen(r, params->keygen_params);
		box_process((struct port *) &port, DELETE, reqdata, r - reqdata);
#endif
		fiber_gc();
	}
}

/* }}} */

