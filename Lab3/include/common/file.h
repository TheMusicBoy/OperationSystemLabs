#pragma once

#include <exception.h>

#include <filesystem>
#include <mutex>

namespace NFile {

////////////////////////////////////////////////////////////////////////////////

class TFile {
public:
    TFile();

    TFile(const std::filesystem::path& filepath);

    TFile(TFile&& file);

    ~TFile();

    TFile& operator=(TFile&& other);

    operator bool() const;

    void Lock();
    void Unlock();

    void Write(const std::string& data);
    void Append(const std::string& data);

    void Read(std::string* output);

private:
    std::filesystem::path FilePath_;
    
    FILE* WriteFileDesc_ = nullptr;

    int LockFileDesc_ = 0;
    std::mutex Mutex_;

};

////////////////////////////////////////////////////////////////////////////////

} // namespace NFile
