/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
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

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <json-glib/json-glib.h>

#include "bundle.h"
#include "bundle_internal.h"

static void __add_json_data_from_bundle(const char *key,
		const int type,
		const bundle_keyval_t *kv,
		void *user_data)
{
	void *basic_val = NULL;
	size_t basic_size = 0;
	void **array_val = NULL;
	unsigned int array_len = 0;
	size_t *array_elem_size = 0;
	JsonObject *json_obj = (JsonObject *)user_data;
	JsonArray *json_arr;
	unsigned int i;
	int ret;

	if (json_obj == NULL)
		return;

	if (bundle_keyval_type_is_array((bundle_keyval_t *)kv)) {
		ret = bundle_keyval_get_array_val((bundle_keyval_t *)kv,
				&array_val, &array_len, &array_elem_size);
		if (ret != BUNDLE_ERROR_NONE)
			return;

		json_arr = json_array_new();
		if (json_arr == NULL)
			return;

		for (i = 0; i < array_len; i++)
			json_array_add_string_element(json_arr, array_val[i]);

		json_object_set_array_member(json_obj, key, json_arr);
	} else {
		ret = bundle_keyval_get_basic_val((bundle_keyval_t *)kv,
				&basic_val, &basic_size);
		if (ret != BUNDLE_ERROR_NONE)
			return;

		json_object_set_string_member(json_obj, key, basic_val);
	}
}

int bundle_to_json(bundle *b, char **json)
{
	JsonObject *object;
	JsonNode *node;
	JsonGenerator *generator = NULL;
	gchar *new_json = NULL;
	gsize length;
	int ret = BUNDLE_ERROR_NONE;

	if (b == NULL || json == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	object = json_object_new();
	if (object == NULL)
		return BUNDLE_ERROR_OUT_OF_MEMORY;

	bundle_foreach(b, __add_json_data_from_bundle, object);

	node = json_node_new(JSON_NODE_OBJECT);
	if (node == NULL) {
		ret = BUNDLE_ERROR_OUT_OF_MEMORY;
		goto exception;
	}

	json_node_set_object(node, object);

	generator = json_generator_new();
	if (generator == NULL) {
		ret = BUNDLE_ERROR_OUT_OF_MEMORY;
		goto exception;
	}

	json_generator_set_root(generator, node);

	new_json = json_generator_to_data(generator, &length);
	if (new_json == NULL) {
		ret = BUNDLE_ERROR_OUT_OF_MEMORY;
		goto exception;
	}

	*json = (char *)new_json;

exception:
	if (generator)
		g_object_unref(generator);
	if (node)
		json_node_free(node);
	if (object)
		json_object_unref(object);

	return ret;
}

static void __add_bundle_data_from_json(JsonObject *object,
		const char *key,
		JsonNode *node,
		gpointer user_data)
{
	bundle *b = (bundle *)user_data;
	JsonNodeType node_type;
	JsonArray *json_arr;
	const gchar *val;
	guint len;
	guint i;
	char **arr_val;

	if (key == NULL || node == NULL)
		return;

	node_type = JSON_NODE_TYPE(node);
	if (node_type == JSON_NODE_ARRAY) {
		json_arr = json_node_get_array(node);
		len = json_array_get_length(json_arr);

		arr_val = (char **)calloc(1, sizeof(char *) * len);
		if (arr_val == NULL)
			return;

		for (i = 0; i < len; i++)
			arr_val[i] = strdup(json_array_get_string_element
					(json_arr, i));

		bundle_add_str_array(b, key, (const char **)arr_val, len);

		for (i = 0; i < len; i++)
			free(arr_val[i]);
		free(arr_val);
	} else if (node_type == JSON_NODE_VALUE) {
		val = json_node_get_string(node);
		if (val)
			bundle_add_str(b, key, (const char *)val);
	}
}

int bundle_from_json(const char *json, bundle **b)
{
	GError *error = NULL;
	JsonParser *parser;
	JsonNode *root;
	JsonObject *root_obj;
	bundle *new_b;
	int ret = BUNDLE_ERROR_NONE;

	if (json == NULL || b == NULL)
		return BUNDLE_ERROR_INVALID_PARAMETER;

	parser = json_parser_new();
	if (parser == NULL)
		return BUNDLE_ERROR_OUT_OF_MEMORY;

	json_parser_load_from_data(parser, json, strlen(json), &error);
	if (error) {
		ret = BUNDLE_ERROR_INVALID_PARAMETER;
		goto exception;
	}

	root = json_parser_get_root(parser);
	if (root == NULL) {
		ret = BUNDLE_ERROR_INVALID_PARAMETER;
		goto exception;
	}

	root_obj = json_node_get_object(root);
	if (root_obj == NULL) {
		ret = BUNDLE_ERROR_INVALID_PARAMETER;
		goto exception;
	}

	new_b = bundle_create();
	if (new_b == NULL) {
		ret = BUNDLE_ERROR_OUT_OF_MEMORY;
		goto exception;
	}

	json_object_foreach_member(root_obj,
			__add_bundle_data_from_json, new_b);

	*b = new_b;

exception:
	if (error)
		g_error_free(error);
	if (parser)
		g_object_unref(parser);

	return ret;
}
