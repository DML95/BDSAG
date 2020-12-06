#include"utils.h"

#include<sstream>
#include<fstream>
#include<chrono>

#include"../console/log.h"
#include"exception.h"
#include"../apiOS.h"

std::string Utils::units=" KMGTP";
std::string Utils::pathExecutable;
std::mt19937 Utils::rng;

Utils::StaticClass Utils::staticClass;

Utils::StaticClass::StaticClass(){
    Log::getLog(Log::trace,INFO_LOG)<<"Iniciando Utils"<<std::endl;
    Utils::getPathExecutable();
    Log::getLog(Log::trace,INFO_LOG)<<"Ruta del ejecutable: "<<Utils::pathExecutable<<std::endl;
    Log::getLog(Log::trace,INFO_LOG)<<"Iniciando el generador de numeros aleatorios"<<std::endl;
    Utils::rng=std::mt19937(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

#ifdef _WIN32

    void Utils::getPathExecutable(){
        Utils::pathExecutable=std::string(MAX_PATH,0);
        HMODULE module=::GetModuleHandle(NULL);
        size_t size=::GetModuleFileName(module,(LPSTR)Utils::pathExecutable.data(),MAX_PATH);
        Utils::pathExecutable.resize(size);
        std::size_t found=Utils::pathExecutable.find_last_of("/\\");
        Utils::pathExecutable.resize(found+1);
    }

#elif __linux__

    void Utils::getPathExecutable(){
        Utils::pathExecutable=std::string(0xff,0);
        size_t size=::readlink("/proc/self/exe",(char*)Utils::pathExecutable.data(),Utils::pathExecutable.size());
        Utils::pathExecutable.resize(size);
        std::size_t found=Utils::pathExecutable.find_last_of("/\\");
        Utils::pathExecutable.resize(found+1);
    }

#endif

std::string Utils::bytesToString(size_t bytes){
    float value=bytes;
    int cont=0;
    for(;value>=1024.;value/=1024.,cont++);
    std::stringstream out;
    out<<value<<' '<<Utils::units[cont]<<'B';
    return out.str();
}

std::string Utils::getDataFile(std::string fileName,bool pathExecutable){
    Log::getLog(Log::debug,INFO_LOG)<<"Leyendo el archivo: "<<fileName<<std::endl;
    std::string pathName=pathExecutable?Utils::pathExecutable+fileName:fileName;
    std::fstream file(pathName,std::ifstream::in|std::ios::binary|std::ios::ate);
    if(!file.is_open())throw Exception(0,"No se ha podido abrir el archivo: "+fileName,INFO_LOG);
    size_t size=file.tellg();
    std::string content(size,0);
    file.seekg(0,std::ios::beg);
    file.read((char*)content.data(),size);
    file.close();
    return content;
}

size_t Utils::getRandom(size_t max){
    Log::getLog(Log::trace,INFO_LOG)<<"Opteniendo un valor aleatorio"<<std::endl;
    std::uniform_int_distribution<std::mt19937::result_type> random(0,max);
    return random(Utils::rng);
}

std::string Utils::getRandom(std::string chars,size_t size){
    Log::getLog(Log::trace,INFO_LOG)<<"Opteniendo un string aleatorio"<<std::endl;
    std::string out(size,0);
    size_t sizeChars=chars.size()-1;
    if(0>sizeChars)throw Exception(0,"Longitud de chars invalida",INFO_LOG);
    std::uniform_int_distribution<std::mt19937::result_type> random(0,sizeChars);
    for(char &car:out){
        car=chars[random(Utils::rng)];
    }
    return out;
}

size_t Utils::getEpoch(){
    auto epoch=std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
}

bool Utils::isMember(rapidjson::Value &value,rapidjson::Document &document,std::string member){
    bool ret=document.HasMember(member);
    if(ret){
        value=document[member];
    }
    return ret;
}
