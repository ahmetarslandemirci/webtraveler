#include <iostream>
#include <Poco/URI.h>
#include <Poco/Net/NetSSL.h>
#include "Utils.h"
#include <Poco/Notification.h>
#include <Poco/Runnable.h>
#include <Poco/NotificationQueue.h>
#include <algorithm>
#include <Poco/Logger.h>
#include <Poco/SimpleFileChannel.h>
#include <Poco/ThreadPool.h>
#include <Poco/File.h>
#include <fstream>

void dump(std::vector<std::string> vector) {
    std::cout << vector.size() << std::endl;
    for(int i=0;i<vector.size(); ++i)
        std::cout << vector.at(i) << std::endl;
}

class TargetUrl: public Poco::Notification {
public :
    TargetUrl(const std::string &url) : url(url) {}
    std::string url;
};

class Worker : public Poco::Runnable {
public:
    Worker(Poco::NotificationQueue &queue) : _queue(queue) {}
    void run() {
        Poco::AutoPtr<Poco::Notification> notification =(_queue.waitDequeueNotification());
        std::string emailRegex  = "([^ \"'<>:;,\}\{\)\(\\\/]+@[^ \"'<>:;,\}\{\)\(\\\/]+\.[a-zA-Z]+)";

        while(notification) {
            TargetUrl *target = dynamic_cast<TargetUrl*>(notification.get());
            if(target) {
                std::vector<std::string> urls, mails;
                Utils::searchInPages(target->url, urls, emailRegex, mails, 1000);
                for(int i=0;i<mails.size();++i){
                    Poco::Logger::get("Emails").information(mails.at(i));
                }

            }
            notification = _queue.waitDequeueNotification();
        }
    }
private:
    Poco::NotificationQueue &_queue;
};


int main() {
    Poco::Net::initializeSSL();
//    std::string emailRegex  = "([^ \"'<>:;,\}\{\)\(\\\/]+@[^ \"'<>:;,\}\{\)\(\\\/]+\.[a-zA-Z]+)";
//    std::string hrefRegex   = "href[ ]*=[ ]*[\"']?([^\"']+)";

    //std::string url = "http://www.bekap.com/";
    //std::string url = "http://www.karyapark.com.tr";
    //std::string url = "http://www.duk.com.tr";
//    std::string url = "https://www.isikpeyzaj.com/";
//    std::string source = Utils::request(url);
//    std::vector<std::string> s = Utils::findAll(hrefRegex,source);
//    std::vector<std::string> urls = Utils::fixUrls(s,url);
//    dump(urls);
//
//    std::cout << "##########################"<<std::endl;
//    std::vector<std::string> ne,data;
//    Utils::searchInPages(url, ne, emailRegex, data, 1000);
//    dump(data);


    // Create logger channel for writing emails
    Poco::SimpleFileChannel *simpleFileChannel = new Poco::SimpleFileChannel("emails.txt");
    Poco::Logger::create("Emails",simpleFileChannel);
    Poco::NotificationQueue queue;

    std::ifstream file("websites");
    std::string str;
    while (std::getline(file, str))
    {
        queue.enqueueNotification(new TargetUrl(str));
        std::cout << str << std::endl;
    }

    const int THREAD_SIZE = 8;

    Poco::ThreadPool &pool = Poco::ThreadPool::defaultPool();
    std::vector<Worker*> workers;
    for(int i=0;i<THREAD_SIZE;++i) {
        workers.push_back(new Worker(queue));
    }
    for(int i=0;i<THREAD_SIZE;++i) {
        pool.start(*workers.at(i));
    }
    queue.wakeUpAll();
    pool.joinAll();


    Poco::Net::uninitializeSSL();
    return 0;
}