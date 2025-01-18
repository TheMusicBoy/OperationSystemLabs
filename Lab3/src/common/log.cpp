#include <iostream>
#include <log.h>

#include <exception.h>

#include <sys/file.h>
#include <unistd.h>

namespace NLogging {

////////////////////////////////////////////////////////////////////////////////

TLoggingPipe::TLoggingPipe() = default;

TLoggingPipe* TLoggingPipe::GetInstance() {
    if (!Instance_) {
        Instance_ = new TLoggingPipe();
    }
    return Instance_;
}

void TLoggingPipe::SetLogFile(const std::filesystem::path& path) {
    try {
        File_ = NFile::TFile(path);
    } catch (std::exception& ex) {
        THROW_EXCEPTION("Failed to set log file (Exception: {})", ex.what());
    }
}

void TLoggingPipe::Print(const std::string& message) {
    try {
        THROW_EXCEPTION_UNLESS(File_, "Log file not specified");
        File_.Lock();
        File_.Append(message + '\n');
        File_.Unlock();
    } catch (std::exception& ex) {
        THROW_EXCEPTION("Failed to write log / {}", ex);
    }
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NLogging
