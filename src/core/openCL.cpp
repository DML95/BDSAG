#include"openCL.h"

#include"../console/log.h"
#include"../utils/exception.h"
#include"../utils/utils.h"
#include"../utils/config.h"

#include <limits>

std::vector<std::string> OpenCL::extensionList={
    "cl_khr_global_int32_base_atomic",
    "cl_khr_global_int32_extended_atomic",
    "cl_khr_int64_base_atomics",
    "cl_khr_int64_extended_atomics"
};

std::vector<OpenCL::device> OpenCL::devices;

OpenCL::StaticClass OpenCL::staticClass;

//opciones del compilador de OpenCL dependiendo de la arquitectura host
#ifdef __x86_64__
    #define BUILD_OPTIONS "-D size_host_t=\"unsigned long\" -D MAX_SIZE_HOST=0xffffffffffffffff"
#else
    #define BUILD_OPTIONS "-D size_host_t=\"unsigned int\" -D MAX_SIZE_HOST=0xffffffff"
#endif

#pragma message BUILD_OPTIONS

OpenCL::StaticClass::StaticClass(){
    Log::getLog(Log::trace,INFO_LOG)<<"Iniciando nucleo OpenCL"<<std::endl;
    std::string source=Utils::getDataFile("program.cl",true);
    size_t maxAllocSizeAllDevices=OpenCL::validAndAddDevices();
    if(!maxAllocSizeAllDevices)throw Exception(0,"Ninguna dispostivo se ha podido iniciar",INFO_LOG);
    size_t sizeDB=Config::getSizeDB();
    Log::getLog(Log::info,INFO_LOG)<<"Total asignacion entre buffers de dispositivos: "<<Utils::bytesToString(maxAllocSizeAllDevices)<<std::endl;
    Log::getLog(Log::info,INFO_LOG)<<"Total asignacion entre buffers de requerido: "<<Utils::bytesToString(sizeDB*sizeof(TYPE_BUFFER))<<std::endl;
    if(maxAllocSizeAllDevices<sizeDB*sizeof(TYPE_BUFFER))throw Exception(0,"Los dispositivos no cuentan con sufiente VRAM",INFO_LOG);
    std::vector<cl::Event> events;
    for(OpenCL::device &deviceStruct:OpenCL::devices){
        Log::getLog(Log::info,INFO_LOG)<<"Iniciando: "<<deviceStruct.device.getInfo<CL_DEVICE_NAME>()<<std::endl;
        OpenCL::createContext(deviceStruct);
        OpenCL::createCommandQueue(deviceStruct);
        OpenCL::createProgram(deviceStruct,source);
        OpenCL::createPrimaryBuffers(deviceStruct,sizeDB,maxAllocSizeAllDevices);
        OpenCL::initPrimaryBuffers(deviceStruct,events);
    }
    cl::Event::waitForEvents(events);
}

bool OpenCL::checkExtensions(cl::Device &device){
    bool checks=true;
    std::string extensions=device.getInfo<CL_DEVICE_EXTENSIONS>();
    for(std::string extension:OpenCL::extensionList){
        bool check=extensions.find(extension)!= std::string::npos;
        Log::getLog(Log::info,INFO_LOG)<<extension<<": "<<check<<std::endl;
        if(!check)checks=false;
    }
    return checks;
}

size_t OpenCL::validAndAddDevices(){
    Log::getLog(Log::trace,INFO_LOG)<<"Listando dispositivos"<<std::endl;
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    size_t maxAllocSizeAllDevices=0;
    //listando platarformas
    for(cl::Platform &platform:platforms){
        Log::getLog(Log::info,INFO_LOG)<<"Plataforma: "<<platform.getInfo<CL_PLATFORM_NAME>()<<std::endl;
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU,&devices);
        //listando GPUs de las platarformas
        for(cl::Device &device:devices){
            Log::getLog(Log::info,INFO_LOG)<<"CL_DEVICE_NAME: "<<device.getInfo<CL_DEVICE_NAME>()<<std::endl;
            size_t maxAllocSize=device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
            Log::getLog(Log::info,INFO_LOG)<<"CL_DEVICE_MAX_MEM_ALLOC_SIZE: "<<Utils::bytesToString(maxAllocSize)<<std::endl;
            if(OpenCL::checkExtensions(device)){
                OpenCL::devices.push_back((OpenCL::device){device,});
                maxAllocSizeAllDevices+=maxAllocSize;
                Log::getLog(Log::info,INFO_LOG)<<"Dispositivo agregado"<<std::endl;
            }else Log::getLog(Log::warn,INFO_LOG)<<"Dispositivo no valido"<<std::endl;
        }
    }
    return maxAllocSizeAllDevices;
}

void OpenCL::createContext(OpenCL::device &deviceStruct){
    Log::getLog(Log::debug,INFO_LOG)<<"Creando contexto"<<std::endl;
    for(OpenCL::device &deviceStruct:OpenCL::devices){
        deviceStruct.context=cl::Context(deviceStruct.device);
    }
}

