#include <subprocess.h>

#include <vector>
#include <memory>
#include <thread>
#include <string.h>
#include <unistd.h>

namespace NSubprocess {

////////////////////////////////////////////////////////////////////////////////

std::future<int> CreateSubprocess(std::string cmd) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("failed fork process");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        char* text = cmd.data();

        std::vector<char*> args;
        while (char* token = strtok_r(text, " ", &text)) {
            args.push_back(token);
        }

        execvp(args[0], args.data());
        perror("failed execvp process");
        exit(EXIT_FAILURE);
    }

    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    std::thread([pid] (std::promise<int>&& promise) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            promise.set_value(WEXITSTATUS(status));
        } else {
            promise.set_value(-1);
        }
    }, std::move(promise)).detach();

    return future;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NSubprocess
