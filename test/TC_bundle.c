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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <bundle.h>
#include <sys/wait.h>
#include <sys/types.h>
#define SUCCESS 1
#define FAIL 0
int ret_bundle_foreach=FAIL;
int ret_bundle_foreach_invalid=FAIL;
int ret_bundle_keyval_typeget=FAIL;
int ret_bundle_exp_imp_p=FAIL;
int ret_bundle_en_dec_p=FAIL;
int ret_exp_imp_cases=FAIL;
int ret_bundle_exp_invalid=FAIL;
int ret_bundle_imp_invalid=FAIL;
int ret_bundle_add=FAIL;
int ret_bundle_get_val=FAIL;
int ret_bundle_dup=FAIL;
int ret_bundle_encode=FAIL;
int ret_bundle_decode=FAIL;
int ret_bundle_free_encoded=FAIL;
int ret_bundle_get_type=FAIL;
int ret_tc_free_exp_argv=FAIL;
int ret_keyval_is_array=FAIL;
int ret_keyval_get_basicval=FAIL;
int ret_bundle_del=FAIL;
int ret_keyval_get_arrval=FAIL;
int ret_bundle_get_str_arr=FAIL;
int ret_bundle_add_str_arr=FAIL;

void iterate_bundleforeach(const char *key,const int type, bundle_keyval_t *kv, void *data)
{
	switch(type)
	{
		case BUNDLE_TYPE_STR:
		{
			//printf("\n CASE: BUNDLE_TYPE_STR ");
			char *basic_val=NULL;
			size_t size;
			bundle_keyval_get_basic_val(kv,(void *)&basic_val,&size);
			//printf("Key: %s ---- Val: %s\n",key,basic_val);
			break;
		}
		case BUNDLE_TYPE_STR_ARRAY:
		{
			//printf("\n CASE: BUNDLE_TYPE_STR_ARRAY: ");
			void **arr=NULL;
			size_t *array_ele_size=NULL;
			unsigned int ele_nos=0;
			int i=0;
			bundle_keyval_get_array_val(kv,&arr,&ele_nos,&array_ele_size);
			//printf("Total Elements: %d\n",ele_nos);
			//printf("Key: %s \n",key);
			for(i=0;i<ele_nos;i++)
			{
				//printf("element is %s\n",(char *)*(arr+i));
			}
			break;
		}
		case BUNDLE_TYPE_BYTE:
		{
			//printf("\n CASE: BUNDLE_TYPE_BYTE ");
			char *basic_val=NULL;
			size_t size;
			bundle_keyval_get_basic_val(kv,(void *)&basic_val,&size);
			//printf("Key: %s ---- Val: %s\n",key,basic_val);
			break;
		}
		case BUNDLE_TYPE_BYTE_ARRAY:
		{
			//printf("\n CASE:BUNDLE_TYPE_BYTE_ARRAY ");
			void **arr=NULL;
			size_t *array_ele_size=NULL;
			unsigned int ele_nos=0;
			int i=0;
			bundle_keyval_get_array_val(kv,&arr,&ele_nos,&array_ele_size);
			//printf("Total Elements: %d\n",ele_nos);
			//printf("Key: %s \n",key);
			for (i=0;i<ele_nos;i++)
			{
				//printf("element is %s\n",(char *)*(arr+i));
			}
			break;
		}
		default:
		{
			//printf("\n no match found");
			//printf("Key: %s is of type %d\n",key, type);
			break;
		}
	
	}
	
	return;
}

void iterate_bundleforeach_withdata(const char *key,const int type, bundle_keyval_t *kv, void *data)
{
	//printf("\n data  received is %d",(int)data);
}

int TC_bundle_foreach()
{
	//printf("\n ====================================================================\n");

	//printf("\nTESTCASE: TC_bundle_foreach");
	bundle *b=NULL;
	char **array=NULL;
	char buf[10]={0,};
	int no_of_ele=10;
	int i=0;
	int data_pass = 35;

	b=bundle_create();
	assert(NULL != b);
/*add elements */
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));

/*bundle_for each with passed data */
	bundle_foreach(b,(void *)iterate_bundleforeach_withdata,(void*)data_pass);

	array=(char **)malloc(no_of_ele*sizeof(char *));
	for (i=0;i<no_of_ele;i++)
	{
		sprintf(buf,"arr_val%d",i);
		*(array+i)=(char *)malloc(sizeof(char *)*strlen(buf));
		strcpy(*(array+i),buf);
	}
	assert(0==bundle_add_str_array(b,"k3", (const char **)array, no_of_ele));



	bundle_foreach(b,(void *)iterate_bundleforeach,NULL);

	assert(0==bundle_free(b));
	return SUCCESS;
}

int  TC_bundle_foreach_invalid()
{
	//printf("\n ====================================================================\n");

	//printf("\nTESTCASE: TC_bundle_foreach_invalid");
	bundle *b=NULL;

	bundle_foreach(b,(void *)iterate_bundleforeach,NULL);  //FIXED:segmentation fault- put check for b=null
	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));
	return SUCCESS;
}
void iterate_bundle_keyvalgettype(const char *key,const int type, bundle_keyval_t *kv, void *data)
{
	
	assert(type == bundle_keyval_get_type(kv));
	return;
}
int TC_bundle_keyval_get_type()
{
	//printf("\n====================================================================\n");

	//printf("\nTESTCASE: TC_bundle_keyval_get_type");
	bundle *b=NULL;
	char **array=NULL;
	char buf[10]={0,};
	int no_of_ele=10;
	int i=0;

	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));

	array=(char **)malloc(no_of_ele*sizeof(char *));
	for (i=0;i<no_of_ele;i++)
	{
		sprintf(buf,"arr_val%d",i);
		*(array+i)=(char *)malloc(sizeof(char *)*strlen(buf));
		strcpy(*(array+i),buf);
	}
	assert(0==bundle_add_str_array(b,"k3", (const char **)array, no_of_ele));


	bundle_foreach(b,(void *)iterate_bundle_keyvalgettype,NULL);
	return SUCCESS;
}

int TC_bundle_export_import_print()
{

	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_export_import_print ");
	bundle *b=NULL;
	bundle *b_new=NULL;
	char **array=NULL;
	char buf[10]={0,};
	int no_of_ele=10;
	int i=0;

	int argc1=0;
	char **argv1=NULL;

	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));

	array=(char **)malloc(no_of_ele*sizeof(char *));
	for (i=0;i<no_of_ele;i++)
	{
		sprintf(buf,"arr_val%d",i);
		*(array+i)=(char *)malloc(sizeof(char *)*strlen(buf));
		strcpy(*(array+i),buf);
	}
	assert(0==bundle_add_str_array(b,"k3", (const char **)array, no_of_ele));


	//printf("\noriginal bundle");
	bundle_foreach(b,(void *)iterate_bundleforeach,NULL);

	argc1=bundle_export_to_argv(b,&argv1);	
	assert(0<argc1);
	b_new=bundle_import_from_argv(argc1,argv1);
	assert(NULL != b_new);

	//printf("\nimported bundle");
	bundle_foreach(b_new,(void *)iterate_bundleforeach,NULL);


	assert(0==bundle_free(b));
	assert(0==bundle_free(b_new));
	return SUCCESS;
}

