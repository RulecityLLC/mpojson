// to workaround visual studio bug ( https://developercommunity.visualstudio.com/content/problem/93889/error-c2872-byte-ambiguous-symbol.html )
#define _HAS_STD_BYTE 0

#include "test_headers.h"
#include <mpolib/mpo_json.h>

void test_json_decode()
{
	mpo_json j;

	string s = "{ \"menu\": null }";

	MpoJsonGenericSPtr res = j.parse(s.c_str(), s.size());
	mpo_json_object *pRes = (mpo_json_object *) res.get();

	TEST_REQUIRE(pRes != 0);

	mpo_json_keypair kp = pRes->GetChildren()->front();
	TEST_CHECK_EQUAL(kp.key, L"menu");
	TEST_CHECK_EQUAL(kp.value->GetType(), MPO_JSON_NULL);

	mpo_json_null *pNull = kp.value->ToNull();

	try
	{
		void *pWillThrow = kp.value->ToString();

		// if we get this far, it means the exception did not occur
		TEST_REQUIRE(false);	// fail the test
	}
	catch (std::exception &)
	{
	}
}

TEST_CASE(json_decode)
{
	test_json_decode();
}

void test_json_encode()
{
	mpo_json j;

	string s = "{\n	\"menu\": null\n}";

	MpoJsonGenericSPtr res = j.parse(s.c_str(), s.size());
	mpo_json_object *pRes = (mpo_json_object *) res.get();

	TEST_REQUIRE(pRes != 0);

	string s1 = j.dump(res);

	TEST_CHECK_EQUAL(s, s1);
}

TEST_CASE(json_encode)
{
	test_json_encode();
}

void test_json_decode2()
{
	mpo_json j;

	string s = 
"{"
"  \"obj\": {"
"    \"num\": 7,"
"    \"str\": \"some text\","
"    \"double\": 6.4,"
"    \"bool\": true,"
"    \"bool2\": false,"
"    \"arr\": ["
"        \"1\","
"        2"
"      ]"
"  }"
"}";

	MpoJsonGenericSPtr res = j.parse(s.c_str(), s.size());
	mpo_json_object *pRes = (mpo_json_object *) res.get();

	TEST_REQUIRE(pRes != 0);

	mpo_json_keypair kp = pRes->GetChildren()->front();
	TEST_CHECK_EQUAL(kp.key, L"obj");
	TEST_REQUIRE_EQUAL(kp.value->GetType(), MPO_JSON_OBJECT);

	mpo_json_object *pObjDisc = kp.value->ToObject();

	map<wstring, MpoJsonGenericSPtr> mChildren = pObjDisc->GetChildrenAsMap();

	MpoJsonGenericSPtr val;
	val = mChildren[L"num"];
	mpo_json_int *pInt = val->ToInt();
	TEST_REQUIRE(pInt);
	TEST_CHECK_EQUAL(7, (int) pInt->GetValue());

	val = mChildren[L"str"];
	mpo_json_string *pStr = val->ToString();
	TEST_REQUIRE(pStr);
	TEST_CHECK_EQUAL(L"some text", pStr->GetValue());

	val = mChildren[L"double"];
	mpo_json_double *pD = val->ToDouble();
	TEST_REQUIRE(pD);
	TEST_CHECK(6.4 == pD->GetValue());

	val = mChildren[L"bool"];
	mpo_json_boolean *pB = val->ToBoolean();
	TEST_REQUIRE(pB);
	TEST_CHECK(pB->GetValue());

	val = mChildren[L"bool2"];
	pB = val->ToBoolean();
	TEST_REQUIRE(pB);
	TEST_CHECK(!pB->GetValue());

	val = mChildren[L"arr"];
	mpo_json_array *pArray = val->ToArray();
	TEST_REQUIRE(pArray);
	list<MpoJsonGenericSPtr> *pLstArray = pArray->GetChildren();
	
	val = pLstArray->front();
	pStr = val->ToString();
	TEST_REQUIRE(pStr);
	TEST_CHECK_EQUAL(L"1", pStr->GetValue());

	val = pLstArray->back();
	pInt = val->ToInt();
	TEST_REQUIRE(pInt);
	TEST_CHECK(2 == pInt->GetValue());

}

TEST_CASE(json_decode2)
{
	test_json_decode2();
}

void test_json_encode2()
{
	mpo_json j;

	string s = 
"{\n"
"	\"obj\": {\n"
"		\"arr\": [\n"
"			\"1\",\n"
"			2,\n"
"			0.00,\n"
"			false,\n"
"			true\n"
"		]\n"
"	}\n"
"}";

	MpoJsonGenericSPtr res = j.parse(s.c_str(), s.size());
	mpo_json_object *pRes = (mpo_json_object *) res.get();

	TEST_REQUIRE(pRes != 0);

	string s2 = j.dump(res);

	TEST_CHECK_EQUAL(s, s2);
}

TEST_CASE(json_encode2)
{
	test_json_encode2();
}

void test_json_decode_error()
{
	mpo_json j;

	string s = "{ garbage! }";

	MpoJsonGenericSPtr res = j.parse(s.c_str(), s.size());
	mpo_json_object *pRes = (mpo_json_object *) res.get();

	TEST_REQUIRE(pRes == 0);

	string strErr = j.get_last_error_msg();
	TEST_CHECK_EQUAL("Parse error: string or '}' expected near 'garbage' (line:1 column:9 position:9)", strErr);
}

TEST_CASE(json_decode_error)
{
	test_json_decode_error();
}

void test_json_encode3()
{
	mpo_json j;

	string s = 
"[\n"
"	\"1\",\n"
"	2,\n"
"	0.00,\n"
"	false,\n"
"	true,\n"
"	{\n"
"		\"key\": null\n"
"	}\n"
"]";

	MpoJsonGenericSPtr res = j.parse(s.c_str(), s.size());
	mpo_json_object *pRes = (mpo_json_object *) res.get();

	TEST_REQUIRE(pRes != 0);

	string s2 = j.dump(res);

	TEST_CHECK_EQUAL(s, s2);
}

TEST_CASE(json_encode3)
{
	test_json_encode3();
}

void test_json_encode_linefeed()
{
	mpo_json j;

	string s = 
"[\n\t\"1\\n2\\n3\\n\"\n]";

	MpoJsonGenericSPtr res = j.parse(s.c_str(), s.size());
	mpo_json_object *pRes = (mpo_json_object *) res.get();

	TEST_REQUIRE(pRes != 0);

	string s2 = j.dump(res);

	TEST_CHECK_EQUAL(s, s2);
}

TEST_CASE(json_encode_linefeed)
{
	test_json_encode_linefeed();
}
