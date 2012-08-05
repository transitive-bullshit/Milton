#include "../include/tinyjson/tinyjson.hpp" 

#define BOOST_AUTO_TEST_MAIN 
#include <boost/test/auto_unit_test.hpp> 
#include <boost/test/floating_point_comparison.hpp> 
#include <boost/lambda/lambda.hpp> 
#include <boost/lambda/casts.hpp> 
#include <ostream> 
#include <string> 
#include <algorithm> 
#include <iterator> 
#include <cstring> 

std::string to_utf8(const std::wstring &ws) 
{ 
	char buffer[1024]; 
	std::size_t size = std::wcstombs(buffer, ws.c_str(), sizeof(buffer)); 
	return std::string(buffer, size); 
}

namespace std 
{ 
	std::ostream &operator<<(std::ostream &os, const std::type_info &ti) 
	{ 
		return os << ti.name(); 
	} 

	std::ostream &operator<<(std::ostream &os, const std::wstring &ws) 
	{ 
		return os << to_utf8(ws); 
	} 

	std::ostream &operator<<(std::ostream &os, const std::basic_string<int> &is) 
	{ 
		std::wstring ws; 
		std::transform(is.begin(), is.end(), std::back_inserter(ws), boost::lambda::ll_static_cast<wchar_t>(boost::lambda::_1)); 
		return os << to_utf8(ws); 
	} 
} 

BOOST_AUTO_TEST_CASE(test1) 
{ 
	std::string json = " { \"Hello\" : \"World\" }"; 
	json::grammar<char>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<char>::object)); 
	json::grammar<char>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<char>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<char>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, "Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_CHECK_EQUAL(it->second->empty(), false); 
	std::string s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::string>(*it->second)); 
	BOOST_CHECK_EQUAL(s, "World"); 
} 

BOOST_AUTO_TEST_CASE(test2) 
{ 
	std::wstring json = L" { \"Hello\" : \"World\" } "; 
	json::grammar<wchar_t>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<wchar_t>::object)); 
	json::grammar<wchar_t>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<wchar_t>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<wchar_t>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, L"Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_CHECK_EQUAL(it->second->empty(), false); 
	std::wstring s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::wstring>(*it->second)); 
	BOOST_CHECK_EQUAL(s, L"World"); 
} 

BOOST_AUTO_TEST_CASE(test3) 
{ 
	std::string json = "{ \"Hello\" : [ \"abc\", 1, 2.5, true, false, null ] }"; 
	json::grammar<char>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<char>::object)); 
	json::grammar<char>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<char>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<char>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, "Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_REQUIRE_EQUAL(it->second->type(), typeid(json::grammar<char>::array)); 
	json::grammar<char>::array arr; 
	BOOST_REQUIRE_NO_THROW(arr = boost::any_cast<json::grammar<char>::array>(*it->second)); 
	BOOST_REQUIRE_EQUAL(arr.size(), 6); 
	json::grammar<char>::array::iterator it2 = arr.begin(); 
	std::string s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::string>(**it2)); 
	BOOST_CHECK_EQUAL(s, "abc"); 
	int i; 
	BOOST_REQUIRE_NO_THROW(i = boost::any_cast<int>(**++it2)); 
	BOOST_CHECK_EQUAL(i, 1); 
	double d; 
	BOOST_REQUIRE_NO_THROW(d = boost::any_cast<double>(**++it2)); 
	BOOST_CHECK_CLOSE(d, 2.5, 0.0000001); 
	bool b; 
	BOOST_REQUIRE_NO_THROW(b = boost::any_cast<bool>(**++it2)); 
	BOOST_CHECK_EQUAL(b, true); 
	BOOST_REQUIRE_NO_THROW(b = boost::any_cast<bool>(**++it2)); 
	BOOST_CHECK_EQUAL(b, false); 
	boost::any any = **++it2; 
	BOOST_CHECK_EQUAL(any.empty(), true); 
} 

BOOST_AUTO_TEST_CASE(test4) 
{ 
	std::wstring json = L"{ \"Hello\" : [ \"abc\", 1, 2.5, true, false, null ] }"; 
	json::grammar<wchar_t>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<wchar_t>::object)); 
	json::grammar<wchar_t>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<wchar_t>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<wchar_t>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, L"Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_REQUIRE_EQUAL(it->second->type(), typeid(json::grammar<wchar_t>::array)); 
	json::grammar<wchar_t>::array arr; 
	BOOST_REQUIRE_NO_THROW(arr = boost::any_cast<json::grammar<wchar_t>::array>(*it->second)); 
	BOOST_REQUIRE_EQUAL(arr.size(), 6); 
	json::grammar<wchar_t>::array::iterator it2 = arr.begin(); 
	std::wstring s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::wstring>(**it2)); 
	BOOST_CHECK_EQUAL(s, L"abc"); 
	int i; 
	BOOST_REQUIRE_NO_THROW(i = boost::any_cast<int>(**++it2)); 
	BOOST_CHECK_EQUAL(i, 1); 
	double d; 
	BOOST_REQUIRE_NO_THROW(d = boost::any_cast<double>(**++it2)); 
	BOOST_CHECK_CLOSE(d, 2.5, 0.0000001); 
	bool b; 
	BOOST_REQUIRE_NO_THROW(b = boost::any_cast<bool>(**++it2)); 
	BOOST_CHECK_EQUAL(b, true); 
	BOOST_REQUIRE_NO_THROW(b = boost::any_cast<bool>(**++it2)); 
	BOOST_CHECK_EQUAL(b, false); 
	boost::any any = **++it2; 
	BOOST_CHECK_EQUAL(any.empty(), true); 
} 

