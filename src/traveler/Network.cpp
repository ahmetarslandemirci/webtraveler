#include "Network.h"
#include <curl/curl.h>
#include <iostream>


uint Network::write_cb(char *in, uint size, uint nmemb, std::string *out) {
    out->append(in);
    return size * nmemb;
}


std::string Network::request(const std::string &url) {
    CURL *curl;
    std::string source = "", user_agent;
    user_agent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_3) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.1 Safari/605.1.15";
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &source);
        curl_easy_perform(curl);

        curl_easy_cleanup(curl);
    }
    return source;
}


std::string Network::get_redirected_url(const std::string &url) {
    std::string redirected_url = "";
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            char *url = NULL;
            curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
            if(url) {
                redirected_url = url;
                free(url);
            }
        }
        curl_easy_cleanup(curl);
    }
    return redirected_url;
}

