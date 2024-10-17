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
#include <json.hpp>
using json = nlohmann::json;

struct AgentApplication {
    enum  AgentState {
        ST_RUNNING,
        ST_WAITTING,
        ST_ENDING,
    } ;

    AgentApplication(int sfd, int ufd) {
        sfd_ = sfd;
        ufd_ = ufd;
        svr_ = new httplib::Server();
        state_ = ST_RUNNING;
    }
    ~AgentApplication() {
        svr_->stop();
        delete svr_;
    }
    void run() {
        svr_->Post("/talk", [&](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
            AgentState state;
            std::vector<std::string> hist;
            {
                std::lock_guard lk(mt_);
                state = state_;
                hist = hist_;
            }
            if ( state == ST_RUNNING ) {
                json msg;
                msg["error"] = "Agent is busy!";
                res.set_content(msg.dump(), "application/json");
                return;
            }
            if ( state == ST_ENDING) {
                json msg;
                msg["error"] = "Agent is over!";
                res.set_content(msg.dump(), "application/json");
                return;
            }
            if (req.is_multipart_form_data()) {
                res.set_content("{'error': 'Don't support multipart post!'}\r\n\r\n", "application/json");
                return;
            }
            
            std::string body;
            content_reader([&](const char *data, size_t data_length) {
                    body.append(data, data_length);
                    return true;
                });
            
            std::cout << body << std::endl;
        });
        svr_->Post("/listen", [&](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
            AgentState state;
            std::vector<std::string> hist;
            {
                std::lock_guard lk(mt_);
                state = state_;
                hist = hist_;
            }
            json msg;
            if ( state == ST_WAITTING ) {
                msg["state"] = "waitting";                
            } else if ( state == ST_RUNNING ) {
                msg["state"] = "running";
            } else {
                msg["state"] = "ending";
            }
            json talk;
            for(size_t i = 0; i < hist.size(); i++) {
                talk.push_back( hist[i] );
            }
            msg["talk"] = talk;
            res.set_content(msg.dump(), "application/json");
        });
        
        svr_->set_mount_point("/", "../www");
        svr_->listen("0.0.0.0", 8888);
    }
    void make() {
        // Reading make's output from sfd, write make's 
        const size_t LEN = 1024*16;
        char buffer[LEN];
        std::string lines = "";
        while(true) {
            int len = read(sfd_, buffer, LEN);
            if ( len <= 0) {
                break;
            }
            buffer[len] = 0;
            std::string message = buffer;

            if ( message.rfind("|=>", 0) == 0 ) {
                std::lock_guard lk(mt_);
                state_ = ST_WAITTING;
                if ( lines != "") {
                    hist_.push_back(lines);
                    lines = "";
                }
            } else if (message.rfind("(=>", 0) == 0) {
                std::lock_guard lk(mt_);
                state_ = ST_RUNNING;
                lines = "";
            } else if (message.rfind("(=>", 0) == 0) {
                std::lock_guard lk(mt_);
                hist_.push_back(lines);
                lines = "";
            } else {
                lines = lines + message;
            }
        }
        {
            std::lock_guard lk(mt_);
            state_ = ST_ENDING;
        }
    }
private:
    AgentState state_;
    httplib::Server* svr_;
    std::vector<std::string> hist_;
    std::mutex mt_;
    int sfd_;
    int ufd_;
};

int main(int argc, const char* argv[] ) {
    if ( argc < 2) {
        std::cout << "please input Makefile!" << std::endl;
        return -1;
    }

    int userfd[2];
    int assisfd[2];
    if ( pipe(assisfd) < 0) {
        std::cout << "Create assist pipe error!" << std::endl;
        return -1;
    }
    if ( pipe(userfd) < 0) {
        std::cout << "Create user pipe error!" << std::endl;
        return -1;
    }

    if (fork() != 0) { // parent process
        close(assisfd[1]);
        close(userfd[0]);
        
        AgentApplication* agent = new AgentApplication(assisfd[0], userfd[1]);
        
        std::thread tserver([&] {
            agent->make();        
        });
        agent->run();

        tserver.join();
        delete agent;
    } else {
        close(assisfd[0]);
        close(userfd[1]);

        dup2(assisfd[1], STDOUT_FILENO);
        //dup2(userfd[1], STDIN_FILENO);
        execlp("/usr/bin/make", "make",  "-f", argv[1], "--no-print-directory", nullptr);
    }
    return 0;
}