BOOST_AUTO_TEST_CASE(test5) 
{ 
	std::string json = "{ \"Hello\" : \"\\r\\n\\b\\\\\\t\\f\" }"; 
	json::grammar<char>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<char>::object)); 
	json::grammar<char>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<char>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<char>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, "Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_REQUIRE_EQUAL(it->second->type(), typeid(std::string)); 
	std::string s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::string>(*it->second)); 
	BOOST_CHECK_EQUAL(s, "\r\n\b\\\t\f"); 
} 

BOOST_AUTO_TEST_CASE(test6) 
{ 
	std::wstring json = L"{ \"Hello\" : \"\\r\\n\\b\\\\\\t\\f\" }"; 
	json::grammar<wchar_t>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<wchar_t>::object)); 
	json::grammar<wchar_t>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<wchar_t>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<wchar_t>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, L"Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_REQUIRE_EQUAL(it->second->type(), typeid(std::wstring)); 
	std::wstring s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::wstring>(*it->second)); 
	BOOST_CHECK_EQUAL(s, L"\r\n\b\\\t\f"); 
} 

BOOST_AUTO_TEST_CASE(test7) 
{ 
	std::string json = "{ \"Hello\" : \"\\u0057\\u006F\\u0072\\u006C\\u0064\\u0021\" }"; 
	json::grammar<char>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<char>::object)); 
	json::grammar<char>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<char>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<char>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, "Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_REQUIRE_EQUAL(it->second->type(), typeid(std::string)); 
	std::string s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::string>(*it->second)); 
	BOOST_CHECK_EQUAL(s, "World!"); 
} 

BOOST_AUTO_TEST_CASE(test8) 
{ 
	std::wstring json = L"{ \"Hello\" : \"\\u0057\\u006F\\u0072\\u006C\\u0064\\u0021\" }"; 
	json::grammar<wchar_t>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<wchar_t>::object)); 
	json::grammar<wchar_t>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<wchar_t>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<wchar_t>::object::iterator it = obj.begin(); 
	BOOST_CHECK_EQUAL(it->first, L"Hello"); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_REQUIRE_EQUAL(it->second->type(), typeid(std::wstring)); 
	std::wstring s; 
	BOOST_REQUIRE_NO_THROW(s = boost::any_cast<std::wstring>(*it->second)); 
	BOOST_CHECK_EQUAL(s, L"World!"); 
} 

BOOST_AUTO_TEST_CASE(test9) 
{ 
	typedef std::basic_string<int> istring; 
	istring json; 
	std::string s = "{ \"Hello\" : \"World\" }"; 
	std::transform(s.begin(), s.end(), std::back_inserter(json), boost::lambda::ll_static_cast<int>(boost::lambda::_1)); 
	json::grammar<int>::variant var; 
	BOOST_REQUIRE_NO_THROW(var = json::parse(json.begin(), json.end())); 
	BOOST_REQUIRE_EQUAL(var->empty(), false); 
	BOOST_REQUIRE_EQUAL(var->type(), typeid(json::grammar<int>::object)); 
	json::grammar<int>::object obj; 
	BOOST_REQUIRE_NO_THROW(obj = boost::any_cast<json::grammar<int>::object>(*var)); 
	BOOST_REQUIRE_EQUAL(obj.size(), 1); 
	json::grammar<int>::object::iterator it = obj.begin(); 
	istring hello; 
	s = "Hello"; 
	std::transform(s.begin(), s.end(), std::back_inserter(hello), boost::lambda::ll_static_cast<int>(boost::lambda::_1)); 
	BOOST_CHECK_EQUAL(it->first, hello); 
	BOOST_REQUIRE(it->second.get() != 0); 
	BOOST_CHECK_EQUAL(it->second->empty(), false); 
	istring world; 
	s = "World"; 
	std::transform(s.begin(), s.end(), std::back_inserter(world), boost::lambda::ll_static_cast<int>(boost::lambda::_1)); 
	istring value; 
	BOOST_REQUIRE_NO_THROW(value = boost::any_cast<istring>(*it->second)); 
	BOOST_CHECK_EQUAL(value, world); 
} 

