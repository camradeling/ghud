#ifndef GITAPI_REQUEST_H
#define GITAPI_REQUEST_H
//--------------------------------------------------------------------------------------------------------------------------
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
//--------------------------------------------------------------------------------------------------------------------------
#include "curl_request.h"
//--------------------------------------------------------------------------------------------------------------------------
namespace GHUDNS
{
//--------------------------------------------------------------------------------------------------------------------------
class GitApiRequest : public CurlRequest
{
public:
	GitApiRequest(std::string url, std::string token):CurlRequest(url,token){}
	virtual nlohmann::json j_reply();
	virtual int perform();
private:
	virtual void add_headers();
	virtual void set_method(){}
	virtual void add_data(){}
};
//--------------------------------------------------------------------------------------------------------------------------
class GitApiDeleteRequest : public GitApiRequest
{
public:
	GitApiDeleteRequest(std::string url, std::string token):GitApiRequest(url,token){}
private:
	virtual void set_method();
};
//--------------------------------------------------------------------------------------------------------------------------
class GitApiPostRequest : public GitApiRequest
{
public:
	GitApiPostRequest(std::string url, std::string token, std::string data):GitApiRequest(url,token){ postdata = data; }
private:
	std::string postdata;
	virtual void set_method();
	virtual void add_data();
};
//--------------------------------------------------------------------------------------------------------------------------
class GitApiPatchRequest : public GitApiRequest
{
public:
	GitApiPatchRequest(std::string url, std::string token, std::string data):GitApiRequest(url,token){ postdata = data; }
private:
	std::string postdata;
	virtual void set_method();
	virtual void add_data();
};
//--------------------------------------------------------------------------------------------------------------------------
}//namespace GHUDNS
//--------------------------------------------------------------------------------------------------------------------------
#endif/*GITAPI_REQUEST_H*/