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
        std::string emailRegex  = "([\\w-]+(?:\\.[\\w-]+)*@(?:[\\w-]+\\.)+[a-zA-Z]{2,7})";
        while(notification) {
            std::cout << _queue.size() << std::endl;
            TargetUrl *target = dynamic_cast<TargetUrl*>(notification.get());
            if(target) {
                std::vector<std::string> mails;
                try {
                    Utils::searchInPages(target->url, emailRegex, mails, 100);
                } catch (Poco::Exception &e) {
                    Poco::Logger::get("errors").error(target->url + " -- " + e.what());
                }
                catch( std::exception &e) {

                }
                for (int i = 0; i < mails.size(); ++i) {
                    Poco::Logger::get("Emails").information(mails.at(i));
                }

            }
            notification->release();
            if(_queue.empty()) break;

            notification = _queue.waitDequeueNotification();
        }
    }
private:
    Poco::NotificationQueue &_queue;
};


int main(int argc, char *argv[]) {

    if(argc != 3) {
        std::cout << "Usage: webtraveler fromWebsitesFile toEmailsFile" << std::endl;
        std::cout << "fromWebsitesFile: From read filename for websites" << std::endl;
        std::cout << "toEmailsFile: To write filename for founded emails" << std::endl;
        return 0;
    }
    std::string websiteFilename = argv[1];
    std::string mailsFilename = argv[2];

    Poco::Net::initializeSSL();

    // Create logger channel for writing emails
    Poco::SimpleFileChannel *simpleFileChannel = new Poco::SimpleFileChannel(mailsFilename);
    Poco::Logger::create("Emails",simpleFileChannel);

    Poco::SimpleFileChannel *logFileChannel = new Poco::SimpleFileChannel("errors.log");
    Poco::Logger::create("errors",logFileChannel);
    Poco::NotificationQueue queue;

    const int THREAD_SIZE = 100;

    Poco::ThreadPool pool(THREAD_SIZE,THREAD_SIZE,60);
    pool.addCapacity(THREAD_SIZE);
    
    std::vector<Worker*> workers;
    std::ifstream file(websiteFilename.c_str());
    if(file.fail()) {
        std::cout << "There is not such a file: " << websiteFilename << std::endl;
        return 0;
    }

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

    while(!queue.empty())
        Poco::Thread::sleep(100);
    Poco::Thread::sleep(1000);

    queue.wakeUpAll();
    pool.joinAll();


    Poco::Net::uninitializeSSL();
    return 0;
}
