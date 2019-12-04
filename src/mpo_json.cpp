#if defined(HAVE_CONFIG_H) || defined(HAVE_MPOLIB_CONFIG_H)
#include <mpolib/mpolib_config.h>
#endif

// only include file if header is available
#ifdef HAVE_JANSSON_H

#include <jansson.h>
#include <mpolib2/mpo_json.h>
#include <mpolib/mpo_misc.h>

void mpo_json_generic::ThrowInvalidCastException()
{
	throw runtime_error("Invalid JSON cast");
}

mpo_json_object *mpo_json_generic::ToObject() { if (GetType() != MPO_JSON_OBJECT) { ThrowInvalidCastException(); } return (mpo_json_object *) this; }
mpo_json_array *mpo_json_generic::ToArray() { if (GetType() != MPO_JSON_ARRAY) { ThrowInvalidCastException(); } return (mpo_json_array *) this; }
mpo_json_string *mpo_json_generic::ToString() { if (GetType() != MPO_JSON_STRING) { ThrowInvalidCastException(); }return (mpo_json_string *) this; }
mpo_json_int *mpo_json_generic::ToInt() { if (GetType() != MPO_JSON_INT) { ThrowInvalidCastException(); } return (mpo_json_int *) this; }
mpo_json_double *mpo_json_generic::ToDouble() { if (GetType() != MPO_JSON_DOUBLE) { ThrowInvalidCastException(); } return (mpo_json_double *) this; }
mpo_json_boolean *mpo_json_generic::ToBoolean() { if (GetType() != MPO_JSON_BOOLEAN) { ThrowInvalidCastException(); } return (mpo_json_boolean *) this; }
mpo_json_null *mpo_json_generic::ToNull() { if (GetType() != MPO_JSON_NULL) { ThrowInvalidCastException(); } return (mpo_json_null *) this; }

////////////////////////////////

MpoJsonGenericSPtr mpo_json_object::GetInstance() { return MpoJsonGenericSPtr(new mpo_json_object(), mpo_json_object::deleter()); }
mpo_json_type mpo_json_object::GetType() { return MPO_JSON_OBJECT; }
list<mpo_json_keypair> *mpo_json_object::GetChildren() { return &m_children; }

// convenience function, won't work for all cases because multiple children can share the same key (although this is discouraged by the RFC)
map<wstring, MpoJsonGenericSPtr> mpo_json_object::GetChildrenAsMap()
{
	map<wstring, MpoJsonGenericSPtr> res;
	for (list<mpo_json_keypair>::const_iterator li = m_children.begin(); li != m_children.end(); li++)
	{
		mpo_json_keypair kp = *li;
		res[kp.key] = kp.value;
	}

	return res;
}

mpo_json_object::mpo_json_object() {}
mpo_json_object::~mpo_json_object() { }
void mpo_json_object::DeleteInstance() { delete this; }

////////////////////////////////

MpoJsonGenericSPtr mpo_json_array::GetInstance() { return MpoJsonGenericSPtr(new mpo_json_array(), mpo_json_array::deleter()); }
mpo_json_type mpo_json_array::GetType() { return MPO_JSON_ARRAY; }
list<MpoJsonGenericSPtr> *mpo_json_array::GetChildren() { return &m_children; }

mpo_json_array::mpo_json_array() {}
mpo_json_array::~mpo_json_array() { }
void mpo_json_array::DeleteInstance() { delete this; }

//////////////////////////////////////////

MpoJsonGenericSPtr mpo_json_string::GetInstance(const wstring &s) { return MpoJsonGenericSPtr(new mpo_json_string(s), mpo_json_string::deleter()); }
mpo_json_type mpo_json_string::GetType() { return MPO_JSON_STRING; }
wstring mpo_json_string::GetValue() { return m_val; }

mpo_json_string::mpo_json_string(const wstring &s) : m_val(s) {}
mpo_json_string::~mpo_json_string() { }
void mpo_json_string::DeleteInstance() { delete this; }

/////////////////////////////////

MpoJsonGenericSPtr mpo_json_int::GetInstance(const MPO_INT64 &i64) { return MpoJsonGenericSPtr(new mpo_json_int(i64), mpo_json_int::deleter()); }
mpo_json_type mpo_json_int::GetType() { return MPO_JSON_INT; }
MPO_INT64 mpo_json_int::GetValue() { return m_val; }

mpo_json_int::mpo_json_int(const MPO_INT64 &i64) : m_val(i64) {}
mpo_json_int::~mpo_json_int() {}
void mpo_json_int::DeleteInstance() { delete this; }

/////////////////////////////////

MpoJsonGenericSPtr mpo_json_double::GetInstance(const double d) { return MpoJsonGenericSPtr(new mpo_json_double(d), mpo_json_double::deleter()); }
mpo_json_type mpo_json_double::GetType() { return MPO_JSON_DOUBLE; }
double mpo_json_double::GetValue() { return m_val; }

mpo_json_double::mpo_json_double(const double d) : m_val(d) {}
mpo_json_double::~mpo_json_double() {}
void mpo_json_double::DeleteInstance() { delete this; }

