#ifndef OPENCL_H_INCLUDED
#define OPENCL_H_INCLUDED

    #include<vector>
    #include<string>
	#include <unordered_map>

    #include"../apiCL.h"

    #define TYPE_BUFFER unsigned long long

    class OpenCL{
        private:

    		static std::unordered_map<cl_device_type,std::string> deviceTipeName;
            static std::vector<std::string> extensionList;
            static std::vector<OpenCL> devices;

            //clase estatica
            struct StaticClass{
                //constructor estatico
                StaticClass();
            };

            static StaticClass staticClass;

            const cl::Device device;
            const cl::Context context;
            const cl::CommandQueue commandQueue;
            const cl::Program program;
            size_t sizeBuffers;
            cl::Buffer hash;
            cl::Buffer time;

            //comprueba que el dispostivo tenga las extensiones correctas
            static bool checkExtensions(cl::Device &device);
            //valida y agrega los dispostivos
            //devuelve la catidad de memoria maxima asignable a un buffer de todos los dispostivos
            static size_t validAndAddDevices();
            //constructor privado
            OpenCL(cl::Device &device,std::string &source);
            //compila el programa
            void buildProgram();
            //crea los buffers primarios
            void createPrimaryBuffers(size_t sizeDB,size_t devicesMaxBuffer);
            //inicia los buffers primarios
            void initPrimaryBuffers(std::vector<cl::Event> &events);
            //establece el contenido de un buffer
            //devuelve un evento de finelizacion
            cl::Event setBuffer(cl::Buffer &buffer,char value);

        public:
            //devuelve los dispotivos
            static std::vector<OpenCL>& getDevices();
            //devuelve el tamaño del buffer
            size_t getSizeBufers();
            //devuelve la lista de comando
            const cl::CommandQueue& getCommandQueue();
            //crea un buffer en un dispostivo
            cl::Buffer createBuffer(size_t size);
            //obtiene una lista de las posiciones de los hashs coincidentes en un dispostivo
            cl::Event getPostionsHash(cl::Buffer &positions,TYPE_BUFFER hash);
            //obtiene una posicion de tiempo caducada en un dispostivo
            cl::Event getPostionExpireTime(size_t &position,size_t epoch,size_t expireTime);
            //obtiene un tiempo en un dispostivo en una posicion
            cl::Event getExpireTime(TYPE_BUFFER &time,size_t position,std::vector<cl::Event> *events=NULL);
            //chequea y establece un tiempo en un dispostivo en una posicion
            cl::Event checkAndSetExpireTime(TYPE_BUFFER &checkTime,TYPE_BUFFER oldTime,size_t position,std::vector<cl::Event> *events=NULL);
            //establece un hash en un dispostivo en una posicion
            cl::Event setPostionHash(TYPE_BUFFER hash,size_t position,std::vector<cl::Event> *events=NULL);
            //cuenta las sesiones activas
            cl::Event getUseCount(size_t &count,size_t epoch,std::vector<cl::Event> *events=NULL);

            //getter y setter
            std::string getPlatform();
            std::string getName();
            std::string getType();

            //operadores sobrecargados para mapas, listas y condiciones
            bool operator==(const OpenCL &device);
            bool operator!=(const OpenCL &device);
            bool operator<(const OpenCL &device);
            bool operator<=(const OpenCL &device);
            bool operator>(const OpenCL &device);
            bool operator>=(const OpenCL &device);
    };

#endif // OPENCL_H_INCLUDED
