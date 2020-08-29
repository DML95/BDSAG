#ifndef APIRAPIDJSON_H_INCLUDED
#define APIRAPIDJSON_H_INCLUDED

    //configuracion de rapidjson

    #include"utils/exception.h"

    //todos los procesadores x86_64 tienen SSE2
    #if defined(__x86_64__)
        #define RAPIDJSON_SSE2
        #pragma message "SSE2 activado para rapidjson"
    #endif

    #define RAPIDJSON_HAS_STDSTRING true
    #define RAPIDJSON_ASSERT(X) if(!(X))throw Exception(0,"Excepcion interna rapidjson",INFO_LOG)

    #include <rapidjson/document.h>
    #include <rapidjson/stringbuffer.h>
    #include <rapidjson/writer.h>

#endif // APIRAPIDJSON_H_INCLUDED
