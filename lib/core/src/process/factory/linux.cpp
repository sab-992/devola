#include <core/process/detail/factory/linux.hpp>

#ifdef __linux__
    void process_n::Factory::addReadySequenceArgs(std::vector<std::string>& vector, pipeFds_t fds) {
        vector.push_back("--ready-fd");
        vector.push_back(std::to_string(fds[WRITE]));
    }

    std::vector<const char*> process_n::Factory::convertToCharVector(const std::vector<std::string>& vector) {
        std::vector<const char*> result(vector.size() + 1);
        for (size_t i = 0; i < vector.size(); i++)
            result[i] = vector[i].c_str();

        result[result.size() - 1] = nullptr;
        return result;
    }

    Process process_n::Factory::create(const std::string& executable, std::vector<std::string> stringArgs, bool waitUntilReady) {
        stringArgs.insert(stringArgs.begin(), executable);

        pipeFds_t pipeFds;
        if (waitUntilReady) {
            pipe(pipeFds.data());
            addReadySequenceArgs(stringArgs, pipeFds);
        }

        std::vector<const char*> args = convertToCharVector(stringArgs);
        Process process(startProcess(args, pipeFds));

        if (waitUntilReady)
            if (not waitForReadySequence(pipeFds.data()))
                throw Exception(std::format("Child process #{}: Failure during ready sequence.", process.id()));

        return process;
    }

    processId_t process_n::Factory::startProcess(const std::vector<const char*> args, pipeFds_t fds) {
        processId_t pid = fork();
        if (pid < 0)
            throw Exception("Error while creating process");

        if (pid == 0) {
            close(fds[READ]);
            execvp(args[0], const_cast<char* const*>(args.data()));
            std::exit(-1);
        }

        return pid;
    }

    bool process_n::Factory::waitForReadySequence(int* pipeFds) {
        close(pipeFds[WRITE]);

        char buf[6] = {};
        read(pipeFds[READ], buf, 5);

        close(pipeFds[READ]);

        if (strcmp(buf, "READY") == 0)
            return true;

        return false;
    };
#endif