#ifndef GHUD_PULL_REQUEST_H
#define GHUD_PULL_REQUEST_H
//--------------------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
//--------------------------------------------------------------------------------------------------------------------------
namespace GHUDNS
{
class GHUD;
class GHUDRepo;
//--------------------------------------------------------------------------------------------------------------------------
typedef struct reviewer_s
{
	std::string name;
	std::string email;
	std::string status = "UNKNOWN";
} GHUDReviewer;
//--------------------------------------------------------------------------------------------------------------------------
class GHUDPullRequest
{
public:
	GHUDPullRequest(std::string url, GHUDRepo* srcrepo) { baseurl = url; repo = srcrepo; }
	void process();
private:
	nlohmann::json create();
	nlohmann::json add_reviewers();
	nlohmann::json check_status();
	nlohmann::json check_review_statuses();
	std::string create_body();
	GHUDRepo* repo;
	std::string baseurl;
	int number=-1;
	std::vector<GHUDReviewer> reviewers;
};
//--------------------------------------------------------------------------------------------------------------------------
}//namespace GHUD
//--------------------------------------------------------------------------------------------------------------------------
#endif/*GHUD_PULL_REQUEST_H*/
