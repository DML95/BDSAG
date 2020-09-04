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

std::vector<OpenCL> OpenCL::devices;

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
    size_t maxAllocSizeAllDevices=OpenCL::validAndAddDevices();
    if(!maxAllocSizeAllDevices)throw Exception(0,"Ninguna dispostivo se ha podido iniciar",INFO_LOG);
    size_t sizeDB=Config::getSizeDB();
    Log::getLog(Log::info,INFO_LOG)<<"Total asignacion entre buffers de dispositivos: "<<Utils::bytesToString(maxAllocSizeAllDevices)<<std::endl;
    Log::getLog(Log::info,INFO_LOG)<<"Total asignacion entre buffers de requerido: "<<Utils::bytesToString(sizeDB*sizeof(TYPE_BUFFER))<<std::endl;
    if(maxAllocSizeAllDevices<sizeDB*sizeof(TYPE_BUFFER))throw Exception(0,"Los dispositivos no cuentan con sufiente VRAM",INFO_LOG);
    std::vector<cl::Event> events;
    for(OpenCL &openCL:OpenCL::devices){
        openCL.createPrimaryBuffers(sizeDB,maxAllocSizeAllDevices);
        openCL.initPrimaryBuffers(events);
    }
    cl::Event::waitForEvents(events);
}

bool OpenCL::checkExtensions(cl::Device &device){
    bool checks=true;
    std::string extensions=device.getInfo<CL_DEVICE_EXTENSIONS>();
    for(std::string extension:OpenCL::extensionList){
        bool check=extensions.find(extension)!= std::string::npos;
        Log::getLog(Log::info,device(),INFO_LOG)<<extension<<": "<<check<<std::endl;
        if(!check)checks=false;
    }
    return checks;
}

size_t OpenCL::validAndAddDevices(){
    Log::getLog(Log::trace,INFO_LOG)<<"Listando dispositivos"<<std::endl;
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    size_t maxAllocSizeAllDevices=0;
    std::string source=Utils::getDataFile("program.cl",true);
    //listando platarformas
    for(cl::Platform &platform:platforms){
        Log::getLog(Log::info,INFO_LOG)<<"Plataforma: "<<platform.getInfo<CL_PLATFORM_NAME>()<<std::endl;
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU,&devices);
        //listando GPUs de las platarformas
        for(cl::Device &device:devices){
            Log::getLog(Log::info,device(),INFO_LOG)<<"CL_DEVICE_NAME: "<<device.getInfo<CL_DEVICE_NAME>()<<std::endl;
            size_t maxAllocSize=device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
            Log::getLog(Log::info,device(),INFO_LOG)<<"CL_DEVICE_MAX_MEM_ALLOC_SIZE: "<<Utils::bytesToString(maxAllocSize)<<std::endl;
            if(OpenCL::checkExtensions(device)){
                OpenCL::devices.push_back(OpenCL(device,source));
                maxAllocSizeAllDevices+=maxAllocSize;
                Log::getLog(Log::info,device(),INFO_LOG)<<"Dispositivo agregado"<<std::endl;
            }else Log::getLog(Log::warn,device(),INFO_LOG)<<"Dispositivo no valido"<<std::endl;
        }
    }
    return maxAllocSizeAllDevices;
}

OpenCL::OpenCL(cl::Device &device,std::string &source):
        device(device),
        context(device),
        commandQueue(context,device,CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE),
        program(context,source),
        sizeBuffers(0){
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Dispostivo iniciando"<<std::endl;
    this->buildProgram();
}

void OpenCL::buildProgram(){
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Compilando el programa"<<std::endl;
    try{
        this->program.build(BUILD_OPTIONS);
    }catch(cl::Error &e){
        Log::getLog(Log::fatal,this->device(),INFO_LOG)<<"Log error:\n"<<this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device)<<std::endl;
        throw e;
    }
}

void OpenCL::createPrimaryBuffers(size_t sizeDB,size_t devicesMaxBuffer){
    size_t maxAllocSize=this->device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
    //creamos el buffer de hash
    //regla de 3 para calcular la reparticion de la memoria entre los dispostivos
    this->sizeBuffers=sizeDB*maxAllocSize/devicesMaxBuffer;
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Nuemro de elementos asignados: "<<this->sizeBuffers<<std::endl;
    size_t size=this->sizeBuffers*sizeof(TYPE_BUFFER);
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Creado buffer hash y time: "<<Utils::bytesToString(size)<<std::endl;
    this->hash=cl::Buffer(
            this->context,
            CL_MEM_READ_WRITE,
            size);
    this->time=cl::Buffer(
            this->context,
            CL_MEM_READ_WRITE,
            size);
}

void OpenCL::initPrimaryBuffers(std::vector<cl::Event> &events){
    Log::getLog(Log::trace,this->device(),INFO_LOG)<<"Iniciando bufferes primarios"<<std::endl;
    //iniciamos el buffer de hash
    events.push_back(OpenCL::setBuffer(this->hash,0));
    //iniciamos el buffer de time
    events.push_back(OpenCL::setBuffer(this->time,0));
}

cl::Event OpenCL::setBuffer(cl::Buffer &buffer,char value){
    size_t size=buffer.getInfo<CL_MEM_SIZE>();
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Iniciando buffer: "<<Utils::bytesToString(size)<<std::endl;
    cl::Kernel kernel(this->program,"setBuffer");
    kernel.setArg(0,buffer);
    kernel.setArg(1,value);
    cl::Event event;
    this->commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(size),cl::NullRange,NULL,&event);
    return event;
}

