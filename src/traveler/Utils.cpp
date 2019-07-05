#include "Utils.h"
#include "Network.h"

#include <Poco/RegularExpression.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

#include <iostream>
#include <algorithm>
#include <queue>
#include <deque>


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
//        std::cout << "1: "<<firstVector.at(1) << std::endl;
//        std::cout << "0: "<<firstVector.at(0) << std::endl;
        unsigned long pos = source.find(firstVector.at(0),offset);

        if( pos != std::string::npos)
            offset = pos + firstVector.at(0).size() ;
        else break;
    }
    return foundedVector;
}


std::vector<std::string> Utils::normalizeUrlList(const std::vector<std::string> &list, const std::string &baseUrl) {
    Poco::URI baseUri(baseUrl);

    std::string base = baseUri.getHost();
    std::vector<std::string> uniqUrls;

    for(int i=0;i<list.size();i++) {
        std::string url = list.at(i);


        if(url.substr(0,4) == "http") {
            if ( (url.size() >= 11 && url.substr(0, 11) == "javascript:")) {
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

        // tel: callto: mailto: gibi protocollerin parse edilmesi problem yaratıryor

        try {
            Poco::URI uri(url);

            // skip for different hosts

            // @todo: this must be replaced white list. for example you can check response headers for content type: text
            // Check for file extensions
            unsigned long lastDot = uri.getPath().find_last_of('.');
            if (lastDot != std::string::npos) {
                std::string ext = uri.getPath().substr(lastDot, uri.getPath().size());
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".css" || ext == ".js" || ext == ".pdf" || ext == ".exe" || ext == ".png"
                    || ext == ".jpg" || ext == ".svg" || ext == ".ico" || ext == ".doc" || ext == ".xls")
                    continue;
            }

            // Check for already added urls
            std::vector<std::string>::iterator it;
            it = std::find(uniqUrls.begin(), uniqUrls.end(), url);
            if (it == uniqUrls.end())
                uniqUrls.push_back(url);
        } catch(Poco::Exception e) {
            continue;
        }
    }
    return uniqUrls;
}


void Utils::searchInPages(const std::string &url, const std::string &regex, std::vector<std::string> &data, unsigned long limit) {
    std::deque<std::string> queue,visited;
    queue.push_back(url);

    std::deque<std::string>::iterator it_que,it_vis;
    std::vector<std::string>::iterator it;
    std::string baseUrl = url;

    while(!queue.empty() && visited.size() < limit) {
        std::string url = queue.front();
        queue.pop_front();

        std::string source = Network::request(url);

        // Search for regex data
        std::vector<std::string> founded = Utils::findAll(regex, source);
        for(unsigned long i = 0;i<founded.size();++i) {
            it = std::find(data.begin(),data.end(),founded.at(i));
            if(it == data.end())
                data.push_back(founded.at(i));

        }

        std::string hrefRegex   = "<a.+href[ ]*=[ ]*[\"'<>]?([^\"';<>]+)";
        std::vector<std::string> links = Utils::normalizeUrlList(Utils::findAll(hrefRegex, source),baseUrl);
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
    std::cout << "\n # Scanned " << visited.size() << " in " << baseUrl << std::endl;
}

std::string Utils::cleanAnchor(const std::string &url) {
    int pos = url.find("#");
    if( pos != std::string::npos )
        return url.substr(0, pos);
    return url;
}

std::string Utils::cleanProtocolShortcut(const std::string &url, const std::string &protocol) {
    if(url.substr(0, 2) == "//")
        return protocol + ":" + url;
    return url;
}

std::string Utils::concatHostPath(const std::string &host, const std::string &path) {
    if(host.at(host.size()-1) == '/')
        return host + path;
    return host + '/' + path;
}

std::string Utils::cleanScheme(const std::string &url) {
    if(url.size()>4 && url.substr(0,4) != "http" && url.find(":") != std::string::npos)
        return "";

    return url;
}