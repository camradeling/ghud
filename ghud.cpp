#include <iostream>
//--------------------------------------------------------------------------------------------------------------------------
#include "ghud.h"
//--------------------------------------------------------------------------------------------------------------------------
GHUDNS::GHUD::GHUD(mxml_node_t* confnode)
{
	mxml_node_t* curnode;
	curnode = mxmlFindElement(confnode, confnode, "user_token", NULL, NULL, MXML_DESCEND);
	if (!curnode) {
		std::cout << "user token not found" << std::endl;
		exit(-1);
	}
	user_token = (char *) mxmlGetText(curnode, NULL);
    for (curnode = mxmlFindElement(confnode, confnode, "repo", NULL, NULL, MXML_DESCEND);
	         curnode != NULL;
	         curnode = mxmlFindElement(curnode, confnode, NULL, NULL, NULL, MXML_NO_DESCEND)) {
    	GHUDNS::GHUDRepo repo(curnode);
    	repos.push_back(repo);
    }
}
//--------------------------------------------------------------------------------------------------------------------------
