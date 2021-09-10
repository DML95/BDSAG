#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

    #include<iostream>
    #include<atomic>
    #include<vector>
    #include<unordered_map>
	#include<array>

    #include"../utils/nullBuffer.h"

    #define INFO_LOG __PRETTY_FUNCTION__,__LINE__

    class Log{
        public:
            enum type{
                none,
                fatal,
                error,
                warn,
                info,
                debug,
                trace,
                all
            };
        private:
            Log();

            constexpr static std::array<const char*,8> mapTypeStr={"NONE","FATAL","ERROR","WARN","INFO","DEBUG","TRACE","ALL"};
            constexpr static std::array<const char*,8> typeStr={"","FATAL","ERROR","WARN","INFO","DEBUG","TRACE",""};

            static std::atomic_int typeValue;
            static NullBuffer nullBuffer;
            static std::ostream nullStream;

            //devuelve un ostream dependiedo del tipo de log
            static std::ostream& getOstream(Log::type type);
        public:
            //establece el nivel maximo a mostrar
            static void setVisibility(Log::type type);
            static void setVisibility(std::string type);

            //envia los logs
            template<typename... T> static void log(const Log::type type,const void* id,const std::string &function,int line,const T&... logs);
            template<typename... T> static void log(const Log::type type,const std::string &function,int line,const T&... logs);
    };

    template<typename... T> void Log::log(const Log::type type,const void* id,const std::string &function,int line,const T&... logs){
    	std::ostream &os=std::clog;
    	if(Log::typeValue.load()>=type){
			os<<'['<<Log::typeStr[type]<<'|'<<function<<'|'<<line<<'|';
			if(id)os<<"ID("<<id<<')';
			os<<"] ";
    		((os<<logs<<' '),...);
    		os<<std::endl;
    	}
    }

    template<typename... T> void Log::log(const Log::type type,const std::string &function,int line,const T&... logs){
        std::ostream &os=std::clog;
        if(Log::typeValue.load()>=type){
        	os<<'['<<Log::typeStr[type]<<'|'<<function<<'|'<<line<<"] ";
        	((os<<logs<<' '),...);
        	os<<std::endl;
        }
    }

#endif // LOG_H_INCLUDED
