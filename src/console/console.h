#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

    #include <atomic>

    class Console{
        public:
            typedef void (*event)();
        private:
            static std::atomic<Console::event> function;
            struct plataformFunctions;
        public:

            static void setCloseEvent(Console::event function);
    };

#endif // CONSOLE_H_INCLUDED
