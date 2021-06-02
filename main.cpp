#include "Console.h"
#include "Logger.h"

int main(){

    LOG::LEVEL = LogLevel::INFO;
    LOG::PRINT_INFO_LEVEL = false;

    Console console;

//    console.setDebugStatus(true);
//    console.setDebugMode(ModeApp::Mesh);
//    console.setFileNameDebug("first.txt");

//    console.setNumberX(2);
//    console.setNumberY(2);

//    Mesh::setDebugMode(true);
//    Mesh::setXDebug(0);
//    Mesh::setYDebug(1);

    console.start();

    return 0;
}

