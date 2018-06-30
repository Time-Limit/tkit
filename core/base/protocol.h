#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "exptype.h"
#include "octets.h"

namespace TCORE
{

class OctetsStream;

class Protocol
{
public:
	virtual OctetsStream& Deserialize(OctetsStream &os) = 0;
	virtual OctetsStream& Serialize(OctetsStream &os) const = 0;
	virtual ~Protocol() {}
};

template<typename PACKET>
class HttpPacketVisitor
{
public:
	HttpPacketVisitor(PACKET &p) : packet(p) {}

	void SetVersion(const std::string &str) { packet.version = str; }
	void SetStatus(unsigned short s) { packet.status = s; }
	void SetStatement(const std::string &str) { packet.statement = str; }
	void SetHeader(const std::string &k, const std::string &v) { packet.headers[k] = v; }
	void SetBody(const std::string &body) { packet.body = body; }
	void SetMethod(const std::string &m) { packet.method = m; }
	void SetURL(const std::string &u) { packet.url = u; }
	void SetArgument(const std::string &k, const std::string &v) { packet.args[k] = v; }

	std::string GetURL() const { return packet.url; }

private:
	PACKET &packet;
};

class HttpRequest : public Protocol
{
	friend class HttpPacketVisitor<HttpRequest>;
	std::string method;
	std::string url;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;
	std::map<std::string, std::string> args;

public:
	HttpRequest() = default;
	HttpRequest(HttpRequest&&) = default;
	HttpRequest& operator=(HttpRequest&&) = default;
	HttpRequest(const HttpRequest &) = default;
	HttpRequest& operator=(const HttpRequest&) = default;

	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};

class HttpResponse : public Protocol
{
	friend class HttpPacketVisitor<HttpResponse>;
	std::string version;
	unsigned short status;
	std::string statement;
	std::map<std::string, std::string> headers;
	std::string body;

public:
	HttpResponse() = default;
	HttpResponse(HttpResponse&&) = default;
	HttpResponse& operator=(HttpResponse&&) = default;
	HttpResponse(const HttpResponse &) = default;
	HttpResponse& operator=(const HttpResponse&) = default;

	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};

class RESP : public Protocol
{
public:
	typedef struct {}	nil_t;
	typedef std::string	str_t;
	typedef int64_t		int_t;
	typedef struct{ str_t type, info; } err_t;
	typedef Octets blk_t;

	class Value
	{
		enum TYPE
		{
			DT_NULL		= 0,
			DT_STRING	= 1,
			DT_INTEGER	= 2,
			DT_ERROR	= 3,
			DT_BULK		= 4,
			DT_ARRAY	= 5,
		};
		unsigned char type;
		Octets data;

		void Encode(OctetsStream &os) const
		{
			os << type << data;
		}

		void Decode(OctetsStream &os)
		{
			os >> type >> data;
		}
	public:
		int Type() const { return type; }

	public:
		bool IsNil() const { return type == DT_NULL; }
		bool IsStr() const { return type == DT_STRING; }
		bool IsInt() const { return type == DT_INTEGER; }
		bool IsErr() const { return type == DT_ERROR; }
		bool IsBlk() const { return type == DT_BULK; }
		bool IsAry() const { return type == DT_ARRAY; }

		const nil_t ToNil() const { return nil_t(); }
		const str_t ToStr() const { return str_t((const char *)data.begin(), (const char *)data.end()); }
		const err_t ToErr() const
		{
			err_t res;
			Octets tmp;
			OctetsStream os(data);
			os >> tmp;
			res.type = std::string((char *)tmp.begin(), tmp.size());
			os >> tmp;
			res.info = std::string((char *)tmp.begin(), tmp.size());
			return res;
		}
		const int_t ToInt() const { int_t d = 0; OctetsStream os(data); os >> d; return d; }
		const blk_t ToBlk() const { return data; }
		const std::vector<Value> ToAry() const
		{
			OctetsStream os(data);
			std::vector<Value>::size_type size = 0;
			os >> size;
			std::vector<Value> res(size);
			for(size_t i = 0; i < size; ++i)
			{
				res[i].Decode(os);
			}
			return res;
		}

	public:
		Value() : Value(nil_t()) {}
		Value(const Value &v) : type(v.type), data(v.data){}
		Value(const nil_t &v) : type(DT_NULL) {}

		Value(const str_t &v) : type(DT_STRING)
		{
			data.insert(data.end(), v.c_str(), v.size());
		}

		Value(int_t d) : type(DT_INTEGER)
		{
			OctetsStream os;
			os << d;
			data = os.GetData();
		}

		Value(const blk_t &d) : type(DT_BULK)
		{
			data = d;
		}

		Value(const err_t &d) : type(DT_ERROR)
		{
			OctetsStream os;
			os << Octets(d.type.c_str(), d.type.size()) << Octets(d.info.c_str(), d.info.size());
			data = os.GetData();
		}

		Value(const std::vector<Value> &arg) : type(DT_ARRAY)
		{
			OctetsStream os;
			os << arg.size();
			for(const auto &p : arg)
			{
				p.Encode(os);
			}
			data = os.GetData();
		}

		Value& operator=(const Value &v)
		{
			if(this == &v) return *this;
			this->type = v.type;
			this->data = v.data;
			return *this;
		}
	};

