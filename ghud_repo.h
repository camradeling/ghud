#ifndef GHUD_REPO_H
#define GHUD_REPO_H
//--------------------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
//--------------------------------------------------------------------------------------------------------------------------
#include "mxml.h"
#include "pull_request.h"
//--------------------------------------------------------------------------------------------------------------------------
namespace GHUDNS
{
class GHUD;
//--------------------------------------------------------------------------------------------------------------------------
class GHUDRepo
{
public:
	GHUDRepo(mxml_node_t* node, GHUD* gh);
	void process();
	GHUD* ghud = nullptr;
	std::string repo_name="";
	std::string source_branch_name="";
	std::string update_branch_name = "";
	nlohmann::json source_branch_head_commit;
	nlohmann::json update_branch_head_commit;
	std::string update_pr_title="updating submodules hashes";
	std::string DESCRIPTION_PLACEHOLDER="";
	std::string PLATFORMS_PLACEHOLDER="";
	std::string JIRANUM_PLACEHOLDER="";
	std::string integrated_commits="";
private:
	std::shared_ptr<GHUDPullRequest> pr;
	void mxml_parse_submodules(mxml_node_t* node);
	std::string url="";
	std::string base_url="";
	std::string workgroup="";
	std::string path="";
	std::string parent_ref="";
	std::vector<GHUDRepo> submodules;
	nlohmann::json list_repos();
	nlohmann::json list_branches();
	nlohmann::json delete_branch(std::string branch);
	nlohmann::json create_branch(std::string branch, std::string sha);
	std::string update_submodules();
	nlohmann::json get_commit(std::string id);
	nlohmann::json get_branch_commits_since(std::string branch, std::string datetime);
	nlohmann::json get_tree(std::string sha);
	nlohmann::json move_branch_head(std::string branch, std::string sha);
};
//--------------------------------------------------------------------------------------------------------------------------
}//namespace GHUD
//--------------------------------------------------------------------------------------------------------------------------
#endif/*GHUD_REPO_H*/