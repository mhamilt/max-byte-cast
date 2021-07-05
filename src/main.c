/*
 Byte-cast: casting data in max msp
 
 Define Functionality:
 
 If argc == 1:
 if argv[0] == 's' then greedily spit out a symbol
 outside of that, use standard printf fmt for data
 If argc > 1:
 i     32-bit int
 h     16-bit int
 f     32-bit float
 d     64-bit float (double)
 c     8-bit character
 s     string until \0
 sxxx  string where xxx is the number of characters
 
 Modifiers:
 u   unsigned      (prepend)
 >   Big endian    (append)
 <   Little endian (append)
 
 (e.g. uh< unsigned 16-bit int little endian)
 
 */
#include "ext.h"
#include "ext_obex.h"
#include "buffer.h"
#include "byte-cast.h"
//------------------------------------------------------------------------------

/// void* to the complete new Max External class so that it can be used in the class methods
/// This will be set to t_class* in the main function
/// @code t_class* c = class_new(...);
/// myExternClass = c;
void* myExternClass;

///
typedef enum _OutletDataType
{
    BYTE_OUT,
    INT16_OUT,
    UINT16_OUT,
    INT32_OUT,
    UINT32_OUT,
    FLOAT_OUT,
    STRING_OUT,
    UNSIGNED_OUT,
    INVALID_OUT,
    UNKNOWN_OUT
}OutletDataType;

//------------------------------------------------------------------------------
/** @struct
 The MaxMSP object
 */
typedef struct _MaxExternalObject
{
    t_object  x_obj;
    t_symbol* x_arrayname;
    t_atom* atomList;
    size_t atomListSize;
    void*  mainOutlet;
    short  inletConnection;
    double gain;
    OutletDataType outletType;
    bool isLittleEndian;
    
} MaxExternalObject;

void setMaxObjectOutletType(MaxExternalObject* maxObjectPtr, long argc, t_atom *argv);

//------------------------------------------------------------------------------
/// External Object Constructor: use this to setup any variables / properties of your DSP Struct or MaxExternalObject
/// Arguement list should be as long as the list of type arguments passed in the class_new call below.
/// @param arg1 first argument to object: should match type given in class_new(...)
/// @returns a void* to an instance of the MaxExternalObject
void* myExternalConstructor(t_symbol *s, long argc, t_atom *argv)
{
    //--------------------------------------------------------------------------
    MaxExternalObject* maxObjectPtr = (MaxExternalObject*)object_alloc(myExternClass);
    //--------------------------------------------------------------------------
    intin(maxObjectPtr, 1);
    maxObjectPtr->atomList = NULL;
    maxObjectPtr->atomListSize = 100;
    maxObjectPtr->isLittleEndian = false;
    
    long numAllocatedAtoms;
    char memoryWasAllocated;
    
    atom_alloc_array(maxObjectPtr->atomListSize,
                     &numAllocatedAtoms,
                     &(maxObjectPtr->atomList),
                     &memoryWasAllocated);
    
    
    setMaxObjectOutletType(maxObjectPtr, argc, argv);
    //--------------------------------------------------------------------------
    return maxObjectPtr;
}

//------------------------------------------------------------------------------
/// @brief what happens when the object is deleted
void myExternDestructor(MaxExternalObject* maxObjectPtr)
{
}
//------------------------------------------------------------------------------

