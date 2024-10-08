#include <iostream>
#include <stdio.h>
#include <regex>
#include <unistd.h>
//--------------------------------------------------------------------------------------------------------------------------
#include "ghud_repo.h"
#include "ghud.h"
#include "gitapi_request.h"
//--------------------------------------------------------------------------------------------------------------------------
static std::regex git_method_regex("git@(.*):(.*)\\/(.*)\\.git");
static std::regex https_method_regex("https:\\/\\/(.*)\\/(.*)\\/(.*)\\.git");
//--------------------------------------------------------------------------------------------------------------------------
GHUDNS::GHUDRepo::GHUDRepo(mxml_node_t* node, GHUD* gh)
{
     ghud = gh;
     mxml_node_t* urlnode;
     mxml_node_t* seeknode;
     urlnode = mxmlFindElement(node, node, "url", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          fprintf(stderr, "url not found\n");
          exit(-1);
     }
     url = (char *) mxmlGetText(urlnode, NULL);
     urlnode = mxmlFindElement(node, node, "branch", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          fprintf(stderr, "source branch name not found\n");
          exit(-1);
     }
     source_branch_name = (char *) mxmlGetText(urlnode, NULL);
     urlnode = mxmlFindElement(node, node, "update_branch", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          fprintf(stderr, "update branch name not found\n");
     }
     else
          update_branch_name = (char *) mxmlGetText(urlnode, NULL);
     urlnode = mxmlFindElement(node, node, "path", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          fprintf(stderr, "path not found");
     }
     else
          path = (char *) mxmlGetText(urlnode, NULL);
     //----------------------
     // PR attributes
     urlnode = mxmlFindElement(node, node, "DESCRIPTION_PLACEHOLDER", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          fprintf(stderr, "DESCRIPTION_PLACEHOLDER not found\n");
     }
     else
          DESCRIPTION_PLACEHOLDER = (char *) mxmlElementGetAttr(urlnode, "text");
     urlnode = mxmlFindElement(node, node, "PLATFORMS_PLACEHOLDER", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          fprintf(stderr, "PLATFORMS_PLACEHOLDER not found\n");
     }
     else
          PLATFORMS_PLACEHOLDER = (char *) mxmlElementGetAttr(urlnode, "text");
     urlnode = mxmlFindElement(node, node, "JIRANUM_PLACEHOLDER", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          fprintf(stderr, "JIRANUM_PLACEHOLDER not found\n");
     }
     else
          JIRANUM_PLACEHOLDER = (char *) mxmlElementGetAttr(urlnode, "text");
     //----------------------
     if (url.size() < 3) {
          fprintf(stderr, "repo url invalid\n");
          exit(-1);
     }
     else if (url.substr(0,3) == "git") {
          std::regex *repo_regex = &git_method_regex;
          std::smatch sm;
          if (!std::regex_search(url, sm, *repo_regex)) {
               fprintf(stderr, "parsing repo url, no match\n");
               exit(-1);
          }
          workgroup = sm[2];
          repo_name = sm[3];
          base_url = ghud->api_url + workgroup + "/" + repo_name;
     }
     else if (url.size() < 5) {
          fprintf(stderr, "repo url invalid\n");
          exit(-1);
     }
     else if (url.substr(0,5) == "https") {
          std::regex *repo_regex = &https_method_regex;
          std::smatch sm;
          if (!std::regex_search(url, sm, *repo_regex)) {
               fprintf(stderr, "parsing repo url, no match\n");
               exit(-1);
          }
          workgroup = sm[2];
          repo_name = sm[3];
          base_url = ghud->api_url + workgroup + "/" + repo_name;
     }
     else {
          fprintf(stderr, "repo url invalid\n");
          exit(-1);
     }
     //----------------------
     urlnode = mxmlFindElement(node, node, "update_pr_title", NULL, NULL, MXML_DESCEND);
     if (!urlnode) {
          fprintf(stderr, "update_pr_title not found, keep default\n");
     }
     else {
          update_pr_title = (char *) mxmlElementGetAttr(urlnode, "text");
     }
     fprintf(stdout, "repo name %s, workgroup %s, base %s, path %s\n",
               repo_name.c_str(), workgroup.c_str(), base_url.c_str(), path.c_str());
     mxml_parse_submodules(node);
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUDRepo::mxml_parse_submodules(mxml_node_t* node)
{
     fprintf(stderr, "parsing submodules of %s\n", repo_name.c_str());
     mxml_node_t* curnode;
     for (curnode = mxmlFindElement(node, node, "submodule", NULL, NULL, MXML_DESCEND);
               curnode != NULL;
               curnode = mxmlFindElement(curnode, node, NULL, NULL, NULL, MXML_NO_DESCEND)) {
     	submodules.push_back(GHUDNS::GHUDRepo(curnode, ghud));
     }
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::list_repos()
{
     std::string url = "https://api.github.com/user/repos";
     GHUDNS::GitApiRequest request(url, ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::list_branches()
{
	std::string url = base_url + "/branches";
	GHUDNS::GitApiRequest request(url, ghud->token());
     request.perform();
	return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::delete_branch(std::string branch)
{
     std::string url = base_url + "/git/refs/heads/" + branch;
     GHUDNS::GitApiDeleteRequest request(url, ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::create_branch(std::string branch, std::string sha)
{
     std::string url = base_url + "/git/refs";
     nlohmann::json data;
     data["ref"] = "refs/heads/" + branch;
     data["sha"] = sha;
     GHUDNS::GitApiPostRequest request(url, ghud->token(), data.dump());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::get_commit(std::string id)
{
     std::string url = base_url + "/commits/" + id;
     GHUDNS::GitApiRequest request(url, ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::get_branch_commits_since(std::string branch, std::string datetime)
{
     std::string url = base_url + "/commits" + "?sha=" + branch + "&since=" +datetime;
     GHUDNS::GitApiRequest request(url, ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
std::string GHUDNS::GHUDRepo::update_submodules()
{
     if (submodules.size() == 0) {
          fprintf(stderr, "no submodules to update\n");
          return "";
     }
     // assuming update_branch is and existing branch name
     // get master tree
     nlohmann::json tree = get_tree(source_branch_head_commit["commit"]["tree"]["sha"]);
     nlohmann::json newtree;
     // traverse submodules and if their branch head and submodule ref don't match
     // add to newtree
     nlohmann::json commit_list;
     for (auto& node : tree["tree"]) {
          if (node["type"] != "commit")
               continue;
          for (auto& submodule : submodules) {
               if (submodule.path == node["path"]) {
                    submodule.parent_ref = node["sha"];
                    if (submodule.parent_ref != submodule.source_branch_head_commit["sha"]) {
                         fprintf(stdout, "submodule %s updating parent repo ref from %s to %s\n",
                                   submodule.repo_name.c_str(),
                                   submodule.parent_ref.c_str(),
                                   nlohmann::to_string(submodule.source_branch_head_commit["sha"]).c_str());
                         nlohmann::json submodule_headref = submodule.get_commit(submodule.parent_ref);
                         nlohmann::json new_commits = submodule.get_branch_commits_since(submodule.source_branch_name,
                                   submodule_headref["commit"]["committer"]["date"]);
                         // exclude parent_ref submodule commit
                         for (auto& comm : new_commits)
                              if (comm["commit"]["committer"]["date"] != submodule_headref["commit"]["committer"]["date"])
                                   commit_list[submodule.repo_name].push_back(comm);
                         node["sha"] = submodule.source_branch_head_commit["sha"];
                         newtree["tree"].push_back(node);
                    }
                    else
                         fprintf(stdout, "submodule %s HEAD %s is equal to parent repo ref %s. Nothing to update\n",
                                   submodule.repo_name.c_str(),
                                   nlohmann::to_string(submodule.source_branch_head_commit["sha"]).c_str(),
                                   submodule.parent_ref.c_str());
               }
          }
     }
     if (newtree.empty()) {
          fprintf(stdout, "Repository %s: no submodules to update. Step out\n", repo_name.c_str());
          return ""; // return empty string
     }
     std::string commit_data="";
     for (auto& slist : commit_list.items()) {
          commit_data += "    " + slist.key() + ":\n\n";
          for (auto& comm : slist.value()) {
               std::string message = comm["commit"]["message"];
               message = ([](std::string message)->std::string{ std::regex newlines_re("\n+");
                                        return std::regex_replace(message, newlines_re, " ");
                                        })(message);
               std::string sha = comm["sha"];
               commit_data += "        " + message + " " + sha + "\n";
          }
          commit_data += "\n";
     }
     for (auto& submodule : submodules) {
          commit_data += submodule.integrated_commits;
     }
     newtree["base_tree"] = source_branch_head_commit["sha"];
     std::string url = "https://api.github.com/repos/" + workgroup + "/" + repo_name + "/git/trees";
     GHUDNS::GitApiPostRequest request(url, ghud->token(), newtree.dump());
     request.perform();
     nlohmann::json tree_reply = request.j_reply();
     //---------------------------------------
     nlohmann::json commit;
     commit["message"] = "updating submodules hash";
     commit["tree"] = tree_reply["sha"];
     commit["parents"].push_back(update_branch_head_commit["sha"]);
     std::string url2 = "https://api.github.com/repos/" + workgroup + "/" + repo_name + "/git/commits";
     GHUDNS::GitApiPostRequest request2(url2, ghud->token(), commit.dump());
     request2.perform();
     nlohmann::json commit_reply = request2.j_reply();
     move_branch_head(update_branch_name, commit_reply["sha"]);
     return commit_data;
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::get_tree(std::string sha)
{
     std::string url = base_url + "/git/trees/" + sha;
     GHUDNS::GitApiRequest request(url, ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::move_branch_head(std::string branch, std::string sha)
{
     std::string url = base_url + "/git/refs/heads/" + branch;
     nlohmann::json data;
     data["sha"] = sha;
     GHUDNS::GitApiPatchRequest request(url, ghud->token(), data.dump());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUDRepo::process()
{
     for (auto& submodule : submodules)
          submodule.process();
     fprintf(stdout, "checking head commit of branch %s\n", source_branch_name.c_str());
     // when passing a branch name - we get branch head commit
     // when passing sha - we get a specific commit
     source_branch_head_commit = get_commit(source_branch_name);
     if(source_branch_head_commit.empty()) {
          fprintf(stderr, "failed to get head commit of %s.\n", source_branch_name.c_str());
          exit(-1);
     }

     // if no update branch name specified - this is probably a leaf
     if (update_branch_name == "")
          return;
     // if branch delete fails - it's no issue
     fprintf(stdout, "trying to delete branch %s\n", update_branch_name.c_str());
     delete_branch(update_branch_name);
     std::string sha = source_branch_head_commit["sha"];
     fprintf(stdout, "forking %s branch from %s commit %s\n", update_branch_name.c_str(),
                                                                 source_branch_name.c_str(),
                                                                 sha.c_str());
     create_branch(update_branch_name, sha);
     update_branch_head_commit = get_commit(update_branch_name);
     if(update_branch_head_commit.empty()) {
          fprintf(stderr, "failed to get head commit of %s.\n", update_branch_name.c_str());
          exit(-1);
     }
     integrated_commits = update_submodules();
     if (integrated_commits == "") {
          return;
     }

     pr = std::shared_ptr<GHUDPullRequest>(new GHUDPullRequest(base_url + "/pulls", this));
     pr->process();
     fprintf(stdout, "checking head commit of branch %s again\n", source_branch_name.c_str());
     // when PR is complete we check head commit again
     // for the upper level repo update
     source_branch_head_commit = get_commit(source_branch_name);
     if(source_branch_head_commit.empty()) {
          fprintf(stderr, "failed to get head commit of %s.\n", source_branch_name.c_str());
          exit(-1);
     }
}
//--------------------------------------------------------------------------------------------------------------------------
