#ifndef SEMAPHORE_H
#define SEMAPHORE_H

    #include <condition_variable>
    #include <mutex>

    //clase para detener y reanudar hilos
    class Semaphore{
        private:
            std::mutex mutex;
            std::condition_variable semaphore;
        public:
            //constructor
            Semaphore();
            //detiene el hilo
            void wait();
            //activa los hilos detenidos
            void notify_all();
    };

#endif
