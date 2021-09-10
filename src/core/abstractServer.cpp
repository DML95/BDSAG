#include"abstractServer.h"

#include"../console/log.h"
#include"../utils/exception.h"

struct MHD_Daemon AbstractServer::*server=0;

int AbstractServer::createResponse(struct MHD_Connection* connection,AbstractServer::Response &response,AbstractServer *abstractServer){
    //se agrega el body
    Log::log(Log::debug,abstractServer,INFO_LOG,"[Respuesta] Cuerpo:",response.body);
    struct MHD_Response *mhdResponse=MHD_create_response_from_buffer (response.body.size(),(void*)response.body.data(),MHD_RESPMEM_MUST_COPY);
    //se agregan las cabeceras
    for(std::pair<std::string,std::string> header:response.headers){
    	Log::log(Log::debug,abstractServer,INFO_LOG,"[Respuesta] Cabecera: [",header.first,':',header.second,']');
        MHD_add_response_header(mhdResponse,header.first.data(),header.second.data());
    }
    //se agrega el codigo de respuesta
    Log::log(Log::debug,abstractServer,INFO_LOG,"[Respuesta] Codigo:",response.code);
    int ret=MHD_queue_response(connection,response.code,mhdResponse);
    MHD_destroy_response(mhdResponse);
    return ret;
}

int AbstractServer::addKeyValue(void *cls,MHD_ValueKind kind, const char *key, const char *value) noexcept{
    std::unordered_map<std::string,std::string> *query=(std::unordered_map<std::string,std::string>*)cls;
    std::string strKey(key);
    std::string strValue;
    if(value)strValue=std::string(value);
    query->insert({strKey,strValue});
    Log::log(Log::trace,INFO_LOG,"[Peticion] Agegado: [",strKey,':',strValue,']');
    return MHD_YES;
}

int AbstractServer::internalConnection(void* cls, struct MHD_Connection* connection,
                const char* url,
                const char* method, const char* version,
                const char* upload_data,
                size_t* upload_data_size, void** con_cls) noexcept{
    int ret=MHD_NO;
    AbstractServer *abstractServer=(AbstractServer*)cls;
    AbstractServer::InfoBodyRequest *infoBodyRequest=(AbstractServer::InfoBodyRequest*)*con_cls;
    if(!infoBodyRequest){
    	Log::log(Log::trace,abstractServer,INFO_LOG,"[Peticion] Pidiendo el cuerpo");
        infoBodyRequest=new InfoBodyRequest;
        infoBodyRequest->processed=false;
        *con_cls=infoBodyRequest;
        ret=MHD_YES;
    }else if(!infoBodyRequest->processed&&*upload_data_size){
    	Log::log(Log::trace,abstractServer,INFO_LOG,"[Peticion] Leyendo el cuerpo");
        if(upload_data)infoBodyRequest->body=std::string(upload_data);
        infoBodyRequest->processed=true;
        *upload_data_size=0;
        ret=MHD_YES;
    }else{
    	Log::log(Log::trace,abstractServer,INFO_LOG,"[Peticion] Cuerpo procesado");
        AbstractServer::Request request;
        request.method=std::string(method);
        Log::log(Log::debug,abstractServer,INFO_LOG,"[Peticion] Metodo:",request.method);
        request.url=std::string(url);
        Log::log(Log::debug,abstractServer,INFO_LOG,"[Peticion] URL:",request.url);
        Log::log(Log::debug,abstractServer,INFO_LOG,"[Peticion] Agregando consultas");
        MHD_get_connection_values(connection,MHD_GET_ARGUMENT_KIND,AbstractServer::addKeyValue,&request.querys);
        Log::log(Log::debug,abstractServer,INFO_LOG,"[Peticion] Agregando cabeceras");
        MHD_get_connection_values(connection,MHD_HEADER_KIND,AbstractServer::addKeyValue,&request.headers);
        request.body=infoBodyRequest->body;
        Log::log(Log::debug,abstractServer,INFO_LOG,"[Peticion] Cuerpo:",request.body);
        try{
            AbstractServer::Response response;
            if(abstractServer->connection(response,request)){
                ret=AbstractServer::createResponse(connection,response,abstractServer);
            }
        }catch(const std::exception &e){
        	Log::log(Log::error,abstractServer,INFO_LOG,"Excepcion incontrolada:\n\t",e.what());
            AbstractServer::Response response;
            response.code=500;
            ret=AbstractServer::createResponse(connection,response,abstractServer);
        }
        delete infoBodyRequest;
    }
    return ret;
}

AbstractServer::AbstractServer(const unsigned short port){
	Log::log(Log::info,this,INFO_LOG,"Iniciando servidor HTTP abstracto puerto:",port);
    this->server=MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
            port,
            NULL,
            NULL,
            AbstractServer::internalConnection,
            this,
            MHD_OPTION_END);
    if(!this->server)throw Exception(0,"No se ha podido iniciar el servidor HTTP",this,INFO_LOG);
}

AbstractServer::~AbstractServer(){
	Log::log(Log::info,this,INFO_LOG,"Cerrando servidor HTTP abstracto");
    MHD_stop_daemon((MHD_Daemon*)this->server);
}
