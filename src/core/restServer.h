#ifndef RESTSERVER_H_INCLUDED
#define RESTSERVER_H_INCLUDED

	//dependencia circular
	class RESTFunctionCommun;

    #include"abstractServer.h"
    #include"../apiRapidjson.h"
	#include"../utils/constant.h"
	#include "restFunctions/restFunctionCommun.h"

    #include<vector>
    #include<exception>
	#include<memory>

    class RESTServer: public AbstractServer{
        private:

    		std::vector<std::shared_ptr<RESTFunctionCommun> > functions;

            //extrae los nodos de una URL
            std::vector<std::string> urlToNodes(std::string &url);
            //combierte el document de rapidjson en string
            static std::string documentToString(rapidjson::Document &document);
            //devuelve el nodo en caso de no encontrarlo -1
            static Constant::nodes getEnumNode(const std::string &node);
            //devuelve el metodo en caso de no encontrarlo -1
            static Constant::methods getEnumMethod(const std::string &method);
        protected:
            //impementacion de AbstractServer
            bool connection(AbstractServer::Response &response,AbstractServer::Request &request);
        public:
            //constructor
            RESTServer(const std::vector<std::shared_ptr<RESTFunctionCommun> > &functions);
            //destructor
            virtual ~RESTServer();

            std::vector<std::shared_ptr<RESTFunctionCommun> >& getFunctions();

    };

#endif // RESTSERVER_H_INCLUDED
