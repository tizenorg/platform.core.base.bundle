/*
 * Copyright (c) 2000 - 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * keyval.c
 * Implementation of keyval object
 */

#include <stdlib.h>

#include "keyval_type.h"
#include "keyval.h"
#include "bundle_log.h"
#include "bundle.h"

static keyval_method_collection_t method = {
	keyval_free,
	keyval_compare,
	keyval_get_encoded_size,
	keyval_encode,
	keyval_decode
};

keyval_t *keyval_new(keyval_t *kv, const char *key,
		const int type, const void *val, const size_t size)
{
	int must_free_obj = kv ? 0 : 1;

	if (!kv) {
		kv = calloc(1, sizeof(keyval_t));
		if (!kv) {
			set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
			return NULL;
		}
	}

	if (kv->key) {
		keyval_free(kv, must_free_obj);
		return NULL;
	}

	kv->key = strdup(key);
	if (!kv->key) {
		set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
		keyval_free(kv, must_free_obj);
		return NULL;
	}

	kv->type = type;
	kv->size = size;

	if (size) {
		kv->val = calloc(1, size);
		if (!kv->val) {
			set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
			keyval_free(kv, 1);
			return NULL;
		}
		if (val)
			memcpy(kv->val, val, size);
	}

	kv->method = &method;

	return kv;
}

void keyval_free(keyval_t *kv, int do_free_object)
{
	if (kv == NULL)
		return;

	if (kv->key) {
		free(kv->key);
		kv->key = NULL;
	}

	if (kv->val != NULL) {
		free(kv->val);
		kv->val = NULL;
	}

	if (do_free_object)
		free(kv);

	return;
}

int keyval_get_data(keyval_t *kv, int *type, void **val, size_t *size)
{
	if (!kv)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	if (keyval_type_is_array(kv->type))
		return BUNDLE_ERROR_INVALID_PARAMETER;

	if (type)
		*type = kv->type;
	if (val)
		*val = kv->val;
	if (size)
		*size = kv->size;

	return 0;
}

/* LCOV_EXCL_START */
int keyval_compare(keyval_t *kv1, keyval_t *kv2)
{
	if (!kv1 || !kv2)
		return -1;

	if (strcmp(kv1->key, kv2->key) != 0)
		return 1;
	if (kv1->type != kv2->type)
		return 1;
	if (kv1->size != kv2->size)
		return 1;

	if (kv1->val == NULL && kv2->val == NULL)
		return 0;
	if (kv1->val == NULL || kv2->val == NULL)
		return 1;
	if (memcmp(kv1->val, kv2->val, kv1->size) != 0)
		return 1;

	return 0;
}
/* LCOV_EXCL_STOP */

size_t keyval_get_encoded_size(keyval_t *kv)
{
	size_t encoded_size;

	if (!kv)
		return 0;

	encoded_size = sizeof(size_t)   /* total size */
		+ sizeof(int)           /* type */
		+ sizeof(size_t)        /* key size */
		+ strlen(kv->key) + 1   /* key (+ null byte) */
		+ sizeof(size_t)        /* size */
		+ kv->size;             /* val */

	return encoded_size;
}

/**
 * encode a keyval to byte
 *
 * @pre			kv must be valid.
 * @post		byte must be freed.
 * @param[in]	kv
 * @param[out]	byte
 * @param[out]	byte_len
 * @return byte_len
 */
size_t keyval_encode(keyval_t *kv, unsigned char **byte, size_t *byte_len)
{
	static const size_t sz_type = sizeof(int);
	static const size_t sz_keysize = sizeof(size_t);
	size_t sz_key = strlen(kv->key) + 1;
	static const size_t sz_size = sizeof(size_t);
	size_t sz_val = kv->size;
	unsigned char *p;

	*byte_len = keyval_get_encoded_size(kv);

	*byte = calloc(1, *byte_len);
	if (!*byte)
		return 0;

	p = *byte;

	memcpy(p, byte_len, sizeof(size_t));
	p += sizeof(size_t);
	memcpy(p, &(kv->type), sz_type);
	p += sz_type;
	memcpy(p, &sz_key, sz_keysize);
	p += sz_keysize;
	memcpy(p, kv->key, sz_key);
	p += sz_key;
	memcpy(p, &(kv->size), sz_size);
	p += sz_size;
	memcpy(p, kv->val, sz_val);
	p += sz_val;

	return *byte_len;
}

/**
 * decode a byte stream to a keyval
 *
 * @param[in]     byte  byte stream.
 * @param[in|out] kv    keyval.
 *                  If kv is NULL, new keyval_t object comes.
 *                  If kv is not NULL, given kv is used. (No new kv is created.)
 * @return        Number of bytes read from byte.
 */
size_t keyval_decode(unsigned char *byte, keyval_t **kv)
{
	static const size_t sz_byte_len = sizeof(size_t);
	static const size_t sz_type = sizeof(int);
	static const size_t sz_keysize = sizeof(size_t);
	static const size_t sz_size = sizeof(size_t);
	size_t byte_len;
	int type;
	size_t keysize;
	char *key;
	size_t size;
	void *val;
	unsigned char *p = byte;

	byte_len = *((size_t *)p);
	p += sz_byte_len;
	type = *((int *)p);
	p += sz_type;
	keysize = *((size_t *)p);
	p += sz_keysize;
	key = (char *)p;
	p += keysize;
	size = *((size_t *)p);
	p += sz_size;
	val = (void *)p;
	p += size;

	if (kv)
		*kv = keyval_new(*kv, key, type, val, size);

	return byte_len;
}

int keyval_get_type_from_encoded_byte(unsigned char *byte)
{
	static const size_t sz_byte_len = sizeof(size_t);
	unsigned char *p = byte;
	int type;

	 p += sz_byte_len;
	type = *((int *)p);

	return type;
}
