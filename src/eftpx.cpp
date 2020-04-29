#include "eftpx.h"
#include <map>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <functional>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;
using json = nlohmann::json;

int G_VERBOSE = 0;
int G_MULTITHREAD = 0;
json G_DROPTABLE;
string G_LOOT_SCENE;
string G_CONTAINER_TYPE;

mutex G_MUTEX;
map<string, int> G_LOOTS;
map<string, function<int(void)>> CMDOPTS;
map<string, string> HELPOPT;

void addArg(string s, string help, function<int(void)> f){
    CMDOPTS[s.substr(0,2)] = f;
    HELPOPT[s] = help;
}

void sendHelp(){
    printf("%-80s\n", "EFTracker: Escape From Tarkov Loot Tracker");
    printf("%-80s\n", "[Usage]: ./eftpx -l [lootscreenshot.png] -c [container type] [...]");
    printf("%-80s\n", "\nArguments:");
    for(auto it = HELPOPT.begin(); it != HELPOPT.end(); ++it){
        printf("\t%-6s%8s%-30s\n", it->first.c_str(), "", it->second.c_str());
    }
    loadDroptable();
    printf("%-80s\n", "\nSupported container types:");
    for(auto it = G_DROPTABLE.begin(); it != G_DROPTABLE.end(); ++it){
        printf("\t- %-30s\n", it.key().c_str());
    }
    exit(0);
}

void parseArgs(int argc, char* argv[]){
    int i = 0;
    addArg("-c [C]", "Container type (case sensitive)", [&](){
        if(!argv[i]) return 0;
        setContainerType(argv[i]);
        return 1;
    });
    addArg("-v", "Enable verbose output", [&](){
        setVerbose(1);
        return 0;
    });
    addArg("-m", "Enable multithreading", [&](){
        setMultithreaded(1);
        return 0;
    });
    addArg("-l [L]", "Path to loot scene image", [&](){
        if(!argv[i]) return 0;
        setLootScene(argv[i]);
        return 1;
    });
    addArg("-h", "Displays this message", [&](){
        sendHelp();
        return 0;
    });
    while(i < argc){
        char* targ = argv[i++];
        if(CMDOPTS.find(targ) != CMDOPTS.end())
            i += CMDOPTS[targ]();
    }
}

void updateLoot(string itemname, int found){
    G_MUTEX.lock();
    G_LOOTS[itemname] = found;
    G_MUTEX.unlock();
}

void loadDroptable(){
    G_DROPTABLE = loadJson("dat/droptable.json");
}

void setLootScene(const char* s){
    G_LOOT_SCENE = string(s);
}

void setContainerType(const char* s){
    G_CONTAINER_TYPE = string(s);
}

void setVerbose(int i){
    G_VERBOSE = i;
}

void setMultithreaded(int i){
    G_MULTITHREAD = i;
}

template<typename T> void vprint(T const& t){
    if(G_VERBOSE)
        cout << t;
}

void die(string s){
    cerr << s << endl;
    exit(1);
}

Mat cropImage(Mat& src, double x, double y, double width, double height){
    cv::Rect c;
    c.x = x;
    c.y = y;
    c.width = width;
    c.height = height;
    return src(c);
}

void replaceAll(string& s, string targ, string repl){
    size_t pos;
    while ((pos = s.find(targ)) != string::npos){
        s.replace(pos, 1, repl);
    }
}

string formatItemname(string s){
    string r(s);
    replaceAll(r, " ", "_");
    replaceAll(r, "/", "-");
    return r;
}

vector<string> lsdir(const char* path){
    DIR *dir;
    struct dirent *ent;
    vector<string> v;
    if((dir = opendir(path)) != NULL){
        while((ent = readdir(dir)) != NULL)
            if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, ".."))
                v.push_back(string(path) + "/" + string(ent->d_name));
        closedir(dir);
    }else
        die("Cannot load directory \"" + string(path) + "\"");
    return v;
}

json loadJson(const char* path){
    json r;
    ifstream fs;
    fs.open(path);
    if (fs.fail())
        die("Cannot open json file \"" + string(path) + "\"");
    fs >> r;
    return r;
}

void initEFT(){
    if(G_LOOT_SCENE.empty())
        die("Please specify a loot scene image file with the '-l' option.");
    loadDroptable();
    if(G_DROPTABLE[G_CONTAINER_TYPE] == nullptr)
        die("Invalid container type: \"" + G_CONTAINER_TYPE + "\"");
    vprint("Verbose output enabled.\n");
    Mat lootScene = loadimg(G_LOOT_SCENE);
    vector<string> f_items;
    for(auto i : G_DROPTABLE[G_CONTAINER_TYPE]["drops"])
        f_items.push_back(i);
    if(G_MULTITHREAD)
        searchLootScene(lootScene, f_items);
    else
        searchLootSceneIterative(lootScene, f_items);

    cout << "Results of scanning \"" << G_LOOT_SCENE << "\":" << endl;
    for(auto it = G_LOOTS.begin(); it != G_LOOTS.end(); ++it)
        if (it->second)
            cout << "[+]  " << it->first << endl;
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

