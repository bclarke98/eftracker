#ifndef GFX_H
#define GFX_H
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

cv::Mat loadimg(std::string);
cv::Mat loadimg(std::string, int);

int searchLootSceneFor(cv::Mat scene, cv::Mat target);
int searchLootSceneFor(cv::Mat scene, cv::Mat target, int showTarget);
void searchLootScene(cv::Mat scene, std::vector<std::string> items);
void searchLootSceneIterative(cv::Mat scene, std::vector<std::string> items);

#endif