int  TC_bundle_dup_print()
{

	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_dup_print");
	bundle *b=NULL;
	char **array=NULL;
	char buf[10]={0,};
	int no_of_ele=10;
	int i=0;
	bundle *dup_b=NULL;

	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));

	array=(char **)malloc(no_of_ele*sizeof(char *));
	for (i=0;i<no_of_ele;i++)
	{
		sprintf(buf,"arr_val%d",i);
		*(array+i)=(char *)malloc(sizeof(char *)*strlen(buf));
		strcpy(*(array+i),buf);
	}
	assert(0==bundle_add_str_array(b,"k3", (const char **)array, no_of_ele));


	//printf("\noriginal bundle  ");
	bundle_foreach(b,(void *)iterate_bundleforeach,NULL);

	dup_b = bundle_dup(b);
	assert(NULL != dup_b);
	//printf("\n duplicated bundle ");
	bundle_foreach(dup_b,(void *)iterate_bundleforeach,NULL);


	assert(0==bundle_free(b));
	assert(0==bundle_free(dup_b));
	return SUCCESS;
}
int  TC_bundle_encode_decode_print()
{

	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_encode_decode_print");
	bundle *b=NULL;
	char **array=NULL;
	char buf[10]={0,};
	int no_of_ele=10;
	int i=0;
	int len=-1;
	bundle_raw *bundle_data;
	bundle *dec_b=NULL;

	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));

	array=(char **)malloc(no_of_ele*sizeof(char *));
	for (i=0;i<no_of_ele;i++)
	{
		sprintf(buf,"arr_val%d",i);
		*(array+i)=(char *)malloc(sizeof(char *)*strlen(buf));
		strcpy(*(array+i),buf);
	}
	assert(0==bundle_add_str_array(b,"k3", (const char **)array, no_of_ele));


	//printf("\noriginal bundle  ");
	bundle_foreach(b,(void *)iterate_bundleforeach,NULL);

	assert(0==bundle_encode(b,&bundle_data,&len));
	dec_b=bundle_decode(bundle_data,len);
	assert(NULL != dec_b);

	//printf("\n decoded bundle ");
	bundle_foreach(dec_b,(void *)iterate_bundleforeach,NULL);

	assert(0==bundle_free(b));
	assert(0==bundle_free(dec_b));
	return SUCCESS;
}

int TC_bundle_export_to_argv_invalid()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_export_to_argv_invalid ");
	bundle *b_null=NULL;
	int argc1=0;
	char **argv1=NULL;

/*CASE1:INVALID b= null,FIXED : segmentation fault in(put check for bundle=null) bundle_foreach (b=0x0, iter=0x4001322c <_iter_export_to_argv>, user_data=0xbead0a24)*/	
	argc1=bundle_export_to_argv(b_null,&argv1);	
	return SUCCESS;

}
int  TC_bundle_import_from_argv_invalid()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_import_from_argv_invalid ");
	bundle *b_new=NULL;
	int argc1=0;
	char **argv1=NULL;


/* INVALID:  passing invalid arguments -not exported/encoded  */
/*CASE1: argc1=0 */
	argc1=0;
	argv1 = calloc(argc1+1,sizeof(char*));
	b_new=bundle_import_from_argv(argc1,argv1);
	assert(NULL != b_new);
	assert(0 == bundle_get_count(b_new));
	bundle_foreach(b_new,(void *)iterate_bundleforeach,NULL);
	assert(0==bundle_free(b_new));

/*CASE2:  argv1 =NULL--FIXED : segmentation fault-put check in code*/
	argc1=2;
	argv1=NULL;
	b_new=bundle_import_from_argv(argc1,argv1);
	//printf("\n cannot form bundle");
	assert(NULL == b_new);
/*CASE3: argc1=4 /argv1!=NULL but all arguments value= null */
	argc1=4;
//	argv1=calloc(1,sizeof(char*));  //sigsegv - atleast no of elements= arc1 should be allocated memory properly.
	argv1=calloc(5,sizeof(char*));
	b_new=bundle_import_from_argv(argc1,argv1);
	assert(NULL != b_new);
	assert(0 == bundle_get_count(b_new));
	assert(0==bundle_free(b_new));
/*CASE5: passing even no of arguments-[max=4] 6 arguments passed-no of bundle elements must be 2.*/
	argc1=4;
	argv1=NULL;
	argv1=calloc(5,sizeof(char*));
	argv1[0]="prog_name";
	argv1[1]="k2";
	argv1[2]="v2";
	argv1[3]="k3";
	argv1[4]="v3";
	argv1[5]="k4";//add extra- should not get added to bundle
	argv1[6]="v4";//add extra- should not get added to bundle
	b_new=bundle_import_from_argv(argc1,argv1);
	assert(NULL != b_new);
	//printf("\n no of elements in imported bundle is%d", bundle_get_count(b_new));
	assert(2 == bundle_get_count(b_new));
	bundle_foreach(b_new,(void *)iterate_bundleforeach,NULL);
	assert(0==bundle_free(b_new));
/*CASE6: passing odd no of arguments- [max=4]3 arguments passed-no of bundle elements will be 1.*/
	argc1=4;
	argv1= NULL;
	argv1=calloc(5,sizeof(char*));
	argv1[0]="prog_name";
	argv1[1]="k4";
	argv1[2]="v4";
	argv1[3]="k5";
	b_new=bundle_import_from_argv(argc1,argv1);
	assert(NULL != b_new);
	//printf("\n no of elements in imported bundle is%d", bundle_get_count(b_new));
	//bundle_foreach(b_new,(void *)iterate_bundleforeach,NULL);
	assert(1 == bundle_get_count(b_new));
	assert(0==bundle_free(b_new));
	return SUCCESS;
}
int TC_bundle_export_import_cases()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_export_import_cases ");
	bundle *b=NULL;
	bundle *b_new=NULL;
	int argc1=0;
	char **argv1=NULL;

	b=bundle_create();
	assert(NULL != b);


/*CASE1:bundle created succesfully- no keyval pair added ||return value of bundle_export_to_argv() >=2 in success and -1 in failure. */	
	argc1=bundle_export_to_argv(b,&argv1);	
	//printf("\nargc is %d",argc1);
	assert(argc1>0);

	b_new=bundle_import_from_argv(argc1,argv1);
	assert(NULL != b_new);
	assert(0 == bundle_get_count(b_new));
	/*reset values*/
	assert(0==bundle_free(b_new));
	argc1=0;
	argv1=NULL; 

