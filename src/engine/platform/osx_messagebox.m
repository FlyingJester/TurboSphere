#include "osx_messagebox.h"
#import <Cocoa/Cocoa.h>
#import <CoreFoundation/CoreFoundation.h>

void TS_OSX_MessageBox(const char * title, const char *content){

    CFStringRef lTitle   = CFStringCreateWithCString(NULL, title,   kCFStringEncodingUTF8);
    CFStringRef lContent = CFStringCreateWithCString(NULL, content, kCFStringEncodingUTF8);

    CFOptionFlags lFlags;

    CFUserNotificationDisplayAlert(0, kCFUserNotificationCautionAlertLevel, NULL, NULL, NULL,
                                   lTitle, lContent, NULL, NULL, NULL, &lFlags);

}
