#include <memory>
#include <errno.h>
#include <pty.h>
#include <string.h>
#include <unistd.h>

#include "vtbuffer.h"

void LaunchProgram(int argc, char* argv[])
{
    // execvp requires an extra nullptr as the last argument
    auto execArgs = std::make_unique<char*[]>(argc+1);
    for (int i = 0; i < argc; i++)
    {
        execArgs[i] = argv[i];
    }
    execArgs[argc] = nullptr;

    execvp(execArgs[0], execArgs.get());
    perror("execvp");
}

void CopyOutput(int fd)
{
    VTBuffer buffer(4096);

    for (;;) {
        // Read from fd, write to buffer
        auto bytesRead = read(fd, buffer.WritePointer(), buffer.WriteCapacity());
        if (bytesRead < 0)
        {
            if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
            {
                if (errno != EIO)
                    perror("read");
                break;
            }
        }
        else
        {
            buffer.BytesWritten(bytesRead);
            // Read from buffer, write to stdout
            auto bytesWritten = write(STDOUT_FILENO, buffer.ReadPointer(), buffer.ReadSize());
            if (bytesWritten < 0)
            {
                if (errno != EINTR)
                {
                    perror("write");
                    break;
                }
            }
            buffer.BytesRead(bytesWritten);
        }
    }
}

int main(int argc, char *argv[])
{
    int master;
    pid_t pid;

    pid = forkpty(&master, NULL, NULL, NULL);

    if (pid < 0)
    {
        perror("forkpty");
        return 1;
    }
    else if (pid == 0)
    {
        // Child
        LaunchProgram(argc - 1, argv + 1);
        return 2;
    }
    else
    {
        // Parent
        CopyOutput(master);
    }

    return 0;
}
