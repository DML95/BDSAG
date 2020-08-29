#include <iostream>

#include"console/console.h"
#include"console/log.h"
#include"utils/semaphore.h"
#include"core/openCL.h"
#include"utils/config.h"
#include"core/db.h"
#include"utils/utils.h"
#include"core/restServer.h"

#include"apiOS.h"

static Semaphore closeSemaphore;

//evento de cerrado de consola
static void eventClose() noexcept{
    Log::getLog(Log::debug,INFO_LOG)<<"Evento de finalizacion"<<std::endl;
    closeSemaphore.notify_all();
}

//inicio de programa
int main(){
    Log::getLog(Log::info,INFO_LOG)<<"Iniciando BDSAG"<<std::endl;
    Console::setCloseEvent(eventClose);
    RESTServer restServer;
    Log::getLog(Log::info,INFO_LOG)<<"BDSAG Iniciada pulse Ctrl+C para finalizar"<<std::endl;
    closeSemaphore.wait();
    Log::getLog(Log::info,INFO_LOG)<<"Finalizando BDSAG"<<std::endl;
    return 0;
}
