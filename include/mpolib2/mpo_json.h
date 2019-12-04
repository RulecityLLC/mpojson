#ifndef MPO_JSON_H
#define MPO_JSON_H

#if defined(HAVE_CONFIG_H) || defined(HAVE_MPOLIB_CONFIG_H)
#include <mpolib/mpolib_config.h>
#endif

// only include file if header is available
#ifdef HAVE_JANSSON_H

#ifdef WIN32
#pragma warning (disable:4251)
#endif

#include "mpo_dll.h"
#include "mpo_deleter.h"
#include "mpo_numstr.h"

#include <list>
#include <map>
#include <string>
#include <stdexcept>

using namespace std;

// object, array, string, number (integer or double), boolean, and null
typedef enum
{
	MPO_JSON_OBJECT, MPO_JSON_ARRAY, MPO_JSON_STRING, MPO_JSON_INT, MPO_JSON_DOUBLE, MPO_JSON_BOOLEAN, MPO_JSON_NULL
} mpo_json_type;

class mpo_json_object;
class mpo_json_array;
class mpo_json_string;
class mpo_json_int;
class mpo_json_double;
class mpo_json_boolean;
class mpo_json_null;

class EXPORT_ME mpo_json_generic
{
public:
	mpo_json_generic() {}

	virtual mpo_json_type GetType() = 0;

	void ThrowInvalidCastException();

	mpo_json_object *ToObject();
	mpo_json_array *ToArray();
	mpo_json_string *ToString();
	mpo_json_int *ToInt();
	mpo_json_double *ToDouble();
	mpo_json_boolean *ToBoolean();
	mpo_json_null *ToNull();
};

typedef shared_ptr<mpo_json_generic> MpoJsonGenericSPtr;

class mpo_json_keypair
{
public:
	wstring key;
	MpoJsonGenericSPtr value;
};

class EXPORT_ME mpo_json_object : public mpo_json_generic, public MpoDeleter
{
public:
	static MpoJsonGenericSPtr GetInstance();
	mpo_json_type GetType();
	list<mpo_json_keypair> *GetChildren();

	// convenience function, won't work for all cases because multiple children can share the same key (although this is discouraged by the RFC)
	map<wstring, MpoJsonGenericSPtr> GetChildrenAsMap();

private:
	mpo_json_object();
	virtual ~mpo_json_object();
	void DeleteInstance();
	list<mpo_json_keypair> m_children;
};

class EXPORT_ME mpo_json_array : public mpo_json_generic, public MpoDeleter
{
public:
	static MpoJsonGenericSPtr GetInstance();
	mpo_json_type GetType();
	list<MpoJsonGenericSPtr> *GetChildren();
private:
	mpo_json_array();
	virtual ~mpo_json_array();
	void DeleteInstance();
	list<MpoJsonGenericSPtr> m_children;
};

class EXPORT_ME mpo_json_string : public mpo_json_generic, public MpoDeleter
{
public:
	static MpoJsonGenericSPtr GetInstance(const wstring &s);
	mpo_json_type GetType();
	wstring GetValue();
private:
	mpo_json_string(const wstring &s);
	virtual ~mpo_json_string();
	void DeleteInstance();
	wstring m_val;
};

class EXPORT_ME mpo_json_int : public mpo_json_generic, public MpoDeleter
{
public:
	static MpoJsonGenericSPtr GetInstance(const MPO_INT64 &i64);
	mpo_json_type GetType();
	MPO_INT64 GetValue();
private:
	mpo_json_int(const MPO_INT64 &i64);
	virtual ~mpo_json_int();
	void DeleteInstance();
	MPO_INT64 m_val;
};

class EXPORT_ME mpo_json_double : public mpo_json_generic, public MpoDeleter
{
public:
	static MpoJsonGenericSPtr GetInstance(const double d);
	mpo_json_type GetType();
	double GetValue();
private:
	mpo_json_double(const double d);
	virtual ~mpo_json_double();
	void DeleteInstance();
	double m_val;
};

class EXPORT_ME mpo_json_boolean : public mpo_json_generic, public MpoDeleter
{
public:
	static MpoJsonGenericSPtr GetInstance(bool b);
	mpo_json_type GetType();
	bool GetValue();
private:
	mpo_json_boolean(bool b);
	virtual ~mpo_json_boolean();
	void DeleteInstance();
	bool m_val;
};

class EXPORT_ME mpo_json_null : public mpo_json_generic, public MpoDeleter
{
public:
	static MpoJsonGenericSPtr GetInstance();
	mpo_json_type GetType();
private:
	mpo_json_null();
	virtual ~mpo_json_null();
	void DeleteInstance();
};

///////////////////////////////////////////////////

class EXPORT_ME mpo_json
{
public:
	mpo_json();

	MpoJsonGenericSPtr parse(const char *buf, unsigned int size);

	// returns UTF-8 encoded string.  Throws exception on error.
	string dump(MpoJsonGenericSPtr root);

	string get_last_error_msg();

private:
	MpoJsonGenericSPtr m_RootSPtr;
	string m_strLastErrorMsg;
};


#endif // HAVE_JANSSON_H

#endif // MPO_JSON_H