/*CASE2: adding elements to bundle and then export-import*/
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));
	assert(0 == bundle_add(b, "k3", "val3"));

	argc1=bundle_export_to_argv(b,&argv1);	
	//printf("\nargc is %d",argc1);
	assert(argc1>0);
	assert(argc1==8); /*argc1 = 2*no of bundle elements +2 = actual count of argv [last null terminator not counted in argc1]*/
	assert(argv1!=NULL);
	assert(argv1[0]==NULL);
	assert(argv1[argc1]==NULL);/*last element must be NULL */

	b_new=bundle_import_from_argv(argc1,argv1);
	assert(NULL != b_new);
	assert(3 == bundle_get_count(b_new));
	assert(0== strcmp(bundle_get_val(b_new,"k1"),bundle_get_val(b,"k1")));
	assert(0== strcmp(bundle_get_val(b_new,"k2"),bundle_get_val(b,"k2")));
	assert(0== strcmp(bundle_get_val(b_new,"k3"),bundle_get_val(b,"k3")));
	bundle_foreach(b_new,(void *)iterate_bundleforeach,NULL);


	assert(0==bundle_free(b_new));
	assert(0==bundle_free(b));
	return SUCCESS;

}
int TC_bundle_add()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE : TC_bundle_add");
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);

	bundle *b2=NULL;

	assert(0 == bundle_add(b, "key1", "val1"));
	assert(0 == bundle_add(b, "key2", "val2"));
/*duplicate key */
	assert(0 != bundle_add(b, "key1", "val123"));
	//printf("\n 1 %s",strerror(errno));
	assert(EPERM == errno );
/*Null key */
	assert(0 != bundle_add(b, NULL, "val123"));
	//printf("\n 2 %s",strerror(errno));
	assert(EKEYREJECTED == errno );
/*Empty(string) key */
	assert(0 != bundle_add(b,"","val123"));
	//printf("\n 2-empty key %s",strerror(errno));
	assert(EKEYREJECTED == errno );
/*Null value */
	assert(0 != bundle_add(b, "key3", NULL));
	//printf("\n 3 %s",strerror(errno));
	assert(EINVAL == errno );
/*Null bundle */
	assert(0 != bundle_add(NULL,"key4", "val123"));
	//printf("\n 4 %s",strerror(errno));
	assert(EINVAL == errno );
/*Null bundle/value - duplicate key */
	assert(0 != bundle_add(NULL,"key4", NULL));
	//printf("\n 5 %s",strerror(errno));
	assert(EINVAL == errno );
/*Null value - duplicate key */
	assert(0 != bundle_add(b,"key4", NULL));
	//printf("\n 6 %s",strerror(errno));
	assert(EINVAL == errno );
/*get value for given key */
	assert(0==strcmp("val1",bundle_get_val(b,"key1")));
	assert(0==strcmp("val2",bundle_get_val(b,"key2")));
/*get count*/
	assert(2==bundle_get_count(b));
/* add one more key-val */
	assert(0 == bundle_add(b, "k3", "v3"));
/*get count again - should be incremented*/
	assert(3 == bundle_get_count(b));
/*add same key-val to different bundle- b2 */

	b2=bundle_create();
	assert(NULL != b2);
	assert(0 == bundle_add(b2, "key1", "val1"));
	assert(0 == bundle_add(b2, "key2", "v2"));
	assert(0==strcmp("v2",bundle_get_val(b2,"key2")));
	assert(2==bundle_get_count(b2));
/*Empty(string) value- allowed */
	assert(0 == bundle_add(b2,"key3",""));
/*Bundle_add without prior bundle_create */
	bundle *b3=NULL;
	assert(0 != bundle_add(b3, "k1", "v1"));
	assert(EINVAL == errno );
	return SUCCESS;
}
int TC_bundle_get_val()
{
	//printf("\n ====================================================================\n");
	//printf("\n TEST CASE :TC_bundle_get_val");
	bundle *b=NULL;
	bundle *b1=NULL;
	b=bundle_create();
	assert(NULL != b);
	assert(0 == bundle_add(b, "key1", "val1"));
	assert(0 == bundle_add(b, "key2", "val2"));

	assert(0==strcmp("val1",bundle_get_val(b,"key1")));
	assert(0==strcmp("val2",bundle_get_val(b,"key2")));
/*wrong key passed as input  */
	assert(NULL==bundle_get_val(b,"key4"));
	assert(ENOKEY == errno );
/*bundle = NULL */
	assert(NULL==bundle_get_val(NULL,"key4"));
	assert(EINVAL == errno );
/*bundle = not created */
	assert(NULL==bundle_get_val(b1,"key4"));
	assert(EINVAL == errno );
/*key = NULL, bundle = proper */
	assert(NULL==bundle_get_val(b,NULL));
	assert(EKEYREJECTED == errno );
/*pass string instead of bundle -sigsegv */
//	assert(NULL==bundle_get_val("bundle","key1"));
//	assert(EINVAL == errno );

	assert(0==bundle_free(b));
	return SUCCESS;

}
#if 0
int TC_bundle_add_byte()
{
/*pass str="aa" instead of byte="aa\0"*/
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE: TC_bundle_add_byte");
	bundle *b=NULL;
	bundle *b1=NULL;
	b=bundle_create();
	assert(NULL != b);
/*valid cases */
	char *data_1byte ="bbbbb";
	char *data_2byte ="aaaa";
	assert(0 == bundle_add_byte(b, "k4", (void *)data_1byte, strlen(data_1byte)+1));
	assert(0==bundle_add_byte(b, "k5", (void *)data_2byte, strlen(data_2byte)+1));
/*duplicate value */
	assert(0==bundle_add_byte(b, "k7", (void *)data_2byte, strlen(data_2byte)+1));
/*value and size NULL */
	assert(0==bundle_add_byte(b, "k8",NULL,NULL));
/*value = NULL, size not NULL */
	assert(0==bundle_add_byte(b, "k9",NULL,9));

/*invalid cases */
/*bundle = NULL */
	assert(0!=bundle_add_byte(b1, "k5", (void *)data_2byte,(size_t)strlen(data_2byte)+1));
	assert(EINVAL == errno );
/*duplicate key */
	 assert(0!=bundle_add_byte(b, "k5","abcd",4));
	assert(EPERM == errno);
/*empty key */
	assert(0!=bundle_add_byte(b,"","lion",4));
	assert(EKEYREJECTED == errno );
	assert(0==bundle_free(b));
	return SUCCESS;
}
#endif
//////
int TC_bundle_add_str_array()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE :TC_bundle_add_str_array");
	const char *arr[]={"v1","v2","v3","v4"};
	bundle *b=NULL;
	bundle *b1=NULL;
	b=bundle_create();
	assert(NULL != b);

	assert(0==bundle_add_str_array(b,"k1",&arr,4));


