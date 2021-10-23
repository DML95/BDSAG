#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

    #include<iostream>
    #include<atomic>
    #include<vector>
    #include<unordered_map>
	#include<array>
	#include<thread>
	#include<list>
	#include<sstream>

	#include "../utils/semaphore.h"

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
            static std::atomic_bool run;
            static std::thread thread;
            static std::mutex mutexLog;
            static std::condition_variable semaphore;
            static std::list<std::shared_ptr<std::string> > listLog;

            //clase estatica
            struct StaticClass{
            	//constructor estatico
                StaticClass();
                //destructor estatico
                ~StaticClass();
            };

            static StaticClass staticClass;

            static void mainThread();
            static void pushLog(const std::string &text);

        public:
            //establece el nivel maximo a mostrar
            static void setVisibility(Log::type type);
            static void setVisibility(std::string type);

            //envia los logs
            template<typename... T> static void log(const Log::type type,const void* id,const std::string &function,int line,const T&... logs);
            template<typename... T> static void log(const Log::type type,const std::string &function,int line,const T&... logs);
    };

    template<typename... T> void Log::log(const Log::type type,const void* id,const std::string &function,int line,const T&... logs){
    	if(Log::typeValue.load()>=type){
    		std::stringstream ss;
			ss<<'['<<Log::typeStr[type]<<'|'<<function<<'|'<<line<<'|';
			if(id)ss<<"ID("<<id<<')';
			ss<<"] ";
    		((ss<<logs<<' '),...);
    		ss<<std::endl;
    		Log::pushLog(ss.str());
    	}
    }

    template<typename... T> void Log::log(const Log::type type,const std::string &function,int line,const T&... logs){
        if(Log::typeValue.load()>=type){
        	std::stringstream ss;
        	ss<<'['<<Log::typeStr[type]<<'|'<<function<<'|'<<line<<"] ";
        	((ss<<logs<<' '),...);
        	ss<<std::endl;
        	Log::pushLog(ss.str());
        }
    }

#endif // LOG_H_INCLUDED
