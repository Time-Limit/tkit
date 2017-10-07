#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <functional>
#include <initializer_list>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class Key;
class Value;
class Config;
class Table;

enum CONFIGEXCEPTION
{
	UNKNOWN_ERROR = -1,

	LUA_LOAD_FILE_FAILED,
	LUA_KEY_INVALID,
	LUA_VALUE_INVALID,

	KEY_EXCEPT_NUMBER,
	KEY_EXCEPT_STRING,
	KEY_TYPE_INVALID,

	VALUE_EXCEPT_NUMBER,
	VALUE_EXCEPT_STRING,
	VALUE_EXCEPT_TABLE,

	TABLE_ENTRY_NOT_FOUND,
};

struct ConfigException
{
	ConfigException(int e) : error(e) {}
	int what() const { return error; }
private:
	int error;
};

class Key
{
	friend class Config;
	template<typename T> friend class std::shared_ptr;
	template<typename T1, typename T2> friend class std::map;
	template<typename T1, typename T2> friend class std::pair;
	friend bool operator< (const Key&, const Key&);
public:
	using NumKey = unsigned int;
	using StrKey = std::string;
public:
	bool IsNum() const { return type == NUMBER; }
	bool IsStr() const { return type == STRING; }

	const NumKey& Num() const {if(!IsNum()) throw ConfigException(KEY_EXCEPT_NUMBER); return num;}
	const StrKey& Str() const {if(!IsStr()) throw ConfigException(KEY_EXCEPT_STRING); return str;}
public:
	Key() : type(NUMBER), num(), str() {}
	Key(const NumKey& rhs) : type(NUMBER), num(rhs), str() {}
	Key(const StrKey& rhs) : type(STRING), num(), str(rhs) {}
	Key(const char * rhs) : Key(StrKey(rhs)) {}
private:

	Key& operator= (const NumKey& rhs) { type = NUMBER, num = rhs, str = StrKey(); return *this; }
	Key& operator= (const StrKey& rhs) { type = STRING, num = NumKey(), str = rhs; return *this; }
	Key& operator= (const char * rhs) 
	{
		this->operator=(StrKey(rhs));
		return *this;
	}
	
	Key(const Key&) = default;
	Key& operator= (const Key&) = default;
	Key(Key&&) = default;
	Key& operator= (Key&&) = default;
private:
	enum
	{
		NUMBER = 0,
		STRING = 1,
	};
private:
	unsigned char type;
	NumKey num;
	StrKey str;
};

class Table
{
	friend class Value;
	template<typename T> friend class std::shared_ptr;
	template<typename T1, typename T2> friend class std::map;
	template<typename T1, typename T2> friend class std::pair;
public:
	const Value& operator[](const Key&) const;
private:
	using EntryMap = std::map<Key, std::shared_ptr<Value>>;
	EntryMap entry_map;
};

class Value
{
	friend Config;
	template<typename T> friend class std::shared_ptr;
	template<typename T1, typename T2> friend class std::map;
	template<typename T1, typename T2> friend class std::pair;
public:
	using NumVal = double;
	using StrVal = std::string;
	using TabVal = Table;
public:
	const Value& operator[](const Key&) const;

	bool IsStr() const { return type == STRING; }
	bool IsNum() const { return type == NUMBER; }
	bool IsTab() const { return type == TABLE; }

	const NumVal& Num() const { if(!IsNum()) throw ConfigException(VALUE_EXCEPT_NUMBER); return num; }
	const StrVal& Str() const { if(!IsStr()) throw ConfigException(VALUE_EXCEPT_STRING); return str; }
	const TabVal& Tab() const { if(!IsTab()) throw ConfigException(VALUE_EXCEPT_TABLE) ; return tab; }
private: Value() : type(NUMBER) , num(), str(), tab() {} Value(const NumVal& rhs) : type(NUMBER), num(rhs), str(), tab() {} Value(const StrVal& rhs) : type(STRING), num(), str(rhs), tab() {} Value(const TabVal& rhs) : type(TABLE),  num(), str(), tab(rhs) {}

	Value& operator=(const NumVal &rhs) {type = NUMBER, num = rhs, str = StrVal(), tab = TabVal(); return *this; }
	Value& operator=(const StrVal &rhs) {type = STRING, num = NumVal(), str = rhs, tab = TabVal(); return *this; }
	Value& operator=(const TabVal &rhs) {type = TABLE,  num = NumVal(), str = StrVal(), tab = rhs; return *this; }

	Value(const Value &) = default;
	Value(Value &&) = default;
	Value& operator=(const Value&) = default;
	Value& operator=(Value &&) = default;
private:
	enum
	{
		NUMBER = 0,
		STRING,
		TABLE,
	};
private:

	void Insert(const Key &k, const Value &v);
private:
	unsigned char type;
	NumVal num;
	StrVal str;
	TabVal tab;
};

class Config
{
public:
	Config(const char *file) : table(Value::TabVal())
	{
		lua_State *L = luaL_newstate();
		std::shared_ptr<lua_State> sp;
		sp.reset(L, [](lua_State *L) {lua_close(L);});
		if(luaL_loadfile(L, file) || lua_pcall(L, 0, 0, 0))
		{
			throw ConfigException(LUA_LOAD_FILE_FAILED);
		}

		lua_getglobal(L, "config");
		Load(sp, table);
	}

	Config(const std::string &file) : Config(file.c_str()) {}

	const Value& operator[](const Key &k) const
	{
		if(!table.IsTab())
		{
			throw ConfigException(VALUE_EXCEPT_TABLE);
		}
		return table.tab[k];
	}

	Config(const Config&) = delete;
	Config& operator= (const Config&) = delete;
private:
	void Load(std::shared_ptr<lua_State>, Value &);
	Value table;
};

class ConfigManager
{
public:
	using ConfigMap = std::map<std::string, Config *>;
private:
	ConfigManager() = default;
	ConfigManager(const ConfigManager &) = delete;
	ConfigManager& operator= (const ConfigManager &) = delete;

	ConfigMap config_map;
public:
	static ConfigManager& GetInstance() { static ConfigManager m; return m; }

	size_t Reset(const std::initializer_list<std::string> &il);
public:
	~ConfigManager()
	{
		for(auto &p : config_map)
		{
			delete p.second;
			p.second = nullptr;
		}
		config_map.clear();
	}
};

#endif