/*bundle= NULL - fail */
	assert(0!=bundle_add_str_array(b1,"key1",NULL,6));
	assert(EINVAL == errno );
/*duplicate key  - fail */
	assert(0!=bundle_add_str_array(b,"k1",NULL,6));
	assert(EPERM == errno );
/*empty key  - fail */
	assert(0!=bundle_add_str_array(b,"",NULL,6));
	assert(EKEYREJECTED == errno );
/*No of elements =  NULL  - success */
	assert(0==bundle_add_str_array(b,"k3",NULL,NULL));
/*add new array with a different key and 6 elements -success */
	assert(0==bundle_add_str_array(b,"k4",NULL,6));

	assert(0==bundle_free(b));
	return SUCCESS;
}
///////
#if 0
int TC_bundle_add_byte_array()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE :TC_bundle_add_byte_array");
	bundle *b=NULL;
	bundle *b1=NULL;
	b=bundle_create();
	assert(NULL != b);
/* cases */
	assert(0==bundle_add_byte_array(b,"k1",NULL,4));
	assert(0==bundle_set_byte_array_element(b,"k1",0,"ccc\0",4));
	assert(0==bundle_set_byte_array_element(b,"k1",1,"dd\0",3));
	assert(0==bundle_set_byte_array_element(b,"k1",2,"eee\0",4));
/*try to add second array element again- over writing not allowed-fail */
	assert(0!=bundle_set_byte_array_element(b,"k1",1,"new\0",4));
	assert(EINVAL == errno );
/*add fifth element - fail */
	assert(0!=bundle_set_byte_array_element(b,"k1",4,"gggg\0",5));
	assert(EINVAL == errno );
/*bundle= NULL - fail */
	assert(0!=bundle_add_byte_array(b1,"key1",NULL,6));
	assert(EINVAL == errno );
/*duplicate key  - fail */
	assert(0!=bundle_add_byte_array(b,"k1",NULL,6));
	assert(EPERM == errno );
/*empty key  - fail */
	assert(0!=bundle_add_byte_array(b,"",NULL,6));
	assert(EKEYREJECTED == errno );
/*No of elements =  NULL  - success */
	assert(0==bundle_add_byte_array(b,"k3",NULL,NULL));
/*add new array with a different key and 6 elements -success */
	assert(0==bundle_add_byte_array(b,"k2",NULL,6));
/* add fifth element- 1st/2nd/3rd/4th not yet added. */
	assert(0==bundle_set_byte_array_element(b,"k2",4,"ccc\0",4));
/* bundle = NULL */
	assert(0!=bundle_set_byte_array_element(b1,"k2",0,"ccc\0",4));
	assert(EINVAL == errno );
/* key = NULL */
	assert(0!=bundle_set_byte_array_element(b,NULL,1,"gggg\0",5));
	assert(EKEYREJECTED == errno );
/* pass string instead of byte in value */
	assert(0==bundle_set_byte_array_element(b,"k2",3,"hello",6));
	assert(0==bundle_set_byte_array_element(b,"k2",5,"world",6));/*note size = size of string+ 1 (for \0) -- if \0 not added then juck characters appear while encode- decode */

//	bundle_foreach(b,(void *)iterate_bundleforeach,NULL);
	assert(0==bundle_free(b));
	return SUCCESS;

}
int TC_bundle_get_byte()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE :TC_bundle_get_byte");
	bundle *b=NULL;
	const char *arr[]={"v1","v2","v3"};
	size_t len= -1;
	void* ret_byte;
	b=bundle_create();
	assert(NULL != b);
 /* add elements */
	assert(0 == bundle_add_byte(b, "k1","aa\0",3));
	assert(0 == bundle_add_byte(b, "k2","bb\0",3));

	assert(0 == bundle_add(b, "key1", "val1"));
	assert(0==bundle_add_str_array(b,"key2",arr,3));
	assert(0==bundle_add_byte_array(b,"key3",NULL,4));
	assert(0==bundle_set_byte_array_element(b,"key3",0,"ccc\0",4));
/*valid tests */
	assert(0==bundle_get_byte(b,"k1",&ret_byte,&len));
	assert(0==strcmp("aa",ret_byte));
	assert(3==len);
	assert(0==bundle_get_byte(b,"k2",&ret_byte,&len));
	assert(0==strcmp("bb",ret_byte));
	assert(3==len);
	assert(0==bundle_get_byte(b,"k2",NULL,&len));
	assert(3==len);
	assert(0==bundle_get_byte(b,"k2",NULL,NULL));
/*invalid tests */
	/* bundle= NULL */
	assert(0!=bundle_get_byte(NULL,"k1",&ret_byte,&len));
	assert(EINVAL == errno );
	/*key = empty */
	assert(0!=bundle_get_byte(b,"",&ret_byte,&len));
	assert(ENOKEY == errno );
	/*key does not exist */
	assert(0!=bundle_get_byte(b,"k3",&ret_byte,&len));
	assert(ENOKEY == errno );
	/*key = NULL */
	assert(0!=bundle_get_byte(b,NULL,&ret_byte,&len));
	assert(EKEYREJECTED == errno );
	/*pass the key for str type bundle-operation not supported */
	assert(0!=bundle_get_byte(b,"key1",&ret_byte,&len));
	assert(ENOTSUP == errno );
	/*pass the key for array type bundle */
	assert(0!=bundle_get_byte(b,"key2",&ret_byte,&len));
	assert(ENOTSUP == errno );
	assert(0!=bundle_get_byte(b,"key3",&ret_byte,&len));
	assert(ENOTSUP == errno );

	assert(0==bundle_free(b));
	return SUCCESS;
}
#endif
/*API prototype changed */
#if 0
int TC_bundle_get_str_array()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE :TC_bundle_get_str_array");
	bundle *b=NULL;
	const char *arr[]={"v1","v2","v3"};
	char **str_arr=NULL;
        int len = -1;
	b=bundle_create();
	assert(NULL != b);
/* add elements */
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0==bundle_add_str_array(b,"k2",arr,3));
	assert(0 == bundle_add_byte(b, "k3","hello\0",6));
	assert(0==bundle_add_byte_array(b,"k4",NULL,4));
	assert(0==bundle_set_byte_array_element(b,"k4",0,"ccc\0",4));
/* valid case test */
	assert(0==bundle_get_str_array(b,"k2",&str_arr,&len));
	assert(0==strcmp("v1",str_arr[0]));
	assert(0==strcmp("v2",str_arr[1]));
	assert(3==len);
	assert(3== (strlen(str_arr[0])+1));
	assert(3== (strlen(str_arr[1])+1));

	assert(0==bundle_get_str_array(b,"k2",NULL,&len));
	assert(3==len);
	assert(0==bundle_get_str_array(b,"k2",NULL,NULL));
