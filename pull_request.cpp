#include <unistd.h>
#include "ghud_repo.h"
#include "ghud.h"
#include "gitapi_request.h"
#include "pull_request.h"
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDPullRequest::create_update_pull_request()
{
     std::string url = baseurl;
     nlohmann::json data;
     data["title"] = "testing pull request functionality";
     data["body"] = "OMG! it's really working!";
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
nlohmann::json GHUDNS::GHUDPullRequest::check_status()
{
     std::string url = baseurl + "/" + std::to_string(number) + "/reviews";
     GHUDNS::GitApiRequest request(url, repo->ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUDPullRequest::process()
{
     nlohmann::json pr = create_update_pull_request();
     number = pr["number"];
     add_reviewers();
     while (1) {
          fprintf(stdout, "checking PR status\n");
          nlohmann::json revstat = check_status();
          if (revstat.size() == 0) {
               fprintf(stdout, "PR %d not yet approved\n",number);
          }
          else {
               for (auto& rev : revstat) {
                    if (rev["state"] == "APPROVED")
                         fprintf(stdout, "PR %d approved\n",number);
                    else
                         fprintf(stdout, "PR %d not yet approved\n",number);
               }
          }
          sleep(15);
     }
}
//--------------------------------------------------------------------------------------------------------------------------
