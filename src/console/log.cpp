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

//std::vector<std::string> Log::typeStr={"","FATAL","ERROR","WARN","INFO","DEBUG","TRACE",""};

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
