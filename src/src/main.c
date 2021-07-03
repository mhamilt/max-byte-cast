/*
 A template for a basic max external object with more explicit documentation
 
 See Readme of repository for build instructions.
 Create an Issue on the repository if anything is amiss or you have any suggestion
 - mhamilt Mar 2020
 */
#include "ext.h"
#include "ext_obex.h"
#include "buffer.h"
//------------------------------------------------------------------------------

/// void* to the complete new Max External class so that it can be used in the class methods
/// This will be set to t_class* in the main function
/// @code t_class* c = class_new(...);
/// myExternClass = c;
void* myExternClass;

//------------------------------------------------------------------------------
/** @struct
 The MaxMSP object
 */
typedef struct _MaxExternalObject
{
    t_object x_obj;
    t_symbol* x_arrayname;
    short inletConnection;
    double gain;
} MaxExternalObject;
//------------------------------------------------------------------------------
/// External Object Constructor: use this to setup any variables / properties of your DSP Struct or MaxExternalObject
/// Arguement list should be as long as the list of type arguments passed in the class_new call below.
/// @param arg1 first argument to object: should match type given in class_new(...)
/// @returns a void* to an instance of the MaxExternalObject
void* myExternalConstructor(t_symbol *s, long argc, t_atom *argv)
{
    // Define Functionality
    //
    // If argc == 1:
    //      if argv[0] == 's' then greedily spit out a symbol
    // outside of that, use standard printf fmt for data
    // If argc > 1:
    // i     32-bit int
    // u     32-bit uint
    // h     16-bit int
    // uh    16-bit uint
    // f     32-bit float
    // d     64-bit float (double)
    // c     8-bit Character
    // s     string until \0
    // xxxs  string where xxx is the number of characters
    //--------------------------------------------------------------------------
    post("%d Arguments", argc);

    if (argc)
    {
        t_atom* ap = &argv[argc-1];
        switch (atom_gettype(ap))
        {
            case A_LONG:
                post("%ld", atom_getlong (ap));
                break;
            case A_FLOAT:
                post("%.2f", atom_getfloat(ap));
                break;
            case A_SYM:
                post("%s", atom_getsym(ap)->s_name);
                break;
            default:
                post("unknown atom type (%ld)", atom_gettype(ap));
                break;
        }
    }
    //--------------------------------------------------------------------------
    MaxExternalObject* maxObjectPtr = (MaxExternalObject*)object_alloc(myExternClass);
    //--------------------------------------------------------------------------
    // inlet_new((t_object*)maxObjectPtr, "signal");
    outlet_new((t_object*)maxObjectPtr, "signal");
    //--------------------------------------------------------------------------
    maxObjectPtr->gain = 1.0;
    return maxObjectPtr;
}

//------------------------------------------------------------------------------
/// @brief what happens when the object is deleted
void myExternDestructor(MaxExternalObject* maxObjectPtr)
{
    post("END");
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
    post("I got a bang!\n");
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
    for (int i = 0; i < argc; ++i)
    {
        atom_getfloatarg(i, argc, argv);
    }
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

//------------------------------------------------------------------------------
/// Bundle all class_addmethod calls into one function.
/// @param c max external class pointer
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
    class_addmethod(c, (method)onList, "list", A_GIMME, 0);
    class_addmethod(c, (method)onFloat, "float", A_FLOAT, 0);
    class_addmethod(c, (method)inletAssistant,"assist", A_CANT,0);
    class_addmethod(c, (method)onPrintMessage, "print", 0);
    class_addmethod(c, (method)onAnyMessage, "anything", A_GIMME, 0);
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

