#ifndef ABSTRACTSERVER_H_INCLUDED
#define ABSTRACTSERVER_H_INCLUDED

    #include<string>
    #include<map>
    #include<unordered_map>

    #include <microhttpd.h>

    class AbstractServer{
        private:
            const struct MHD_Daemon *server;

            typedef struct{
                std::string body;
                bool processed;
            }InfoBodyRequest;

        protected:

            typedef struct{
                std::string method;
                std::string url;
                std::unordered_map<std::string,std::string> querys;
                std::unordered_map<std::string,std::string> headers;
                std::string body;
            }Request;

            typedef struct{
                int code;
                std::map<std::string,std::string> headers;
                std::string body;
            }Response;

            //llamada virtual externa
            virtual bool connection(AbstractServer::Response &response,AbstractServer::Request &request)=0;
        private:

            //crea una respuesta para microhttpd a partir del body y cabeceras
            static int createResponse(struct MHD_Connection* connection,AbstractServer::Response &response,AbstractServer *abstractServer);
            //agrega una clave valor
            static int addKeyValue(void *cls,MHD_ValueKind kind, const char *key, const char *value) noexcept;
            //llamada interna de microhttpd
            static int internalConnection(void* cls, struct MHD_Connection* connection,
                const char* url,
                const char* method, const char* version,
                const char* upload_data,
                size_t* upload_data_size, void** con_cls) noexcept;

        public:
            //constructor
            AbstractServer(unsigned short port);
            //destructor
            ~AbstractServer();

    };

#endif // ABSTRACTSERVER_H_INCLUDED
