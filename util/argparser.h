#pragma once
#include<map>
#include<string>
#include<assert.h>
#define Var2Str(x) (#x)
#define GetIntVar(x) if(argmap.find(Var2Str(x))!=argmap.end())\
    {\
        x=atoi(argmap[Var2Str(x)].c_str());\
    }\

/**
 * return a map of <argName, argValue> as  <key, value>
 * argName should start with - or --
 */ 
static std::map<std::string,std::string> parseArgs(int argc,char** argv)
{
    std::map<std::string,std::string> argmap;
    assert(("number of args must be multiples of 2",argc&1==1));
    for(int i=1;i!=argc;i+=2)
    {
        assert(("arg name should start with -",argv[i][0]=='-'));
        assert(("arg value shouldn't start with -",argv[i+1][0]!='-'));
        if(argv[i][1]=='-')
        {
            argmap.emplace(std::make_pair(argv[i]+2,argv[i+1]));
        }
        else 
        {
            argmap.emplace(std::make_pair(argv[i]+1,argv[i+1]));
        }
    }
    return argmap;
}
