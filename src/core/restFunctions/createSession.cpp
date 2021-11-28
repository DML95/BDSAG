#include "createSession.h"

#include"../db.h"
#include"../../utils/utils.h"

//POST /database/session
bool CreateSession::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::post)return false;
	if(nodes.size()!=2)return false;
	if(nodes[1]!=Constant::session)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int CreateSession::operation(Response &response,const Request &request) {
	Log::log(Log::debug,this,INFO_LOG,"POST /database/session");
    std::unordered_map<std::string,std::string>::iterator headerIterator=request.headers->find("User-Agent");
    if(headerIterator==request.headers->end()){
    	Log::log(Log::warn,this,INFO_LOG,"User-Agent no encontrado");
    	CreateSession::createMessageError(*response.json,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    if(!request.json->IsObject()){
    	Log::log(Log::warn,this,INFO_LOG,"JSON invalido");
    	CreateSession::createMessageError(*response.json,"JSON");
        return 400;
    }
    rapidjson::Value value;
    if(!Utils::isMember(value,*request.json,"expiretime")||!value.IsInt64()||(data.expireTime=value.GetInt64())<=0){
    	Log::log(Log::warn,this,INFO_LOG,"expiretime invalido");
    	CreateSession::createMessageError(*response.json,"JSON: expiretime");
        return 400;
    }
    if(!Utils::isMember(value,*request.json,"value")||!value.IsString()){
    	Log::log(Log::warn,this,INFO_LOG,"value invalido");
    	CreateSession::createMessageError(*response.json,"JSON: value");
        return 400;
    }
    data.value=value.GetString();
    switch(DB::createSession(data)){
        case DB::dbFull:
        	Log::log(Log::warn,this,INFO_LOG,"base de datos llena");
            return 507;
    }
    auto &allocator=response.json->GetAllocator();
    response.json->SetObject();
    response.json->AddMember("id",data.session,allocator);
    response.json->AddMember("expireepoch",data.expireTime,allocator);
    return 201;
}
