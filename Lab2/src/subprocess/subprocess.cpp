#include <subprocess.h>

#include <iostream>

#include <vector>
#include <thread>
#include <string.h>
#include <memory>

// #define _WIN32

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace NSubprocess {

////////////////////////////////////////////////////////////////////////////////

std::future<int> CreateSubprocess(std::string cmd) {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

#if defined(_WIN32) || defined(_WIN64)
    std::unique_ptr<STARTUPINFOA> si = std::make_unique<STARTUPINFOA>();
    std::unqieu_ptr<PROCESS_INFORMATION> pi = std::make_unique<PROCESS_INFORMATION>();

    ZeroMemory(si.get(), sizeof(si));
    si->cb = sizeof(*si);
    ZeroMemory(pi.get(), sizeof(pi));

    if (!CreateProcessA(
            nullptr,
            cmd.c_str(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi
    )) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        promise.set_value(-1);
        return;
    }


    std::thread([cmd, promise = std::move(promise)]() mutable {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
            promise.set_value(-1);
        } else {
            promise.set_value(static_cast<int>(exitCode));
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }).detach();
#else
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("failed fork process");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        char* text = cmd.data();

        std::vector<char*> args;
        while (char* token = strtok_r(text, " ", &text)) {
            args.push_back(token);
        }

        execvp(args[0], args.data());
        perror("failed execvp process");
        exit(EXIT_FAILURE);
    }

    std::thread([pid] (std::promise<int> p) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            p.set_value(WEXITSTATUS(status));
        } else {
            p.set_value(-1);
        }
    }, std::move(promise)).detach();

    return future;
#endif
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NSubprocess
