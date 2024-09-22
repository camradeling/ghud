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
	std::string url;
	std::string base_url;
	std::string workgroup;
	std::string repo_name;
	std::string branch;
	std::vector<GHUDRepo> submodules;
	nlohmann::json list_repos();
	nlohmann::json list_branches();
	nlohmann::json delete_branch(std::string branch);
	nlohmann::json create_branch(std::string branch, std::string sha);
	nlohmann::json get_branch_head_commit();
	void process();
private:
	GHUD* ghud = nullptr;
	void mxml_parse_submodules(mxml_node_t* node);
};
//--------------------------------------------------------------------------------------------------------------------------
}//namespace GHUD
//--------------------------------------------------------------------------------------------------------------------------
#endif/*GHUD_REPO_H*/