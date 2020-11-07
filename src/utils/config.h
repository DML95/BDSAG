#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

    #include<string>

    class Config{
        private:
            static std::string config;
            static size_t sizeDB;
            static size_t sizeSesion;
            static unsigned short port;

            //clase estatica
            struct StaticClass{
                //constructor estatico
                StaticClass();
            };

            static StaticClass staticClass;

        public:
            static std::string getConfig();
            static size_t getSizeDB();
            static size_t getSizeSesion();
            static unsigned short getPort();
    };

#endif // CONFIG_H_INCLUDED
