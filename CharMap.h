#include <memory.h>
#ifndef CHARMAP_H
#define CHARMAP_H

class CharMap
{
private:

    const static int char_set_size=65536;
    const static int char_size=10000;
    int _map[char_set_size];
    char _reversemap[char_size];
    int _index;
public:
    CharMap(){_index=0;memset(_map,-1,sizeof(_map));}
    typedef char char_t;
    int size(){return _index;}
    int Map(char_t c)
    {
        if(_map[c]==-1)
        {
            _reversemap[_index]=c;
            return _map[c]=_index++;
        }
        return _map[c];
    }
    char_t ReverseMap(int i)
    {
        return _reversemap[i];
    }
    int operator[](char_t c){return Map(c);}
};

#endif // CHARMAP_H

