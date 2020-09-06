#ifndef RESTSERVER_H_INCLUDED
#define RESTSERVER_H_INCLUDED

    #include"abstractServer.h"
    #include"../apiRapidjson.h"

    #include<vector>
    #include<exception>

    class RESTServer: public AbstractServer{
        private:
            //extrae los nodos de una URL
            std::vector<std::string> urlToNodes(std::string &url);
            //combierte el document de rapidjson en string
            static std::string documentToString(rapidjson::Document &document);
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
            //crea un json de error
            static void createMessageError(rapidjson::Document &document,std::string error);
        protected:
            //impementacion de AbstractServer
            bool connection(AbstractServer::Response &response,AbstractServer::Request &request);
        public:
            //constructor
            RESTServer();

    };

#endif // RESTSERVER_H_INCLUDED
