#include <Utils.h>
#include <gtest/gtest.h>
using namespace std;


class UtilsTest : public ::testing::Test {

    virtual void SetUp() {
        this->test_url  = "http://example.com/test.html";
        this->host      = "http://example.com";
        this->path      = "index.html";
    };

    virtual void TearDown() {
    };
protected:
    std::string test_url;
    std::string host;
    std::string path;
};

TEST_F(UtilsTest, CleanAnchors_PositiveTest) {
    std::string url = Utils::cleanAnchor( this->test_url + "#anchor");
    ASSERT_STREQ( url.c_str(), test_url.c_str());
}

TEST_F(UtilsTest, CleanAnchors_JustAnchorTest) {
    std::string url = Utils::cleanAnchor( "#" );
    ASSERT_STREQ(url.c_str(), "");
}

TEST_F(UtilsTest, ConcatHostAndPath_WithoutSlashInHost_Test) {
    std::string url = Utils::concatHostPath(host, path);
    std::string expected_url = this->host + '/' + this->path;
    ASSERT_STREQ(url.c_str(),  expected_url.c_str());
}

TEST_F(UtilsTest, ConcatHostAndPath_WithSlashInHost_Test) {
    this->host += '/';
    std::string url = Utils::concatHostPath(host, path);
    std::string expected_url = this->host + this->path;
    ASSERT_STREQ(url.c_str(),  expected_url.c_str());
}

TEST_F(UtilsTest, CleanProtocolShortcut_OneCharacterUrl_Test) {
    test_url = "#";
    std::string url = Utils::cleanProtocolShortcut(test_url);
    ASSERT_STREQ(url.c_str(), test_url.c_str());
}

TEST_F(UtilsTest, CleanProtocolShortcut_CleanUrl_Test) {
    test_url = "//example.com";
    std::string url = Utils::cleanProtocolShortcut(test_url);
    std::string expected_url = "http:" + test_url;
    ASSERT_STREQ(url.c_str(), expected_url.c_str());
}

TEST_F(UtilsTest, CleanScheme_DifferentSchemes_Test) {
    test_url = "tel:0123456789";
    std::string url = Utils::cleanScheme(test_url);
    ASSERT_STREQ(url.c_str(), "");

    test_url = "javascript:321321321321";
    url = Utils::cleanScheme(test_url);
    ASSERT_STREQ(url.c_str(), "");

    test_url = "a:a35sd4as5d4";
    url = Utils::cleanScheme(test_url);
    ASSERT_STREQ(url.c_str(), "");
}

TEST_F(UtilsTest, NormalizeUrls_UrlTest_Test) {
    std::string base_url = "http://example.com/";

    std::vector<std::string> urls;
    urls.push_back("http://example.com/index.html");
    urls.push_back("http://example.com/contact.html#test");
    urls.push_back("pages.html");
    urls.push_back("//example.com/main.html");
    urls.push_back("index.php?asd=test");

    std::vector<std::string> expected_urls;
    expected_urls.push_back("http://example.com/index.html");
    expected_urls.push_back("http://example.com/contact.html");
    expected_urls.push_back("http://example.com/pages.html");
    expected_urls.push_back("http://example.com/main.html");
    expected_urls.push_back("http://example.com/index.php?asd=test");

    std::vector<std::string> normalized_urls = Utils::normalizeUrlList(urls, base_url);

    for( auto it = expected_urls.begin(); it != expected_urls.end(); it++) {
        bool founded = false;
        for( auto it2 = normalized_urls.begin(); it2 != normalized_urls.end(); it2++) {
            if( *it == *it2 )
                founded = true;
        }
        if(founded == false)
            ASSERT_EQ(1, founded);
    }
}