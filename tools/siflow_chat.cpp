#include <unistd.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;


int main(int argc, const char* argv[]) {
    if ( argc < 6 ) {
        std::cout << "./chat model_name history.json user.txt assistant.txt new_history.json [output.json]" << std::endl;
        return -1;
    }

    const std::string model = argv[1];
    const std::string history_file = argv[2];
    const std::string user_file = argv[3];
    const std::string assistant_file = argv[4];
    const std::string _history_file = argv[5];
    const std::string json_file = argc >= 7 ? argv[6] : "";

    const char* key = std::getenv("SIFLOW_API_KEY");
    if ( key == nullptr) {
        std::cout << "Can't get SIFLOW_API_KEY enviroment." << std::endl;
        return -1;
    }
    std::string api_key = std::string("Bearer ") + key;

    httplib::SSLClient cli("api.siliconflow.cn");
    cli.enable_server_certificate_verification(false);
    cli.set_read_timeout(300, 0);

    httplib::Headers info;
    info.emplace("Authorization", api_key);
    info.emplace("accept", "application/json");

    json data;
    data["model"] = model;
    data["temperature"] = 0.3;

    json messages;
    {
        std::ifstream iif(history_file);
        iif >> messages;
    }
    {
        json umsg;
        umsg["role"] = "user";

        std::ifstream iif(user_file);
        std::ostringstream ss;
        ss << iif.rdbuf();
        umsg["content"] = ss.str();

        messages.push_back(umsg);
    }
    data["messages"] = messages;

    // path, headers, body, content_type
    std::string body = data.dump();
    auto res = cli.Post("/v1/chat/completions", info, body, "application/json");

    if ( res ) {
        json rdata = json::parse( res.value().body );
        if ( rdata["choices"].is_null() ) {
            std::cout << res.value().body << std::endl;
            return -1;
        }
        {
            messages.push_back(  rdata["choices"][0]["message"] );
            std::ofstream oof(_history_file);
            oof << std::setw(4) << messages << std::endl;
        }
        {
            std::ofstream oof(assistant_file);
            oof << rdata["choices"][0]["message"]["content"].get<std::string>();
        }

        // parsing JSON
        if ( json_file != "") {
            std::string c = rdata["choices"][0]["message"]["content"].get<std::string>();
            auto first = c.find("```JSON") == std::string::npos ?  c.find("```json") : c.find("```JSON");
            auto last = first;
            if (first != std::string::npos) {
                last = c.find("```", first+3);
            }
            if ( first != std::string::npos && last != std::string::npos ) {
                c = c.substr(first + 7, last - first - 7);
            }
            std::ofstream oof(json_file);
            oof << c;
        }
    } else {
        std::cout << res.error() << std::endl;
        return -1;
    }
    return 0;
}

