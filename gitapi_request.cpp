#include "gitapi_request.h"
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GitApiRequest::add_headers()
{
	struct curl_slist *chunk = NULL;
	/* Remove a header curl would otherwise add by itself */
	chunk = curl_slist_append(chunk, "Accept: application/vnd.github+json");
	/* Add an auth header */
	std::string htok = "Authorization: Bearer " + user_token;
	chunk = curl_slist_append(chunk, htok.c_str());
	chunk = curl_slist_append(chunk, "X-GitHub-Api-Version: 2022-11-28");
	chunk = curl_slist_append(chunk, "User-Agent: Test-GitAPI-App");
	/* set our custom set of headers */
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GitApiRequest::j_reply()
{
	nlohmann::json jf = nlohmann::json::parse(reply.c_str());
	return jf;
}
//--------------------------------------------------------------------------------------------------------------------------
