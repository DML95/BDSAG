#ifndef NULLBUFFER_H_INCLUDED
#define NULLBUFFER_H_INCLUDED

    #include<streambuf>

    class NullBuffer : public std::streambuf{
        public:
            int overflow(int c);
    };

#endif // NULLBUFFER_H_INCLUDED
