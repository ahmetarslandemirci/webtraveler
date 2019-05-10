#include "Network.h"
#include <netdb.h>

#include <iostream> // for debug

bool Network::checkDNS(const std::string &url) {
    hostent *host = NULL;
    host = gethostbyname(url.c_str());
    if(host != NULL) {
        return true;
    }
    return false;
}


std::string Network::request(const std::string &url) {
    CurlWrapper *curl = CurlWrapper::create(true);
    curl->setOption(CURLOPT_URL, url.c_str());
    curl->setOption(CURLOPT_HEADER, 0L);
    curl->setOption(CURLOPT_FOLLOWLOCATION, 1L);
    curl->setOption(CURLOPT_TIMEOUT, 20L);
    curl->setOption(CURLOPT_SSL_VERIFYHOST, 0L);
    curl->setOption(CURLOPT_SSL_VERIFYPEER, 0L);
    curl->setOption(CURLOPT_MAXREDIRS, 5);
    using namespace std;
    string userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_3) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/11.1.1 Safari/605.1.15";
    map<string,string> headers;
    headers.insert(pair<string,string>("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"));
    headers.insert(pair<string,string>("Accept-Language","en-US,en;q=0.5"));
    headers.insert(pair<string,string>("User-Agent",userAgent));

    curl->setHeaders(headers);
    string response = curl->exec();
    delete curl;

    return response;
}

