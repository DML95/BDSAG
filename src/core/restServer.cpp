#include"restServer.h"

#include"../utils/config.h"
#include"../console/log.h"
#include"../utils/exception.h"
#include"db.h"
#include"../utils/utils.h"

std::vector<std::string> RESTServer::urlToNodes(std::string &url){
    size_t postion=0;
    size_t postionTemp=0;
    bool run;
    char car=url[postion++];
    if(car!='/')throw Exception(0,"Error en el formato de la URL",this,INFO_LOG);
    std::vector<std::string> nodes;
    do{
        size_t postionTemp=url.find_first_of('/',postion);
        std::string value=url.substr(postion,postionTemp-postion);
        Log::getLog(Log::trace,this,INFO_LOG)<<"Agregando nodo: "<<value<<std::endl;
        nodes.push_back(value);
        postion=postionTemp+1;
        run=postionTemp!=std::string::npos;
    }while(run);
    return nodes;
}

std::string RESTServer::documentToString(rapidjson::Document &document){
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);
    return sb.GetString();
}

void RESTServer::createMessageError(rapidjson::Document &document,std::string error){
    auto &allocator=document.GetAllocator();
    document.SetObject();
    document.AddMember("error",error,allocator);
}

int RESTServer::databaseSessionPOST(rapidjson::Document &response,rapidjson::Document &request,std::unordered_map<std::string,std::string> &headers){
    Log::getLog(Log::debug,this,INFO_LOG)<<"POST /database/session"<<std::endl;
    std::unordered_map<std::string,std::string>::iterator headerIterator=headers.find("User-Agent");
    if(headerIterator==headers.end()){
        Log::getLog(Log::warn,this,INFO_LOG)<<"User-Agent no encontrado"<<std::endl;
        RESTServer::createMessageError(response,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    if(!request.IsObject()){
        Log::getLog(Log::warn,this,INFO_LOG)<<"JSON invalido"<<std::endl;
        RESTServer::createMessageError(response,"JSON");
        return 400;
    }
    rapidjson::Value value;
    if(!Utils::isMember(value,request,"expiretime")||!value.IsInt64()||(data.expireTime=value.GetInt64())<=0){
        Log::getLog(Log::warn,this,INFO_LOG)<<"expiretime invalido"<<std::endl;
        RESTServer::createMessageError(response,"JSON: expiretime");
        return 400;
    }
    if(!Utils::isMember(value,request,"value")||!value.IsString()){
        Log::getLog(Log::warn,this,INFO_LOG)<<"value invalido"<<std::endl;
        RESTServer::createMessageError(response,"JSON: value");
        return 400;
    }
    data.value=value.GetString();
    switch(DB::createSession(data)){
        case DB::dbFull:
            Log::getLog(Log::warn,this,INFO_LOG)<<"base de datos llena"<<std::endl;
            return 507;
    }
    auto &allocator=response.GetAllocator();
    response.SetObject();
    response.AddMember("id",data.session,allocator);
    response.AddMember("expireepoch",data.expireTime,allocator);
    return 201;
}

int RESTServer::databaseSessionGET(rapidjson::Document &response,std::string &id,std::unordered_map<std::string,std::string> &headers){
    Log::getLog(Log::debug,this,INFO_LOG)<<"GET /database/session/{id}"<<std::endl;
    std::unordered_map<std::string,std::string>::iterator headerIterator=headers.find("User-Agent");
    if(headerIterator==headers.end()){
        Log::getLog(Log::warn,this,INFO_LOG)<<"User-Agent no encontrado"<<std::endl;
        RESTServer::createMessageError(response,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    data.session=id;
    switch(DB::getSession(data)){
        case DB::sessionNotFound:
            Log::getLog(Log::warn,this,INFO_LOG)<<"sesion no encontrada"<<std::endl;
            return 404;
    }
    auto &allocator=response.GetAllocator();
    response.SetObject();
    response.AddMember("value",data.value,allocator);
    response.AddMember("expireepoch",data.expireTime,allocator);
    return 200;
}

RESTServer::RESTServer():
        AbstractServer(Config::getPort()){
    Log::getLog(Log::info,this,INFO_LOG)<<"Iniciando servidor REST"<<std::endl;
}

bool RESTServer::connection(AbstractServer::Response &response,AbstractServer::Request &request){
    Log::getLog(Log::debug,this,INFO_LOG)<<"Conexion establecida"<<std::endl;
    response.headers["Content-Type"]="application/json";
    response.code=404;
    std::vector<std::string> nodes=this->urlToNodes(request.url);
    switch(nodes.size()){
        case 2:
            if(nodes[0]=="database"){
                if(nodes[1]=="session"){
                    if(request.method=="POST"){
                        rapidjson::Document requestDoc;
                        requestDoc.Parse(request.body);
                        rapidjson::Document responseDoc;
                        response.code=this->databaseSessionPOST(responseDoc,requestDoc,request.headers);
                        if(responseDoc.IsObject()){
                            response.body=RESTServer::documentToString(responseDoc);
                        }
                    }else{
                        Log::getLog(Log::warn,this,INFO_LOG)<<"Metodo invalido"<<std::endl;
                        response.code=405;
                    }
                }
            }
            break;
        case 3:
            if(nodes[0]=="database"){
                if(nodes[1]=="session"){
                    if(request.method=="GET"){
                        rapidjson::Document responseDoc;
                        response.code=databaseSessionGET(responseDoc,nodes[2],request.headers);
                        if(responseDoc.IsObject()){
                            response.body=RESTServer::documentToString(responseDoc);
                        }
                    }else{
                        Log::getLog(Log::warn,this,INFO_LOG)<<"Metodo invalido"<<std::endl;
                        response.code=405;
                    }
                }
            }
            break;
    }
    return true;
}

