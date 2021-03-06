/*
 *  bundle
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jayoun Lee <airjany@samsung.com>, Sewook Park <sewook7.park@samsung.com>,
 * Jaeho Lee <jaeho81.lee@samsung.com>
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
#include <tet_api.h>
#include "bundle.h"
#include <string.h>
#include <stdlib.h>

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_ApplicationFW_bundle_decode_func_01(void);
static void utc_ApplicationFW_bundle_decode_func_02(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{ utc_ApplicationFW_bundle_decode_func_01, POSITIVE_TC_IDX },
	{ utc_ApplicationFW_bundle_decode_func_02, NEGATIVE_TC_IDX },
	{ NULL, 0 }
};

static void startup(void)
{
}

static void cleanup(void)
{
}

/**
 * @brief Positive test case of bundle_decode()
 */
static void utc_ApplicationFW_bundle_decode_func_01(void)
{
	int r = 0;
	int len;
	bundle_raw *raw = NULL;

	bundle *b = bundle_create();
	if(!b) {
		tet_result(TET_UNINITIATED);
		return;
	}
	if(bundle_add(b, "a", "123")) {
		tet_result(TET_UNINITIATED);
		goto cleanup;
	}
	
	r = bundle_encode(b, &raw, &len);
	if(NULL == raw) {
		tet_result(TET_UNINITIATED);
		goto cleanup;
	}

	bundle *b_dec = bundle_decode(raw, len);
	if(NULL == b_dec) {
		tet_result(TET_FAIL);
		goto cleanup;
	}
	if(0 != strcmp("123", bundle_get_val(b, "a"))) {
		tet_result(TET_FAIL);
		goto cleanup;
	}

	tet_result(TET_PASS);

cleanup:
	if(b) bundle_free(b);
	if(raw) free(raw);
	return;
}

/**
 * @brief Negative test case of ug_init bundle_decode()
 */
static void utc_ApplicationFW_bundle_decode_func_02(void)
{
	if(NULL != bundle_decode(NULL, 1)) {
		tet_result(TET_FAIL);
		goto cleanup;
	}

	tet_result(TET_PASS);
cleanup:
	return;
}
