#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;

int main(int argc, const char* argv[]) {
    if ( argc != 4 ) {
        std::cout << "./chat history.json user.txt assistant.txt" << std::endl;
        return -1;
    }

    const std::string history_file = argv[1];
    const std::string user_file = argv[2];
    const std::string assistant_file = argv[3];

    const char* key = std::getenv("API_KEY");
    if ( key == nullptr) {
        std::cout << "Can't get API_KEY enviroment." << std::endl;
        return -1;
    }
    std::string api_key = std::string("Bearer ") + key;

    httplib::SSLClient cli("api.moonshot.cn");
    cli.enable_server_certificate_verification(false);
    cli.set_read_timeout(120, 0);

    httplib::Headers info;
    info.emplace("Authorization", api_key);

    json data;
    data["model"] = "moonshot-v1-8k";
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
        {
            messages.push_back(  rdata["choices"][0]["message"] );
            std::ofstream oof(history_file);
            oof << std::setw(4) << messages << std::endl;
        }
        {
            std::ofstream oof(assistant_file);
            oof << rdata["choices"][0]["message"]["content"].get<std::string>();
        }
    } else {
        std::cout << res.error() << std::endl;
    }
}

