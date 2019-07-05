#ifndef WEBTRAVELER_NETWORK_H
#define WEBTRAVELER_NETWORK_H
#include <string>

class Network {
public:
    /**
     * This function will make a GET request and return response to you.
     * @param url an url address
     * @return response of server
     */
    static std::string request(const std::string &url);

    static uint write_cb(char *in, uint size, uint nmemb, std::string *out);
};


#endif //WEBTRAVELER_NETWORK_H
