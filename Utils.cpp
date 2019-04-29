#include "Utils.h"
#include <Poco/RegularExpression.h>

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/Context.h>
#include <Poco/URI.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <Poco/TextConverter.h>
#include <Poco/Latin1Encoding.h>
#include <Poco/UTF8Encoding.h>
#include <curlwrapper.h>

// Regex ust contain one group ex: asd([0-9]+)dsa
std::vector<std::string> Utils::findAll(const std::string &regex, const std::string &source) {
    Poco::RegularExpression expr(regex);
    std::vector<std::string> firstVector, foundedVector;

    unsigned long offset = 0;
    unsigned long numberOf = 0;
    while(true) {
        try {
            numberOf = expr.split(source, offset, firstVector);
        } catch(Poco::Exception e) {
            std::cout << e.what() << std::endl;
            break;
        }
        catch( std::exception e) {
            break;
        }
        if(numberOf == 0) break;
        foundedVector.push_back(firstVector.at(1));

        unsigned long pos = source.find(firstVector.at(0),offset);

        if( pos != std::string::npos)
            offset = pos + firstVector.at(0).size() ;
        else break;
    }
    return foundedVector;
}

std::string Utils::request(std::string &url, int maxRedirect) {
    CurlWrapper* curlWrapper = CurlWrapper::create(true);
    std::cout << "URL: " << url << std::endl;
    curlWrapper->setOption(CURLOPT_URL,url.c_str());
    curlWrapper->setOption(CURLOPT_FOLLOWLOCATION, true);
    curlWrapper->setOption(CURLOPT_TIMEOUT, 10L);
    curlWrapper->setOption(CURLOPT_SSL_VERIFYHOST, 0L);
    curlWrapper->setOption(CURLOPT_SSL_VERIFYPEER, 0L);
    curlWrapper->appendHeader("User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_3) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/11.1.1 Safari/605.1.15");
    curlWrapper->appendHeader("Accept-Language: en-US,en;q=0.5");
    curlWrapper->appendHeader("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    std::string source = curlWrapper->exec();
    delete curlWrapper;
    return source;

}

std::vector<std::string> Utils::fixUrls(const std::vector<std::string> &urlList, const std::string &baseUrl) {
    Poco::URI baseUri(baseUrl);
    std::string base = baseUri.getHost();
    std::vector<std::string> uniqUrls;

    for(size_t i=0;i<urlList.size();i++) {
        std::string url = urlList.at(i);
        if(url.substr(0,4) == "http") {
            if (url == "#" || (url.size() >= 11 && url.substr(0, 11) == "javascript:")) {
                continue;
            } else if (url.size() >= 2 && url.at(0) == '/' && url.at(1) == '/') {
                // write http or https
            } else if (url.size() >= 1 && url.at(0) == '/') {
                if (baseUrl.at(baseUrl.size() - 1) == '/')
                    url = baseUrl + url.substr(1, url.size() - 1);
                else
                    url = baseUrl + url;
            }
        }
        else {

        }
        Poco::URI uri(url);

        // skip for different hosts
        if(baseUri.getHost() != uri.getHost())
            continue;

        // Check for file extensions
        unsigned long lastDot = uri.getPath().find_last_of('.');
        if(lastDot != std::string::npos) {
            std::string ext = uri.getPath().substr(lastDot, uri.getPath().size());
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if(ext == ".css" || ext == ".js" || ext == ".pdf" || ext == ".exe" || ext == ".png"
                || ext == ".jpg" || ext == ".svg" || ext == ".ico") continue;
        }

        // Check for already added urls
        std::vector<std::string>::iterator it;
        it = std::find(uniqUrls.begin(),uniqUrls.end(),url);
        if(it == uniqUrls.end())
            uniqUrls.push_back(url);
    }
    return uniqUrls;
}

void Utils::searchInPages(std::string &url, std::vector<std::string> &nextList, const std::string &regex, std::vector<std::string> &data, unsigned long limit) {
    std::string source = Utils::request(url);
    std::vector<std::string> founded = Utils::findAll(regex, source);
    std::vector<std::string>::iterator it;

    //std::cout << founded.size() << std::endl;
    for(unsigned long i = 0;i<founded.size();++i) {
        it = std::find(data.begin(),data.end(),founded.at(i));
        if(it == data.end()) {
            //std::transform(founded.at(i).begin(),founded.at(i).end(),founded.at(i).begin(),::tolower);
            data.push_back(founded.at(i));
        }
    }

    //std::cout << url << std::endl;
    nextList.push_back(url);

    std::string hrefRegex   = "href[ ]*=[ ]*[\"']?([^\"']+)";
    std::vector<std::string> list = Utils::fixUrls(Utils::findAll(hrefRegex,source),url);
    //dump(list);
    for(unsigned long i = 0;i < list.size() && nextList.size() <= limit ;i++) {

        std::string next_url = list.at(i);
        //std::cout << next_url << list.size() << std::endl;

        it = std::find(nextList.begin(),nextList.end(),next_url);

        if(it == nextList.end()) {
            searchInPages(next_url, nextList, regex, data);
        }

    }
}

void Utils::searchInPages(std::string &url, const std::string &regex, std::vector<std::string> &data, unsigned long limit) {
    std::deque<std::string> queue,visited;
    queue.push_back(url);

    std::deque<std::string>::iterator it_que,it_vis;
    std::vector<std::string>::iterator it;

    while(!queue.empty() && visited.size() < limit) {
        std::string url = queue.front();
        queue.pop_front();

        std::string source = Utils::request(url);

        // Search for regex data
        std::vector<std::string> founded = Utils::findAll(regex, source);
        for(unsigned long i = 0;i<founded.size();++i) {
            it = std::find(data.begin(),data.end(),founded.at(i));
            if(it == data.end())
                data.push_back(founded.at(i));

        }

        std::string hrefRegex   = "href[ ]*=[ ]*[\"'<>]?([^\"';<>]+)";
        std::vector<std::string> links = Utils::fixUrls(Utils::findAll(hrefRegex, source),url);
        visited.push_back(url);
        //std::cout << "Lnk size: " << links.size() << std::endl;
        for(int i = 0; i < links.size() && queue.size()+visited.size() <= limit ;i++) {
            it_que = std::find(queue.begin(), queue.end(), links.at(i));
            it_vis = std::find(visited.begin(), visited.end(), links.at(i));
            if(it_que == queue.end() && it_vis == visited.end()) {
                queue.push_back(links.at(i));
                //std::cout << links.at(i) << std::endl;
            }
        }
    }
}

//void Utils::bfs(std::queue<std::string> &queue,)
