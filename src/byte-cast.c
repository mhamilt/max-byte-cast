//
//  byte-cast.cpp
//  max-external
//
//  Created by mhamilt7 on 04/07/2021.
//

#include "byte-cast.h"
//------------------------------------------------------------------------------
t_symbol* bytesToSymbol(short argc, t_atom* argv)
{
    char text[MaxAtomLength];
    
    int length = (argc >= MaxAtomLength) ? MaxAtomLength-1 : argc;
    
    for (int i = 0; i < length; i++)
    {
        t_atom* ap = &argv[i];
        switch (atom_gettype(ap))
        {
            case A_LONG:
                if((atom_getlong (ap) & 0xFF) > 31 || (atom_getlong (ap) & 0xFF) < 127)
                {
                    text[i] = (char)(atom_getlong (ap) & 0xFF);
                    break;
                }
            case A_FLOAT:
            case A_SYM:
            default:
                text[i] = ' ';
                break;
        }
        
    }
    text[length] =  '\0';
    
    return gensym(text);
}
//------------------------------------------------------------------------------
int64_t  bytesToInt    (short argc, t_atom* argv, bool littleEndian)
{
    uint8_t bytes[8];
    int64_t outputValue;
    short numBytes = ((argc / 2) * 2);
    numBytes = (numBytes > 8) ? 8 : numBytes;
    
    if (littleEndian)
    {
        for (int i = 0; i < numBytes; ++i)
            bytes[i] =  (uint8_t)(atom_getlong (argv + i) & 0xFF);
    }
    else
    {
        for (int i = 0; i < numBytes; ++i)
            bytes[i] =  (uint8_t)(atom_getlong (argv + (argc - 1) - i) & 0xFF);
    }
    
    switch (numBytes)
    {
        case 2:
            outputValue = *(int16_t*)bytes;
            break;
        case 4:
            outputValue = *(int32_t*)bytes;
            break;
        case 8:
            outputValue = *(int64_t*)bytes;
            break;
    }
    
    return outputValue;
}
//------------------------------------------------------------------------------
uint64_t  bytesToUInt    (short argc, t_atom* argv, bool littleEndian)
{
    uint8_t bytes[8];
    uint64_t outputValue;
    short numBytes = ((argc / 2) * 2);
    numBytes = (numBytes > 8) ? 8 : numBytes;
    
    if (littleEndian)
    {
        for (int i = 0; i < numBytes; ++i)
            bytes[i] =  (uint8_t)(atom_getlong (argv + i) & 0xFF);
    }
    else
    {
        for (int i = 0; i < numBytes; ++i)
            bytes[i] =  (uint8_t)(atom_getlong (argv + (argc - 1) - i) & 0xFF);
    }
    
    
    switch (numBytes)
    {
        case 2:
            outputValue = *(uint16_t*)bytes;
            break;
        case 4:
            outputValue = *(uint32_t*)bytes;
            break;
        case 8:
            outputValue = *(uint64_t*)bytes;
            break;
    }
    
    return outputValue;
}
//------------------------------------------------------------------------------
float bytesToFloat  (short argc, t_atom* argv, bool littleEndian)
{
    uint8_t bytes[4];
    
    if(littleEndian)
    {
        for (int i = 0 ; i < 4; ++i) // little endian
            bytes[i] = (atom_getlong (argv + i) & 0xFF);
    }
    else
    {
        for (int i = 0 ; i < 4; ++i)
            bytes[i] = (atom_getlong (argv + (argc - 1) - i) & 0xFF);
    }
    return *((float*)bytes);
}

//------------------------------------------------------------------------------
double bytesToDouble  (short argc, t_atom* argv, bool littleEndian)
{
    uint8_t bytes[8];
    
    if(littleEndian)
    {
        for (int i = 0 ; i < 8; ++i) // little endian
            bytes[i] = (atom_getlong (argv + i) & 0xFF);
    }
    else
    {
        for (int i = 0 ; i < 8; ++i)
            bytes[i] = (atom_getlong (argv + (argc - 1) - i) & 0xFF);
    }
    return *((double*)bytes);
}
//------------------------------------------------------------------------------
