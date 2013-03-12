/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd. All rights reserved.
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
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "bundle.h"

void test_bundle_create(void)
{
	bundle *b;

	b = bundle_create();
	assert(NULL != b);

	bundle_free(b);
}

void test_bundle_add_get(void)
{
	bundle *b = NULL;

	/* new bundle */
	b = bundle_create();
	assert(0 == bundle_add(b, "key1", "val1"));
	assert(0 == bundle_add(b, "key2", "val2"));

	assert(0 == strcmp("val2", bundle_get_val(b, "key2")));
	assert(2 == bundle_get_count(b));

	/* key duplication check 
	 * If key is already exist, raise error code, and ignore new val.
	 * old value is preserved.
	 */
	assert(0 != bundle_add(b, "key1", "val123"));
	assert(EPERM == errno);
	/* still has "val1" for "key1" */
	assert(0 == strcmp("val1", bundle_get_val(b, "key1")));
	/* still count is 2 */
	assert(2 == bundle_get_count(b));

	/* 3rd val */
	bundle_add(b, "key3", "val3");
	assert(3 == bundle_get_count(b));	/* now 3 */

	bundle_free(b);
}

void test_bundle_add_invalid(void)
{
	bundle *b;

	assert(0 != bundle_add(NULL, "a", "b"));
	assert(EINVAL == errno);

	b = bundle_create();
	assert(-1 == bundle_add(b, NULL, "val1"));
	assert(EKEYREJECTED == errno);

	assert(0 != bundle_add(b, "key1", NULL));
	assert(EINVAL == errno);

	assert(0 != bundle_add(b, "", "aaa"));
	assert(EKEYREJECTED == errno);

	bundle_free(b);
}

void test_bundle_get_invalid(void)
{
	bundle *b;

	assert(NULL == bundle_get_val(NULL, "key1"));
	assert(EINVAL == errno);

	b = bundle_create();

	assert(NULL == bundle_get_val(b, "key4"));
	assert(ENOKEY == errno);

	assert(NULL == bundle_get_val(NULL, "aaa"));
	assert(EINVAL == errno);

	assert(NULL == bundle_get_val(b, NULL));
	assert(EKEYREJECTED == errno);

	bundle_free(b);
}

void iter_f(const char *key, const char *val, void *data)
{
	char *keys[] = { "1", "2", "3" };
	char *vals[] = { "a", "b", "c" };
	int i;

	for (i = 0; i < 3; i++) {
		if (0 == strcmp(key, keys[i])) {
			assert(0 == strcmp(val, vals[i]));
			break;
		}
	}
}

void test_bundle_del(void)
{
	bundle *b;

	assert(0 != bundle_del(NULL, "key"));
	assert(EINVAL == errno);

	b = bundle_create();

	assert(0 != bundle_del(b, NULL));
	assert(EKEYREJECTED == errno);

	assert(0 != bundle_del(b, ""));
	assert(EKEYREJECTED == errno);

	assert(0 != bundle_del(b, "key"));
	assert(ENOKEY == errno);

	assert(0 == bundle_get_count(b));

	bundle_add(b, "k1", "v1");
	const char *v = bundle_get_val(b, "k1");
	assert(NULL != v && 0 == strcmp("v1", v));

	assert(1 == bundle_get_count(b));

	assert(0 == bundle_del(b, "k1"));
	assert(NULL == bundle_get_val(b, "k1"));
	assert(ENOKEY == errno);

	assert(0 == bundle_get_count(b));

	bundle_free(b);
}

void test_bundle_iterate(void)
{
	bundle *b;

	b = bundle_create();

	bundle_add(b, "1", "a");
	bundle_add(b, "2", "b");
	bundle_add(b, "3", "c");

	bundle_iterate(b, iter_f, NULL);

	bundle_free(b);
}

void test_bundle_encode_decode(void)
{
	bundle *b1, *b2;
	bundle_raw *r;
	int size_r;

	b1 = bundle_create();
	bundle_add(b1, "k1", "v1");
	bundle_add(b1, "k2", "v2");
	bundle_encode(b1, &r, &size_r);

	/* show serialized data */
	/*
	char *c = r;
	for(; c < r+size_r; c++) {
		printf("%c", *c);
		if ('\0' == *c) printf("\n");
	}
	*/

	b2 = bundle_decode(r, size_r);

	assert(0 == strcmp(bundle_get_val(b1, "k1"), bundle_get_val(b2, "k1")));
	assert(0 == strcmp(bundle_get_val(b1, "k2"), bundle_get_val(b2, "k2")));

	bundle_free(b1);
	bundle_free(b2);
	free(r);
}

void test_bundle_2byte_chars(void)
{
	bundle *b;
	char *k1 = "?ë„ë¦¬ì–¸ë£¨aaabb?¸ã…?°ssì²˜í‘¸?€";
	char *v1 = "?ˆã…‚ì§œccc?œì–¸sssë£¨í¼ë¬?;
	char *k2 = "ljvvn?Œì±‚?¹ã…‡?´ì§•?„í’;ë¨?sdfxa?ã…£?ã…ë¦?;
	char *v2 = "axs?´ã…‡?¼í¼?ë†”ë¯¸ã…‡";

	b = bundle_create();

	bundle_add(b, k1, v1);
	assert(0 == strcmp(v1, bundle_get_val(b, k1)));
	bundle_add(b, k2, v2);
	assert(0 == strcmp(v2, bundle_get_val(b, k2)));

	bundle_free(b);
}

static void _dup_check_cb(const char *k, const char *v, void *data)
{
	bundle *b = (bundle *) data;

	assert(0 == strcmp(bundle_get_val(b, k), v));
}

void test_bundle_dup(void)
{
	bundle *b1, *b2;
	b1 = bundle_create();
	bundle_add(b1, "k1", "v1");
	bundle_add(b1, "k2", "v2");

	b2 = bundle_dup(b1);

	assert(NULL != b2);
	bundle_iterate(b2, _dup_check_cb, b1);
	assert(bundle_get_count(b1) == bundle_get_count(b2));

	bundle_free(b1);
	bundle_free(b2);
}

void test_bundle_convert_argv(void)
{

	bundle *b1, *b2;

	b1 = bundle_create();
	bundle_add(b1, "k1", "v1");
	bundle_add(b1, "k2", "v2");

	/* export test */
	int argc = 0;
	char **argv = NULL;
	argc = bundle_export_to_argv(b1, &argv);
	assert(5 == argc);
	assert(NULL != argv);
	assert(NULL == argv[0]);
	assert(0 == strcmp("k1", argv[1]) && 0 == strcmp("v1", argv[2]));
	assert(0 == strcmp("k2", argv[3]) && 0 == strcmp("v2", argv[4]));
	assert(NULL == argv[5]);	/* Last item must be NULL */

	/* import test */
	b2 = bundle_import_from_argv(argc, argv);
	assert(NULL != b2);
	assert(2 == bundle_get_count(b2));
	assert(0 == strcmp("v1", bundle_get_val(b2, "k1")));
	assert(0 == strcmp("v2", bundle_get_val(b2, "k2")));

	free(argv);
	bundle_free(b1);
	bundle_free(b2);
}

int main(int argc, char **argv)
{
	test_bundle_create();
	test_bundle_add_get();
	test_bundle_add_invalid();
	test_bundle_get_invalid();
	test_bundle_del();
	test_bundle_iterate();
	test_bundle_encode_decode();
	test_bundle_2byte_chars();
	test_bundle_dup();
	test_bundle_convert_argv();

	return 0;
}

