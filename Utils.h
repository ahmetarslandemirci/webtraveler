#ifndef WEBTRAVELER_UTILS_H
#define WEBTRAVELER_UTILS_H
#include <vector>
#include <string>
#include <queue>
#include <deque>

class Utils {
public:
    static std::vector<std::string> findAll(const std::string &regex, const std::string &source);
    static std::string request(std::string &url);
    static std::vector<std::string> fixUrls(const std::vector<std::string> &urlList, const std::string &baseUrl);
    static void searchInPages(std::string &url, std::vector<std::string> &nextList, const std::string &regex, std::vector<std::string> &data, unsigned long limit = 100);

    static void searchInPages(std::string &url, const std::string &regex, std::vector<std::string> &data, unsigned long limit = 100);
};


#endif //WEBTRAVELER_UTILS_H