/// @brief This is the function called by MAX/MSP when the cursor is over an inlet or
/// outlet.
/// @param maxObjectPtr object pointer
/// @param box still don't know what this is
/// @param message either inlet  1 or outlet 2
/// @param arg inlet / outlet index
/// @param dstString pointer to destination: limited to 60 chars.
void inletAssistant(MaxExternalObject* maxObjectPtr,
                    void *box,
                    long message,
                    long arg,
                    char *dstString)
{
    typedef enum _InletAssistantType {
        INLET = 1,
        OUTLET = 2,
    } InletAssistantType;
    
    switch ((InletAssistantType)message)
    {
        case INLET: // inletMessage
            switch (arg)
            {
                case 0:
                    sprintf(dstString, "inlet 1");
                    break;
                case 1:
                    sprintf(dstString, "inlet 2");
                    break;
                default:
                    sprintf(dstString, "some other inlet");
            }
            break;
        case OUTLET:  // outletMessage
            switch (arg)
            {
                case 0:
                    sprintf(dstString, "outlet 1");
                    break;
                case 1:
                    sprintf(dstString, "outlet 2");
                    break;
                default:
                    sprintf(dstString, "some other outlet");
            }
            break;
    }
}

//------------------------------------------------------------------------------

/// This gets called when we receive a bang
/// @param maxObjectPtr object pointer
void onBang(MaxExternalObject* maxObjectPtr)
{
    
}

/// This gets called when we receive a float
/// @param maxObjectPtr object pointer
/// @param floatIn 
void onFloat(MaxExternalObject* maxObjectPtr, double floatIn)
{
    maxObjectPtr->gain = floatIn;
}




//------------------------------------------------------------------------------

/// This gets called when a list is sent to the object
/// @param maxObjectPtr object pointer
/// @param s message selector contains the text of a message and a pointer to the message object
/// @param argc number of atoms in the argv array
/// @param argv array of atoms holding the arguments.
void onList(MaxExternalObject* maxObjectPtr,
            t_symbol *s,
            short argc,
            t_atom *argv)
{
    int numAtoms = 0;
    size_t dataSize = 4;
    
    switch (maxObjectPtr->outletType)
    {
        case INT16_OUT:
            dataSize = 2;
        case INT32_OUT:
            for (int i = 0, j = 0; i < argc; i+=dataSize, ++j)
            {
                if((argc - i) < dataSize)
                    break;
                atom_setlong(maxObjectPtr->atomList + j,
                             bytesToInt(dataSize, argv + i, maxObjectPtr->isLittleEndian));
                numAtoms++;
            }
            break;
            
        case UINT16_OUT:
            dataSize = 2;
        case UINT32_OUT:
            for (int i = 0, j = 0; i < argc; i += dataSize, ++j)
            {
                if((argc - i) < dataSize)
                    break;
                atom_setlong(maxObjectPtr->atomList + j,
                             bytesToUInt(dataSize, argv + i, maxObjectPtr->isLittleEndian));
                numAtoms++;
            }
            break;
            
        case FLOAT_OUT:
            for (int i = 0, j = 0; i < argc; i+=4, ++j)
            {
                if((argc - i) < 4)
                    break;
                atom_setfloat(maxObjectPtr->atomList + j,
                              bytesToFloat(4, argv + i, maxObjectPtr->isLittleEndian));
                numAtoms++;
            }
            break;
            
        case STRING_OUT:
            atom_setsym(maxObjectPtr->atomList, bytesToSymbol(argc, argv));
            numAtoms++;
            break;
            
        case BYTE_OUT:
            for (int i = 0; i < argc; ++i)
            {
                atom_setlong(maxObjectPtr->atomList + i, atom_getlong(argv + i));
                numAtoms++;
            }
            break;
        default:
            break;
    }
    
    
    
    outlet_list(maxObjectPtr->mainOutlet, NULL, numAtoms, maxObjectPtr->atomList);
}

void onInt (MaxExternalObject* maxObjectPtr, t_atom_long intIn)
{
    uint8_t* bytes = (uint8_t*)&intIn;
    t_atom byteAtoms[8];
    
    for (int i = 0; i < 8; ++i)
    {
        t_atom_long byteAsLong = (t_atom_long)bytes[i];
        atom_setlong(byteAtoms + i, byteAsLong);
    }
    onList(maxObjectPtr, NULL, 8, byteAtoms);
}

//------------------------------------------------------------------------------

void onPrintMessage(MaxExternalObject* x)
{    
    post("Print some info about the object\n");
}

//------------------------------------------------------------------------------

