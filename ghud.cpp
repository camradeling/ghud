#include <iostream>
#include <fstream>
//--------------------------------------------------------------------------------------------------------------------------
#include "ghud.h"
//--------------------------------------------------------------------------------------------------------------------------
GHUDNS::GHUD::GHUD(mxml_node_t* confnode)
{
	mxml_node_t* curnode;
	curnode = mxmlFindElement(confnode, confnode, "user_token_env", NULL, NULL, MXML_DESCEND);
	if (!curnode) {
		std::cout << "user_token_env not found" << std::endl;
		exit(-1);
	}
	char* user_token_env = (char *) mxmlGetText(curnode, NULL);
	if (!user_token_env) {
		fprintf(stderr, "user_token_env invalid\n");
		exit(-1);
	}
	char* env_p = nullptr;
	if ((env_p = std::getenv(user_token_env)) == nullptr) {
		fprintf(stderr, "user_token_env invalid\n");
		exit(-1);
	}
	user_token = std::string(env_p);
	for (curnode = mxmlFindElement(confnode, confnode, "repo", NULL, NULL, MXML_DESCEND);
			curnode != NULL;
			curnode = mxmlFindElement(curnode, confnode, NULL, NULL, NULL, MXML_NO_DESCEND)) {
    	GHUDNS::GHUDRepo repo(curnode, this);
    	repos.push_back(repo);
    }
    curnode = mxmlFindElement(confnode, confnode, "reviewers", NULL, NULL, MXML_DESCEND);
	if (!curnode) {
		std::cout << "reviewers node not found" << std::endl;
		exit(-1);
	}
	fprintf(stdout, "parsing reviewers\n");
	parse_reviewers(curnode);
	std::ifstream myfile ("pull_request.md");
	std::string line;
	while (getline(myfile, line))
		pr_template += line + "\n";
	fprintf(stdout, "print pr_template: %s\n", pr_template.c_str());
}
//--------------------------------------------------------------------------------------------------------------------------
void GHUDNS::GHUD::parse_reviewers(mxml_node_t* node)
{
	mxml_node_t* curnode;
	for (curnode = mxmlFindElement(node, node, "reviewer", NULL, NULL, MXML_DESCEND);
	         curnode != NULL;
	         curnode = mxmlFindElement(curnode, node, NULL, NULL, NULL, MXML_NO_DESCEND)) {
		mxml_node_t* textnode = mxmlFindElement(curnode, curnode, "name", NULL, NULL, MXML_DESCEND);
		if (textnode == NULL) {
			fprintf(stderr, "couldn't find reviewer name node\n");
			exit(-1);
		}
		std::string name = (char *) mxmlGetText(textnode, NULL);
		textnode = mxmlFindElement(curnode, curnode, "email", NULL, NULL, MXML_DESCEND);
		if (textnode == NULL) {
			fprintf(stderr, "couldn't find reviewer email node\n");
			exit(-1);
		}
		std::string email = (char *) mxmlGetText(textnode, NULL);
		fprintf(stdout, "adding reviewer %s <%s>\n",name.c_str(), email.c_str());
		reviewers.push_back((GHUDNS::GHUDReviewer){ .name=name, .email=email });
    }
}
//--------------------------------------------------------------------------------------------------------------------------