	typedef std::vector<Value> ary_t;

private:
	Value value;

public:
	RESP() : Protocol() {}
	RESP(const RESP::Value &v) : Protocol(), value(v) {}

	Value GetValue() const { return value; }

public:
	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};

/*
class RESP : public Protocol
{
public:
	enum DATA_TYPE
	{
		DT_NULL		= 0,
		DT_STRING	= 1,
		DT_NUMBER	= 2,
		DT_ERROR	= 3,
		DT_BULK		= 4,
		DT_ARRAY	= 5,
	};

	struct Data;
	typedef std::shared_ptr<Data> DataPtr;

	struct ErrorInfo
	{
		std::string type, info;
	};

	typedef void*		nil_t;
	typedef std::string	str_t;
	typedef ErrorInfo	err_t;
	typedef int64_t		num_t;
	typedef Octets		blk_t;
	typedef std::vector<DataPtr> arr_t;

	struct Data
	{
		friend class RESP;

		virtual DATA_TYPE Type() const = 0;
		virtual ~Data() {}

		virtual const nil_t nil() const { std::abort(); throw std::string("Method was not implemented."); return nullptr; }
		virtual const str_t& string() const { std::abort(); throw std::string("Method was not implemented."); static str_t _; return _; }
		virtual const err_t& error() const { std::abort(); throw std::string("Method was not implemented."); static err_t _; return _; }
		virtual const num_t number() const { std::abort(); throw std::string("Method was not implemented."); return 0; }
		virtual const blk_t& bulk() const { std::abort(); throw std::string("Method was not implemented."); static blk_t _; return _; }
		virtual const arr_t& array() const { std::abort(); throw std::string("Method was not implemented."); static arr_t _; return _; }

		virtual void string(const str_t &) { std::abort(); throw std::string("Method was not implemented."); }
		virtual void number(num_t) { std::abort(); throw std::string("Method was not implemented."); }
		virtual void error(const err_t &) { std::abort(); throw std::string("Method was not implemented."); }
		virtual void bulk(const blk_t&) { std::abort(); throw std::string("Method was not implemented."); }
		virtual void array(const arr_t&) { std::abort(); throw std::string("Method was not implemented."); }
	private:
		virtual bool IsNull() const { return false; }
		virtual OctetsStream& Deserialize(OctetsStream &os) { return os; } 
		virtual OctetsStream& Serialize(OctetsStream &os) const { return os; } 
	};

	struct Null : public Data
	{
		DATA_TYPE Type() const override { return DT_NULL; }
		virtual const nil_t nil() const override { return nullptr; }
	};

	class String : public Data
	{
	public:
		DATA_TYPE Type() const override { return DT_STRING; }
		String() : Data() {}
		String(const str_t d) : Data(), data(d) {}
		virtual const str_t& string() const override { return data; }
		virtual void string(const str_t& d) override { data = d; }
	private:
		str_t data;

		virtual OctetsStream& Deserialize(OctetsStream &os) override;
		virtual OctetsStream& Serialize(OctetsStream &os) const override;
	};

	class Number : public Data
	{
	public:
		Number() : Data(), data(0) {} 
		Number(num_t d) : Data(), data(d) {}
		DATA_TYPE Type() const override { return DT_NUMBER; }
		virtual const num_t number() const override { return data; }
		virtual void number(num_t d) override { data = d; }
	private:
		num_t data;

		virtual OctetsStream& Deserialize(OctetsStream &os) override;
		virtual OctetsStream& Serialize(OctetsStream &os) const override;
	};

	struct Error : public Data
	{
	public:
		DATA_TYPE Type() const override { return DT_ERROR; }
		virtual const err_t& error() const override { return data; }
		virtual void error(const err_t &d) override { data = d; }
	private:
		err_t data;

		virtual OctetsStream& Deserialize(OctetsStream &os) override;
		virtual OctetsStream& Serialize(OctetsStream &os) const override;
	};

	class Bulk : public Data
	{
	public:
		Bulk() : Data(), size(0) {}
		Bulk(const blk_t &d) : Data(), data(d), size(d.size()) {}
		DATA_TYPE Type() const override { return DT_BULK; }
		virtual const blk_t& bulk() const override { return data; }
		virtual void bulk(const blk_t &d) override { data = d; }
	private:
		Octets data;
		int size;
		virtual bool IsNull() const override { return size == -1; }

		virtual OctetsStream& Deserialize(OctetsStream &os) override;
		virtual OctetsStream& Serialize(OctetsStream &os) const override;
	};

	class Array : public Data
	{
	public:
		Array() : Data(), size(0) {}
		Array(const arr_t& d) : Data(), data(d), size(d.size())  {}
		DATA_TYPE Type() const override { return DT_ARRAY; }
		virtual const arr_t& array() const { return data; }
		virtual void array(const arr_t &d) override { data = d; }
	private:
		arr_t data;
		int size;
		virtual bool IsNull() const override { return size == -1; }

		virtual OctetsStream& Deserialize(OctetsStream &os) override;
		virtual OctetsStream& Serialize(OctetsStream &os) const override;
	};
private:
	DataPtr data_ptr;

public:
	DataPtr GetData() const { return data_ptr; }
	void SetData(DataPtr ptr) { data_ptr = ptr; } 

public:
	virtual OctetsStream& Deserialize(OctetsStream &os) override;
	virtual OctetsStream& Serialize(OctetsStream &os) const override;
};
*/

}

#endif
