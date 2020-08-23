#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

    #include"openCL.h"

    class DB{
        public:
            typedef struct{
                std::string session;
                std::string userAgent;
                std::string value;
                size_t expireTime;
            }data;

            enum status{
                ok,
                dbFull,
                sessionNotFound
            };

        private:

            typedef struct{
                std::mutex mutex;
                std::string session;
                std::string value;
                size_t expireTime;
            }internalData;

            typedef struct{
                int id;
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
            static std::vector<OpenCL::infoDevice> infoDevices;
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
            //devuelve un tama�o estimado para evitar desbordamientos para el buffer
            static size_t getEstimatedSizeBuffer(OpenCL::infoDevice &infoDevice);
        public:
            //crea una sesion en la BD
            static DB::status createSession(DB::data &data);
            //busca una sesion en la BD
            static DB::status getSession(DB::data &data);
    };

#endif // DB_H_INCLUDED