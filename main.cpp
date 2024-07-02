//--------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <curl/curl.h>
//--------------------------------------------------------------------------------------------------------------------------
#include "mxml.h"
#include "ghud.h"
#include "gitapi_request.h"
//--------------------------------------------------------------------------------------------------------------------------
std::string configfile = "config.xml";
//--------------------------------------------------------------------------------------------------------------------------
int main (int argc, char ** argv)
{
    FILE* fpconf=nullptr;
    mxml_node_t* tree = nullptr;
    bool confok = true;
    if ((fpconf = fopen(configfile.c_str(), "r")) != NULL)        // открыть файл с конфигурацией в формате XML
    {
        tree = mxmlLoadFile (nullptr, fpconf, MXML_NO_CALLBACK);       // считать конфигурацию
        fclose(fpconf);
        if (tree == nullptr)
        {
            fprintf(stderr, "config file invalid\n");
            confok = false;
            exit(-1);
        }
    }
    else
    {
        fprintf(stderr, "cant open config file\n");
        confok = false;
        exit(-1);
    }
    GHUDNS::GHUD ghud(tree);
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------------
