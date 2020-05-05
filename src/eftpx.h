#ifndef EFTPX_H
#define EFTPX_H
#include <string>
#include <vector>
#include <iostream>
#include "json.hpp"

/* Global variables */
extern int G_VERBOSE;
extern int G_MULTITHREAD;
extern std::string G_LOOT_SCENE;
extern std::string G_CONTAINER_TYPE;
extern nlohmann::json G_DROPTABLE;

void parseArgs(int argc, char* argv[]);

void loadDroptable();
void setLootScene(const char*);
void setContainerType(const char*);
void setVerbose(int);
void setMultithreaded(int);
void updateLoot(std::string, int);

void initEFT();

/* Utilities */
void die(std::string);
nlohmann::json loadJson(const char*);
template<typename T> void vprint(T const& t){
    if(G_VERBOSE)
        std::cout << t;
}
std::vector<std::string> lsdir(const char*);
std::string formatItemname(std::string s);
void replaceAll(std::string& s, std::string targ, std::string repl);


#endif
