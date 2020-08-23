#ifndef OPENCL_H_INCLUDED
#define OPENCL_H_INCLUDED

    #include<vector>
    #include<string>

    #include"../apiCL.h"

    #define TYPE_BUFFER unsigned long long

    class OpenCL{
        public:
            typedef struct{
                int id;
                size_t size;
                cl::CommandQueue &commandQueue;
            }infoDevice;
        private:
            typedef struct{
                cl::Buffer hash;
                cl::Buffer time;
            }buffers;

            typedef struct{
                cl::Device device;
                cl::Context context;
                cl::CommandQueue commandQueue;
                cl::Program program;
                OpenCL::buffers buffers;
            }device;

            static std::vector<std::string> extensionList;
            static std::vector<OpenCL::device> devices;

            //clase estatica
            struct StaticClass{
                //constructor estatico
                StaticClass();
            };

            static StaticClass staticClass;

            //comprueba que el dispostivo tenga las extensiones correctas
            static bool checkExtensions(cl::Device &device);
            //valida y agrega los dispostivos
            //devuelve la catidad de memoria maxima asignable a un buffer de todos los dispostivos
            static size_t validAndAddDevices();
            //crea el contexto
            static void createContext(OpenCL::device &deviceStruct);
            //crea la cola de comandos
            static void createCommandQueue(OpenCL::device &deviceStruct);
            //crea el programa
            static void createProgram(OpenCL::device &deviceStruct,std::string source);
            //crea los buffers primarios
            static void createPrimaryBuffers(OpenCL::device &deviceStruct,size_t sizeDB,size_t devicesMaxBuffer);
            //inicia los buffers primarios
            static void initPrimaryBuffers(OpenCL::device &deviceStruct,std::vector<cl::Event> &events);
            //establece el contenido de un buffer
            //devuelve un evento de finelizacion
            static cl::Event setBuffer(OpenCL::device &deviceStruct,cl::Buffer &buffer,char value);
        public:
            //devuelve informacion de los dispotivos
            static std::vector<OpenCL::infoDevice> getInfoDevices();
            //crea un buffer en un dispostivo
            static cl::Buffer createBuffer(unsigned int idDevice,size_t size);
            //obtiene una lista de las posiciones de los hashs coincidentes en un dispostivo
            static cl::Event getPostionsHash(unsigned int idDevice,cl::Buffer &positions,TYPE_BUFFER hash);
            //obtiene una posicion de tiempo caducada en un dispostivo
            static cl::Event getPostionExpireTime(unsigned int idDevice,size_t &position,size_t epoch,size_t expireTime);
            //obtiene un tiempo en un dispostivo en una posicion
            static cl::Event getExpireTime(unsigned int idDevice,TYPE_BUFFER &time,size_t position,std::vector<cl::Event> &events);
            //establece un tiempo en un dispostivo en una posicion
            static cl::Event setExpireTime(unsigned int idDevice,TYPE_BUFFER time,size_t position,std::vector<cl::Event> &events);
            //establece un hash en un dispostivo en una posicion
            static cl::Event setPostionHash(unsigned int idDevice,TYPE_BUFFER hash,size_t position,std::vector<cl::Event> &events);
    };

#endif // OPENCL_H_INCLUDED
