#include <Network.h>
#include <gtest/gtest.h>

using namespace std;


class NetworkTest : public ::testing::Test {

    virtual void SetUp() {
    };

    virtual void TearDown() {
    };

};

TEST_F(NetworkTest, ConnectGoogle) {
    Network network;
    std::string source = network.request("https://www.google.com");
    ASSERT_STRNE( source.c_str(), "");
}

TEST_F(NetworkTest, GetRedirectedUrlForGoogle) {
    Network network;
    std::string target_url = "https://google.com";

    std::string url = network.get_redirected_url(target_url);

//    std::cout << "Url: " << target_url << std::endl;
//    std::cout << "Redirected Url: " << url << std::endl;

    ASSERT_STRNE( url.c_str(), "");
}