#ifndef EFTPX_H
#define EFTPX_H
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "json.hpp"

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

void initEFT();

cv::Mat loadimg(std::string);
cv::Mat loadimg(std::string, int);
nlohmann::json loadJson(const char*);
std::vector<std::string> lsdir(const char*);

int searchLootSceneFor(cv::Mat scene, cv::Mat target);
int searchLootSceneFor(cv::Mat scene, cv::Mat target, int showTarget);
void searchLootScene(cv::Mat scene, std::vector<std::string> items);
void searchLootSceneIterative(cv::Mat scene, std::vector<std::string> items);

std::string formatItemname(std::string s);
void replaceAll(std::string& s, std::string targ, std::string repl);


#endif
