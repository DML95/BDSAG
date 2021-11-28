#include "updateSession.h"

#include"../db.h"
#include"../../utils/utils.h"

//PATCH /database/session/{id}
bool UpdateSession::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::patch)return false;
	if(nodes.size()!=3)return false;
	if(nodes[1]!=Constant::session)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int UpdateSession::operation(Response &response,const Request &request) {
	Log::log(Log::debug,this,INFO_LOG,"PATCH /database/session");
    std::unordered_map<std::string,std::string>::iterator headerIterator=request.headers->find("User-Agent");
    if(headerIterator==request.headers->end()){
    	Log::log(Log::warn,this,INFO_LOG,"User-Agent no encontrado");
    	UpdateSession::createMessageError(*response.json,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    if(!request.json->IsObject()){
    	Log::log(Log::warn,this,INFO_LOG,"JSON invalido");
    	UpdateSession::createMessageError(*response.json,"JSON");
        return 400;
    }
    rapidjson::Value value;
    data.updateExpireTime=Utils::isMember(value,*request.json,"expiretime");
    if(data.updateExpireTime){
        if(!value.IsInt64()||(data.expireTime=value.GetInt64())<=0){
        	Log::log(Log::warn,this,INFO_LOG,"expiretime invalido");
        	UpdateSession::createMessageError(*response.json,"JSON: expiretime");
            return 400;
        }
    }
    data.updateValue=Utils::isMember(value,*request.json,"value");
    if(data.updateValue){
        if(!value.IsString()){
        	Log::log(Log::warn,this,INFO_LOG,"value invalido");
        	UpdateSession::createMessageError(*response.json,"JSON: value");
            return 400;
        }
        data.value=value.GetString();
    }
    data.session=(*request.nodes)[2];//{id}
    switch(DB::patchSession(data)){
        case DB::sessionNotFound:
        	Log::log(Log::warn,this,INFO_LOG,"sesion no encontrada");
            return 404;
    }
    auto &allocator=response.json->GetAllocator();
    response.json->SetObject();
    response.json->AddMember("value",data.value,allocator);
    response.json->AddMember("expireepoch",data.expireTime,allocator);
    return 200;
}