////////////////////////////////////

MpoJsonGenericSPtr mpo_json_boolean::GetInstance(bool b) { return MpoJsonGenericSPtr(new mpo_json_boolean(b), mpo_json_boolean::deleter()); }
mpo_json_type mpo_json_boolean::GetType() { return MPO_JSON_BOOLEAN; }
bool mpo_json_boolean::GetValue() { return m_val; }

mpo_json_boolean::mpo_json_boolean(bool b) : m_val(b) {}
mpo_json_boolean::~mpo_json_boolean() {}
void mpo_json_boolean::DeleteInstance() { delete this; }

////////////////////////////////

MpoJsonGenericSPtr mpo_json_null::GetInstance() { return MpoJsonGenericSPtr(new mpo_json_null(), mpo_json_null::deleter()); }
mpo_json_type mpo_json_null::GetType() { return MPO_JSON_NULL; }

mpo_json_null::mpo_json_null() {}
mpo_json_null::~mpo_json_null() {}
void mpo_json_null::DeleteInstance() { delete this; }

///////////////

mpo_json::mpo_json()
{
	m_RootSPtr = mpo_json_object::GetInstance();
}

void mpo_json_decode_helper(MpoJsonGenericSPtr *pDst, json_t *value);

wstring mpo_json_encode_helper(mpo_json_generic *value, int iDepth);

void do_object_decode(list<mpo_json_keypair> *pLstParent, json_t *pCurSrc)
{
	const char *key;
	json_t *value;
	mpo_json_keypair kp;

	json_object_foreach(pCurSrc, key, value)
	{
		kp.key = mpom::FromUTF8ExW(key);
		mpo_json_decode_helper(&kp.value, value);
		pLstParent->push_back(kp);
	}

}

void do_array_decode(list<MpoJsonGenericSPtr> *pLstParent, const json_t *pCurSrc)
{
	json_t *value;
	MpoJsonGenericSPtr dst;
	size_t stSize = json_array_size(pCurSrc);

	for (size_t i = 0; i < stSize; i++)
	{
		value = json_array_get(pCurSrc, i);
		mpo_json_decode_helper(&dst, value);
		pLstParent->push_back(dst);
	}
}

void mpo_json_decode_helper(MpoJsonGenericSPtr *pDst, json_t *value)
{
	int iType = json_typeof(value);
	json_type type = (json_type) iType;
	switch (type)
	{
	case JSON_STRING:
		*pDst = mpo_json_string::GetInstance(mpom::FromUTF8ExW(json_string_value(value)));
		break;
	case JSON_OBJECT:
		*pDst = mpo_json_object::GetInstance();
		{
			mpo_json_object *pObj = (*pDst)->ToObject();
			do_object_decode(pObj->GetChildren(), value);	// recursion!
		}
		break;
	case JSON_ARRAY:
		*pDst = mpo_json_array::GetInstance();
		{
			mpo_json_array *pArray = (*pDst)->ToArray();
			do_array_decode(pArray->GetChildren(), value);
		}
		break;
	case JSON_INTEGER:
		*pDst = mpo_json_int::GetInstance(json_integer_value(value));
		break;
	case JSON_REAL:
		*pDst = mpo_json_double::GetInstance(json_real_value(value));
		break;
	case JSON_TRUE:
		*pDst = mpo_json_boolean::GetInstance(true);
		break;
	case JSON_FALSE:
		*pDst = mpo_json_boolean::GetInstance(false);
		break;
	default:	// null
		*pDst = mpo_json_null::GetInstance();
		break;
	}
}

MpoJsonGenericSPtr mpo_json::parse(const char *buf, unsigned int size)
{
	MpoJsonGenericSPtr pRes;

	json_error_t err;
	json_t *pRoot = json_loadb(buf, size, 0, &err);

	int iType0;
	json_type type0;

	// if decoding failed
	if (pRoot == NULL)
	{
		// set error message
		m_strLastErrorMsg = "Parse error: ";
		m_strLastErrorMsg += err.text;
		m_strLastErrorMsg += " (line:" + numstr::ToStr(err.line) + " column:" + numstr::ToStr(err.column) + " position:" + numstr::ToStr(err.position) + ")";
		goto done;
	}

	iType0 = json_typeof(pRoot);
	type0 = (json_type) iType0;

	// root node must be either object or array according to RFC4627 ( "A JSON text is a serialized object or array." )
	if (type0 == JSON_ARRAY)
	{
		m_RootSPtr = mpo_json_array::GetInstance();
		mpo_json_array *pArray = m_RootSPtr->ToArray();
		do_array_decode(pArray->GetChildren(), pRoot);	// start recursive process
	}
	else
	{
		m_RootSPtr = mpo_json_object::GetInstance();
		mpo_json_object *pObject = m_RootSPtr->ToObject();
		do_object_decode(pObject->GetChildren(), pRoot);	// start recursive process
	}

	json_decref(pRoot);

	pRes = m_RootSPtr;

done:
	return pRes;
}

