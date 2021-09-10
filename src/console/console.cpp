#include"console.h"

#include"../apiOS.h"
#include"log.h"

std::atomic<Console::event> Console::function(0);

struct Console::plataformFunctions{
    #ifdef _WIN32
        static BOOL handlerRoutine(DWORD dwCtrlType){
            Log::log(Log::trace,INFO_LOG,"Llamada a handlerRoutine");
            Console::event function=Console::function.load();
            function();
            return true;
        }

	#elif __linux__

        static void closeEvent(sig_atomic_t s){
        	Log::log(Log::trace,INFO_LOG,"Llamada a closeEvent");
        	Console::event function=Console::function.load();
        	function();
        }

    #endif
};

void Console::setCloseEvent(Console::event function){
    Log::log(Log::trace,INFO_LOG,"Puntero:",(void*)function);
    Console::function.store(function);
    #ifdef _WIN32
        ::SetConsoleCtrlHandler(Console::plataformFunctions::handlerRoutine,(bool)function);
	#elif __linux__
        ::signal(SIGINT,Console::plataformFunctions::closeEvent);
    #endif
}
