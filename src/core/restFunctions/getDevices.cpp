#include "getDevices.h"

#include"../db.h"
#include"../../utils/utils.h"

GetDevices::GetDevices():
	RESTFunctionCommun("GET", "/database/devices"){

}

bool GetDevices::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::get)return false;
	if(nodes.size()!=2)return false;
	if(nodes[1]!=Constant::devices)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int GetDevices::operation(Response &response,const Request &request) {
	Log::log(Log::debug,this,INFO_LOG,"GET /database/devices");
	auto &allocator=response.json->GetAllocator();
	response.json->SetObject();
	rapidjson::Document devices;
	devices.SetArray();
	for(OpenCL &deviceCL:OpenCL::getDevices()){
		rapidjson::Value device;
		device.SetObject();
		device.AddMember("platform",deviceCL.getPlatform(), allocator);
		device.AddMember("name",deviceCL.getName(), allocator);
		device.AddMember("type",deviceCL.getType(), allocator);
		device.AddMember("reservedsessions",deviceCL.getSizeBufers(), allocator);
		devices.PushBack(device, allocator);
	}
	response.json->AddMember("devices", devices, allocator);
	return 200;
}
