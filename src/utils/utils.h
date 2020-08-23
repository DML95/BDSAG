#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

    #include<string>
    #include<random>

    #include"../apiRapidjson.h"

    class Utils{
        private:
            static std::string units;
            static std::string pathExecutable;
            static std::mt19937 rng;

            //clase estatic
            struct StaticClass{
                //constructor estatico
                StaticClass();
            };

            static StaticClass staticClass;
            //obtiene la ruta del ejecutable
            static void getPathExecutable();
        public:
            //combierte bytes en un string con unidades
            static std::string bytesToString(size_t bytes);
            //obtiene el contenido de un fichero
            static std::string getDataFile(std::string fileName,bool pathExecutable);
            //obtiene eun valor aleatorio
            static size_t getRandom(size_t max);
            //obtiene un string aleatorio
            static std::string getRandom(std::string chars,size_t size);
            //obtinen la hora epoch en segundos
            static size_t getEpoch();
            //optiene un mienbro del documento y devuelve si existe
            static bool isMember(rapidjson::Value &value,rapidjson::Document &document,std::string member);
    };

#endif // UTILS_H_INCLUDED
