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
 * bundle.c
 */

#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bundle.h"
#include "bundle_internal.h"
#include "keyval.h"
#include "keyval_array.h"
#include "keyval_type.h"
#include "bundle_log.h"

#define CHECKSUM_LENGTH 32
#define TAG_IMPORT_EXPORT_CHECK "`zaybxcwdveuftgsh`"

/* ADT */
struct _bundle_t {
	keyval_t *kv_head;
};

/**
 * Find a kv from bundle
 */
static keyval_t *_bundle_find_kv(bundle *b, const char *key)
{
	keyval_t *kv;

	if (b == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	if (key == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	kv = b->kv_head;
	while (kv != NULL) {
		if (strcmp(key, kv->key) == 0) {
			set_last_result(BUNDLE_ERROR_NONE);
			return kv;
		}
		kv = kv->next;
	}
	/* Not found */
	set_last_result(BUNDLE_ERROR_KEY_NOT_AVAILABLE);
	return NULL;
}

/**
 * Append kv into bundle
 */
static int _bundle_append_kv(bundle *b, keyval_t *new_kv)
{
	keyval_t *kv;

	if (b->kv_head == NULL) {
		b->kv_head = new_kv;
	} else {
		kv = b->kv_head;
		while (kv->next)
			kv = kv->next;
		kv->next = new_kv;
	}

	return BUNDLE_ERROR_NONE;
}

static int _bundle_add_kv(bundle *b, const char *key, const void *val,
		const size_t size, const int type, const unsigned int len)
{
	keyval_t *kv;
	keyval_t *new_kv;
	keyval_array_t *kva;

	/* basic value check */
	if (b == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;
	if (key == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;
	if (strlen(key) == 0)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	kv = _bundle_find_kv(b, key);
	if (kv) /* Key already exists */
		return BUNDLE_ERROR_KEY_EXISTS;

	if (keyval_type_is_array(type)) {
		/* array type */
		kva = keyval_array_new(NULL, key, type,
				(const void **) val, len);
		new_kv = (keyval_t *)kva;
	} else {
		/* normal type */
		new_kv = keyval_new(NULL, key, type, val, size);
	}

	if (!new_kv)
		return BUNDLE_ERROR_OUT_OF_MEMORY;

	_bundle_append_kv(b, new_kv);

	return BUNDLE_ERROR_NONE;
}

static int _bundle_get_val(bundle *b, const char *key, const int type,
		void **val, size_t *size, unsigned int *len,
		size_t **array_element_size)
{
	keyval_t *kv;
	keyval_array_t *kva;

	kv = _bundle_find_kv(b, key);
	if (!kv) /* Key doesn't exist */
		return get_last_result();

	if (BUNDLE_TYPE_ANY != type && type != kv->type)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	if (keyval_type_is_array(type)) {
		kva = (keyval_array_t *)kv;
		keyval_array_get_data(kva, NULL, (void ***)val,
				len, array_element_size);
	} else {
		keyval_get_data(kv, NULL, val, size);
	}

	return BUNDLE_ERROR_NONE;
}

/**
 * global initialization
 * Run only once.
 */
static void _bundle_global_init(void)
{
	static int _is_done;

	if (_is_done)
		return;

	/* Run init functions */
	keyval_type_init();

	_is_done = 1;
	return;
}

/* APIs */
bundle *bundle_create(void)
{
	bundle *b = NULL;

	_bundle_global_init();

	b = calloc(1, sizeof(bundle));	/* fill mem with NULL */
	if (b == NULL) {
		BUNDLE_EXCEPTION_PRINT(
				"Unable to allocate memory for bundle\n");
		set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
		goto exception;
	}

	set_last_result(BUNDLE_ERROR_NONE);
	return b;

exception:
	return NULL;
}

int bundle_free(bundle *b)
{
	keyval_t *kv;
	keyval_t *tmp_kv;

	if (b == NULL) {
		BUNDLE_EXCEPTION_PRINT("Bundle is already freed\n");
		return BUNDLE_ERROR_INVALID_PARAMETER;
	}

	/* Free keyval list */
	kv = b->kv_head;
	while (kv) {
		tmp_kv = kv;
		kv = kv->next;
		tmp_kv->method->free(tmp_kv, 1);
	}

	/* free bundle */
	free(b);

	return BUNDLE_ERROR_NONE;
}

int bundle_add_str(bundle *b, const char *key, const char *str)
{
	if (!str)
		return BUNDLE_ERROR_INVALID_PARAMETER;
	return _bundle_add_kv(b, key, str, strlen(str) + 1, BUNDLE_TYPE_STR, 1);
}

int bundle_get_str(bundle *b, const char *key, char **str)
{
	return _bundle_get_val(b, key, BUNDLE_TYPE_STR, (void **)str,
			NULL, NULL, NULL);
}

int bundle_add(bundle *b, const char *key, const char *val)
{
	return bundle_add_str(b, key, val);
}

int bundle_del(bundle *b, const char *key)
{
	keyval_t *kv;
	keyval_t *prev_kv = NULL;

	/* basic value check */
	if (b == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;
	if (key == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;
	if (strlen(key) == 0)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	kv = b->kv_head;
	while (kv) {
		if (strcmp(key, kv->key) == 0)
			break;
		prev_kv = kv;
		kv = kv->next;
	}

	if (kv == NULL) {
		return BUNDLE_ERROR_KEY_NOT_AVAILABLE;
	} else {
		if (prev_kv)
			prev_kv->next = kv->next;
		if (kv == b->kv_head)
			b->kv_head = kv->next;
		kv->method->free(kv, 1);
	}

	return BUNDLE_ERROR_NONE;
}

const char *bundle_get_val(bundle *b, const char *key)
{
	char *val = NULL;
	int ret = BUNDLE_ERROR_NONE;

	ret = bundle_get_str(b, key, &val);
	set_last_result(ret);

	return val;
}

/**
 * @brief used by bundle_get_count() API, to count number of items in a bundle
 */
static void _bundle_get_count_iter(const char *k, const int type,
		const bundle_keyval_t *kv, void *user_data)
{
	int *count = (int *)user_data;
	*count += 1;
}

int bundle_get_count(bundle *b)
{
	int count = 0;

	if (b == NULL)
		return count;
	bundle_foreach(b, _bundle_get_count_iter, &count);
	return count;
}

void bundle_iterate(bundle *b, bundle_iterate_cb_t callback, void *data)
{
	keyval_t *kv;

	if (b == NULL || callback == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return;
	}

	kv = b->kv_head;
	while (kv) {
		callback(kv->key, kv->val, data);
		kv = kv->next;
	}

	set_last_result(BUNDLE_ERROR_NONE);
}

void bundle_foreach(bundle *b, bundle_iterator_t iter, void *user_data)
{
	keyval_t *kv;

	if (b == NULL || iter == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return;	/* TC_FIX if b = NULL- error handling */
	}

	kv = b->kv_head;
	while (kv) {
		iter(kv->key, kv->type, kv, user_data);
		kv = kv->next;
	}

	set_last_result(BUNDLE_ERROR_NONE);
}

/* keyval functions */
int bundle_keyval_get_type(bundle_keyval_t *kv)
{
	if (kv == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return -1;
	}
	set_last_result(BUNDLE_ERROR_NONE);
	return kv->type;
}

int bundle_keyval_type_is_array(bundle_keyval_t *kv)
{
	if (kv == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return -1;
	}
	set_last_result(BUNDLE_ERROR_NONE);
	return keyval_type_is_array(kv->type);
}

int bundle_keyval_type_is_measurable(bundle_keyval_t *kv)
{
	if (kv == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return -1;
	}
	set_last_result(BUNDLE_ERROR_NONE);
	return keyval_type_is_measurable(kv->type);
}

int bundle_keyval_get_basic_val(bundle_keyval_t *kv, void **val, size_t *size)
{
	return keyval_get_data(kv, NULL, val, size);
}

int bundle_keyval_get_array_val(bundle_keyval_t *kv, void ***array_val,
		unsigned int *array_len, size_t **array_item_size)
{
	return keyval_array_get_data((keyval_array_t *)kv, NULL,
			array_val, array_len, array_item_size);
}

bundle *bundle_dup(bundle *b_from)
{
	bundle *b_to;
	keyval_t *kv_from;
	keyval_t *kv_to;
	keyval_array_t *kva_from;
	int i;

	if (b_from == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	b_to = bundle_create();
	if (b_to == NULL)
		return NULL;

	kv_from = b_from->kv_head;
	while (kv_from != NULL) {
		if (keyval_type_is_array(kv_from->type)) {
			kva_from = (keyval_array_t *)kv_from;
			kv_to = (keyval_t *)keyval_array_new(NULL, kv_from->key,
					kv_from->type, NULL, kva_from->len);
			if (!kv_to)
				goto err;

			for (i = 0; i < kva_from->len; i++) {
				if (((keyval_array_t *)kv_from)->array_val[i]) {
					keyval_array_set_element(
							(keyval_array_t *)kv_to, i,
							((keyval_array_t *)kv_from)->array_val[i],
							((keyval_array_t *)kv_from)->array_element_size[i]);
				}
			}

			_bundle_append_kv(b_to, kv_to);
		} else {
			if (_bundle_add_kv(b_to, kv_from->key,
				kv_from->val, kv_from->size, kv_from->type, 0))
				goto err;
		}

		kv_from = kv_from->next;
	}

	return b_to;

err:
	bundle_free(b_to);
	return NULL;
}

int bundle_encode(bundle *b, bundle_raw **r, int *len)
{
	keyval_t *kv;
	unsigned char *m;
	unsigned char *p_m;
	unsigned char *byte;
	size_t byte_len;
	gchar *chksum_val;
	size_t msize = 0;

	if (b == NULL || r == NULL || len == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	/* calculate memory size */
	kv = b->kv_head;
	while (kv != NULL) {
		msize += kv->method->get_encoded_size(kv);
		kv = kv->next;
	}

	m = calloc(msize + CHECKSUM_LENGTH, sizeof(unsigned char));
	if (unlikely(m == NULL))
		return BUNDLE_ERROR_OUT_OF_MEMORY;

	p_m = m + CHECKSUM_LENGTH; /* temporary pointer */

	kv = b->kv_head;
	while (kv != NULL) {
		byte = NULL;
		byte_len = 0;

		kv->method->encode(kv, &byte, &byte_len);
		memcpy(p_m, byte, byte_len);

		p_m += byte_len;
		kv = kv->next;

		free(byte);
	}

	/* compute checksum from the data */
	chksum_val = g_compute_checksum_for_string(G_CHECKSUM_MD5,
			(const char *)(m + CHECKSUM_LENGTH), msize);
	/* prefix checksum to the data */
	memcpy(m, chksum_val, CHECKSUM_LENGTH);
	if (r) {
		/* base64 encode for whole string checksum and data */
		*r = (unsigned char *)g_base64_encode(m, msize + CHECKSUM_LENGTH);
		if (len)
			*len = strlen((char *)*r);
	}
	free(m);
	g_free(chksum_val); /* free checksum string */

	return BUNDLE_ERROR_NONE;
}

int bundle_free_encoded_rawdata(bundle_raw **r)
{
	if (!*r)
		return BUNDLE_ERROR_INVALID_PARAMETER; /* TC_FIX - double free sigabrt handling */

	free(*r);
	*r = NULL;
	return BUNDLE_ERROR_NONE;
}

bundle *bundle_decode(const bundle_raw *r, const int data_size)
{
	bundle *b;
	bundle_raw *p_r;
	unsigned char *d_str;
	gint state = 0;
	guint save = 0;
	unsigned int d_len_raw;
	unsigned char *d_r;
	unsigned int d_len;
	char *extract_cksum;
	gchar *compute_cksum;
	size_t bytes_read;
	keyval_t *kv;
	int type;

	if (r == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	extract_cksum = calloc(CHECKSUM_LENGTH + 1, sizeof(char));
	if (unlikely(extract_cksum == NULL)) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	/*
	 * base 64 decode of input string
	 * Since base64 encodes 3 bytes in 4 chars (+3 may be needed in case of non-zero state)
	 * refer to: https://developer.gnome.org/glib/stable/glib-Base64-Encoding.html#g-base64-decode-step
	 */
	d_str = malloc((data_size / 4) * 3 + 3);
	if (unlikely(d_str == NULL)) {
		set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
		free(extract_cksum);
		return NULL;
	}

	d_len_raw = g_base64_decode_step((char *)r, data_size, d_str, &state, &save);
	if (d_len_raw < CHECKSUM_LENGTH) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		free(d_str);
		free(extract_cksum);
		return NULL;
	}

	/* extract checksum from the received string */
	strncpy(extract_cksum, (const char *)d_str, CHECKSUM_LENGTH);
	/* compute checksum for the data */
	compute_cksum = g_compute_checksum_for_string(G_CHECKSUM_MD5,
			(const char *)(d_str + CHECKSUM_LENGTH),
			d_len_raw - CHECKSUM_LENGTH);
	/*
	 * compare checksum values- extracted from the received
	 * string and computed from the data
	 */
	if (strcmp(extract_cksum, compute_cksum) != 0) {
		free(d_str);
		free(extract_cksum);
		g_free(compute_cksum);
		return NULL;
	}
	d_r = d_str + CHECKSUM_LENGTH;
	d_len = d_len_raw - CHECKSUM_LENGTH;

	/* re-construct bundle */
	b = bundle_create();
	if (b == NULL) {
		free(d_str);
		free(extract_cksum);
		g_free(compute_cksum);
		return NULL;
	}

	p_r = (bundle_raw *)d_r;
	while (p_r < d_r + d_len - 1) {
		kv = NULL;	/* To get a new kv */

		/* Find type, and use decode function according to type */
		type = keyval_get_type_from_encoded_byte(p_r);
		if (keyval_type_is_array(type))
			bytes_read = keyval_array_decode(p_r,
					(keyval_array_t **) &kv);
		else
			bytes_read = keyval_decode(p_r, &kv);

		if (kv)
			_bundle_append_kv(b, kv);
		else
			break;
		p_r += bytes_read;
	}

	free(extract_cksum);
	g_free(compute_cksum);
	free(d_str);

	set_last_result(BUNDLE_ERROR_NONE);
	return b;
}

struct _argv_idx {
	int argc;
	char **argv;
	int idx;
};

int bundle_encode_raw(bundle *b, bundle_raw **r, int *len)
{
	keyval_t *kv = NULL;
	unsigned char *m = NULL;
	unsigned char *p_m = NULL;
	unsigned char *byte = NULL;
	size_t byte_len;
	gchar *chksum_val = NULL;
	size_t msize = 0;

	if (b == NULL || r == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	/* calculate memory size */
	kv = b->kv_head;
	while (kv != NULL) {
		msize += kv->method->get_encoded_size(kv);
		kv = kv->next;
	}

	m = calloc(msize+CHECKSUM_LENGTH, sizeof(unsigned char));
	if (unlikely(m == NULL))
		return BUNDLE_ERROR_OUT_OF_MEMORY;

	p_m = m + CHECKSUM_LENGTH;	/* temporary pointer */

	kv = b->kv_head;
	while (kv != NULL) {
		byte = NULL;
		byte_len = 0;

		kv->method->encode(kv, &byte, &byte_len);
		memcpy(p_m, byte, byte_len);

		p_m += byte_len;
		kv = kv->next;

		free(byte);
	}

	/* compute checksum from the data */
	chksum_val = g_compute_checksum_for_string(G_CHECKSUM_MD5,
			(const char *)(m + CHECKSUM_LENGTH), msize);
	/* prefix checksum to the data */
	memcpy(m, chksum_val, CHECKSUM_LENGTH);

	*r = m;
	*len = msize + CHECKSUM_LENGTH;
	g_free(chksum_val); /* free checksum string */

	return BUNDLE_ERROR_NONE;
}

bundle *bundle_decode_raw(const bundle_raw *r, const int data_size)
{
	bundle *b = NULL;
	bundle_raw *p_r = NULL;
	unsigned char *d_str = NULL;
	unsigned int d_len_raw;
	unsigned char *d_r = NULL;
	unsigned int d_len;
	char *extract_cksum;
	gchar *compute_cksum = NULL;
	size_t bytes_read;
	keyval_t *kv;
	int type;

	if (r == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;
	}

	extract_cksum = calloc(CHECKSUM_LENGTH + 1, sizeof(char));
	if (unlikely(extract_cksum == NULL)) {
		set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
		return NULL;
	}

	/* base 64 decode of input string*/
	/* d_str = g_base64_decode((char*)r, &d_len_raw); */
	d_str = (unsigned char *)r;
	d_len_raw = data_size;
	/* extract checksum from the received string */
	strncpy(extract_cksum, (const char *)d_str, CHECKSUM_LENGTH);
	/* compute checksum for the data */
	compute_cksum = g_compute_checksum_for_string(G_CHECKSUM_MD5,
			(const char *)(d_str + CHECKSUM_LENGTH),
			d_len_raw - CHECKSUM_LENGTH);
	/*
	 * compare checksum values- extracted from the received
	 * string and computed from the data
	 */
	if (strcmp(extract_cksum, compute_cksum) != 0) {
		free(extract_cksum);
		g_free(compute_cksum);
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;
	}
	d_r = d_str + CHECKSUM_LENGTH;
	d_len = d_len_raw - CHECKSUM_LENGTH;

	/* re-construct bundle */
	b = bundle_create();
	if (b == NULL) {
		free(extract_cksum);
		g_free(compute_cksum);
		return NULL;
	}

	p_r = (bundle_raw *)d_r;
	while (p_r < d_r + d_len - 1) {
		kv = NULL;	/* To get a new kv */

		/* Find type, and use decode function according to type */
		type = keyval_get_type_from_encoded_byte(p_r);
		if (keyval_type_is_array(type))
			bytes_read = keyval_array_decode(p_r,
					(keyval_array_t **) &kv);
		else
			bytes_read = keyval_decode(p_r, &kv);

		if (kv)
			_bundle_append_kv(b, kv);
		else
			break;
		p_r += bytes_read;
	}

	free(extract_cksum);
	g_free(compute_cksum);
	set_last_result(BUNDLE_ERROR_NONE);

	return b;
}

void _iter_export_to_argv(const char *key, const int type, const keyval_t *kv,
		void *user_data)
{
	struct _argv_idx *vi = (struct _argv_idx *)user_data;
	unsigned char *byte = NULL;
	unsigned char *encoded_byte = NULL;
	size_t byte_len = 0;

	vi->argv[vi->idx] = (char *)key;
	if (kv->method->encode((struct keyval_t *)kv, &byte, &byte_len) == 0) {
		/* TODO: encode FAILED! */
		BUNDLE_EXCEPTION_PRINT("bundle: FAILED to encode keyval: %s\n",
				key);
		return;
	}

	encoded_byte = (unsigned char *)g_base64_encode(byte, byte_len);
	if (encoded_byte == NULL) {
		BUNDLE_EXCEPTION_PRINT("bundle: failed to encode byte\n");
		return;
	}

	vi->argv[vi->idx + 1] = (char *)encoded_byte;
	(vi->idx) += 2;

	free(byte);
}

int bundle_export_to_argv(bundle *b, char ***argv)
{
	struct _argv_idx vi;
	int argc;
	int item_count;

	if (b == NULL || argv == NULL) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return -1;
	}

	item_count = bundle_get_count(b);
	argc = 2 * item_count + 2;
	/* 2 more count for argv[0] and arv[1] = encoded */
	*argv = calloc(argc + 1, sizeof(char *));
	if (!*argv) {
		set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
		return -1;
	}

	vi.argc = argc;
	vi.argv = *argv;
	vi.idx = 2;				/* start from index 2*/
	vi.argv[1] = TAG_IMPORT_EXPORT_CHECK;	/* set argv[1] as encoded*/
	/* BUNDLE_LOG_PRINT("\nargument 1 is %s",vi.argv[1]); */

	bundle_foreach(b, _iter_export_to_argv, &vi);

	set_last_result(BUNDLE_ERROR_NONE);
	return argc;
}

int bundle_free_exported_argv(int argc, char ***argv)
{
	int i;

	if (argc < 1)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	if (!*argv || argc < 2)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	for (i = 3; i < argc; i += 2)
		free((*argv)[i]); /* need to free value from g_base64_encode() */
	free(*argv);
	*argv = NULL;

	return BUNDLE_ERROR_NONE;
}

bundle *bundle_import_from_argv(int argc, char **argv)
{
	int idx;
	int type;
	keyval_t *kv;
	keyval_array_t *kva;
	unsigned char *byte;
	char *encoded_byte;
	unsigned int byte_size;
	bundle *b;

	if (!argv) {
		set_last_result(BUNDLE_ERROR_INVALID_PARAMETER);
		return NULL;  /* TC_FIX error handling for argv =NULL*/
	}

	b = bundle_create();
	if (!b) {
		set_last_result(BUNDLE_ERROR_OUT_OF_MEMORY);
		return NULL;
	}

	if (argc < 2) {
		set_last_result(BUNDLE_ERROR_NONE);
		return b;
	}

	if (!argv[1] || strcmp(argv[1], TAG_IMPORT_EXPORT_CHECK)) {
		/* start idx from one as argv[1] is user given argument */
		for (idx = 1; idx + 1 < argc; idx = idx + 2)
			bundle_add(b, argv[idx], argv[idx + 1]);

		return b;
	}

	/* start idx from 2 as argv[1] is encoded */
	for (idx = 2; idx + 1 < argc; idx = idx + 2) {
		kv = NULL;
		kva = NULL;

		encoded_byte = argv[idx+1];

		/* base64_decode */
		byte = g_base64_decode(encoded_byte, &byte_size);
		if (byte == NULL) {
			if (b)
				set_last_result(bundle_free(b));
			return NULL;
		}

		type = keyval_get_type_from_encoded_byte(byte);
		if (keyval_type_is_array(type)) {
			if (keyval_array_decode(byte, &kva) == 0) /* TODO: error! */
				BUNDLE_EXCEPTION_PRINT("Unable to Decode array\n");
			kv = (keyval_t *)kva;
		} else {
			if (keyval_decode(byte, &kv) == 0) /* TODO: error! */
				BUNDLE_EXCEPTION_PRINT("Unable to Decode\n");
		}
		_bundle_append_kv(b, kv);

		free(byte);
		byte = NULL;
	}

	set_last_result(BUNDLE_ERROR_NONE);
	return b;
}

int bundle_get_type(bundle *b, const char *key)
{
	keyval_t *kv = _bundle_find_kv(b, key);

	if (kv) {
		return kv->type;
	} else {
		set_last_result(BUNDLE_ERROR_KEY_NOT_AVAILABLE);
		return BUNDLE_TYPE_NONE;
	}
}

/*
 * Get length of an array
 */
unsigned int bundle_get_array_len(bundle *b, const char *key)
{
	return BUNDLE_ERROR_NONE;
}

/*
 * Get size of an item in byte, of given pointer
 */
size_t bundle_get_array_val_size(bundle *b, const char *key,
		const void *val_ptr)
{
	return BUNDLE_ERROR_NONE;
}

static int bundle_set_array_val(bundle *b, const char *key, const int type,
		const unsigned int idx, const void *val, const size_t size)
{
	keyval_t *kv;
	keyval_array_t *kva;

	kv = _bundle_find_kv(b, key);
	if (kv == NULL)
		return get_last_result();

	if (type != kv->type)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	/* TODO: Is this needed? */
	if (!keyval_type_is_array(kv->type))
		return BUNDLE_ERROR_INVALID_PARAMETER;

	kva = (keyval_array_t *)kv;
	if (!keyval_array_is_idx_valid(kva, idx))
		return BUNDLE_ERROR_INVALID_PARAMETER;

	/* NULL value test (TODO: is this needed?) */
	if (!kva->array_val)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	return keyval_array_set_element(kva, idx, (void *)val, size);
}

int bundle_add_str_array(bundle *b, const char *key, const char **str_array,
		const int len)
{
	return _bundle_add_kv(b, key, str_array, 0, BUNDLE_TYPE_STR_ARRAY, len);
}

int bundle_get_val_array(bundle *b, const char *key, char ***str_array,
		int *len)
{
	return _bundle_get_val(b, key, BUNDLE_TYPE_STR_ARRAY,
			(void **)str_array, NULL, (unsigned int *)len, NULL);
}

const char **bundle_get_str_array(bundle *b, const char *key, int *len)
{
	int ret = BUNDLE_ERROR_NONE;
	const char **arr_val = NULL;

	ret = bundle_get_val_array(b, key, (char ***)&arr_val, len);
	set_last_result(ret);

	return arr_val;
}

int bundle_compare(bundle *b1, bundle *b2)
{
	keyval_t *kv1;
	keyval_t *kv2;

	if (!b1 || !b2)
		return -1;

	if (bundle_get_count(b1) != bundle_get_count(b2))
		return 1;

	for (kv1 = b1->kv_head; kv1 != NULL; kv1 = kv1->next) {
		kv2 = _bundle_find_kv(b2, kv1->key);
		if (!kv2)
			return 1;
		if (kv1->method->compare(kv1, kv2))
			return 1;
	}

	return 0;
}

int bundle_set_str_array_element(bundle *b, const char *key,
		const unsigned int idx, const char *val)
{
	if (!val)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	return bundle_set_array_val(b, key, BUNDLE_TYPE_STR_ARRAY,
			idx, val, strlen(val) + 1);
}

int bundle_add_byte(bundle *b, const char *key, const void *byte,
		const size_t size)
{
	return _bundle_add_kv(b, key, byte, size, BUNDLE_TYPE_BYTE, 1);
}

int bundle_get_byte(bundle *b, const char *key, void **byte, size_t *size)
{
	return _bundle_get_val(b, key, BUNDLE_TYPE_BYTE, (void **)byte,
			size, NULL, NULL);
}

int bundle_add_byte_array(bundle *b, const char *key, void **byte_array,
		const unsigned int len)
{
	return _bundle_add_kv(b, key, byte_array, 0,
			BUNDLE_TYPE_BYTE_ARRAY, len);
}

int bundle_get_byte_array(bundle *b, const char *key, void ***byte_array,
		unsigned int *len, unsigned int **array_element_size)
{
	return _bundle_get_val(b, key, BUNDLE_TYPE_BYTE_ARRAY,
			(void **)byte_array, NULL, len, array_element_size);
}

int bundle_set_byte_array_element(bundle *b, const char *key,
		const unsigned int idx, const void *val, const size_t size)
{
	return bundle_set_array_val(b, key, BUNDLE_TYPE_BYTE_ARRAY,
			idx, val, size);
}
