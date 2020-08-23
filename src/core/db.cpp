#include"db.h"

#include"../console/log.h"
#include"../utils/config.h"
#include"../utils/utils.h"
#include"../utils/exception.h"

#include<set>

#include"../apiOS.h"

std::vector<DB::internalData> DB::datas;
std::vector<OpenCL::infoDevice> DB::infoDevices=OpenCL::getInfoDevices();
//caracteres de base64 para URL
std::string DB::chars="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

DB::StaticClass DB::staticClass;

DB::StaticClass::StaticClass(){
    Log::getLog(Log::trace,INFO_LOG)<<"Iniciando nucleo BD"<<std::endl;
    try{
        DB::datas=std::vector<DB::internalData>(Config::getSizeDB());
    }catch(std::exception &e){
        throw Exception(0,"No se ha podido reservar suficiente RAM",INFO_LOG);
    }
}

DB::pointerDevice DB::pointerToPointerDevice(size_t pointer){
    DB::pointerDevice pointerDevice={-1,0};
    if(pointer<=DB::datas.size()){
        for(OpenCL::infoDevice &infoDevice:DB::infoDevices){
            if(pointer<infoDevice.size){
                pointerDevice.id=infoDevice.id;
                pointerDevice.pointer=pointer;
                break;
            }
            pointer-=infoDevice.size;
        }
    }
    return pointerDevice;
}

size_t DB::pointerDeviceToPointer(DB::pointerDevice &pointerDevice){
    size_t pointer=std::numeric_limits<size_t>::max();
    size_t cont=0;
    for(OpenCL::infoDevice &infoDevice:DB::infoDevices){
        if(infoDevice.id==pointerDevice.id){
            pointer=cont+pointerDevice.pointer;
            break;
        }
        cont+=infoDevice.size;
    }
    return pointer;
}

DB::pointerDevice DB::getAndUpdatePostionTime(size_t expireTime,DB::status &status){
    Log::getLog(Log::debug,INFO_LOG)<<"Opteniendo posicion de tiempo"<<std::endl;
    status=DB::ok;
    size_t epoch=Utils::getEpoch();
    bool run;
    std::set<int> excludeDevice;
    DB::pointerDevice pointerDevice;
    do{
        //se busca un dispostivo que no este lleno
        do{
            //para mantener la proporcionalidad de la reparticion de datos entre los dispostivos
            //se asigna un puntero aleatorio y se calcula a que dispostivo pertenece
            pointerDevice=DB::pointerToPointerDevice(Utils::getRandom(DB::datas.size()-1));
        }while(excludeDevice.find(pointerDevice.id)!=excludeDevice.end());
        OpenCL::getPostionExpireTime(pointerDevice.id,pointerDevice.pointer,epoch,expireTime).wait();
        run=std::numeric_limits<size_t>::max()==pointerDevice.pointer;
        if(run){
            excludeDevice.insert(pointerDevice.id);
            if(excludeDevice.size()>=DB::infoDevices.size()){
                Log::getLog(Log::info,INFO_LOG)<<"No se ha podido encontrar ningun tiempo libre"<<std::endl;
                status=DB::dbFull;
            }
        }
    }while(run&&status==DB::ok);
    pointerDevice.value=epoch+expireTime;
    return pointerDevice;
}

TYPE_BUFFER DB::getHash(std::string &string){
    TYPE_BUFFER hash=0xaaaaaaaaaaaaaaaa;
    for(char &byte:string){
        hash*=31;
        hash+=byte;
    }
    return hash;
}

size_t DB::getEstimatedSizeBuffer(OpenCL::infoDevice &infoDevice){
    size_t size=infoDevice.size*sizeof(TYPE_BUFFER);
    size>>=11;
    if(size<0x100)size=0x100;
    return size;
}