wstring get_indentation(int iDepth)
{
	wstring wstrResult;
	for (int i = 0; i < iDepth; i++)
	{
		wstrResult += L"\t";
	}

	return wstrResult;
}

wstring escape_string(const wstring &wstrSrc)
{
	wstring wstrResult;

	for (wstring::const_iterator si = wstrSrc.begin(); si != wstrSrc.end(); si++)
	{
		switch (*si)
		{
		case '"':
			wstrResult += L"\\\"";
			break;
		case '\\':
			wstrResult += L"\\\\";
			break;
		case '\b':
			wstrResult += L"\\b";
			break;
		case '\f':
			wstrResult += L"\\f";
			break;
		case '\n':
			wstrResult += L"\\n";
			break;
		case '\r':
			wstrResult += L"\\r";
			break;
		case '\t':
			wstrResult += L"\\t";
			break;
		default:
			// if this is another control character
			if (*si < ' ')
			{
				wstrResult += L"\\u";
				wstrResult += mpom::str_conv(numstr::ToStr(*si, 16, 4));
			}
			// else we do not need to escape it
			else
			{
				wstrResult += *si;
			}
			break;
		}
	}

	return wstrResult;
}

wstring do_object_encode(mpo_json_object *pObject, int iDepth)
{
	wstring wstrResult = L"{\n";
	
	list<mpo_json_keypair> *pLstChildren = pObject->GetChildren();
	bool bFirst = true;

	for (list<mpo_json_keypair>::const_iterator li = pLstChildren->begin(); li != pLstChildren->end(); li++)
	{
		if (!bFirst)
		{
			wstrResult += L",\n";
		}

		wstrResult += get_indentation(iDepth + 1) + L"\"" + escape_string((*li).key) + L"\": ";
		mpo_json_generic *value = (*li).value.get();
		wstrResult += mpo_json_encode_helper(value, iDepth + 1);
		bFirst = false;
	}

	wstrResult += L"\n" + get_indentation(iDepth) + L"}";

	return wstrResult;
}

wstring do_array_encode(mpo_json_array *pArray, int iDepth)
{
	wstring wstrResult = L"[\n";
	list<MpoJsonGenericSPtr> *pLstChildren = pArray->GetChildren();
	bool bFirst = true;
	for (list<MpoJsonGenericSPtr>::const_iterator li = pLstChildren->begin(); li != pLstChildren->end(); li++)
	{
		if (!bFirst)
		{
			wstrResult += L",\n";
		}
		mpo_json_generic *value = (*li).get();
		wstrResult += get_indentation(iDepth + 1) + mpo_json_encode_helper(value, iDepth + 1);
		bFirst = false;
	}

	wstrResult += L"\n" + get_indentation(iDepth) + L"]";
	return wstrResult;
}

wstring mpo_json_encode_helper(mpo_json_generic *value, int iDepth)
{
	wstring wstrResult;
	mpo_json_type type = value->GetType();

	switch (type)
	{
	default: // object
		{
			mpo_json_object *pObj = value->ToObject();
			wstrResult += do_object_encode(pObj, iDepth);
		}
		break;
	case MPO_JSON_ARRAY:
		{
			mpo_json_array *pArray = value->ToArray();
			wstrResult += do_array_encode(pArray, iDepth);
		}
		break;
	case MPO_JSON_STRING:
		{
			mpo_json_string *pStr = value->ToString();
			wstrResult += L"\"" + escape_string(pStr->GetValue()) + L"\"";
		}
		break;
	case MPO_JSON_INT:
		{
			mpo_json_int *pInt = value->ToInt();
			wstrResult += mpom::str_conv(numstr::ToStr(pInt->GetValue()));
		}
		break;
	case MPO_JSON_DOUBLE:
		{
			mpo_json_double *pD = value->ToDouble();

			// the precision here is chosen somewhat arbitrarily and can be tweaked if needed
			wstrResult += mpom::str_conv(numstr::ToStr(pD->GetValue(), 0, 0, 2));
		}
		break;
	case MPO_JSON_BOOLEAN:
		{
			mpo_json_boolean *pB = value->ToBoolean();
			bool b = pB->GetValue();
			wstrResult += b ? L"true" : L"false";
		}
		break;
	case MPO_JSON_NULL:
		{
			wstrResult += L"null";
		}
		break;
	}

	return wstrResult;
}

string mpo_json::dump(MpoJsonGenericSPtr root)
{
	string strRes;
	wstring wstrRes;
	mpo_json_type type = root->GetType();

	if (type == MPO_JSON_OBJECT)
	{
		mpo_json_object *pObj = root->ToObject();
		wstrRes = do_object_encode(pObj, 0);
	}
	else if (type == MPO_JSON_ARRAY)
	{
		mpo_json_array *pArray = root->ToArray();
		wstrRes = do_array_encode(pArray, 0);
	}
	// else it's an unsupported type
	else
	{
		throw runtime_error("Root must be either an object or an array");
	}

	return mpom::ToUTF8Ex(wstrRes);
}

string mpo_json::get_last_error_msg()
{
	return m_strLastErrorMsg;
}

#endif // HAVE_JANSSON_H
