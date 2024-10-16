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
        
        httplib::Server *svr_ = new httplib::Server();
        svr_->set_mount_point("/", "../www");

        svr_->listen("0.0.0.0", 8888);
    } else {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        
        execlp("/usr/bin/make", "make",  "-f", argv[1], "--no-print-directory", nullptr);
    }

    return 0;
}

