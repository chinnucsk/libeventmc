/* libeventmc - Memcached client bindings for libevent.
 * Copyright (C) 2010 Admeld Inc, Milosz Tanski <mtanski@admeld.com>
 *
 * The source code for the libmeldmc library is licensed under the MIT license or
 * at your option under the GPL version 2 license. The contents of the both
 * licenses are contained within the libevemtmc distribution in COPYING.txt file.
 *
 */

#ifndef __MEMCACHED_API_H__
#define __MEMCACHED_API_H__

#include <arpa/inet.h>

struct memcached_api;

enum memcached_hash_type {
  MEMCACHED_HASH_CUSTOM,
  MEMCACHED_HASH_NONE,
  MEMCACHED_HASH_CRC32,
  MEMCACHED_HASH_KETAMA,
};

struct memcached_host {
  union {
    struct sockaddr_in   addr_4;
    struct sockaddr_in6  addr_6;
  } sockaddr;

  struct memcached_server *server_conn;
};

/*
 * API callback function prototypes.
 *
 */

/* Command calls backs */
typedef void (*memcached_cb_get)(struct memcached_api *api, enum memcached_result status, const char *key,
                                 ssize_t key_len, const void *data, ssize_t data_len, uint64_t cas, void *api_baton,
                                 void *cmd_baton);
typedef void (*memcached_cb_add)(struct memcached_api *api, enum memcached_result status, uint64_t cas,
                                 void *api_baton, void *cmd_baton);

typedef void (*memcached_cb_set)(struct memcached_api *api, enum memcached_result status, uint64_t cas,
                                 void *api_baton, void *cmd_baton);

typedef void (*memcached_cb_replace)(struct memcached_api *api, enum memcached_result status, uint64_t cas,
                                     void *api_baton, void *cmd_baton);

/* A command we don't know about came back. */
typedef void (*memcached_cb_unknown)(struct memcached_api *api, const struct memcached_msg *in_msg, void *api_baton);


/* Hashing function callback prototype (for specifing custom hash functions). */
typedef int (*memcached_hash_func)(const char *const key, ssize_t key_len, const struct memcached_host *hosts,
                                   int num_hosts);

/* Key transform functions */
typedef int (*memcached_keytrans_func)(const char *in_key, size_t in_len, const char **out_key, size_t *out_len);

/*
 * Built in hash functions.
 *
 */

int memcached_hash_none(const char *key, ssize_t key_len, const struct memcached_host *hosts, int num_hosts);
int memcached_hash_crc32(const char *key, ssize_t key_len, const struct memcached_host *hosts, int num_hosts);
int memcached_hash_ketama(const char *key, ssize_t key_len, const struct memcached_host *hosts, int num_hosts);

/*
 * Build in key transformation functions.
 * 
 */
int memcached_key_none(const char *in_key, size_t in_len, const char **out_key, size_t *out_len);
int memcached_key_len250(const char *in_key, size_t in_len, const char **out_key, size_t *out_len);
int memcached_key_md5(const char *in_key, size_t in_len, const char **out_key, size_t *out_len);


/*
 * Misc. built in callback functions.
 */

/* Ignore unknown id returns. */
void memcached_unkown_id_ignore(struct memcached_api *api, const struct memcached_msg *in_msg, void *api_baton);

/*
 * Initlize and tear down an memcached api instance.
 */

struct memcached_api *memcached_api_init(struct event_base *event_base, memcached_hash_func hash_func,
                                         memcached_keytrans_func key_fun, memcached_cb_unknown cb_unknown_id,
                                         int num_hosts, struct sockaddr **hosts, enum memcached_conn conn_type,
                                         void *api_baton);

void memcached_api_free(struct memcached_api *api);


/*
 * Operations on the current memcached api instance.
 */

/* Prune current pending commands (and fault them). */
void memcached_api_prune_pending(struct memcached_api *api);

/*
 * Abstracted memcachecd commands.
 *
 */

int memcached_api_get(struct memcached_api *api, const char *key, size_t key_len, memcached_cb_get callback,
                      void *cb_data);
int memcached_api_add(struct memcached_api *ap, const char *const key, size_t key_len, void *data, size_t data_len,
                      memcached_cb_add callback, void *cb_data);
int memcached_api_set(struct memcached_api *api, const char *const key, size_t key_len, void *data, size_t data_len,
                      uint64_t cas, uint32_t flags, uint32_t expiry, memcached_cb_set callback_func,
                      void *callback_data);

#endif /* __MEMCACHED_API__ */
