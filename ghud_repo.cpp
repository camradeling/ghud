#include <iostream>
#include <stdio.h>
#include <regex>
//--------------------------------------------------------------------------------------------------------------------------
#include "ghud_repo.h"
#include "ghud.h"
#include "gitapi_request.h"
//--------------------------------------------------------------------------------------------------------------------------
std::string update_branch_name = "test_update_submodules";
//--------------------------------------------------------------------------------------------------------------------------
GHUDNS::GHUDRepo::GHUDRepo(mxml_node_t* node, GHUD* gh)
{
	ghud = gh;
	mxml_node_t* urlnode;
     urlnode = mxmlFindElement(node, node, "url", NULL, NULL, MXML_DESCEND);
     if(!urlnode) {
     	std::cout << "url not found" << std::endl;
     	exit(-1);
     }
     url = (char *) mxmlGetText(urlnode, NULL);
     urlnode = mxmlFindElement(node, node, "branch", NULL, NULL, MXML_DESCEND);
     if(!urlnode) {
     	std::cout << "branch name not found" << std::endl;
     	exit(-1);
     }
     branch = (char *) mxmlGetText(urlnode, NULL);
     std::regex repo_regex("(.*):(.*)/(.*)\\..*");
     std::smatch sm;
     if (!std::regex_search(url, sm, repo_regex)) {
     	fprintf(stderr, "parsing repo url, no match\n");
     	exit(-1);
     }
     base_url = sm[1];
     workgroup = sm[2];
     repo_name = sm[3];
     fprintf(stdout, "repo name %s, workgroup %s, base %s\n", repo_name.c_str(), workgroup.c_str(), base_url.c_str());
     mxml_parse_submodules(node);
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUDRepo::mxml_parse_submodules(mxml_node_t* node)
{
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
	std::string url = "https://api.github.com/repos/" + workgroup + "/" + repo_name + "/branches";
	GHUDNS::GitApiRequest request(url, ghud->token());
     request.perform();
	return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::delete_branch(std::string branch)
{
     std::string url = "https://api.github.com/repos/" + workgroup + "/" + repo_name + "/git/refs/heads/" + branch;
     GHUDNS::GitApiDeleteRequest request(url, ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::create_branch(std::string branch, std::string sha)
{
     std::string url = "https://api.github.com/repos/" + workgroup + "/" + repo_name + "/git/refs";
     nlohmann::json data;
     data["ref"] = "refs/heads/" + branch;
     data["sha"] = sha;
     std::string postdata = data.dump();
     GHUDNS::GitApiPostRequest request(url, ghud->token(), postdata);
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
nlohmann::json GHUDNS::GHUDRepo::get_branch_head_commit()
{
     std::string url = "https://api.github.com/repos/" + workgroup + "/" + repo_name + "/commits/" + branch;
     GHUDNS::GitApiRequest request(url, ghud->token());
     request.perform();
     return request.j_reply();
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUDRepo::process()
{
	nlohmann::json branches = list_branches();
     if (branches.empty()) {
          fprintf(stderr, "branch check failed\n");
          exit(-1);
     }
     nlohmann::json curnode;
     nlohmann::json updnode;
     for (auto& brnode : branches.items()) {
          if (brnode.value().contains("name") && brnode.value()["name"] == branch) {
               curnode = brnode;
          }
          else if (brnode.value().contains("name") && brnode.value()["name"] == update_branch_name) {
               updnode = brnode;
          }
     }
     if(!curnode.empty())
          fprintf(stdout, "branch %s exists.\n", branch.c_str());
     else {
          fprintf(stdout, "branch %s doesn\'t exist.\n", branch.c_str());
          exit(-1);
     }
     fprintf(stdout, "checking head commit of ranch %s\n", branch.c_str());
     curnode = get_branch_head_commit();
     if(curnode.empty()) {
          fprintf(stdout, "failed to get head commit of %s.\n", branch.c_str());
          exit(-1);
     }

     if(!updnode.empty()) {
          fprintf(stdout, "branch %s exists. Deleting\n", update_branch_name.c_str());
          delete_branch(update_branch_name);
     }
     std::string sha = curnode["sha"];
     fprintf(stderr, "forking %s branch from %s commit %s\n", update_branch_name.c_str(),
                                                                 branch.c_str(),
                                                                 sha.c_str());
     create_branch(update_branch_name, sha);
}
//--------------------------------------------------------------------------------------------------------------------------
