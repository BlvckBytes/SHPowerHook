#include <IOKit/IOLib.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/IOService.h>
#include <IOKit/IONotifier.h>
#include <sys/vnode.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include "SHPowerHook.hpp"

#define FILE_PATH "/Users/blvckbytes/Desktop/shpowerhook.log"

// This required macro defines the class's constructors, destructors,
// and several other methods I/O Kit requires.
OSDefineMetaClassAndStructors(me_blvckbytes_SHPowerHook, IOService)

// Define the driver's superclass.
#define super IOService

IONotifier *notifier;
vfs_context_t mCtx;
long f_off = 0L;

int append_logfile(const char* text)
{
    vnode *vp;
    int error = 0, len = (int) strlen(text);
    
    if ((error = vnode_open(FILE_PATH, (O_CREAT | O_APPEND | FWRITE | O_NOFOLLOW), S_IRUSR | S_IWUSR, VNODE_LOOKUP_NOFOLLOW, &vp, mCtx)))
    {
        IOLog("Could not open file!\n");
    }
    else
    {
        if ((error = vnode_isreg(vp)) == VREG)
        {
            if ((error = vn_rdwr(UIO_WRITE, vp, (char *) text, len, f_off, UIO_SYSSPACE, IO_APPEND | IO_NOCACHE | IO_NODELOCKED | IO_UNIT, vfs_context_ucred(mCtx), (int *) 0, vfs_context_proc(mCtx))))
            {
                IOLog("Error, vn_rdwr(%s) failed with error %d!\n", FILE_PATH, error);
            }
            else
            {
                f_off += len;
            }
        }
        else
        {
            IOLog("Error, vnode_isreg(%s) failed with error %d!\n", FILE_PATH, error);
        }
    
        if ((error = vnode_close(vp, FWASWRITTEN, mCtx)))
        {
            IOLog("vnode_close() failed with error %d!\n", error);
        }
    }
    
    return error;
}

static IOReturn power_event_handler(
        void *target, void *refCon,
        UInt32 messageType, IOService *provider,
        void* messageArgument, vm_size_t argSize)
{
    
    const char *msg = NULL;
    
    switch (messageType)
    {
        // Uninterruptable sleep initialized
        case kIOMessageSystemWillSleep:
            msg = "kIOMessageSystemWillSleep";
            break;
    
        // Wakeup is complete, all hardware is loaded
        case kIOMessageSystemHasPoweredOn:
            msg = "kIOMessageSystemHasPoweredOn";
            break;
            
        // System is about to power off
        case kIOMessageSystemWillPowerOff:
            msg = "kIOMessageSystemWillPowerOff";
            break;
            
        // System is about to restart
        case kIOMessageSystemWillRestart:
            msg = "kIOMessageSystemWillRestart";
            break;
            
        case kIOMessageDeviceWillPowerOff:
            msg = "kIOMessageDeviceWillPowerOff";
            break;
            
        case kIOMessageDeviceHasPoweredOn:
            msg = "kIOMessageDeviceHasPoweredOn";
            break;
    }
    
    // Central point for logging, for future added timestamp prefix
    if (msg != NULL)
    {
        append_logfile(msg);
    }
    
    acknowledgeSleepWakeNotification(refCon);
    return kIOReturnSuccess;
}

bool me_blvckbytes_SHPowerHook::init(OSDictionary *dict)
{
    bool result = super::init(dict);
    
    // Create fs context
    mCtx = vfs_context_create(NULL);
    
    // Register interest for sys power events
    notifier = registerPrioritySleepWakeInterest(&power_event_handler, this);
    
    return result;
}

void me_blvckbytes_SHPowerHook::free(void)
{
    IOLog("Freeing...\n");
    
    // Relase power notifier if existing
    if (notifier)
        notifier->remove();
    
    // Release fs context if existing
    if (mCtx)
        vfs_context_rele(mCtx);
    
    IOLog("Freed!\n");
    super::free();
}

bool me_blvckbytes_SHPowerHook::start(IOService *provider)
{
    bool result = super::start(provider);
    IOLog("Started!\n");
    return result;
}

void me_blvckbytes_SHPowerHook::stop(IOService *provider)
{
    IOLog("Stopped!\n");
    super::stop(provider);
}
