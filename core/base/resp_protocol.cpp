#include "protocol.h"
#include "octets.h"

using namespace TCORE;

/*
 * + >> string
 * - >> error
 * : >> number
 * $ >> bulk
 * * >> array
 */

OctetsStream& RESP::Serialize(OctetsStream &os) const
{
	if(value.IsStr())
	{
		str_t data = value.ToStr();
		os.push_back("+", 1);
		os.push_back(data.c_str(), data.size());
		os.push_back("\r\n", 2);
	}
	else if(value.IsErr())
	{
		err_t data = value.ToErr();
		os.push_back("-", 1);
		os.push_back(data.type.c_str(), data.type.size());
		os.push_back(" ", 1);
		os.push_back(data.info.c_str(), data.info.size());
		os.push_back("\r\n", 2);
	}
	else if(value.IsInt())
	{
		int_t data = value.ToInt();
		std::stringstream ss;
		ss << data;
		os.push_back(":", 1);
		os.push_back(ss.str().c_str(), ss.str().size());
		os.push_back("\r\n", 2);
	}
	else if(value.IsBlk())
	{
		blk_t data = value.ToBlk();
		os.push_back("$", 1);
		std::stringstream ss;
		ss << data.size();
		os.push_back(ss.str().c_str(), ss.str().size());
		os.push_back("\r\n", 2);
		os.push_back(data.begin(), data.size());
		os.push_back("\r\n", 2);
	}
	else if(value.IsAry())
	{
		ary_t data = value.ToAry();
		os.push_back("*", 1);
		std::stringstream ss;
		ss << data.size();
		os.push_back(ss.str().c_str(), ss.str().size());
		os.push_back("\r\n", 2);
		for(const auto &p : data)
		{
			RESP tmp(p);
			tmp.Serialize(os);
		}
	}
	else
	{
		os.push_back("$-1\r\n", 5);
	}
	return os;
}

OctetsStream& RESP::Deserialize(OctetsStream &os)
{
	unsigned char flag = 0;
	os >> flag;
	Value tmp;
	switch(flag)
	{
		case '+':
		{
			str_t data;
			char c = 0;
			os >> c;
			while(c != '\r') { data += c; os >> c; }
			os >> c;
			tmp = Value(data);
		}break;
		case '-':
		{
			err_t data;
			char c = 0;
			os >> c;
			while(c != ' ')
			{
				data.type += c;
				os >> c;
			}
			os >> c;
			while(c != '\r') { data.info += c; os >> c; }
			os >> c;
			tmp = Value(data);
		}break;
		case ':':
		{
			int_t data = 0;
			char c = 0;
			os >> c;
			bool positive = true;
			if(c == '-') positive = false;
#define IsNumber(c) ('0' <= (c) && (c) <= '9')
			if(IsNumber(c)) { data = c - '0'; }
			else { throw std::string("TCORE::RESP, except number"); return os; }
			os >> c;
			while(c != '\r')
			{
				if(IsNumber(c)) { (data *= 10) += c - '0'; }
				else { throw std::string("TCORE::RESP, except number"); return os; }
				os >> c;
			}
			os >> c;
			if(false == positive) { data *= -1; }
#undef IsNumber
			tmp = Value(data);
		}break;
		case '$':
		{
			blk_t data;
			char c;
			os >> c;
			bool positive = true;
			if(c == '-') positive = false;
			int size = 0;
#define IsNumber(c) ('0' <= (c) && (c) <= '9')

			if(IsNumber(c)) { size = c - '0'; }
			else { throw std::string("except number"); return os; }
			os >> c;
			while(c != '\r')
			{
				if(IsNumber(c)) { (size *= 10) += c - '0'; }
				else { throw std::string("except number"); return os; }
				os >> c;
			}
			os >> c;
			if(false == positive) { size *= -1; }
			if(size == -1) { tmp = Value(nil_t()); }
			else
			{
				data = os.pop_front(size);
				os >> c >> c;
				tmp = Value(data);
			}
#undef IsNumber
		}break;
		case '*':
		{
			unsigned char c;
			os >> c;
			bool positive = true;
			if(c == '-') positive = false;
			int size = 0;
#define IsNumber(c) ('0' <= (c) && (c) <= '9')
			if(IsNumber(c)) { size = c - '0'; }
			else { throw std::string("except number"); return os; }
			os >> c;
			while(c != '\r')
			{
				if(IsNumber(c)) { (size *= 10) += c - '0'; }
				else { throw std::string("except number"); return os; }
				os >> c;
			}
			os >> c;
			if(false == positive) { size *= -1; }
			if(size == -1) { tmp = Value(nil_t());}
			else
			{
				ary_t data(size);
				for(int i = 0; i < size; ++i)
				{
					RESP resp;
					resp.Deserialize(os);
					data[i] = resp.GetValue();
				}
				tmp = Value(data);
			}
#undef IsNumber
		}break;
		default:
		{
			throw "TCORE::RESP::Deserialize, unkown type";
		}break;
	}

	value = tmp;

	return os;
}