void OpenCL::createCommandQueue(OpenCL::device &deviceStruct){
    Log::getLog(Log::debug,INFO_LOG)<<"Creando cola de comandos"<<std::endl;
    deviceStruct.commandQueue=cl::CommandQueue(
            deviceStruct.context,
            deviceStruct.device,
            CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
}

void OpenCL::createProgram(OpenCL::device &deviceStruct,std::string source){
    Log::getLog(Log::debug,INFO_LOG)<<"Creando el programa"<<std::endl;
    deviceStruct.program=cl::Program(deviceStruct.context,source);
    try{
        deviceStruct.program.build(BUILD_OPTIONS);
    }catch(cl::Error &e){
        Log::getLog(Log::fatal,INFO_LOG)<<"Log error:\n"<<deviceStruct.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceStruct.device)<<std::endl;
        throw e;
    }
}

void OpenCL::createPrimaryBuffers(OpenCL::device &deviceStruct,size_t sizeDB,size_t devicesMaxBuffer){
    size_t maxAllocSize=deviceStruct.device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
    //creamos el buffer de hash
    //regla de 3 para calcular la reparticion de la memoria entre los dispostivos
    size_t size=sizeDB*sizeof(TYPE_BUFFER)*maxAllocSize/devicesMaxBuffer;
    deviceStruct.buffers.hash=cl::Buffer(
            deviceStruct.context,
            CL_MEM_READ_WRITE,
            size);
    Log::getLog(Log::debug,deviceStruct.buffers.hash(),INFO_LOG)<<"Creado buffer hash: "<<Utils::bytesToString(size)<<std::endl;
    //creamos el buffer de time
    //regla de 3 para calcular la reparticion de la memoria entre los dispostivos
    size=sizeDB*sizeof(TYPE_BUFFER)*maxAllocSize/devicesMaxBuffer;
    deviceStruct.buffers.time=cl::Buffer(
            deviceStruct.context,
            CL_MEM_READ_WRITE,
            size);
    Log::getLog(Log::debug,deviceStruct.buffers.time(),INFO_LOG)<<"Creado buffer time: "<<Utils::bytesToString(size)<<std::endl;
}

void OpenCL::initPrimaryBuffers(OpenCL::device &deviceStruct,std::vector<cl::Event> &events){
    Log::getLog(Log::trace,deviceStruct.buffers.hash(),INFO_LOG)<<"Iniciando bufferes primarios"<<std::endl;
    //iniciamos el buffer de hash
    events.push_back(OpenCL::setBuffer(deviceStruct,deviceStruct.buffers.hash,0));
    //iniciamos el buffer de time
    events.push_back(OpenCL::setBuffer(deviceStruct,deviceStruct.buffers.time,0));
}

cl::Event OpenCL::setBuffer(OpenCL::device &deviceStruct,cl::Buffer &buffer,char value){
    size_t size=buffer.getInfo<CL_MEM_SIZE>();
    Log::getLog(Log::debug,buffer(),INFO_LOG)<<"Iniciando buffer: "<<Utils::bytesToString(size)<<std::endl;
    cl::Kernel kernel(deviceStruct.program,"setBuffer");
    kernel.setArg(0,buffer);
    kernel.setArg(1,value);
    cl::Event event;
    deviceStruct.commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(size),cl::NullRange,NULL,&event);
    return event;
}

std::vector<OpenCL::infoDevice> OpenCL::getInfoDevices(){
    std::vector<OpenCL::infoDevice> infoDevices;
    for(int cont=0,size=OpenCL::devices.size();cont<size;cont++){
        OpenCL::device &deviceStruct=OpenCL::devices[cont];
        OpenCL::infoDevice infoDevice={
            cont,
            deviceStruct.buffers.hash.getInfo<CL_MEM_SIZE>()/sizeof(TYPE_BUFFER),
            deviceStruct.commandQueue
        };
        infoDevices.push_back(infoDevice);
    }
    return infoDevices;
}

cl::Buffer OpenCL::createBuffer(unsigned int idDevice,size_t size){
    OpenCL::device &deviceStruct=OpenCL::devices[idDevice];
    cl::Buffer buffer(deviceStruct.context,CL_MEM_READ_WRITE,size);
    Log::getLog(Log::debug,buffer(),INFO_LOG)<<"Creado buffer: "<<Utils::bytesToString(size)<<std::endl;
    return buffer;
}

