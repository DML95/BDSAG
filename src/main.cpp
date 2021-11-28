#include <iostream>

#include"console/console.h"
#include"console/log.h"
#include"utils/semaphore.h"
#include"core/openCL.h"
#include"utils/config.h"
#include"core/db.h"
#include"utils/utils.h"
#include"core/restServer.h"

#include"core/restFunctions/countSessions.h"
#include"core/restFunctions/createSession.h"
#include"core/restFunctions/deleteSession.h"
#include"core/restFunctions/getConfig.h"
#include"core/restFunctions/getDevices.h"
#include"core/restFunctions/getSession.h"
#include"core/restFunctions/getSessions.h"
#include"core/restFunctions/updateSession.h"

#include"apiOS.h"

static Semaphore closeSemaphore;

//evento de cerrado de consola
static void eventClose() noexcept{
    Log::log(Log::debug,INFO_LOG,"Evento de finalizacion");
    closeSemaphore.notify_all();
}

//carga las funciones para el servidor rest
static std::vector<std::shared_ptr<RESTFunctionCommun> > loadFunctions(){
	std::vector<std::shared_ptr<RESTFunctionCommun> > functions;
	functions.push_back(std::make_shared<GetSession>());
	functions.push_back(std::make_shared<CreateSession>());
	functions.push_back(std::make_shared<UpdateSession>());
	functions.push_back(std::make_shared<DeleteSession>());
	functions.push_back(std::make_shared<GetSessions>());
	functions.push_back(std::make_shared<CountSessions>());
	functions.push_back(std::make_shared<GetConfig>());
	functions.push_back(std::make_shared<GetDevices>());
	return functions;
}

//inicio de programa
int main(){
    Log::log(Log::info,INFO_LOG,"Iniciando BDSAG");
    Console::setCloseEvent(eventClose);
    RESTServer restServer(loadFunctions());
    Log::log(Log::info,INFO_LOG,"BDSAG Iniciada pulse Ctrl+C para finalizar");
    closeSemaphore.wait();
    Log::log(Log::info,INFO_LOG,"Finalizando BDSAG");
    return 0;
}
