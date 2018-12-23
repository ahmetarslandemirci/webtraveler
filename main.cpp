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
    explicit Worker(Poco::NotificationQueue &queue) : _queue(queue) {}
    void run() {
        Poco::Notification::Ptr notification = _queue.waitDequeueNotification();
        std::string emailRegex  = "((?!\\S*\\.(?:jpg|png|gif|bmp)(?:[\\s\\n\\r]|$))[\\w._+-]{2,}@[\\w.-]{3,65}\\.[\\w]{2,4})";

        while(notification) {
            TargetUrl *target = dynamic_cast<TargetUrl*>(notification.get());
            if(target) {
                std::vector<std::string> mails;
                //std::cout << target->url << std::endl;
                try {
                    Utils::searchInPages(target->url, emailRegex, mails, 100);
                } catch (Poco::Exception &e) {
                    Poco::Logger::get("errors").error(target->url + " -- " + e.what());
                }
                for (int i = 0; i < mails.size(); ++i) {
                    Poco::Logger::get("Emails").information(mails.at(i));
                }
                target->release();
            }

            notification = _queue.dequeueNotification();
        }
    }
private:
    Poco::NotificationQueue &_queue;
};


int main() {
    Poco::Net::initializeSSL();

    // Create logger channel for writing emails
    Poco::SimpleFileChannel *simpleFileChannel = new Poco::SimpleFileChannel("emails.txt");
    Poco::Logger::create("Emails",simpleFileChannel);

    Poco::SimpleFileChannel *logFileChannel = new Poco::SimpleFileChannel("errors.log");
    Poco::Logger::create("errors",logFileChannel);
    Poco::NotificationQueue queue;

    const int THREAD_SIZE = 16;

    Poco::ThreadPool &pool = Poco::ThreadPool::defaultPool();

    std::vector<Worker*> workers;
    std::ifstream file("websites");
    std::string str;
    while (std::getline(file, str)) {
        queue.enqueueNotification(new TargetUrl(str));
    }

    std::cout << "Queue size: " << queue.size() << std::endl;

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