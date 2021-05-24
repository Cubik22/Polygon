#include "Console.h"
#include "Logger.h"

int main(){

    LOG::LEVEL = LogLevel::WARN;
    LOG::PRINT_INFO_LEVEL = false;

    Console console;

    console.setNumberX(2);
    console.setNumberY(2);

//    console.setDebugMode(true);
//    console.setFileNameDebug("a.txt");

//    Mesh::setDebugMode(true);
//    Mesh::setXDebug(2);
//    Mesh::setYDebug(0);

    console.start();

    return 0;
}

