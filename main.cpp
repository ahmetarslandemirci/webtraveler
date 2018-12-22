#include <iostream>
#include <Poco/URI.h>
#include <Poco/Net/NetSSL.h>
#include "Utils.h"

void dump(std::vector<std::string> vector) {
    for(int i=0;i<vector.size(); ++i)
        std::cout << vector.at(i) << std::endl;
}

std::vector<std::string> getMails(std::string url, std::vector<std::string> &searchedList,std::vector<std::string> &)

int main() {
    Poco::Net::initializeSSL();
    std::cout << "Hello, World!" << std::endl;
    std::string emailRegex  = "([\w\.-]+@[\w\.-]+)";
    std::string hrefRegex   = "href[ ]*=[ ]*[\"']?([^\"']+)";

    std::string url = "http://www.bekap.com/";
    //std::string url = "http://www.karyapark.com.tr";
    std::string source = Utils::request(url);
    std::vector<std::string> s = Utils::findAll(hrefRegex,source);


    std::vector<std::string> urls = Utils::fixUrls(s,url);
    dump(urls);


    Poco::Net::uninitializeSSL();
    return 0;
}