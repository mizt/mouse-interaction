#import <Cocoa/Cocoa.h>
#import "MouseInteraction.h"

@interface AppDelegate:NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate
-(void)applicationDidFinishLaunching:(NSNotification*)aNotification {
   
    MouseInteraction::$()->add({.x=988,.y=570},1000,true,Ease::OutQuad);
    MouseInteraction::$()->add({.x=885,.y=570},1000,true,Ease::OutQuad);
    MouseInteraction::$()->add({.x=988,.y=570},1000,true,Ease::OutQuad);
    MouseInteraction::$()->add({.x=885,.y=570},1000,true,Ease::OutQuad);
   
}
-(void)applicationWillTerminate:(NSNotification *)aNotification {
}

@end

int main (int argc, const char * argv[]) {
    id app = [NSApplication sharedApplication];
    id delegat = [AppDelegate alloc];
    [app setDelegate:delegat];
    [app run];
    return 0;
}