DB::status DB::createSession(DB::data &data){
    Log::getLog(Log::debug,INFO_LOG)<<"Creando una sesion\n\tValor: "<<data.value<<
            "\n\tTiempo: "<<data.expireTime<<
            "\n\tUserAgent: "<<data.userAgent<<std::endl;
    DB::status status;
    DB::pointerDevice pointerDevice=DB::getAndUpdatePostionTime(data.expireTime,status);
    if(status==DB::ok){
        std::string session=Utils::getRandom(DB::chars,Config::getSizeSesion());
        size_t pointer=DB::pointerDeviceToPointer(pointerDevice);
        if(pointer==std::numeric_limits<size_t>::max())throw Exception(0,"Puntero erroneo",INFO_LOG);
        std::string sesionExtended=session+data.userAgent;
        TYPE_BUFFER hash=DB::getHash(sesionExtended);
        DB::internalData &internalData=DB::datas[pointer];
        internalData.mutex.lock();
            std::vector<cl::Event> events={
                OpenCL::setPostionHash(pointerDevice.id,hash,pointerDevice.pointer,events)
            };
            internalData.value=data.value;
            internalData.session=sesionExtended;
            data.expireTime=internalData.expireTime=pointerDevice.value;
            data.session=session;
            cl::Event::waitForEvents(events);
        internalData.mutex.unlock();
    }
    return status;
}

DB::status DB::getSession(DB::data &data){
    Log::getLog(Log::debug,INFO_LOG)<<"Creando una sesion\n\tSesion: "<<data.session<<
            "\n\tUserAgent: "<<data.userAgent<<std::endl;
    size_t epoch=Utils::getEpoch();
    DB::status status=DB::sessionNotFound;
    std::string sessionExtended=data.session+data.userAgent;
    TYPE_BUFFER hash=DB::getHash(sessionExtended);
    std::vector<DB::dataDevice> dataDevices(DB::infoDevices.size());
    size_t cont=0;
    //enviamos intrucciones de busqueda a todos los dispostivos
    for(OpenCL::infoDevice &infoDevice:DB::infoDevices){
        DB::dataDevice &dataDevice=dataDevices[cont++];
        size_t size=DB::getEstimatedSizeBuffer(infoDevice);
        dataDevice.buffer=OpenCL::createBuffer(infoDevice.id,size);
        dataDevice.events={
            OpenCL::getPostionsHash(infoDevice.id,dataDevice.buffer,hash)
        };
        dataDevice.structPostionsMap=(DB::structPostions*)infoDevice.commandQueue.enqueueMapBuffer(
                dataDevice.buffer,CL_FALSE,CL_MAP_READ,0,size,&dataDevice.events,&dataDevice.events[0]);
        infoDevice.commandQueue.flush();
    }
    //comprobamos si alguna busqueda es correcta
    cont=0;
    //recoremos los dispositivos
    for(OpenCL::infoDevice &infoDevice:DB::infoDevices){
        DB::dataDevice &dataDevice=dataDevices[cont++];
        //continuamos si no se ha encontrado una sesion
        if(status==DB::sessionNotFound){
            cl::Event::waitForEvents(dataDevice.events);
            size_t size=dataDevice.structPostionsMap->cont;
            //recorremos el buffer de salida
            for(size_t contMap=0;contMap<size;contMap++){
                size_t postionDevice=dataDevice.structPostionsMap->postions[contMap];
                DB::pointerDevice pointerDevice={
                    infoDevice.id,
                    postionDevice
                };
                size_t pointer=DB::pointerDeviceToPointer(pointerDevice);
                DB::internalData &internalData=DB::datas[pointer];
                internalData.mutex.lock();
                    TYPE_BUFFER time;
                    std::vector<cl::Event> events;//no se usa
                    OpenCL::getExpireTime(infoDevice.id,time,postionDevice,events).wait();
                    //se comprueba que la sesion haya expirado y sea valida
                    if(internalData.expireTime>epoch&&
                            time==internalData.expireTime&&
                            internalData.session==sessionExtended){
                        //se devuelven los valores de la sesion
                        data.value=internalData.value;
                        data.expireTime=internalData.expireTime;
                        status=DB::ok;
                    }
                internalData.mutex.unlock();
            }
        }
        //se cierra el map
        infoDevice.commandQueue.enqueueUnmapMemObject(dataDevice.buffer,dataDevice.structPostionsMap);
    }
    return status;
}
