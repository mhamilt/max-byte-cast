//
//  byte-cast.hpp
//  max-external
//
//  Created by mhamilt7 on 04/07/2021.
//

#ifndef byte_cast_h
#define byte_cast_h

#include <stdio.h>
#include "ext.h"
#include "ext_obex.h"
#include "buffer.h"
//------------------------------------------------------------------------------
/// <#Description#>
/// @param argc <#argc description#>
/// @param argv <#argv description#>
t_symbol* bytesToSymbol(short argc, t_atom* argv)
{
    const size_t maxLength = 100;
    char text[maxLength];
    
    int length = (argc >= maxLength) ? maxLength-1 : argc;
    
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
/// /// decode list of t_atom_long to an unsigned int
/// @param argc number of bytes: rounded down to closest power of two and extra bytes discarded
/// @param argv <#argv description#>
/// @param littleEndian flag endianness true for little, false for big
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
/// decode list of t_atom_long to an unsigned int
/// @param argc number of bytes: rounded down to closest power of two and extra bytes discarded
/// @param argv <#argv description#>
/// @param littleEndian flag endianness true for little, false for big
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
/// <#Description#>
/// @param argc <#argc description#>
/// @param argv <#argv description#>
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
            bytes[3 - i] = (atom_getlong (argv + i) & 0xFF);
    }
    return *((float*)bytes);
}


//------------------------------------------------------------------------------

#endif /* byte_cast_hpp */
