#include "Console.h"
#include "Logger.h"

int main(){

    LOG::LEVEL = LogLevel::INFO;

    Console console;
    console.start();

    return 0;

//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
}

