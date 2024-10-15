#include <iostream>
#include <chrono>
#include <tuple>
#include <list>
#include <unistd.h>
#include <sys/wait.h>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <sstream>
#include <thread>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

int main(int argc, const char* argv[] ) {
    if ( argc < 2) {
        std::cout << "please input Makefile!" << std::endl;
        return -1;
    }

    int fd[2];
    if ( pipe(fd) < 0) {
        std::cout << "Create pipe error!" << std::endl;
        return -1;
    }

   	if (fork() != 0) { // parent process
        close(fd[1]); // Close the write end of the pipe

        unsigned char words[1024];
        while(1) {
            int len = read(fd[0], words, 1024);
            if ( len <= 0 ) {
                break;
            }
            words[len] = 0;
            std::cout << words << std::endl;
        }
    } else {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        
        execlp("/usr/bin/make", "make",  "-f", argv[1], "--no-print-directory", nullptr);
    }

    return 0;
}

