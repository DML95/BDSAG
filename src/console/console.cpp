#include"console.h"

#include"../apiOS.h"
#include"log.h"

std::atomic<Console::event> Console::function(0);

struct Console::plataformFunctions{
    #ifdef _WIN32
        static BOOL handlerRoutine(DWORD dwCtrlType){
            Log::getLog(Log::trace,INFO_LOG)<<"Llamada a handlerRoutine"<<std::endl;
            Console::event function=Console::function.load();
            function();
            return true;
        }
    #endif
};

void Console::setCloseEvent(Console::event function){
    Log::getLog(Log::trace,INFO_LOG)<<"Puntero: "<<(void*)function<<std::endl;
    Console::function.store(function);
    #ifdef _WIN32
        ::SetConsoleCtrlHandler(Console::plataformFunctions::handlerRoutine,(bool)function);
    #endif
}
