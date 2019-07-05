#ifndef WEBTRAVELER_UTILS_H
#define WEBTRAVELER_UTILS_H
#include <vector>
#include <string>


class Utils {
public:
    static std::vector<std::string> findAll(const std::string &regex, const std::string &source);

    static std::vector<std::string> normalizeUrlList(const std::vector<std::string> &list, const std::string &baseUrl);

    static void searchInPages(const std::string &url, const std::string &regex, std::vector<std::string> &data, unsigned long limit = 100);

    static std::string cleanAnchor(const std::string &url);

    static std::string concatHostPath(const std::string &, const std::string &);

    static std::string cleanProtocolShortcut(const std::string &url, const std::string &protocol = "http");

    static std::string cleanScheme(const std::string &url);
};


#endif //WEBTRAVELER_UTILS_H
