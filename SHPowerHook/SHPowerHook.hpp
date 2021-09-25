#include <IOKit/IOService.h>

class me_blvckbytes_SHPowerHook : public IOService
{

OSDeclareDefaultStructors(me_blvckbytes_SHPowerHook)

public:
    virtual bool init(OSDictionary *dictionary = 0);
    virtual void free(void);
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
};
