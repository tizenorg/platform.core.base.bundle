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

#ifndef __BUNDLE_LOG_H__
#define __BUNDLE_LOG_H__

#include <dlog/dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "BUNDLE"

#ifdef _DEBUG_MODE_
#define BUNDLE_LOG_PRINT(FMT, ARG...) \
	do { \
		printf("%5d", getpid()); \
		printf("%s() : "FMT"\n", __FUNCTION__, ##ARG); \
	} while (0)

#define BUNDLE_EXCEPTION_PRINT(FMT, ARG...) \
	do { \
		printf("%5d", getpid()); \
		printf("%s() : "FMT"\n", __FUNCTION__, ##ARG); \
	} while (0)

#define BUNDLE_ASSERT_PRINT(FMT, ARG...) \
	do { \
		printf("%5d", getpid()); \
		printf("%s() : "FMT"\n", __FUNCTION__, ##ARG); \
	} while (0)
#else
#define BUNDLE_LOG_PRINT(FMT, ARG...) SLOGD(FMT, ##ARG);
#define BUNDLE_EXCEPTION_PRINT(FMT, ARG...) SLOGW(FMT, ##ARG);
#define BUNDLE_ASSERT_PRINT(FMT, ARG...) SLOGE(FMT, ##ARG);
#endif /* _DEBUG_MODE_ */

#endif /* __BUNDLE_LOG_H__ */