/* invalid cases test */
	/*bundle= null */
	assert(0!=bundle_get_str_array(NULL,"k2",&str_arr,&len));
	assert(EINVAL == errno );
	/*key = empty */
	assert(0!=bundle_get_str_array(b,"",&str_arr,&len));
	assert(ENOKEY == errno );
	/*invalid key */
	assert(0!=bundle_get_str_array(b,"key1",&str_arr,&len));
	assert(ENOKEY == errno );
	/*key = NULL */
	assert(0!=bundle_get_str_array(b,NULL,&str_arr,&len));
	assert(EKEYREJECTED == errno );
	/*key for invalid type of element */
	assert(0!=bundle_get_str_array(b,"k1",&str_arr,&len));
	assert(ENOTSUP == errno );
	assert(0!=bundle_get_str_array(b,"k3",&str_arr,&len));
	assert(ENOTSUP == errno );
	assert(0!=bundle_get_str_array(b,"k4",&str_arr,&len));
	assert(ENOTSUP == errno );

	assert(0==bundle_free(b));
	return SUCCESS;
}
#endif
#if 1
int TC_bundle_get_str_array()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE :TC_bundle_get_str_array");
	bundle *b=NULL;
	const char *arr[]={"v1","v2","v3"};
	const char **str_arr=NULL;
	const char **str_arr1=NULL;
	const char **str_arr2=NULL;
        int len = -1;
	b=bundle_create();
	assert(NULL != b);
/* add elements */
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0==bundle_add_str_array(b,"k2",arr,3));
/* valid case test */
	str_arr=bundle_get_str_array(b,"k2",&len);
	/*printf("\nreturned array ele 1 is %s",str_arr[0]);
	printf("\nreturned array ele 2 is %s",str_arr[1]);
	printf("\nreturned array ele 3 is %s",str_arr[2]);*/
	assert(3==len);
	//assert(0==strcmp("v1",str_arr[0]));
	str_arr1=bundle_get_str_array(b,"k2",&len);
	assert(3==len);
	str_arr2=bundle_get_str_array(b,"k2",NULL);
/* invalid cases test */
	/*bundle= null */
	assert(NULL ==bundle_get_str_array(NULL,"k2",&len));
	assert(EINVAL == errno );
	/*key = empty */
	assert(NULL==bundle_get_str_array(b,"",&len));
	assert(ENOKEY == errno );
	/*invalid key */
	assert(NULL==bundle_get_str_array(b,"key1",&len));
	assert(ENOKEY == errno );
	/*key = NULL */
	assert(NULL==bundle_get_str_array(b,NULL,&len));
	assert(EKEYREJECTED == errno );
	/*key for invalid type of element */
	assert(NULL==bundle_get_str_array(b,"k1",&len));
	assert(ENOTSUP == errno );
	assert(0==bundle_free(b));
	return SUCCESS;
}
#endif
#if 0
int TC_bundle_get_byte_array()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE :TC_bundle_get_byte_array");
	bundle *b=NULL;
	const char *arr[]={"v1","v2","v3"};
	void **byte_arr=NULL;
        unsigned int len = -1;
	unsigned int *ele_size= NULL;
	b=bundle_create();
	assert(NULL != b);
/* add elements */
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0==bundle_add_str_array(b,"k2",arr,3));
	assert(0 == bundle_add_byte(b, "k3","hello\0",6));
	assert(0==bundle_add_byte_array(b,"k4",NULL,4));
	assert(0==bundle_set_byte_array_element(b,"k4",0,"ccc\0",4));
	assert(0==bundle_set_byte_array_element(b,"k4",1,"dddd\0",5));
/* valid case test */
	assert(0==bundle_get_byte_array(b,"k4",&byte_arr,&len,&ele_size));
	assert(0==strcmp("ccc",byte_arr[0]));
	assert(0==strcmp("dddd",byte_arr[1]));
	assert(4==len);
	assert(4== (strlen(byte_arr[0])+1));
	assert(5== (strlen(byte_arr[1])+1));

	assert(0==bundle_get_byte_array(b,"k4",NULL,&len,&ele_size));
	assert(4==len);
	assert(0==bundle_get_byte_array(b,"k4",NULL,NULL,&ele_size));
/* invalid cases test */
	/*bundle= null */
	assert(0!=bundle_get_byte_array(NULL,"k4",&byte_arr,&len,&ele_size));
	assert(EINVAL == errno );
	/*key = empty */
	assert(0!=bundle_get_byte_array(b,"",&byte_arr,&len,&ele_size));
	assert(ENOKEY == errno );
	/*invalid key */
	assert(0!=bundle_get_byte_array(b,"key1",&byte_arr,&len,&ele_size));
	assert(ENOKEY == errno );
	/*key = NULL */
	assert(0!=bundle_get_byte_array(b,NULL,&byte_arr,&len,&ele_size));
	assert(EKEYREJECTED == errno );
	/*key for invalid type of element */
	assert(0!=bundle_get_byte_array(b,"k1",&byte_arr,&len,&ele_size));
	assert(ENOTSUP == errno );
	assert(0!=bundle_get_byte_array(b,"k2",&byte_arr,&len,&ele_size));
	assert(ENOTSUP == errno );
	assert(0!=bundle_get_byte_array(b,"k3",&byte_arr,&len,&ele_size));
	assert(ENOTSUP == errno );

	assert(0==bundle_free(b));
	return SUCCESS;
}
#endif
int TC_bundle_get_type()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_get_type");
	char **array=NULL;
	char buf[10]={0,};
	int no_of_ele=10;
	int i=0;
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);

	/* add elements */
	assert(0 == bundle_add(b, "k1", "val1"));
	array=(char **)malloc(no_of_ele*sizeof(char *));
	for (i=0;i<no_of_ele;i++)
	{
		sprintf(buf,"arr_val%d",i);
		*(array+i)=(char *)malloc(sizeof(char *)*strlen(buf));
		strcpy(*(array+i),buf);
	}
	assert(0==bundle_add_str_array(b,"k2", (const char **)array, no_of_ele));

	/*valid case test for bundle_get_type */
	assert(BUNDLE_TYPE_STR==bundle_get_type(b,"k1"));
	assert(BUNDLE_TYPE_STR_ARRAY==bundle_get_type(b,"k2"));
	
	/* in valid cases test */
	assert(BUNDLE_TYPE_NONE==bundle_get_type(b,"k5"));
	assert(ENOKEY == errno);
	assert(BUNDLE_TYPE_NONE==bundle_get_type(b,NULL));
	assert(ENOKEY == errno);
	assert(BUNDLE_TYPE_NONE==bundle_get_type(b,""));
	assert(ENOKEY == errno);
	assert(0==bundle_free(b));
	return SUCCESS;
}
void iterate_is_keyval_type_array(const char *key,const int type, bundle_keyval_t *kv, void *data)
{
	
	if(0==strcmp(key,"k1"))
	assert(0 == bundle_keyval_type_is_array(kv));
	else if(0==strcmp(key,"k2"))
	assert(1 == bundle_keyval_type_is_array(kv));
	else if(0==strcmp(key,"k3"))
	assert(0  == bundle_keyval_type_is_array(kv));
	else if(0==strcmp(key,"k4"))
	assert(1  == bundle_keyval_type_is_array(kv));
	return;
}
int TC_bundle_keyval_type_is_array()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE : TC_bundle_keyval_type_is_array");
	bundle *b=NULL;
	const char *arr[]={"v1","v2","v3"};
	b=bundle_create();
	assert(NULL != b);
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0==bundle_add_str_array(b,"k2",arr,3));

	bundle_foreach(b,(void *)iterate_is_keyval_type_array,NULL);
	return SUCCESS;

}
void iterate_keyval_get_basicval(const char *key,const int type, bundle_keyval_t *kv, void *data)
{
	void **val=NULL;
	size_t len =-1;	
	if(0==strcmp(key,"k1"))
	assert(0 == bundle_keyval_get_basic_val(kv,val,&len));
	else if(0==strcmp(key,"k2"))
	assert(0!= bundle_keyval_get_basic_val(kv,val,&len));
	else if(0==strcmp(key,"k3"))
	assert(0  == bundle_keyval_get_basic_val(kv,val,&len));
	else if(0==strcmp(key,"k4"))
	assert(0!= bundle_keyval_get_basic_val(kv,val,&len));
	return;
}
int TC_bundle_keyval_get_basic_val()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE:TC_bundle_keyval_get_basic_val");
	const char *arr[]={"v1","v2","v3"};
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0==bundle_add_str_array(b,"k2",arr,3));

	bundle_foreach(b,(void *)iterate_keyval_get_basicval,NULL);
	return SUCCESS;
}