/// If any message is recieved an we have not already dealt with it
/// @param maxObjectPtr object pointer
/// @param s message selector contains the text of a message and a pointer to the message object
/// @param argc number of atoms in the argv array
/// @param argv array of atoms holding the arguments.
void onAnyMessage(MaxExternalObject* maxObjectPtr, t_symbol *s, long argc, t_atom *argv)
{
    object_post( (t_object*)maxObjectPtr,
                "This method was invoked by sending the ’%s’ message to this object.",
                s->s_name);
}
void toggleEndianness(MaxExternalObject *maxObjectPtr, long n)
{
    maxObjectPtr->isLittleEndian = (bool)n;
}

//------------------------------------------------------------------------------
/// Bundle all class_addmethod calls into one function.
/// @param c max external class pointer
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
    class_addmethod(c, (method)onList, "list", A_GIMME, 0);
    class_addmethod(c, (method)onFloat, "float", A_FLOAT, 0);
    class_addmethod(c, (method)onInt, "int", A_LONG, 0);
    class_addmethod(c, (method)inletAssistant,"assist", A_CANT,0);
    class_addmethod(c, (method)onPrintMessage, "print", 0);
    class_addmethod(c, (method)onAnyMessage, "anything", A_GIMME, 0);
    class_addmethod(c, (method)toggleEndianness, "in1", A_LONG, 0);
}
//------------------------------------------------------------------------------
int C74_EXPORT main(void)
{
    t_class* c = class_new("byte-cast",
                           (method)myExternalConstructor,
                           (method)myExternDestructor,
                           (short)sizeof(MaxExternalObject),
                           0L,
                           A_GIMME,
                           0);
    
    coupleMethodsToExternal(c);
    
    class_register(CLASS_BOX, c);
    
    myExternClass = c;
    
    return 0;
}
//------------------------------------------------------------------------------


void setMaxObjectOutletType(MaxExternalObject* maxObjectPtr, long argc, t_atom *argv)
{
    if (argc)
    {
        t_atom* argument = &argv[0];
        t_symbol* outletType = atom_getsym(argument);
        maxObjectPtr->outletType = UNKNOWN_OUT;
        
        long argumentStringLength = strlen(outletType->s_name);
        const char* argumentString = outletType->s_name;
        short currIndex = 0;
        bool isUnsigned = false;
        
        if (argumentString[currIndex] == 'u')
        {
            isUnsigned = true;
            currIndex++;
        }
        
        if(argumentString[currIndex] == 'i')
            maxObjectPtr->outletType = (isUnsigned)?UINT32_OUT:INT32_OUT;
        else if (argumentString[currIndex] == 'h')
            maxObjectPtr->outletType = (isUnsigned)?UINT16_OUT:INT16_OUT;
        else if(argumentString[currIndex] == 'f')
            maxObjectPtr->outletType = FLOAT_OUT;
        else if(argumentString[currIndex] == 's')
            maxObjectPtr->outletType = STRING_OUT;
        else if(argumentString[currIndex] == 'b')
            maxObjectPtr->outletType = BYTE_OUT;
        
        currIndex++;
        
        if(currIndex < argumentStringLength)
        {
            if (argumentString[currIndex] == '<')
                maxObjectPtr->isLittleEndian = true;
            else if (argumentString[currIndex] == '>')
                maxObjectPtr->isLittleEndian = false;
            else
                maxObjectPtr->outletType = UNKNOWN_OUT;
            
            currIndex++;
        }
        
        if(currIndex < argumentStringLength)
            maxObjectPtr->outletType = UNKNOWN_OUT;
        
        if (maxObjectPtr->outletType == UNKNOWN_OUT)
        {
            object_post((t_object*)maxObjectPtr,
                        "%s is not a valid format",
                        outletType->s_name);
            maxObjectPtr->outletType = INVALID_OUT;
        }
        
        maxObjectPtr->mainOutlet = listout(maxObjectPtr);
        
    }
}
