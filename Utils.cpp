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

// Regex ust contain one group ex: asd([0-9]+)dsa
std::vector<std::string> Utils::findAll(const std::string &regex, const std::string &source) {
    Poco::RegularExpression expr(regex);
    std::vector<std::string> firstVector, foundedVector;

    unsigned long offset = 0;
    unsigned long numberOf = 0;
    while(true) {
        try {
            numberOf = expr.split(source, offset, firstVector);
        } catch(Poco::Exception &e) {
            std::cout << e.what() << std::endl;
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

std::string Utils::request(const std::string &url) {
    Poco::URI uri(url);
    std::string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";
    //std::cout <<"Host:" << uri.getHost() << std::endl;
    Poco::Net::HTTPResponse response;
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
    std::ostringstream ss;

    if(uri.getScheme() == "http") {
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        try {
            session.sendRequest(request);

        } catch (Poco::Exception &e) {
            std::cout << e.what() << std::endl;
            return std::string();
        }
        std::istream& s = session.receiveResponse(response);
        ss << s.rdbuf();
    }
    else if(uri.getScheme() == "https") {
        Poco::Net::Context::Ptr pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), pContext );
        try {
            session.sendRequest(request);

        } catch (Poco::Exception &e) {
            std::cout << e.what() << std::endl;
            return std::string();
        }
        std::istream& s = session.receiveResponse(response);
        ss << s.rdbuf();
    }

    //std::cout << response.get("Location") << std::endl;
    if(response.getStatus() == 302) {
        uri = Poco::URI(response.get("Location"));
        return Utils::request(uri.toString());
    }

    if( response.getStatus() != 200)
        return std::string();

    return ss.str();
}

std::vector<std::string> Utils::fixUrls(const std::vector<std::string> &urlList, const std::string &site) {
    Poco::URI baseURI(site);
    std::string baseUrl = baseURI.getHost();
    std::vector<std::string> uniqUrls;

    for(size_t i=0;i<urlList.size();i++) {
        Poco::URI uri(urlList.at(i));

        // skip for different hosts
        if(!uri.getHost().empty() && baseUrl != uri.getHost())
            continue;

        // Check for file extensions
        unsigned long lastDot = uri.getPath().find_last_of('.');
        if(lastDot != std::string::npos) {
            std::string ext = uri.getPath().substr(lastDot, uri.getPath().size());
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if(ext == ".css" || ext == ".js" || ext == ".pdf" || ext == ".exe" || ext == ".png" || ext == ".jpg" || ext == ".svg" || ext == ".ico") continue;
        }

        Poco::URI n(baseURI,uri.getPath());

        // Check for already added urls
        std::vector<std::string>::iterator it;
        it = std::find(uniqUrls.begin(),uniqUrls.end(),n.toString());
        if(it == uniqUrls.end())
            uniqUrls.push_back(n.toString());
    }
    return uniqUrls;
}

void Utils::searchInPages(const std::string &url, std::vector<std::string> &nextList, const std::string &regex, std::vector<std::string> &data, unsigned long limit) {
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

    std::cout << url << std::endl;
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

void Utils::searchInPages(const std::string &url, const std::string &regex, std::vector<std::string> &data, unsigned long limit) {
    std::deque<std::string> queue,visited;
    queue.push_back(url);

    std::deque<std::string>::iterator it_que,it_vis;
    std::vector<std::string>::iterator it;

    unsigned long visitedPageCount = 0;
    while(!queue.empty() && visitedPageCount < limit) {
        std::string url = queue.front();
        queue.pop_front();

        std::cout << "URL => " << url << std::endl;
        std::string source = Utils::request(url);

        // Search for regex data
        //std::cout << source << std::endl;
        std::vector<std::string> founded = Utils::findAll(regex, source);
        std::cout << queue.size() << " : " << visited.size() << std::endl;
        for(unsigned long i = 0;i<founded.size();++i) {
            it = std::find(data.begin(),data.end(),founded.at(i));
            if(it == data.end()) {
                //std::transform(founded.at(i).begin(),founded.at(i).end(),founded.at(i).begin(),::tolower);
                data.push_back(founded.at(i));
            }
        }

        std::string hrefRegex   = "href[ ]*=[ ]*[\"']?([^\"']+)";
        std::vector<std::string> links = Utils::fixUrls(Utils::findAll(hrefRegex, source),url);
        visited.push_back(url);
        //std::cout << "Lnk size: " << links.size() << std::endl;
        for(int i = 0; i<links.size() ;i++) {

            it_que = std::find(queue.begin(), queue.end(), links.at(i));
            it_vis = std::find(visited.begin(), visited.end(), links.at(i));
            if(it_que == queue.end() && it_vis == visited.end()) {
                queue.push_back(links.at(i));
            }
        }

        visitedPageCount+=1;
    }
}

//void Utils::bfs(std::queue<std::string> &queue,)