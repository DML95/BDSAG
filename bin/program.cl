#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomic: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomic: enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics: enable

typedef struct{
    size_host_t cont;
    size_host_t postions[];
}structPostions;

__kernel void setBuffer(__global char *buffer,const char value){
	buffer[get_global_id(0)]=value;
}

//structPostions->cont:          iniciado a 0
//structPostions->postions[...]: iniciado a 0xffffffff...
__kernel void getPostionsHash(__global structPostions *structPostions,
        unsigned long hash,
        __global const unsigned long *hashBuffer){
    size_t position=get_global_id(0);
    if(hashBuffer[position]==hash){
        size_host_t cont=atomic_inc(&structPostions->cont);
        structPostions->postions[cont]=position;
    }
}

__kernel void getExpireTime(__global size_host_t *value,
        __global unsigned long *timeBuffer,
        size_host_t position){
    size_host_t time;
    do{
        time=atomic_xchg(&timeBuffer[position],MAX_SIZE_HOST);
    }while(time==MAX_SIZE_HOST);    //Lock
    *value=time;
    atomic_xchg(&timeBuffer[position],time);    //UnLock
}

__kernel void checkAndSetExpireTime(__global unsigned long *timeBuffer,
        size_host_t position,
        __global size_host_t *checkTime,
        size_host_t oldValue){
    size_host_t time;
    do{
        time=atomic_xchg(&timeBuffer[position],MAX_SIZE_HOST);
    }while(time==MAX_SIZE_HOST);    //Lock
    bool check=time==oldValue;
    if(check){
        time=*checkTime;
    }
    *checkTime=check;
    atomic_xchg(&timeBuffer[position],time);    //UnLock
}

//position: iniciado a 0xffffffff...
//position devuelve 0xffffffff... si no encuentra ningun espacio libre
__kernel void getPostionExpireTime(__global size_host_t *position,
        size_host_t epoch,
        size_host_t expireTime,
        __global unsigned long *timeBuffer){
    size_t localPosition=get_global_id(0);
    size_host_t time;
    do{
        time=atomic_xchg(&timeBuffer[localPosition],MAX_SIZE_HOST);
    }while(time==MAX_SIZE_HOST);    //Lock
    if(epoch>time){
        if(atomic_cmpxchg(position,MAX_SIZE_HOST,localPosition)==MAX_SIZE_HOST){
            time=epoch+expireTime;
        }
    }
    atomic_xchg(&timeBuffer[localPosition],time);   //UnLock
}
