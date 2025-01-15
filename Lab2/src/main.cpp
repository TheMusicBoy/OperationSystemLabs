#include <iostream>
#include <subprocess/subprocess.h>

int main() {
    std::cout << "Start executing program!" << std::endl;
    auto p1 = NSubprocess::CreateSubprocess("src/test1 some text");
    std::cout << "Start count down" << std::endl;
    std::cout << "Subprocess returned status " << p1.get() << std::endl;

    std::cout << "Start executing program!" << std::endl;
    auto p2 = NSubprocess::CreateSubprocess("src/test2 text some");
    std::cout << "Start count down" << std::endl;
    std::cout << "Subprocess returned status " << p2.get() << std::endl;
}
