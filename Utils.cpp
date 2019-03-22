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

    Poco::URI uri(url);
    std::string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";
    std::cout << "Request: " << url << std::endl;
    Poco::Net::HTTPResponse response;
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
    request.add("User-Agent", "Mozilla/5.0 (X11; ArchLinux; Linux x86_64; rv:64.0) Gecko/20100101 Firefox/64.0");

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
        s.clear();
    }
    else if(uri.getScheme() == "https") {
        Poco::Net::Context::Ptr pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE);
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), pContext );
        try {
            session.sendRequest(request);

        } catch (Poco::Exception &e) {
            std::cout << e.what() << std::endl;
            return std::string();
        }
        std::istream& s = session.receiveResponse(response);
        ss << s.rdbuf();
        s.clear();
        //std::cout << "Here..."<< std::endl;
    }

    if(response.getStatus() == 301) {
        std::cout << "Loc:" << response.get("Location") << std::endl;
        if(maxRedirect == 0) return "";
        try {
            url = response.get("Location");
            return Utils::request(url, --maxRedirect);
        } catch(Poco::Exception e) {return "";}
    }
    //std::cout << "hOLLAA"<< std::endl;
    return ss.str();
}

std::vector<std::string> Utils::fixUrls(const std::vector<std::string> &urlList, const std::string &baseUrl) {
    Poco::URI baseUri(baseUrl);
    std::string base = baseUri.getHost();
    std::vector<std::string> uniqUrls;

    for(size_t i=0;i<urlList.size();i++) {
        std::string url = urlList.at(i);

        if(url == "#" || (url.size() >= 11 && url.substr(0,11) == "javascript:")) {
            continue;
        }
        else if(url.size() >= 2 && url.at(0) == '/' && url.at(1) == '/') {
            // write http or https
        }
        else if(url.size() >= 1 && url.at(0) == '/') {
            if ( baseUrl.at(baseUrl.size()-1) == '/')
                url = baseUrl + url.substr(1, url.size()-1);
            else
                url = baseUrl + url;
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

        //std::cout << "URL => " << url << std::endl;
        std::string source = Utils::request(url);

        // Search for regex data
        std::vector<std::string> founded = Utils::findAll(regex, source);
        for(unsigned long i = 0;i<founded.size();++i) {
            it = std::find(data.begin(),data.end(),founded.at(i));
            if(it == data.end()) {
                data.push_back(founded.at(i));
            }
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
            }
        }
    }
}

//void Utils::bfs(std::queue<std::string> &queue,)