std::vector<OpenCL>& OpenCL::getDevices(){
    return OpenCL::devices;
}

size_t OpenCL::getSizeBufers(){
    return this->sizeBuffers;
}

const cl::CommandQueue& OpenCL::getCommandQueue(){
    return this->commandQueue;
}

cl::Buffer OpenCL::createBuffer(size_t size){
    cl::Buffer buffer(this->context,CL_MEM_READ_WRITE,size);
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Creado buffer: "<<Utils::bytesToString(size)<<std::endl;
    return buffer;
}

cl::Event OpenCL::getPostionsHash(cl::Buffer &positions,TYPE_BUFFER hash){
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Opteniendo las posiciones del HASH: "<<hash<<std::endl;
    //se establece el buffer a 0xff
    std::vector<cl::Event> events={
        this->setBuffer(positions,0xff)
    };
    size_t zeroVal=0;
    //se establece StructPostions.cont a 0
    this->commandQueue.enqueueWriteBuffer(positions,CL_FALSE,0,sizeof(size_t),&zeroVal,&events,&events[0]);
    cl::Kernel kernel(this->program,"getPostionsHash");
    kernel.setArg(0,positions);
    kernel.setArg(1,hash);
    kernel.setArg(2,this->hash);
    this->commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(this->sizeBuffers),cl::NullRange,&events,&events[0]);
    return events[0];
}

cl::Event OpenCL::getPostionExpireTime(size_t &position,size_t epoch,size_t expireTime){
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Opteniendo una posicion de tiempo libre\n\tEpoch: "<<epoch<<"\n\tTiempo de expiracion: "<<expireTime<<std::endl;
    //se establece el position a 0xff
    position=std::numeric_limits<size_t>::max();
    cl::Buffer positionBuffer=this->createBuffer(sizeof(size_t));
    std::vector<cl::Event> events(1);
    this->commandQueue.enqueueWriteBuffer(positionBuffer,CL_FALSE,0,sizeof(size_t),&position,NULL,&events[0]);
    cl::Kernel kernel(this->program,"getPostionExpireTime");
    kernel.setArg(0,positionBuffer);
    kernel.setArg(1,epoch);
    kernel.setArg(2,expireTime);
    kernel.setArg(3,this->time);
    this->commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(this->sizeBuffers),cl::NullRange,&events,&events[0]);
    this->commandQueue.enqueueReadBuffer(positionBuffer,CL_FALSE,0,sizeof(size_t),&position,&events,&events[0]);
    return events[0];
}

cl::Event OpenCL::getExpireTime(TYPE_BUFFER &time,size_t position,std::vector<cl::Event> &events){
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Opteniendo un tiempo en la posicion: "<<position<<std::endl;
    cl::Buffer value=this->createBuffer(sizeof(TYPE_BUFFER));
    cl::Kernel kernel(this->program,"getExpireTime");
    kernel.setArg(0,value);
    kernel.setArg(1,this->time);
    kernel.setArg(2,position);
    std::vector<cl::Event> internalEvents(1);
    this->commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(1),cl::NullRange,&events,&internalEvents[0]);
    this->commandQueue.enqueueReadBuffer(value,CL_FALSE,0,sizeof(TYPE_BUFFER),&time,&internalEvents,&internalEvents[0]);
    return internalEvents[0];
}

cl::Event OpenCL::checkAndSetExpireTime(TYPE_BUFFER &checkTime,TYPE_BUFFER oldTime,size_t position,std::vector<cl::Event> &events){
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Estableciendo un tiempo en la posicion: "<<position<<std::endl;
    cl::Buffer checkTimeBuffer=this->createBuffer(sizeof(TYPE_BUFFER));
    std::vector<cl::Event> internalEvents(1);
    this->commandQueue.enqueueWriteBuffer(checkTimeBuffer,CL_FALSE,0,sizeof(TYPE_BUFFER),&checkTime,&events,&internalEvents[0]);
    cl::Kernel kernel(this->program,"checkAndSetExpireTime");
    kernel.setArg(0,this->time);
    kernel.setArg(1,position);
    kernel.setArg(2,checkTimeBuffer);
    kernel.setArg(3,oldTime);
    this->commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,cl::NDRange(1),cl::NullRange,&internalEvents,&internalEvents[0]);
    this->commandQueue.enqueueReadBuffer(checkTimeBuffer,CL_FALSE,0,sizeof(TYPE_BUFFER),&checkTime,&internalEvents,&internalEvents[0]);
    return internalEvents[0];
}

cl::Event OpenCL::setPostionHash(TYPE_BUFFER hash,size_t position,std::vector<cl::Event> &events){
    Log::getLog(Log::debug,this->device(),INFO_LOG)<<"Estableciendo el hash("<<hash<<") en la posicion: "<<position<<std::endl;
    cl::Event event;
    this->commandQueue.enqueueWriteBuffer(this->hash,CL_FALSE,sizeof(TYPE_BUFFER)*position,sizeof(TYPE_BUFFER),&hash,&events,&event);
    return event;
}

bool OpenCL::operator==(const OpenCL &device){
    return this->device()==device.device();
}

bool OpenCL::operator!=(const OpenCL &device){
    return this->device()!=device.device();
}

bool OpenCL::operator<(const OpenCL &device){
    return this->device()<device.device();
}

bool OpenCL::operator<=(const OpenCL &device){
    return this->device()<=device.device();
}

bool OpenCL::operator>(const OpenCL &device){
    return this->device()>device.device();
}

bool OpenCL::operator>=(const OpenCL &device){
    return this->device()>=device.device();
}
