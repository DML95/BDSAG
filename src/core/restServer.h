#ifndef RESTSERVER_H_INCLUDED
#define RESTSERVER_H_INCLUDED

    #include"abstractServer.h"
    #include"../apiRapidjson.h"

    #include<vector>
    #include<exception>

    class RESTServer: public AbstractServer{
        private:

    		enum enumNodes{
    			otherNode=-1,
    			database,
				session,
				config,
				devices,
				sessions,
				count,
    		};

    		enum enumMethods{
    			otherMethod=-1,
    			get,
				post,
				patch,
				deleete,
    		};

    		static std::unordered_map<std::string,RESTServer::enumNodes> mapNodes;
    		static std::unordered_map<std::string,RESTServer::enumMethods> mapMethods;

            //extrae los nodos de una URL
            std::vector<std::string> urlToNodes(std::string &url);
            //combierte el document de rapidjson en string
            static std::string documentToString(rapidjson::Document &document);
            //GET /database/config
            //devuelve la configuaracion actual de la BD
            std::string databaseConfigGET();
            //GET /database/devices
            //devuelve los dispostivos asociados a la BD
            std::string databaseDevicesGET();
            //POST /database/session
            //crea una sesion
            int databaseSessionPOST(rapidjson::Document &response,rapidjson::Document &request,std::unordered_map<std::string,std::string> &headers);
            //GET /database/session/{id}
            //busca una sesion
            int databaseSessionGET(rapidjson::Document &response,std::string &id,std::unordered_map<std::string,std::string> &headers);
            //PATCH /database/session/{id}
            //modifica una sesion
            int databaseSessionPATCH(rapidjson::Document &response,rapidjson::Document &request,std::string &id,std::unordered_map<std::string,std::string> &headers);
            //DELETE /database/session/{id}
            //elimina una sesion
            int databaseSessionDELETE(rapidjson::Document &response,std::string &id,std::unordered_map<std::string,std::string> &headers);
            //GET /database/sessions/count
            //cuanta el numero de sesiones creadas
            int databaseSessionsCountGET(rapidjson::Document &response);
            //GET /database/sessions?value={regex}
            //devuelve las sesiones que su valor corresponda con la expresion regular
            int databaseSessionsGET(rapidjson::Document &response,std::unordered_map<std::string,std::string> &querys);
            //crea un json de error
            static void createMessageError(rapidjson::Document &document,std::string error);
            //devuelve el nodo en caso de no encontrarlo -1
            static RESTServer::enumNodes getEnumNode(std::string node);
            //devuelve el metodo en caso de no encontrarlo -1
            static RESTServer::enumMethods getEnumMethod(std::string method);
        protected:
            //impementacion de AbstractServer
            bool connection(AbstractServer::Response &response,AbstractServer::Request &request);
        public:
            //constructor
            RESTServer();

    };

#endif // RESTSERVER_H_INCLUDED
