#include <sstream>

#include"exception.h"

Exception::Exception(int code,std::string message,std::string function,int line):
        code(code),
        message(message),
        function(function),
        line(line){
    this->id=0;
    std::stringstream ss;
    ss<<"[FATAL|"<<function<<'|'<<line<<"] Excepcion\n\tcodigo: "<<code<<"\n\tmensaje: "<<message<<std::endl;
    this->fatalMessage=ss.str();
}


Exception::Exception(int code,std::string message,const void *id,std::string function,int line):
        code(code),
        message(message),
        function(function),
        line(line),
        id(id){
    std::stringstream ss;
    ss<<"[FATAL|"<<function<<'|'<<line<<"|ID("<<id<<")] Excepcion\n\tcodigo: "<<code<<"\n\tmensaje: "<<message<<std::endl;
    this->fatalMessage=ss.str();
}

void Exception::printLog(Log::type type){
    if(this->id){
    	Log::log(type,this->id,function,line,"Excepcion\n\tcodigo:",code,"\n\tmensaje:",message);
    }else{
    	Log::log(type,function,line,"Excepcion\n\tcodigo:",code,"\n\tmensaje:",message);
    }
}

const char* Exception::what() const throw() {
    return this->fatalMessage.data();
}
