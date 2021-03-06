/* libeventmc - Memcached client bindings for libevent.
 * Copyright (C) 2010 Admeld Inc, Milosz Tanski <mtanski@admeld.com>
 *
 * The source code for the libmeldmc library is licensed under the MIT license or
 * at your option under the GPL version 2 license. The contents of the both
 * licenses are contained within the libevemtmc distribution in COPYING.txt file.
 *
 */

/* libc */
#include <stdlib.h>
#include <string.h>
/* openssl's libcrypto */
#include <openssl/md5.h>
/* lieventdmc */
#include "memcached_server.h"
#include "memcached_api.h"

int memcached_key_none(const char *in_key, size_t in_len, const char **out_key, size_t *out_len)
{
  if ((*out_key = strndup(in_key, in_len)) == NULL)
    return -1;

  *out_len = in_len;
  return 0;
}

int memcached_key_len250(const char *in_key, size_t in_len, const char **out_key, size_t *out_len)
{
  /* Duplicate key. */
  if ((*out_key = strndup(in_key, 250)) == NULL)
    return -1;

  *out_len = (in_len <= 250) ? in_len : strlen(*out_key);

  return 0;
}

static inline char tohex(unsigned char i)
{
  static const char *table = "0123456789abcdef";
  return table[i];
}

int memcached_key_md5(const char *in_key, size_t in_len, const char **out_key, size_t *out_len)
{
  char *cur;
  unsigned char digest[MD5_DIGEST_LENGTH];

  if ((*out_key = malloc(sizeof(char) * 32)) == NULL)
    return -1;

  *out_len = 32;

  MD5((const unsigned char *) in_key, in_len, (unsigned char *) &digest);

  cur = (char *) *out_key;
  for (int i=0; i < MD5_DIGEST_LENGTH; i++) {
    *cur = tohex((digest[i] >> 4) & 0x0f);
    cur++;

    *cur = tohex(digest[i] & 0x0f);
    cur++;
 }

  return 0;
}
