#include"config.h"

#include"../console/log.h"
#include"utils.h"
#include"../apiRapidjson.h"

Lua Config::lua;
size_t Config::sizeDB;
size_t Config::sizeSesion;
unsigned short Config::port;
std::string Config::logs;

Config::StaticClass Config::staticClass;

Config::StaticClass::StaticClass(){
    Log::log(Log::trace,INFO_LOG,"Cargando configuaracion: config.lua");
    std::string config=Utils::getDataFile("config.lua",true);
    Config::lua.setCode(config);
    Config::lua.pushGlobalName("main");
    Config::lua.call(0);
    Config::lua.pop();
    Config::lua.pushGlobalName("getConfig");
    Config::lua.call(4);
    //sizeDB
    long long tempValue=Config::lua.getInteger(-4);
    Log::log(Log::trace,INFO_LOG,"[config.lua:getConfig] sizeDB:",tempValue);
	if(tempValue<=0){
		throw Exception(0,"[config.lua:getConfig] sizeDB invalido",INFO_LOG);
	}
	Config::sizeDB=tempValue;
	//sizeSesion
	tempValue=Config::lua.getInteger(-3);
	Log::log(Log::trace,INFO_LOG,"[config.lua:getConfig] sizeSesion:",tempValue);
	if(tempValue<=0){
		throw Exception(0,"[config.lua:getConfig] sizeSesion invalido",INFO_LOG);
	}
	Config::sizeSesion=tempValue;
	//port
	tempValue=Config::lua.getInteger(-2);
	Log::log(Log::trace,INFO_LOG,"[config.lua:getConfig] port:",tempValue);
	if(tempValue<=0x0||tempValue>=0xffff){
		throw Exception(0,"[config.lua:getConfig] port invalido",INFO_LOG);
	}
	Config::port=tempValue;
	//logs
	std::string stringTemp=Config::lua.getString(-1);
	Log::log(Log::trace,INFO_LOG,"[config.lua:getConfig] logs:",stringTemp);
	Log::setVisibility(stringTemp);
	Config::logs=stringTemp;
	Config::lua.pop();
}

bool Config::checkDeviceUse(std::string &platformName,std::string &deviceType,std::string &deviceName){
	Config::lua.pushGlobalName("checkDeviceUse");
	Config::lua.push(platformName);
	Config::lua.push(deviceType);
	Config::lua.push(deviceName);
	Config::lua.call(1);
	bool ret=Config::lua.getBoolean(-1);
	Config::lua.pop();
	Log::log(Log::info,INFO_LOG,"[config.lua:checkDeviceUse] use:",ret);
	return ret;
}

size_t Config::getSizeDB(){
    return Config::sizeDB;
}

size_t Config::getSizeSesion(){
    return Config::sizeSesion;
}

unsigned short Config::getPort(){
    return Config::port;
}

std::string Config::getLogs(){
	return Config::logs;
}
