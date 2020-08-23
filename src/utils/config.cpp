#include"config.h"

#include"../console/log.h"
#include"utils.h"
#include"../apiRapidjson.h"

size_t Config::sizeDB;
size_t Config::sizeSesion;
unsigned short Config::port;

Config::StaticClass Config::staticClass;

Config::StaticClass::StaticClass(){
    Log::getLog(Log::trace,INFO_LOG)<<"Cargando configuaracion: config.json"<<std::endl;
    std::string config=Utils::getDataFile("config.json",true);
    rapidjson::Document documen;
    documen.Parse(config);
    if(!documen.IsObject()){
        throw Exception(0,"[config.json] JSON invalido",INFO_LOG);
    }
    rapidjson::Value value;
    long long tempValue;
    //leyendo sizeDB
    if(!Utils::isMember(value,documen,"sizeDB")||!value.IsInt64()||(tempValue=value.GetInt64())<=0){
        throw Exception(0,"[config.json] sizeDB invalido",INFO_LOG);
    }
    Config::sizeDB=tempValue;
    Log::getLog(Log::info,INFO_LOG)<<"[config.json] sizeDB: "<<Config::sizeDB<<std::endl;
    //leyendo sizeSesion
    if(!Utils::isMember(value,documen,"sizeSesion")||!value.IsInt64()||(tempValue=value.GetInt64())<=0){
        throw Exception(0,"[config.json] sizeSesion invalido",INFO_LOG);
    }
    Config::sizeSesion=tempValue;
    Log::getLog(Log::info,INFO_LOG)<<"[config.json] sizeSesion: "<<Config::sizeSesion<<std::endl;
    //leyendo port
    if(!Utils::isMember(value,documen,"port")||!value.IsInt64()||(tempValue=value.GetInt64())<0||tempValue>0xffff){
        throw Exception(0,"[config.json] port invalido",INFO_LOG);
    }
    Config::port=tempValue;
    Log::getLog(Log::info,INFO_LOG)<<"[config.json] port: "<<Config::port<<std::endl;
    //leyendo logs
    std::string stringTemp;
    if(!Utils::isMember(value,documen,"logs")||!value.IsString()){
        throw Exception(0,"[config.json] logs invalido",INFO_LOG);
    }
    stringTemp=value.GetString();
    Log::setVisibility(stringTemp);
    Log::getLog(Log::info,INFO_LOG)<<"[config.json] logs: "<<stringTemp<<std::endl;
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
