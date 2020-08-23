#include"semaphore.h"
#include"../console/log.h"

Semaphore::Semaphore():
        semaphore(){
    Log::getLog(Log::trace,this,INFO_LOG)<<"Loadding"<<std::endl;
}

void Semaphore::wait(){
    Log::getLog(Log::trace,this,INFO_LOG)<<"Wait"<<std::endl;
    std::unique_lock<std::mutex> uniqueLock(this->mutex);
    this->semaphore.wait(uniqueLock);
}

void Semaphore::notify_all(){
    Log::getLog(Log::trace,this,INFO_LOG)<<"Notify_all"<<std::endl;
    this->semaphore.notify_all();
}
