#include "Console.h"
#include "Logger.h"

int main(){

    LOG::LEVEL = LogLevel::INFO;
    LOG::PRINT_INFO_LEVEL = false;

//    Mesh::setForceConcave(true);

    Console console;

    console.setDebugStatus(true);
    console.setDebugMode(ModeApp::Cut);
    console.setFileNameDebug("first.txt");

//    console.setNumberX(3);
//    console.setNumberY(3);

//    console.setNumberBorder(1);

//    Mesh::setDebugMode(true);
//    Mesh::setXDebug(2);
//    Mesh::setYDebug(0);

    console.start();

    return 0;
}

