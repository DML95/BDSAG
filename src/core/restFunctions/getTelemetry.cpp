#include <core/restFunctions/getTelemetry.h>
#include <core/restServer.h>
#include <utils/constant.h>
#include <utils/exception.h>
#include <atomic>
#include <thread>
#include <vector>
#include"../../console/log.h"

void GetTelemetry::threadInit(){
	auto nextTime=std::chrono::steady_clock::now();
	while(this->runThread.load()){
		nextTime+=std::chrono::seconds(1);
		std::this_thread::sleep_until(nextTime);
		Log::log(Log::trace,this,INFO_LOG,"Refrescando telemetria");
		this->updateTelemetry();
	}
}

void GetTelemetry::updateTelemetry(){
	std::unique_lock writeLock(this->mutex);
	this->operations.clear();
	for(auto function:this->restServer->getFunctions()){
		Operation operation={
				function->getMethod(),
				function->getPath(),
				function->getAndResetCount()
		};
		this->operations.push_back(operation);
	}
}

GetTelemetry::GetTelemetry():
	RESTFunctionCommun("GET", "/database/telemetry"){

}

GetTelemetry::~GetTelemetry(){
	this->end();
}

bool GetTelemetry::filter(Constant::methods method,const std::vector<Constant::nodes> &nodes) {
	if(method!=Constant::get)return false;
	if(nodes.size()!=2)return false;
	if(nodes[1]!=Constant::telemetry)return false;
	if(nodes[0]!=Constant::database)return false;
	return true;
}

int GetTelemetry::operation(Response &response,const Request &request) {
	Log::log(Log::debug,this,INFO_LOG,"GET /database/telemetry");
	auto &allocator=response.json->GetAllocator();
	response.json->SetObject();
	rapidjson::Document operationsJson;
	operationsJson.SetArray();
	{
		std::shared_lock readLock(this->mutex);
		for(auto &operation:this->operations){
			rapidjson::Value operationJson;
			operationJson.SetObject();
			operationJson.AddMember("method",operation.method, allocator);
			operationJson.AddMember("path",operation.path, allocator);
			operationJson.AddMember("callspersecond",operation.callspersecond, allocator);
			operationsJson.PushBack(operationJson, allocator);
		}
	}
	response.json->AddMember("operations", operationsJson, allocator);
	return 200;
}

void GetTelemetry::init(RESTServer &restServer){
	RESTFunctionCommun::init(restServer);
	if(this->thread&&this->thread->joinable()){
		throw Exception(0,"Hilo ya iniciado",INFO_LOG);
	}
	this->runThread.store(true);
	this->thread=std::make_unique<std::thread>(&GetTelemetry::threadInit,this);
	Log::log(Log::info,this,INFO_LOG,"Iniciando hilo telemetria");
}

void GetTelemetry::end(){
	RESTFunctionCommun::end();
	this->runThread.store(false);
	if(this->thread){
		this->thread->join();
	}
	Log::log(Log::info,this,INFO_LOG,"Cerrado hilo telemetria");
}
