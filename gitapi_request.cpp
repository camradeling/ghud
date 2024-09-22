#include <iostream>
#include "gitapi_request.h"
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GitApiRequest::add_headers()
{
	struct curl_slist *chunk = NULL;
	// Remove a header curl would otherwise add by itself
	chunk = curl_slist_append(chunk, "Accept: application/vnd.github+json");
	// Add an auth header
	std::string htok = "Authorization: Bearer " + user_token;
	chunk = curl_slist_append(chunk, htok.c_str());
	chunk = curl_slist_append(chunk, "X-GitHub-Api-Version: 2022-11-28");
	chunk = curl_slist_append(chunk, "User-Agent: Test-GitAPI-App");
	// set our custom set of headers
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GitApiRequest::j_reply()
{
	try {
		nlohmann::json jf = nlohmann::json::parse(reply.c_str());
		return jf;
	}
	catch(nlohmann::json::parse_error& ex) {
		fprintf(stderr, "error parsing reply\n");
		return nlohmann::json(); //empty
	}
}
//--------------------------------------------------------------------------------------------------------------------------
int GHUDNS::GitApiRequest::perform()
{
	int res = GHUDNS::CurlRequest::perform();
	if (CURL_DEBUG)
		std::cout << std::setw(4) << j_reply();
	return res;
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GitApiDeleteRequest::set_method()
{
	if (curl)
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	// TODO: need to make check for success, http code will be 204
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GitApiPostRequest::set_method()
{
	if (curl)
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	// TODO: need to make check for success, http code will be 204
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GitApiPostRequest::add_data()
{
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)postdata.size());
		fprintf(stdout, "post data: %s\n", postdata.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
	}
}
//--------------------------------------------------------------------------------------------------------------------------
