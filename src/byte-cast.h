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
#define MaxAtomLength 400
/// <#Description#>
/// @param argc <#argc description#>
/// @param argv <#argv description#>
t_symbol* bytesToSymbol(short argc, t_atom* argv);
//------------------------------------------------------------------------------
/// /// decode list of t_atom_long to an unsigned int
/// @param argc number of bytes: rounded down to closest power of two and extra bytes discarded
/// @param argv <#argv description#>
/// @param littleEndian flag endianness true for little, false for big
int64_t  bytesToInt    (short argc, t_atom* argv, bool littleEndian);
//------------------------------------------------------------------------------
/// decode list of t_atom_long to an unsigned int
/// @param argc number of bytes: rounded down to closest power of two and extra bytes discarded
/// @param argv <#argv description#>
/// @param littleEndian flag endianness true for little, false for big
uint64_t  bytesToUInt    (short argc, t_atom* argv, bool littleEndian);
//------------------------------------------------------------------------------
/// <#Description#>
/// @param argc <#argc description#>
/// @param argv <#argv description#>
float bytesToFloat  (short argc, t_atom* argv, bool littleEndian);

//------------------------------------------------------------------------------
/// <#Description#>
/// @param argc <#argc description#>
/// @param argv <#argv description#>
double bytesToDouble  (short argc, t_atom* argv, bool littleEndian);
//------------------------------------------------------------------------------

#endif /* byte_cast_hpp */
