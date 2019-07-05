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