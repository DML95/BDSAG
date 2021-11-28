#include "deleteSession.h"

#include"../db.h"
#include"../../utils/utils.h"

//DELETE /database/session/{id}
bool DeleteSession::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::deleete)return false;
	if(nodes.size()!=3)return false;
	if(nodes[1]!=Constant::session)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int DeleteSession::operation(Response &response,const Request &request) {
	Log::log(Log::debug,this,INFO_LOG,"DELETE /database/session/{id}");
    std::unordered_map<std::string,std::string>::iterator headerIterator=request.headers->find("User-Agent");
    if(headerIterator==request.headers->end()){
    	Log::log(Log::warn,this,INFO_LOG,"User-Agent no encontrado");
    	DeleteSession::createMessageError(*response.json,"header: User-Agent");
        return 400;
    }
    DB::data data;
    data.userAgent=headerIterator->second;
    data.session=(*request.nodes)[2];//{id}
    switch(DB::deleteSession(data)){
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