void iterate_is_keyval_get_arrayval(const char *key,const int type, bundle_keyval_t *kv, void *data)
{
	void ***array_val=NULL;
	unsigned int *array_len =NULL;	
	size_t **ele_size=NULL;
	if(0==strcmp(key,"k1"))
	assert(0 != bundle_keyval_get_array_val(kv,array_val,array_len,ele_size));
	else if(0==strcmp(key,"k2"))
	assert(0 == bundle_keyval_get_array_val(kv,array_val,array_len,ele_size));
	else if(0==strcmp(key,"k3"))
	assert(0 != bundle_keyval_get_array_val(kv,array_val,array_len,ele_size));
	else if(0==strcmp(key,"k4"))
	assert(0 == bundle_keyval_get_array_val(kv,array_val,array_len,ele_size));
	return;
}
int TC_bundle_keyval_get_array_val()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE :TC_bundle_keyval_get_array_val");
	const char *arr[]={"v1","v2","v3"};
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0==bundle_add_str_array(b,"k2",arr,3));

	bundle_foreach(b,(void *)iterate_is_keyval_get_arrayval,NULL);
	return SUCCESS;
}

int TC_bundle_encode()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_encode");
	bundle *b=NULL;
	bundle_raw *bundle_data;
	int len = -1;
/*bundle not created-encode - b= NULL*/
	assert(0!=bundle_encode(b,&bundle_data,&len));
/*bundle created- no element added -encode */
	b=bundle_create();
	assert(NULL != b);
	assert(0==bundle_encode(b,&bundle_data,&len));
/*2 elements added - encode */
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));

	assert(0==bundle_encode(b,&bundle_data,&len));
	assert(0==bundle_free(b));
	return SUCCESS;
}
int TC_bundle_decode()
{
	//printf("\n ====================================================================\n");
	//printf("\nTESTCASE: TC_bundle_decode");
	bundle *b=NULL;
	bundle_raw *bundle_data=NULL;
	bundle *dec_b=NULL;
	int len = -1;
/*invalid bundle_data and len values- not encoded */
	dec_b=bundle_decode(bundle_data,len);
	assert(NULL == dec_b);
	assert(EINVAL == errno);
/*invalid bundle_data and len values- not encoded-any random string passed -FIXED */
	dec_b=bundle_decode("not_encodeddata",len);
	assert(NULL == dec_b);
//	assert(EINVAL == errno);
/*encode- decode but incorrect value for length  */
	b=bundle_create();
	assert(NULL != b);
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));
	assert(0==bundle_encode(b,&bundle_data,&len));
	dec_b=bundle_decode(bundle_data,9);
	assert(NULL != dec_b);
	bundle_foreach(dec_b,(void *)iterate_bundleforeach,NULL);
	assert(0==bundle_free(b));
	assert(0==bundle_free(dec_b));
	return SUCCESS;
}
#if 0
void TC_bundle_free_encoded_raw_data()
{
	printf("\n ====================================================================\n");
	printf("\n TESTCASE: TC_bundle_free_encoded_raw_data");
	bundle_raw *bundle_data=NULL;
	bundle_raw *bundle_data_null=NULL;
	int len =-1;
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));
/* valid case */
	assert(0==bundle_encode(b,&bundle_data,&len));
	assert(0==bundle_free_encoded_rawdata(bundle_data));
/* FIXED -sigabrt trying to free twice */
if(0!=bundle_free_encoded_rawdata(bundle_data))
	printf("\nalready freed");
	assert(0!=bundle_free_encoded_rawdata(bundle_data));
	assert(0!=bundle_free_encoded_rawdata(bundle_data));
/*trying to free bundle_data= NULL */
	assert(0!=bundle_free_encoded_rawdata(bundle_data_null));

	assert(0==bundle_free(b));
}
#endif
int TC_bundle_free_encoded_raw_data()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE: TC_bundle_free_encoded_raw_data");
	bundle_raw *bundle_data=NULL;
	bundle_raw *bundle_data_null=NULL;
	int len =-1;
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));
/* valid case */
	assert(0==bundle_encode(b,&bundle_data,&len));
	assert(0==bundle_free_encoded_rawdata(&bundle_data));
/* FIXED -sigabrt trying to free twice */
if(0!=bundle_free_encoded_rawdata(&bundle_data))
	//printf("\nalready freed");
	assert(0!=bundle_free_encoded_rawdata(&bundle_data));
	assert(0!=bundle_free_encoded_rawdata(&bundle_data));
/*trying to free bundle_data= NULL */
	assert(0!=bundle_free_encoded_rawdata(&bundle_data_null));

	assert(0==bundle_free(b));
	return SUCCESS;
}
int TC_bundle_free_exported_argv()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE :TC_bundle_free_exported_argv");
	int argc1=0;
	char **argv1=NULL;
	int argc2=0;
	char **argv2=NULL;
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));
	argc1=bundle_export_to_argv(b,&argv1);	
	assert(0<argc1);
