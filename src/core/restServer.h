#ifndef RESTSERVER_H_INCLUDED
#define RESTSERVER_H_INCLUDED

    #include"abstractServer.h"
    #include"../apiRapidjson.h"

    #include<vector>
    #include<exception>

    class RESTServer: public AbstractServer{
        private:
            std::vector<std::string> urlToNodes(std::string url);
            static std::string documentToString(rapidjson::Document &document);
            int databaseSessionPOST(rapidjson::Document &response,rapidjson::Document &request,std::unordered_map<std::string,std::string> &headers);
            int databaseSessionGET(rapidjson::Document &response,std::string &id,std::unordered_map<std::string,std::string> &headers);

            static void createMessageError(rapidjson::Document &document,std::string error);
        protected:
            bool connection(AbstractServer::Response &response,AbstractServer::Request &request);
        public:
            RESTServer();

    };

#endif // RESTSERVER_H_INCLUDED
