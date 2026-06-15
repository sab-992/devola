#pragma once

#ifdef __linux__
    #include <core/exception.h>
    #include <core/process/process.h>
    #include <core/str/split.h>
    #include <string>
    #include <sys/wait.h>
    #include <unistd.h>


    namespace process_n
    {
        class Factory {
        private:
            using pipeFds_t = std::array<int, 2>;

        public:
            Factory() = delete;
            ~Factory() = default;

            static Process create(const std::string& executable, std::vector<std::string> stringArgs, bool waitUntilReady);

        private:
            inline static const uint8_t WRITE = 1;
            inline static const uint8_t READ = 0;

            static void addReadySequenceArgs(std::vector<std::string>& vector, pipeFds_t fds);
            static std::vector<const char*> convertToCharVector(const std::vector<std::string>& vector);
            static processId_t startProcess(const std::vector<const char*> args, pipeFds_t fds);
            static bool waitForReadySequence(int* pipeFds);
        };
    }
#endif