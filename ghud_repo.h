#ifndef GHUD_REPO_H
#define GHUD_REPO_H
//--------------------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
//--------------------------------------------------------------------------------------------------------------------------
#include "mxml.h"
//--------------------------------------------------------------------------------------------------------------------------
namespace GHUDNS
{
class GHUD;
//--------------------------------------------------------------------------------------------------------------------------
class GHUDRepo
{
public:
	GHUDRepo(mxml_node_t* node, GHUD* gh);
	std::string url="";
	std::string base_url="";
	std::string workgroup="";
	std::string repo_name="";
	std::string source_branch_name="";
	std::string update_branch_name = "";
	std::string path="";
	nlohmann::json source_branch_head_commit;
	nlohmann::json update_branch_head_commit;
	std::vector<GHUDRepo> submodules;
	nlohmann::json list_repos();
	nlohmann::json list_branches();
	nlohmann::json delete_branch(std::string branch);
	nlohmann::json create_branch(std::string branch, std::string sha);
	nlohmann::json update_submodules();
	nlohmann::json get_branch_head_commit(std::string branch);
	nlohmann::json get_tree(std::string sha);
	nlohmann::json move_branch_head(std::string branch, std::string sha);
	nlohmann::json create_update_pull_request();
	nlohmann::json add_pull_request_reviewers(nlohmann::json pr);
	nlohmann::json check_pull_request_status(nlohmann::json pr);
	void process();
private:
	GHUD* ghud = nullptr;
	void mxml_parse_submodules(mxml_node_t* node);
};
//--------------------------------------------------------------------------------------------------------------------------
}//namespace GHUD
//--------------------------------------------------------------------------------------------------------------------------
#endif/*GHUD_REPO_H*/