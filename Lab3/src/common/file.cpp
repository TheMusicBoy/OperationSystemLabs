#include <file.h>
#include <iostream>

#include <sys/file.h>
#include <unistd.h>

namespace NFile {

namespace {

////////////////////////////////////////////////////////////////////////////////

void ClearDesc(FILE** fileDesc) {
    if (*fileDesc) {
        fclose(*fileDesc);
        *fileDesc = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////////////////

TFile::TFile() = default;

TFile::TFile(const std::filesystem::path& filepath)
    : FilePath_(filepath)
{}

TFile::~TFile() {
    auto guard = std::lock_guard(Mutex_);
    if (LockFileDesc_) {
        Unlock();
    }
    ClearDesc(&WriteFileDesc_);
}

TFile& TFile::operator=(TFile&& other) {
    if (&other != this) {
        Unlock();
        ClearDesc(&WriteFileDesc_);

        WriteFileDesc_ = other.WriteFileDesc_;
        LockFileDesc_ = other.LockFileDesc_;
        FilePath_ = std::move(other.FilePath_);

        other.WriteFileDesc_ = nullptr;
        other.LockFileDesc_ = 0;
    }
    return *this;
}

TFile::operator bool() const {
    return !FilePath_.empty();
}

void TFile::Lock() {
    auto guard = std::lock_guard(Mutex_);

    if (LockFileDesc_) {
        return;
    }

    std::filesystem::create_directory(FilePath_.parent_path());
    int TempFileDesc_ = open(FilePath_.c_str(), O_CREAT | O_RDONLY, S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR);
    THROW_EXCEPTION_IF(TempFileDesc_ < 0, "Failed to lock file / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);
    THROW_EXCEPTION_IF(flock(TempFileDesc_, LOCK_EX), "Failed to lock file (Errno: {}, FilePath: {})", errno, FilePath_);
    LockFileDesc_ = TempFileDesc_;
}

void TFile::Unlock() {
    auto guard = std::lock_guard(Mutex_);

    if (!LockFileDesc_) {
        return;
    }

    THROW_EXCEPTION_IF(flock(LockFileDesc_, LOCK_UN), "Failed to unlock file (Errno: {}, FilePath: {})", errno, FilePath_);
    THROW_EXCEPTION_IF(close(LockFileDesc_) < 0, "Failed to close file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);
    LockFileDesc_ = 0;
}

void TFile::Read(std::string* output) {
    auto guard = std::lock_guard(Mutex_);

    FILE* ReadFileDesc_ = fopen(FilePath_.c_str(), "r");
    THROW_EXCEPTION_UNLESS(ReadFileDesc_, "Failed to read / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);

    fseek(ReadFileDesc_, 0, SEEK_END); 
    long long fileSize = ftell(ReadFileDesc_);
    fseek(ReadFileDesc_, 0, SEEK_SET); 

    output->resize(fileSize);
    fread(output->data(), fileSize, 1, ReadFileDesc_);
}

void TFile::Write(const std::string& data) {
    auto guard = std::lock_guard(Mutex_);

    ClearDesc(&WriteFileDesc_);

    WriteFileDesc_ = fopen(FilePath_.c_str(), "w");
    THROW_EXCEPTION_UNLESS(WriteFileDesc_, "Failed to write / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);

    fprintf(WriteFileDesc_, "%s", data.data());
    ClearDesc(&WriteFileDesc_);
}

void TFile::Append(const std::string& data) {
    auto guard = std::lock_guard(Mutex_);
    
    std::filesystem::create_directory(FilePath_.parent_path());

    WriteFileDesc_ = fopen(FilePath_.c_str(), "a");
    THROW_EXCEPTION_UNLESS(WriteFileDesc_, "Failed to write / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);
    fprintf(WriteFileDesc_, "%s", data.data());
    fclose(WriteFileDesc_);
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NFile
