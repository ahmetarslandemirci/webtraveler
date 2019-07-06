#include <iostream>
#include <Poco/URI.h>
#include <Poco/Net/NetSSL.h>
#include "traveler/Utils.h"
#include <Poco/Notification.h>
#include <Poco/Runnable.h>
#include <Poco/NotificationQueue.h>
#include <algorithm>
#include <Poco/Logger.h>
#include <Poco/SimpleFileChannel.h>
#include <Poco/ThreadPool.h>
#include <Poco/File.h>
#include <fstream>
#include <Network.h>


class TargetUrl: public Poco::Notification {
public :
    TargetUrl(const std::string &url) : url(url) {}
    std::string url;
};

class Worker : public Poco::Runnable {
public:
    explicit Worker(Poco::NotificationQueue &queue) : _queue(queue) {}
    void run() {
        Poco::Notification::Ptr notification = _queue.dequeueNotification();
        std::string emailRegex  = "([\\w-]+(?:\\.[\\w-]+)*@(?:[\\w-]+\\.)+[a-zA-Z]{2,7})";
        while( notification ) {
            TargetUrl *target = dynamic_cast<TargetUrl*>(notification.get());
            if(target) {
                std::vector<std::string> mails;
                std::string url = Network::get_redirected_url(target->url);
                if( !url.empty()) {
                    std::cout << "Current Url: " << url << std::endl;
                    try {
                        Utils::searchInPages(url, emailRegex, mails, 10);
                    } catch (Poco::Exception &e) {
                        Poco::Logger::get("errors").error(url + " -- " + e.what() + "--" + e.displayText());
                    }
                    catch (std::exception &e) {
                    }

                    for (int i = 0; i < mails.size(); ++i) {
                        std::cout << mails.at(i) << std::endl;
                        Poco::Logger::get("Emails").information(mails.at(i));
                    }
                }
            }
            notification = _queue.dequeueNotification();
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

    // Create logger channel for writing emails
    Poco::SimpleFileChannel *simpleFileChannel = new Poco::SimpleFileChannel(mailsFilename);
    Poco::Logger::create("Emails",simpleFileChannel);

    Poco::SimpleFileChannel *logFileChannel = new Poco::SimpleFileChannel("errors.log");
    Poco::Logger::create("errors",logFileChannel);
    Poco::NotificationQueue queue;

    const int THREAD_SIZE = 50;

    Poco::ThreadPool pool(THREAD_SIZE,THREAD_SIZE,120);
    pool.addCapacity(THREAD_SIZE);
    
    std::vector<Worker*> workers;
    std::ifstream file(websiteFilename.c_str());
    if(file.fail()) {
        std::cout << "There is not such a file: " << websiteFilename << std::endl;
        return 0;
    }

    std::string str;
    while (std::getline(file, str))
        queue.enqueueNotification(new TargetUrl(str));

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


    return 0;
}
