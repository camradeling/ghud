#include <iostream>
#include <stdio.h>
#include <regex>
#include <unistd.h>
//--------------------------------------------------------------------------------------------------------------------------
#include "ghud_repo.h"
#include "ghud.h"
#include "gitapi_request.h"
//--------------------------------------------------------------------------------------------------------------------------
GHUDNS::GHUDRepo::GHUDRepo(mxml_node_t* node, GHUD* gh)
{
	ghud = gh;
	mxml_node_t* urlnode;
     mxml_node_t* seeknode;
     urlnode = mxmlFindElement(node, node, "url", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
     	std::cout << "url not found" << std::endl;
     	exit(-1);
     }
     url = (char *) mxmlGetText(urlnode, NULL);
     urlnode = mxmlFindElement(node, node, "branch", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          std::cout << "source branch name not found" << std::endl;
          exit(-1);
     }
     source_branch_name = (char *) mxmlGetText(urlnode, NULL);
     urlnode = mxmlFindElement(node, node, "update_branch", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          std::cout << "update branch name not found" << std::endl;
     }
     else
          update_branch_name = (char *) mxmlGetText(urlnode, NULL);
     urlnode = mxmlFindElement(node, node, "path", NULL, NULL, MXML_DESCEND_FIRST);
     if(!urlnode) {
          std::cout << "path not found" << std::endl;
     }
     else
          path = (char *) mxmlGetText(urlnode, NULL);
     std::regex repo_regex("(.*):(.*)/(.*)\\..*");
     std::smatch sm;
     if (!std::regex_search(url, sm, repo_regex)) {
     	fprintf(stderr, "parsing repo url, no match\n");
     	exit(-1);
     }
     workgroup = sm[2];
     repo_name = sm[3];
     base_url = "https://api.github.com/repos/" + workgroup + "/" + repo_name;
     urlnode = mxmlFindElement(node, node, "update_pr_title", NULL, NULL, MXML_DESCEND);
     if (!urlnode) {
          std::cout << "update_pr_title not found, keep default" << std::endl;
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
                    if (node["sha"] != submodule.source_branch_head_commit["sha"]) {
                         fprintf(stdout, "submodule %s updating parent repo ref from %s to %s\n",
                                   submodule.repo_name.c_str(),
                                   nlohmann::to_string(node["sha"]).c_str(),
                                   nlohmann::to_string(submodule.source_branch_head_commit["sha"]).c_str());
                         nlohmann::json submodule_headref = submodule.get_commit(node["sha"]);
                         //fprintf(stderr, "submodule_headref: %s\n", nlohmann::to_string(submodule_headref).c_str());
                         commit_list[submodule.repo_name] = submodule.get_branch_commits_since(submodule.source_branch_name,
                                   submodule_headref["commit"]["committer"]["date"]);
                         node["sha"] = submodule.source_branch_head_commit["sha"];
                         newtree["tree"].push_back(node);
                    }
                    else
                         fprintf(stdout, "submodule %s HEAD %s is equal to parent repo ref %s. Nothing to update\n",
                                   submodule.repo_name.c_str(),
                                   nlohmann::to_string(submodule.source_branch_head_commit["sha"]).c_str(),
                                   nlohmann::to_string(node["sha"]).c_str());
               }
          }
     }
     if (newtree.empty())
          return nlohmann::json(); // return empty object
     std::string commit_data = "Commits included:\n";
     for (auto& slist : commit_list.items()) {
          commit_data += "\t" + slist.key() + ":\n";
          for (auto& comm : slist.value()) {
               commit_data += "\t\t" + nlohmann::to_string(comm["commit"]["message"]) + "\n";
          }
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
     nlohmann::json branches = list_branches();
     if (branches.empty()) {
          fprintf(stderr, "branch list failed\n");
          exit(-1);
     }
     nlohmann::json curnode;
     nlohmann::json updnode;
     for (auto& brnode : branches.items()) {
          if (brnode.value().contains("name") && brnode.value()["name"] == source_branch_name) {
               curnode = brnode;
          }
          else if (brnode.value().contains("name") && brnode.value()["name"] == update_branch_name) {
               updnode = brnode;
          }
     }
     if(curnode.empty()) {
          fprintf(stdout, "branch %s doesn\'t exist.\n", source_branch_name.c_str());
          exit(-1);
     }
     fprintf(stdout, "checking head commit of branch %s\n", source_branch_name.c_str());
     // when passing a branch name - we get branch head commit
     // when passing sha - we get a specific commit
     source_branch_head_commit = get_commit(source_branch_name);
     if(source_branch_head_commit.empty()) {
          fprintf(stdout, "failed to get head commit of %s.\n", source_branch_name.c_str());
          exit(-1);
     }

     // if no update branch name specified - this is probably a leaf
     if (update_branch_name == "")
          return;
     if(!updnode.empty()) {
          fprintf(stdout, "branch %s exists. Deleting\n", update_branch_name.c_str());
          delete_branch(update_branch_name);
     }
     std::string sha = source_branch_head_commit["sha"];
     fprintf(stdout, "forking %s branch from %s commit %s\n", update_branch_name.c_str(),
                                                                 source_branch_name.c_str(),
                                                                 sha.c_str());
     create_branch(update_branch_name, sha);
     update_branch_head_commit = get_commit(update_branch_name);
     if(update_branch_head_commit.empty()) {
          fprintf(stdout, "failed to get head commit of %s.\n", update_branch_name.c_str());
          exit(-1);
     }
     std::string prbody = update_submodules();
     if (prbody == "")
          return;

     pr = std::shared_ptr<GHUDPullRequest>(new GHUDPullRequest(base_url + "/pulls", this, prbody));
     pr->process();
}
//--------------------------------------------------------------------------------------------------------------------------