cl::Event OpenCL::getPostionsHash(unsigned int idDevice,cl::Buffer &positions,TYPE_BUFFER hash){
    Log::getLog(Log::debug,INFO_LOG)<<"Opteniendo las posiciones del HASH: "<<hash<<std::endl;
    OpenCL::device &deviceStruct=OpenCL::devices[idDevice];
    //se establece el buffer a 0xff
    std::vector<cl::Event> events={
        OpenCL::setBuffer(deviceStruct,positions,0xff)
    };
    size_t zeroVal=0;
    //se establece StructPostions.cont a 0
    deviceStruct.commandQueue.enqueueWriteBuffer(positions,CL_FALSE,0,sizeof(size_t),&zeroVal,&events,&events[0]);
    cl::Kernel kernel(deviceStruct.program,"getPostionsHash");
    kernel.setArg(0,positions);
    kernel.setArg(1,hash);
    kernel.setArg(2,deviceStruct.buffers.hash);
    size_t size=deviceStruct.buffers.hash.getInfo<CL_MEM_SIZE>()/sizeof(TYPE_BUFFER);
    deviceStruct.commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(size),cl::NullRange,&events,&events[0]);
    return events[0];
}

cl::Event OpenCL::getPostionExpireTime(unsigned int idDevice,size_t &position,size_t epoch,size_t expireTime){
    Log::getLog(Log::debug,INFO_LOG)<<"Opteniendo una posicion de tiempo libre\n\tEpoch: "<<epoch<<"\n\tTiempo de expiracion: "<<expireTime<<std::endl;
    OpenCL::device &deviceStruct=OpenCL::devices[idDevice];
    //se establece el position a 0xff
    position=std::numeric_limits<size_t>::max();
    cl::Buffer positionBuffer=OpenCL::createBuffer(idDevice,sizeof(size_t));
    std::vector<cl::Event> events(1);
    deviceStruct.commandQueue.enqueueWriteBuffer(positionBuffer,CL_FALSE,0,sizeof(size_t),&position,NULL,&events[0]);
    cl::Kernel kernel(deviceStruct.program,"getPostionExpireTime");
    kernel.setArg(0,positionBuffer);
    kernel.setArg(1,epoch);
    kernel.setArg(2,expireTime);
    kernel.setArg(3,deviceStruct.buffers.time);
    size_t size=deviceStruct.buffers.time.getInfo<CL_MEM_SIZE>()/sizeof(TYPE_BUFFER);
    deviceStruct.commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(size),cl::NullRange,&events,&events[0]);
    deviceStruct.commandQueue.enqueueReadBuffer(positionBuffer,CL_FALSE,0,sizeof(size_t),&position,&events,&events[0]);
    return events[0];
}

cl::Event OpenCL::getExpireTime(unsigned int idDevice,TYPE_BUFFER &time,size_t position,std::vector<cl::Event> &events){
    Log::getLog(Log::debug,INFO_LOG)<<"Opteniendo un tiempo en la posicion: "<<position<<std::endl;
    OpenCL::device &deviceStruct=OpenCL::devices[idDevice];
    cl::Buffer value=OpenCL::createBuffer(idDevice,sizeof(TYPE_BUFFER));
    cl::Kernel kernel(deviceStruct.program,"getExpireTime");
    kernel.setArg(0,value);
    kernel.setArg(1,deviceStruct.buffers.time);
    kernel.setArg(2,position);
    std::vector<cl::Event> internalEvents(1);
    deviceStruct.commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(1),cl::NullRange,&events,&internalEvents[0]);
    deviceStruct.commandQueue.enqueueReadBuffer(value,CL_FALSE,0,sizeof(TYPE_BUFFER),&time,&internalEvents,&internalEvents[0]);
    return internalEvents[0];
}

cl::Event OpenCL::setExpireTime(unsigned int idDevice,TYPE_BUFFER time,size_t position,std::vector<cl::Event> &events){
    Log::getLog(Log::debug,INFO_LOG)<<"Estableciendo un tiempo en la posicion: "<<position<<std::endl;
    OpenCL::device &deviceStruct=OpenCL::devices[idDevice];
    cl::Buffer value=OpenCL::createBuffer(idDevice,sizeof(TYPE_BUFFER));
    cl::Kernel kernel(deviceStruct.program,"setExpireTime");
    kernel.setArg(0,deviceStruct.buffers.time);
    kernel.setArg(1,position);
    kernel.setArg(2,time);
    std::vector<cl::Event> internalEvents(1);
    deviceStruct.commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(1),cl::NullRange,&events,&internalEvents[0]);
    deviceStruct.commandQueue.enqueueReadBuffer(value,CL_FALSE,0,sizeof(TYPE_BUFFER),&time,&internalEvents,&internalEvents[0]);
    return internalEvents[0];
}

cl::Event OpenCL::setPostionHash(unsigned int idDevice,TYPE_BUFFER hash,size_t position,std::vector<cl::Event> &events){
    Log::getLog(Log::debug,INFO_LOG)<<"Estableciendo el hash("<<hash<<") en la posicion: "<<position<<std::endl;
    OpenCL::device &deviceStruct=OpenCL::devices[idDevice];
    cl::Event event;
    deviceStruct.commandQueue.enqueueWriteBuffer(deviceStruct.buffers.hash,CL_FALSE,sizeof(TYPE_BUFFER)*position,sizeof(TYPE_BUFFER),&hash,&events,&event);
    return event;
}
