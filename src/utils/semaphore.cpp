#include"semaphore.h"
#include"../console/log.h"

Semaphore::Semaphore():
        semaphore(){
	Log::log(Log::trace,this,INFO_LOG,"Loadding");
}

void Semaphore::wait(){
	Log::log(Log::trace,this,INFO_LOG,"Wait");
    std::unique_lock<std::mutex> uniqueLock(this->mutex);
    this->semaphore.wait(uniqueLock);
}

void Semaphore::notify_all(){
	Log::log(Log::trace,this,INFO_LOG,"Notify_all");
    this->semaphore.notify_all();
}
