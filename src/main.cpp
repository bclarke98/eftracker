#include <thread>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include "eftpx.h"

using namespace cv;
using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]){
    parseArgs(argc, argv);
    //int i;
    //for(i = 0; i < argc; i++){
    //    if (strcmp(argv[i], "-c") == 0 && i + 1 < argc)
    //        setContainerType(argv[++i]);
    //    if (strcmp(argv[i], "-l") == 0 && i + 1 < argc)
    //        setLootScene(argv[++i]);
    //    if (strcmp(argv[i], "-v") == 0)
    //        setVerbose(1);
    //    if (strcmp(argv[i], "-m") == 0)
    //        setMultithreaded(1);
    //}
    initEFT();
    return 0;
}
