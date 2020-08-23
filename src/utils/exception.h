#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

    #include<string>

    #include"../console/log.h"

    class Exception:public std::exception{
        private:
            const int code;
            const std::string message;
            const std::string function;
            const int line;
            const void *id;
            std::string fatalMessage;
        public:
            //constructor
            Exception(int code,std::string message,std::string function,int line);
            Exception(int code,std::string message,const void *id,std::string function,int line);
            //agrega una linea de log
            void printLog(Log::type type);
            virtual const char* what() const throw();
    };

#endif // EXCEPTION_H_INCLUDED
