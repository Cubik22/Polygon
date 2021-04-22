#include "Console.h"
#include "Logger.h"

int main(){

    LOG::LEVEL = LogLevel::DEBUG;
    LOG::PRINT_INFO_LEVEL = false;

    Console console;
    console.start();

    return 0;

//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
}

