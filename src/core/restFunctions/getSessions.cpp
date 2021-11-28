#include "getSessions.h"

#include"../db.h"
#include"../../utils/utils.h"

//GET /database/sessions?value={regex}
bool GetSessions::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::get)return false;
	if(nodes.size()!=2)return false;
	if(nodes[1]!=Constant::sessions)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int GetSessions::operation(Response &response,const Request &request) {
	std::unordered_map<std::string,std::string>::iterator queryIterator=request.querys->find("value");
	if(queryIterator==request.querys->end()){
		Log::log(Log::warn,this,INFO_LOG,"value no encontrado");
		GetSessions::createMessageError(*response.json,"query: value");
		return 400;
	}
	std::vector<DB::data> datas=DB::getValueSession(queryIterator->second);
	int ret=204;
	if(!datas.empty()){
		ret=200;
		auto &allocator=response.json->GetAllocator();
		response.json->SetObject();
		rapidjson::Document sessions;
		sessions.SetArray();
		for(DB::data &data:datas){
			rapidjson::Value session;
			session.SetObject();
			session.AddMember("id",data.session, allocator);
			session.AddMember("expireepoch",data.expireTime, allocator);
			session.AddMember("useragent",data.userAgent, allocator);
			session.AddMember("value",data.value, allocator);
			sessions.PushBack(session, allocator);
		}
		response.json->AddMember("sessions", sessions, allocator);
	}
	return ret;
}
