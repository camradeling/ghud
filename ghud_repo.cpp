#include <iostream>
//--------------------------------------------------------------------------------------------------------------------------
#include "ghud_repo.h"
//--------------------------------------------------------------------------------------------------------------------------
GHUDNS::GHUDRepo::GHUDRepo(mxml_node_t* node)
{
	mxml_node_t* urlnode;
    urlnode = mxmlFindElement(node, node, "url", NULL, NULL, MXML_DESCEND);
    if(!urlnode)
    {
    	std::cout << "url not found" << std::endl;
    	exit(-1);
    }
    url = (char *) mxmlGetText(urlnode, NULL);
    urlnode = mxmlFindElement(node, node, "branch", NULL, NULL, MXML_DESCEND);
    if(!urlnode)
    {
    	std::cout << "branch name not found" << std::endl;
    	exit(-1);
    }
    branch = (char *) mxmlGetText(urlnode, NULL);
    mxml_parse_submodules(node);
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUDRepo::mxml_parse_submodules(mxml_node_t* node)
{
	mxml_node_t* curnode;
    for (curnode = mxmlFindElement(node, node, "submodule", NULL, NULL, MXML_DESCEND);
         curnode != NULL;
         curnode = mxmlFindElement(curnode, node, NULL, NULL, NULL, MXML_NO_DESCEND))
    {
    	submodules.push_back(GHUDNS::GHUDRepo(curnode));
    }
}
//--------------------------------------------------------------------------------------------------------------------------