#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

	#include <shared_mutex>

    #include"openCL.h"

    class DB{
        public:
            typedef struct{
                std::string session;
                std::string userAgent;
                bool updateValue;
                std::string value;
                bool updateExpireTime;
                size_t expireTime;
            }data;

            enum status{
                ok,
                dbFull,
                sessionNotFound
            };

        private:

            typedef struct{
                std::shared_mutex mutex;
                std::string session;
                std::string value;
                size_t expireTime;
            }internalData;

            typedef struct{
                OpenCL *device;
                size_t pointer;
                TYPE_BUFFER value;
            }pointerDevice;

            //tiene que cincidir con la structura del programa de opencl
            typedef struct{
                size_t cont;
                size_t postions[];
            }structPostions;

            typedef struct{
                cl::Buffer buffer;
                std::vector<cl::Event> events;
                DB::structPostions *structPostionsMap;
            }dataDevice;

            static std::vector<DB::internalData> datas;
            static std::vector<OpenCL> &openCl;
            static std::string chars;

            //clase estatica
            struct StaticClass{
                //constructor estatico
                StaticClass();
            };

            static StaticClass staticClass;


            //calcula a que dispositivo y posicion le pertenece un puntero
            static DB::pointerDevice pointerToPointerDevice(size_t pointer);
            //calcula a que puntero le pertenece un dispositivo y posicion
            static size_t pointerDeviceToPointer(DB::pointerDevice &pointerDevice);
            //obtiene una posicion de tiempo caducada y lo actualiza al tiempo de expircion
            static DB::pointerDevice getAndUpdatePostionTime(size_t expireTime,DB::status &status);
            //obtiene un hash a partir de una cadena
            static TYPE_BUFFER getHash(std::string &string);
            //devuelve un tamaño estimado para evitar desbordamientos para el buffer
            static size_t getEstimatedSizeBuffer(OpenCL &device);
            //correccion de error producido en algunos cotroladores que genera que clFlush devuelva CL_OUT_OF_RESOURCES
            //la correccion consiste en llamar repetidas veces a clFlush hasta que no devuelva CL_OUT_OF_RESOURCES
            static void flush(const cl::CommandQueue &commandQueue);
            //busca una sesion por su hash y llama a una funcion cuando encuentra coincidencias
            //si se repite el hash se llama varias veces
            static DB::status findSession(bool (*sessionCallBack)(DB::pointerDevice&,DB::internalData&,DB::data&,std::string&,size_t),DB::data &data);
            //chequea si la sesion es correcta y la devuelve en caso afirmativo
            static bool checkAndGetSession(DB::pointerDevice &pointerDevice,DB::internalData &internalData,DB::data &data,std::string &sessionExtended,size_t epoch);
            //chequea si la sesion es correcta y la modifica en caso afirmativo
            static bool checkAndPatchSession(DB::pointerDevice &pointerDevice,DB::internalData &internalData,DB::data &data,std::string &sessionExtended,size_t epoch);
            //chequea si la sesion es correcta y la elimina en caso afirmativo
            static bool checkAndDeleteSession(DB::pointerDevice &pointerDevice,DB::internalData &internalData,DB::data &data,std::string &sessionExtended,size_t epoch);
        public:
            //crea una sesion en la BD
            static DB::status createSession(DB::data &data);
            //devuelve una sesion en la BD
            static DB::status getSession(DB::data &data);
            //modifica una sesion en la BD
            static DB::status patchSession(DB::data &data);
            //elimina una sesion de la BD
            static DB::status deleteSession(DB::data &data);
            //cuanta las sesiones activas
            static size_t countSessions();
            //devuelve las sesiones que tengan un valor coincidente
            static std::vector<DB::data> getValueSession(std::string regex);
    };

#endif // DB_H_INCLUDED
