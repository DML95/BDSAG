#ifndef API_H_INCLUDED
#define API_H_INCLUDED

    #ifdef _WIN32

        //configuracion de Win32

        #define WIN32_LEAN_AND_MEAN

        #include <windows.h>

	#elif __linux__

		#include <unistd.h>
		#include <signal.h>

	#endif

#endif // API_H_INCLUDED
