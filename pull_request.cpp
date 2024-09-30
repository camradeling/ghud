#include <unistd.h>
#include "ghud_repo.h"
#include "ghud.h"
#include "gitapi_request.h"
#include "pull_request.h"
//--------------------------------------------------------------------------------------------------------------------------
std::string GHUDNS::GHUDPullRequest::create_body()
{
     // TODO: check for errors
     std::string result = repo->ghud->pr_template;
     result = result.replace(result.find("DESCRIPTION_PLACEHOLDER", 0),
               strlen("DESCRIPTION_PLACEHOLDER"), repo->DESCRIPTION_PLACEHOLDER);
     result = result.replace(result.find("PLATFORMS_PLACEHOLDER", 0),
               strlen("PLATFORMS_PLACEHOLDER"), repo->PLATFORMS_PLACEHOLDER);
     result = result.replace(result.find("JIRANUM_PLACEHOLDER", 0),
               strlen("JIRANUM_PLACEHOLDER"), repo->JIRANUM_PLACEHOLDER);
     result = result.replace(result.find("COMMITS_PLACEHOLDER", 0),
               strlen("COMMITS_PLACEHOLDER"), repo->integrated_commits);
     return result;
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDPullRequest::create()
{
     std::string url = baseurl;
     nlohmann::json data;
     data["title"] = repo->update_pr_title;
     data["body"] = create_body();
     data["head"] = repo->update_branch_name;
     data["base"] = repo->source_branch_name;
     GHUDNS::GitApiPostRequest request(url, repo->ghud->token(), data.dump());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDPullRequest::add_reviewers()
{
     reviewers.insert(reviewers.end(), repo->ghud->reviewers.begin(), repo->ghud->reviewers.end());
     nlohmann::json data;
     for (auto& reviewer : reviewers)
          data["reviewers"].push_back(reviewer.name);
     std::string url = baseurl + "/" + std::to_string(number) + "/requested_reviewers";
     GHUDNS::GitApiPostRequest request(url, repo->ghud->token(), data.dump());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDPullRequest::check_review_statuses()
{
     std::string url = baseurl + "/" + std::to_string(number) + "/reviews";
     GHUDNS::GitApiRequest request(url, repo->ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDPullRequest::check_status()
{
     std::string url = baseurl + "/" + std::to_string(number);
     GHUDNS::GitApiRequest request(url, repo->ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUDPullRequest::process()
{
     nlohmann::json pr = create();
     number = pr["number"];
     add_reviewers();
     while (1) {
          fprintf(stdout, "checking PR status\n");
          nlohmann::json revstat = check_review_statuses();
          int approved = 0;
          if (revstat.size() == 0) {
               fprintf(stdout, "PR %d not yet approved\n",number);
          }
          else {
               for (auto& rev : revstat) {
                    if (rev["state"] == "APPROVED") {
                         fprintf(stdout, "PR %d approved\n",number);
                         approved++;
                    }
                    else
                         fprintf(stdout, "PR %d not yet approved\n",number);
               }
          }
          nlohmann::json prstat = check_status();
          fprintf(stdout, "PR is %s\n", nlohmann::to_string(prstat["state"]).c_str());
          if (prstat["state"] == "closed") {
               fprintf(stdout, "consider this merged for now\n");
               break;
          }
          sleep(15);
     }
}
//--------------------------------------------------------------------------------------------------------------------------
