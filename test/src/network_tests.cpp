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
    std::string source = Network::request("https://www.google.com");
    ASSERT_STRNE( source.c_str(), "");
}