/*valid args */
	assert(0 ==bundle_free_exported_argv(argc1,&argv1));
/*invalid args- argc =0,argv = NULL */
	assert(0 !=bundle_free_exported_argv(argc2,&argv2));
/*invalid args- argc =8 , argv= NULL */
	assert(0 !=bundle_free_exported_argv(8,&argv2));
/* FIXED double free -sigabrt */
	assert(0 !=bundle_free_exported_argv(argc1,&argv1));
	return SUCCESS;
}

#if 0
void TC_bundle_free_exported_argv()
{
	printf("\n ====================================================================\n");
	printf("\n TESTCASE :TC_bundle_free_exported_argv");
	int argc1=0;
	char **argv1=NULL;
	int argc2=0;
	char **argv2=NULL;
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);

	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0 == bundle_add(b, "k2", "val2"));
	argc1=bundle_export_to_argv(b,&argv1);	
	assert(0<argc1);
/*valid args */
	assert(0 ==bundle_free_exported_argv(argc1,argv1));
/*invalid args- argc =0,argv = NULL */
	assert(0 !=bundle_free_exported_argv(argc2,argv2));
/*invalid args- argc =8 , argv= NULL */
	assert(0 !=bundle_free_exported_argv(8,argv2));
/* FIXED double free -sigabrt */
//	assert(0 !=bundle_free_exported_argv(argc1,argv1));
}
#endif
int TC_bundle_del()
{
	//printf("\n ====================================================================\n");
	//printf("\n TESTCASE :TC_bundle_del");
	const char *arr[]={"v1","v2","v3"};
	bundle *b=NULL;
	b=bundle_create();
	assert(NULL != b);
/* add elements */
	assert(0 == bundle_add(b, "k1", "val1"));
	assert(0==bundle_add_str_array(b,"k2",arr,3));
	assert(2 == bundle_get_count(b));
/*valid case tests */
	assert(0 == bundle_del(b, "k1"));
	assert(NULL == bundle_get_val(b, "k1"));
	assert(ENOKEY == errno);
	assert(1 == bundle_get_count(b));

	assert(0 == bundle_del(b, "k2"));
	assert(0 == bundle_get_count(b));
/*invalid case tests */
	/*bundle = NULL */
	assert(0 != bundle_del(NULL, "key"));
	assert(EINVAL == errno);
	/*key =NULL */

	assert(0 != bundle_del(b, NULL));
	assert(EKEYREJECTED == errno);
	/*key = empty */
	assert(0 != bundle_del(b, ""));
	assert(EKEYREJECTED == errno);
	/*key does not exist- invalid key */
	assert(0 != bundle_del(b, "key"));
	assert(ENOKEY == errno);
	/* invalid key-again pass the same key */
	assert(0 != bundle_del(b, "k1"));
	assert(ENOKEY == errno);

	assert(0 == bundle_get_count(b));
	assert(0==bundle_free(b));
	return SUCCESS;
}

