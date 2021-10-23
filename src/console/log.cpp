#include <ostream>
#include<sstream>

#include"log.h"
#include"../utils/exception.h"

/*std::unordered_map<std::string,Log::type> Log::mapStringType={
    {"NONE",Log::none},
    {"FATAL",Log::fatal},
    {"ERROR",Log::error},
    {"WARN",Log::warn},
    {"INFO",Log::info},
    {"DEBUG",Log::debug},
    {"TRACE",Log::trace},
    {"ALL",Log::all}
};*/

std::atomic_int Log::typeValue(Log::all);
std::atomic_bool Log::run;
std::thread Log::thread;
std::mutex Log::mutexLog;
std::condition_variable Log::semaphore;
std::list<std::shared_ptr<std::string> > Log::listLog;

//std::vector<std::string> Log::typeStr={"","FATAL","ERROR","WARN","INFO","DEBUG","TRACE",""};

Log::StaticClass Log::staticClass;

void Log::mainThread(){
	Log::log(Log::debug,INFO_LOG,"Iniciando hilo Log");
	bool noEmpty;
	do{
		std::shared_ptr<std::string> smartText;
		{
			std::unique_lock<std::mutex> lock(Log::mutexLog);
			noEmpty=!Log::listLog.empty();
			if(noEmpty){
				smartText=Log::listLog.front();
				Log::listLog.pop_front();
			}else{
				Log::semaphore.wait(lock);
			}
		}
		if(noEmpty){
			std::ostream &os=std::clog;
			os<<*smartText.get();
			os.flush();
		}
	}while(Log::run.load()||noEmpty);
}

void Log::pushLog(const std::string &text){
	std::shared_ptr<std::string> smartText=std::make_shared<std::string>(text);
	Log::semaphore.notify_all();
	std::unique_lock<std::mutex> lock(Log::mutexLog);
	Log::listLog.push_back(smartText);
}

Log::StaticClass::StaticClass(){
	Log::log(Log::debug,INFO_LOG,"Iniciando nucleo Log");
	Log::run.store(true);
	Log::thread=std::thread(Log::mainThread);
}

Log::StaticClass::~StaticClass(){
	Log::log(Log::debug,INFO_LOG,"Cerrando nucleo Log");
	Log::run.store(false);
	Log::semaphore.notify_all();
	Log::thread.join();
}

void Log::setVisibility(Log::type type){
    Log::typeValue.store(type);
}

void Log::setVisibility(std::string type){
    Log::log(Log::info,INFO_LOG,"Estableciendo visivilidad de logs:",type);
    int typeVal=-1;
    for(int cont=0,size=Log::mapTypeStr.size();cont<size&&typeVal<0;cont++){
    	if(!type.compare(Log::mapTypeStr[cont])){
    		typeVal=cont;
    	}
    }
    if(typeVal<0){
        throw Exception(0,"valor de log invalido",INFO_LOG);
    }
    Log::typeValue.store(typeVal);
}
