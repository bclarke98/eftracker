#include "gfx.h"
#include "eftpx.h"
#include <thread>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

Mat cropImage(Mat& src, double x, double y, double width, double height){
    cv::Rect c;
    c.x = x;
    c.y = y;
    c.width = width;
    c.height = height;
    return src(c);
}

int searchLootSceneFor(Mat scene, Mat target){
    return searchLootSceneFor(scene, target, 0);
}

int searchLootSceneFor(Mat scene, Mat target, int showTarget){
    Mat field = scene.clone();
    if (scene.size().width != 1920 || scene.size().height != 1080)
        resize(scene, field, cv::Size(1920, 1080));
    Mat loot = cropImage(field, 1260, 0, 660, 360);
    Mat res(loot.rows - target.rows + 1, loot.cols - target.cols + 1, CV_32FC1);
    matchTemplate(loot, target, res, TM_CCOEFF_NORMED);
    int found = 0;
    for(;;){
        double minVal, maxVal, threshold = 0.8;
        Point minLoc, maxLoc;
        minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc);
        if(maxVal >= threshold){
            rectangle(loot,
                    maxLoc,
                    Point(maxLoc.x + target.cols, maxLoc.y + target.rows),
                    CV_RGB(0,255,0), 2);
            floodFill(res, maxLoc, 0);
            found++;
        }else{
            break;
        }
    }
    //imshow("Result", loot);
    if(showTarget)
        imshow("Target", target);
    return found;
}

void searchWrapper(Mat scene, Mat target, string itemname){
    int r = searchLootSceneFor(scene, target);
    vprint("[" + string(r ? "+" : "-") + "]  " + itemname + "\n");
    updateLoot(itemname, r);
}

void searchLootSceneIterative(Mat scene, vector<string> items){
    map<string, Mat> targets;
    for (auto i : items){
        Mat t = loadimg("img/" + formatItemname(i) + ".png", 0);
        if(!t.empty())
            targets[i] = t;
    }
    for (auto it = targets.begin(); it != targets.end(); ++it){
        string n = it->first;
        Mat targ = it->second;
        searchWrapper(scene, targ, n);
    }
}

void searchLootScene(Mat scene, vector<string> items){
    vector<thread> threads;
    map<string, Mat> targets;
    for (auto i : items){
        Mat t = loadimg("img/" + formatItemname(i) + ".png", 0);
        if(!t.empty())
            targets[i] = t;
        else
            cout << "CANNOT LOAD IMAGE \"" << i << endl;
    }
    for (auto it = targets.begin(); it != targets.end(); ++it){
        string n = it->first;
        Mat targ = it->second;
        threads.emplace_back(thread(searchWrapper, scene, targ, n));
    }
    for (auto& th : threads)
        th.join();
}

Mat loadimg(string path){
    return loadimg(path, 1);
}

Mat loadimg(string path, int dieOnFail){
    Mat img = imread(path, IMREAD_GRAYSCALE);
    if(dieOnFail && img.empty())
        die("Cannot load image \"" + path + "\"");
    return img;
}

