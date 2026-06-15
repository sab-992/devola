#pragma once

#ifdef __linux__
    #include <unistd.h>
    #include <sys/wait.h>


    using processId_t = pid_t;
#endif