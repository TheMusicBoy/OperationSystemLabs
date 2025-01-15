#include <iostream>
#include <subprocess/subprocess.h>

int main() {
    std::cout << "Start executing program!" << std::endl;
    auto p1 = NSubprocess::CreateSubprocess("./a.out some text");
    std::cout << "Start count down" << std::endl;
    p1.wait();
    std::cout << "Printed value" << std::endl;
}