int
main(int argc, char **argv)
{
	/*print the results */
	printf("\n************************************************************************\n");
	printf("\n Test Suite Report \n");
	printf("\n Output : 1 = Success , On failure the respective test case aborts and prints the assert log \n");
	printf("\n************************************************************************\n");
	printf("TC ID\t\tRESULT\n");

	pid_t tc_bundle_foreach = fork();
	if (tc_bundle_foreach  == 0)
	{
		 ret_bundle_foreach = TC_bundle_foreach();
		printf("TC_bundle_foreach\t%d\n",ret_bundle_foreach);
		exit(1);	
	}
	waitpid(tc_bundle_foreach,NULL,WNOHANG);
	

	pid_t tc_bundle_foreach_invalid = fork();
	if (tc_bundle_foreach_invalid  == 0)
	{
		ret_bundle_foreach_invalid  =TC_bundle_foreach_invalid();
		printf("TC_bundle_foreach_invalid\t%d\n",ret_bundle_foreach_invalid);
		exit(1);	
	}
	waitpid(tc_bundle_foreach,NULL,WNOHANG);

	pid_t tc_bundle_keyval_gettype = fork();
	if ( tc_bundle_keyval_gettype  == 0)
	{
		ret_bundle_keyval_typeget = TC_bundle_keyval_get_type();
		printf("TC_bundle_keyval_get_type\t%d\n",ret_bundle_keyval_typeget );
		exit(1);	
	}
	waitpid(tc_bundle_keyval_gettype ,NULL,WNOHANG);

	pid_t tc_bundle_export_import_print = fork();
	if ( tc_bundle_export_import_print == 0)
	{
		ret_bundle_exp_imp_p = TC_bundle_export_import_print();
		printf("TC_bundle_export_import_print\t%d\n",ret_bundle_exp_imp_p);
		exit(1);	
	}
	waitpid(tc_bundle_export_import_print ,NULL,WNOHANG);

	pid_t tc_bundle_encode_decode_print = fork();
	if ( tc_bundle_encode_decode_print == 0)
	{
		ret_bundle_en_dec_p = TC_bundle_encode_decode_print();
		printf("TC_bundle_encode_decode_print\t%d\n",ret_bundle_en_dec_p);
		exit(1);	
	}
	waitpid(tc_bundle_encode_decode_print ,NULL,WNOHANG);

	pid_t tc_bundle_exp_imp_case = fork();
	if ( tc_bundle_exp_imp_case  == 0)
	{
		ret_exp_imp_cases = TC_bundle_export_import_cases();
		printf("TC_bundle_export_import_cases\t%d\n",ret_exp_imp_cases);
		exit(1);	
	}
	waitpid(tc_bundle_exp_imp_case  ,NULL,WNOHANG);

	pid_t tc_bundle_exp_invalid = fork();
	if ( tc_bundle_exp_invalid  == 0)
	{
		ret_bundle_exp_invalid  = TC_bundle_export_to_argv_invalid();
		printf("TC_bundle_export_to_argv_invalid\t%d\n",ret_bundle_exp_invalid);
		exit(1);	
	}
	waitpid(tc_bundle_exp_invalid ,NULL,WNOHANG);

	pid_t tc_bundle_imp_invalid = fork();
	if ( tc_bundle_imp_invalid  == 0)
	{
		ret_bundle_imp_invalid  = TC_bundle_import_from_argv_invalid();
		printf("TC_bundle_import_from_argv_invalid\t%d\n",ret_bundle_imp_invalid  );
		exit(1);	
	}
	waitpid(tc_bundle_imp_invalid ,NULL,WNOHANG);

	pid_t tc_bundle_add = fork();
	if ( tc_bundle_add == 0)
	{
		ret_bundle_add   = TC_bundle_add();
		printf("TC_bundle_add\t%d\n",ret_bundle_add);
		exit(1);	
	}
	waitpid(tc_bundle_add  ,NULL,WNOHANG);

	pid_t tc_bundle_get_val = fork();
	if ( tc_bundle_get_val == 0)
	{
		ret_bundle_get_val   = TC_bundle_get_val();
		printf("TC_bundle_get_val\t%d\n",ret_bundle_get_val);
		exit(1);	
	}
	waitpid( tc_bundle_get_val  ,NULL,WNOHANG);

	pid_t tc_bundle_dup = fork();
	if ( tc_bundle_dup == 0)
	{
		ret_bundle_dup   = TC_bundle_dup_print();
		printf("TC_bundle_dup_print\t%d\n",ret_bundle_dup );
		exit(1);	
	}
	waitpid( tc_bundle_dup  ,NULL,WNOHANG);

	pid_t tc_bundle_encode = fork();
	if ( tc_bundle_encode == 0)
	{
		ret_bundle_encode   = TC_bundle_encode();
		printf("TC_bundle_encode\t%d\n",ret_bundle_encode );
		exit(1);	
	}
	waitpid( tc_bundle_encode  ,NULL,WNOHANG);

	pid_t tc_bundle_decode = fork();
	if ( tc_bundle_decode == 0)
	{
		ret_bundle_decode   = TC_bundle_decode();
		printf("TC_bundle_decode\t%d\n",ret_bundle_decode );
		exit(1);	
	}
	waitpid( tc_bundle_decode  ,NULL,WNOHANG);

	pid_t tc_bundle_free_encoded = fork();
	if ( tc_bundle_free_encoded == 0)
	{
		ret_bundle_free_encoded  = TC_bundle_free_encoded_raw_data();
		printf("TC_bundle_free_encoded_raw_data\t%d\n",ret_bundle_free_encoded  );
		exit(1);	
	}
	waitpid( tc_bundle_free_encoded   ,NULL,WNOHANG);
#if 0
	pid_t tc_bundle_add_byte = fork();
	if ( tc_bundle_add_byte  == 0)
	{
		ret_bundle_add_byte   = TC_bundle_add_byte();
		printf("TC_bundle_add_byte\t%d\n",ret_bundle_add_byte   );
		exit(1);	
	}
	waitpid(tc_bundle_add_byte   ,NULL,WNOHANG);

	pid_t tc_bundle_add_byte_arr = fork();
	if ( tc_bundle_add_byte_arr  == 0)
	{
		ret_bundle_add_byte_arr   = TC_bundle_add_byte_array();
		printf("TC_bundle_add_byte_array\t%d\n",ret_bundle_add_byte_arr   );
		exit(1);	
	}
	waitpid(tc_bundle_add_byte_arr   ,NULL,WNOHANG);
#endif
	pid_t tc_bundle_get_type = fork();
	if ( tc_bundle_get_type  == 0)
	{
		ret_bundle_get_type  = TC_bundle_get_type();
		printf("TC_bundle_get_type\t%d\n",ret_bundle_get_type   );
		exit(1);	
	}
	waitpid( tc_bundle_get_type   ,NULL,WNOHANG);

	pid_t tc_keyval_is_array = fork();
	if ( tc_keyval_is_array  == 0)
	{
		ret_keyval_is_array   = TC_bundle_keyval_type_is_array();
		printf("TC_bundle_keyval_type_is_array\t%d\n",ret_keyval_is_array    );
		exit(1);	
		exit(1);	
	}
	waitpid( tc_keyval_is_array  ,NULL,WNOHANG);

	pid_t tc_keyval_get_basicval  = fork();
	if ( tc_keyval_get_basicval  == 0)
	{
		ret_keyval_get_basicval  = TC_bundle_keyval_get_basic_val();
		printf("TC_bundle_keyval_get_basic_val\t%d\n",ret_keyval_get_basicval   );
		exit(1);	
	}
	waitpid( tc_keyval_get_basicval  ,NULL,WNOHANG);
#if 0
	pid_t tc_bundle_get_byte = fork();
	if ( tc_bundle_get_byte  == 0)
	{
		ret_bundle_get_byte  = TC_bundle_get_byte();
		printf("TC_bundle_get_byte\t%d\n",ret_bundle_get_byte  );
		exit(1);	
	}
	waitpid( tc_bundle_get_byte   ,NULL,WNOHANG);

	pid_t tc_bundle_get_byte_arr = fork();
	if ( tc_bundle_get_byte_arr  == 0)
	{
		ret_bundle_get_byte_arr  = TC_bundle_get_byte_array();
		printf("TC_bundle_get_byte_array\t%d\n",ret_bundle_get_byte_arr  );
		exit(1);	
	}
	waitpid( tc_bundle_get_byte_arr   ,NULL,WNOHANG);
#endif
	pid_t tc_free_exp_argv = fork();
	if (  tc_free_exp_argv == 0)
	{
		ret_tc_free_exp_argv = TC_bundle_free_exported_argv();
		printf("TC_bundle_free_exported_argv\t%d\n",ret_tc_free_exp_argv );
		exit(1);	
	}
	waitpid( tc_free_exp_argv ,NULL,WNOHANG);

	pid_t tc_bundle_del = fork();
	if (  tc_bundle_del == 0)
	{
		ret_bundle_del = TC_bundle_del();
		printf("TC_bundle_del\t%d\n",ret_bundle_del );
		exit(1);	
	}
	waitpid( tc_bundle_del ,NULL,WNOHANG);

	pid_t tc_keyval_get_arrval  = fork();
	if ( tc_keyval_get_arrval  == 0)
	{
		ret_keyval_get_arrval  = TC_bundle_keyval_get_array_val();
		printf("TC_bundle_keyval_get_array_val\t%d\n",ret_keyval_get_arrval  );
		exit(1);	
	}
	waitpid( tc_keyval_get_arrval  ,NULL,WNOHANG);

	pid_t tc_bundle_get_str_arr = fork();
	if ( tc_bundle_get_str_arr  == 0)
	{
		ret_bundle_get_str_arr  = TC_bundle_get_str_array();
		printf("TC_bundle_get_str_array\t%d\n",ret_bundle_get_str_arr  );
		exit(1);	
	}
	waitpid( tc_bundle_get_str_arr   ,NULL,WNOHANG);

	pid_t tc_bundle_add_str_arr = fork();
	if ( tc_bundle_add_str_arr  == 0)
	{
		ret_bundle_add_str_arr  = TC_bundle_add_str_array();
		printf("TC_bundle_add_str_array\t%d\n",ret_bundle_add_str_arr   );
		exit(1);	
	}
	waitpid( tc_bundle_add_str_arr ,NULL,WNOHANG);
	
	return 0;
}
