#ifndef WEBTRAVELER_NETWORK_H
#define WEBTRAVELER_NETWORK_H
#include <string>
#include <curlwrapper.h>

class Network {
public:
    /**
     * This function will check the DOMAIN name is existing.
     * @param url domain name without protocol (like google.com)
     * @return if domain name exist it'll return true else return false
     */
    static bool checkDNS(const std::string &url);

    /**
     * This function will make a GET request and return response to you.
     * @param url an url address
     * @return response of server
     */
    static std::string request(const std::string &url);
};


#endif //WEBTRAVELER_NETWORK_H
