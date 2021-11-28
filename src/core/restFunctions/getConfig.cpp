#include "getConfig.h"

#include"../../utils/config.h"

//GET /database/config
bool GetConfig::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::get)return false;
	if(nodes.size()!=2)return false;
	if(nodes[1]!=Constant::config)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int GetConfig::operation(Response &response,const Request &request) {
	Log::log(Log::debug,this,INFO_LOG,"GET /database/config");
	auto &allocator=response.json->GetAllocator();
	response.json->SetObject();
	response.json->AddMember("sizeDB",Config::getSizeDB(),allocator);
	response.json->AddMember("sizeSesion",Config::getSizeSesion(),allocator);
	response.json->AddMember("port",Config::getPort(),allocator);
	response.json->AddMember("logs",Config::getLogs(),allocator);
	return 200;
}
