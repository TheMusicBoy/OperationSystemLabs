#include <iostream>
#include <subprocess/subprocess.h>
#include <common/log.h>
#include <synced_counter.h>

#include <filesystem>
#include <thread>
#include <chrono>
#include <functional>

#include <unistd.h>

namespace {

////////////////////////////////////////////////////////////////////////////////

void SetupLog() {
    NLogging::TLoggingPipe::GetInstance()->SetLogFile("./data/log.txt");
}

void IncreamentCycle(const std::filesystem::path& cachePath) {
    NCounter::TSyncedCounter counter(cachePath);
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ++counter;
        LOG("Counter value: {}", int(counter));
    }
}

std::string GetCurrentDateStr() {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string time = std::ctime(&now);
    time.pop_back();
    return time;
}

void PrintTimeAndPidCycle() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        LOG("Date: {}, Pid: {}", GetCurrentDateStr(), getpid());
    }
}

void FirstCopy(const std::filesystem::path& cachePath) {
    LOG("Date: {}, Pid: {}", GetCurrentDateStr(), getpid());
    NCounter::TSyncedCounter counter(cachePath);
    counter += 10;
    LOG("Date: {}", GetCurrentDateStr());
}

void SecondCopy(const std::filesystem::path& cachePath) {
    LOG("Date: {}, Pid: {}", GetCurrentDateStr(), getpid());
    NCounter::TSyncedCounter counter(cachePath);
    counter *= 2;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    counter /= 2;
    LOG("Date: {}", GetCurrentDateStr());
}

void CreateCopiesCycle(const std::filesystem::path& cachePath) {
    std::future<int> firstCopy;
    std::future<int> secondCopy;
    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (!firstCopy.valid() || firstCopy.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) {
            firstCopy = NSubprocess::CreateSubprocess(std::bind_front(&FirstCopy, cachePath));
        }
        if (!secondCopy.valid() || secondCopy.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) {
            secondCopy = NSubprocess::CreateSubprocess(std::bind_front(&SecondCopy, cachePath));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

} // namespace

int main() {
    SetupLog();

    LOG("Date: {}, Pid: {}", GetCurrentDateStr(), getpid());

    std::thread(std::bind(&PrintTimeAndPidCycle)).detach();
    std::thread(std::bind(&IncreamentCycle, std::filesystem::temp_directory_path() / "mycounter.bin")).detach();
    std::thread(std::bind(&CreateCopiesCycle, std::filesystem::temp_directory_path() / "mycounter.bin")).detach();

    NCounter::TSyncedCounter counter(std::filesystem::temp_directory_path() / "mycounter.bin");
    while (true) {
        int newValue;
        std::cin >> newValue;
        counter = newValue;
    }
}
