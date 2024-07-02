#include "curl_request.h"
//--------------------------------------------------------------------------------------------------------------------------
size_t GHUD::CurlRequest::write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (!userp) {
		fprintf(stderr, "no class context provided\n");
		return -1;
	}
	GHUD::CurlRequest* ctx = (GHUD::CurlRequest*)userp;
	size_t realsize = size * nmemb;
    ctx->reply.append((const char*)contents, realsize);
    return realsize;
}
//--------------------------------------------------------------------------------------------------------------------------
int GHUD::CurlRequest::perform()
{
	if (!git_url.size())
		return -1;
	if (!user_token.size())
		return -2;
    curl = curl_easy_init();
    if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, git_url.c_str());
    /* example.com is redirected, so we tell libcurl to follow redirection */ 
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlRequest::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)this);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    // add headers
    add_headers();
    /* Perform the request, res will get the return code */ 
    reply_code = curl_easy_perform(curl);
    /* Check for errors */ 
    if(reply_code != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(reply_code));
        return -3;
    }
    /* always cleanup */ 
    curl_easy_cleanup(curl);
    }
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------------