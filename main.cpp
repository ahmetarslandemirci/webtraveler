#include <iostream>
#include <Poco/URI.h>
#include <Poco/Net/NetSSL.h>
#include "Utils.h"

#include <algorithm>
void dump(std::vector<std::string> vector) {
    std::cout << vector.size() << std::endl;
    for(int i=0;i<vector.size(); ++i)
        std::cout << vector.at(i) << std::endl;
}



int main() {
    Poco::Net::initializeSSL();
    std::cout << "Hello, World!" << std::endl;
    std::string emailRegex  = "([^ \"'<>:;,\}\{\)\(\\\/]+@[^ \"'<>:;,\}\{\)\(\\\/]+\.[a-zA-Z]+)";
    std::string hrefRegex   = "href[ ]*=[ ]*[\"']?([^\"']+)";

    //std::string url = "http://www.bekap.com/";
    //std::string url = "http://www.karyapark.com.tr";
    //std::string url = "http://www.duk.com.tr";
    std::string url = "https://www.isikpeyzaj.com/";
    std::string source = Utils::request(url);
    std::vector<std::string> s = Utils::findAll(hrefRegex,source);
    std::vector<std::string> urls = Utils::fixUrls(s,url);
    dump(urls);

    std::cout << "##########################"<<std::endl;
    std::vector<std::string> ne,data;
    Utils::searchInPages(url, ne, emailRegex, data, 1000);
    dump(data);

    Poco::Net::uninitializeSSL();
    return 0;
}