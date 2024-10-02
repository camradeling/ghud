#ifndef GHUD_H
#define GHUD_H
//--------------------------------------------------------------------------------------------------------------------------
#include <vector>
#include <string>
//--------------------------------------------------------------------------------------------------------------------------
#include "mxml.h"
#include "ghud_repo.h"
//--------------------------------------------------------------------------------------------------------------------------
namespace GHUDNS
{
//--------------------------------------------------------------------------------------------------------------------------
class GHUD
{
public:
	GHUD(mxml_node_t* confnode);
	void process();
	void parse_reviewers(mxml_node_t* node);
	std::string token() {return user_token;}
	std::vector<GHUDReviewer> reviewers;
	std::string pr_template="";
	std::string api_url="";
public:
	std::string user_token;
	std::vector<GHUDRepo> repos;
};
//--------------------------------------------------------------------------------------------------------------------------
}//namespace GHUD
//--------------------------------------------------------------------------------------------------------------------------
#endif/*GHUD_H*/