#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
	#include "../core/lua.h"

    #include<string>

    class Config{
        private:
    		static Lua lua;
            static size_t sizeDB;
            static size_t sizeSesion;
            static unsigned short port;
            static std::string logs;

            //clase estatica
            struct StaticClass{
                //constructor estatico
                StaticClass();
            };

            static StaticClass staticClass;

        public:

            static bool checkDeviceUse(std::string &platformName,std::string &deviceType,std::string &deviceName);

            static size_t getSizeDB();
            static size_t getSizeSesion();
            static unsigned short getPort();
            static std::string getLogs();
    };

#endif // CONFIG_H_INCLUDED
