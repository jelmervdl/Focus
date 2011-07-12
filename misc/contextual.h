#ifndef __CONTEXTUAL__
#define __CONTEXTUAL__

#include "context.h"

class Contextual
{

    virtual Context	get_context(){return Context();};
};

#endif
