#ifndef CURL_REQUEST_H
#define CURL_REQUEST_H
//--------------------------------------------------------------------------------------------------------------------------
#include <string>
#include <curl/curl.h>
//--------------------------------------------------------------------------------------------------------------------------
namespace GHUDNS
{
//--------------------------------------------------------------------------------------------------------------------------
class CurlRequest
{
public:
	CurlRequest(std::string url, std::string token):git_url(url),user_token(token){}
	static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
	virtual int perform();
	virtual std::string s_reply() { return reply; }
protected:
	virtual void add_headers(){}
	virtual void set_method(){}
	virtual void add_data(){}
	CURL *curl;
	CURLcode reply_code;
	std::string method;
	std::string git_url;
	std::string user_token;
	std::string reply;
	long http_code = 0;
};
//--------------------------------------------------------------------------------------------------------------------------
}//namespace GHUDNS
//--------------------------------------------------------------------------------------------------------------------------
#endif/*CURL_REQUEST_H*/