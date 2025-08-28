#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <getopt.h>
//--------------------------------------------------------------------------------------------------------------------------
#include "mxml.h"
#include "ghud.h"
#include "gitapi_request.h"
//--------------------------------------------------------------------------------------------------------------------------
int main (int argc, char ** argv)
{
    std::string configfile = "config.xml"; //default
    std::string templatefile = "pull_request.md"; //default
    int option_index = 0;
    int c;
    static struct option long_options[] = {
            {"config",  required_argument,NULL,0},
            {"template",  required_argument,NULL,0},
            {NULL,    0,                 NULL,  0 }
    };
    while((c = getopt_long(argc, argv, "c:t:",long_options,&option_index)) != -1) {
        switch(c) {
            case 0:
                if(option_index == 0) {
                    configfile = std::string(optarg);
                }
                break;
            case 'c':
                configfile = std::string(optarg);
                break;
            case 't':
                templatefile = std::string(optarg);
                break;
            case '?':
                fprintf(stderr, "unknown option: %c\n", option_index);
                break;
        }
    }
    FILE* fpconf=nullptr;
    mxml_node_t* tree = nullptr;
    bool confok = true;
    // открыть файл с конфигурацией в формате XML
    if ((fpconf = fopen(configfile.c_str(), "r")) != NULL) {
        // считать конфигурацию
        tree = mxmlLoadFile (nullptr, fpconf, MXML_NO_CALLBACK);
        fclose(fpconf);
        if (tree == nullptr) {
            fprintf(stderr, "config file invalid\n");
            confok = false;
            exit(-1);
        }
    }
    else {
        fprintf(stderr, "cant open config file\n");
        confok = false;
        exit(-1);
    }
    std::ifstream myfile (templatefile);
    if (!myfile) {
        fprintf(stderr, "no PR template found");
        exit(-1);
    }
    std::string line;
    std::string pr_template;
    while (getline(myfile, line))
        pr_template += line + "\n";
    GHUDNS::GHUD ghud(tree, pr_template);

    for (auto repo : ghud.repos) {
        repo.process();
    }
    fprintf(stdout, "All updates finished\n");
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------------
