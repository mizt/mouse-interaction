#import <Cocoa/Cocoa.h>
#import "MouseInteraction.h"



@interface AppDelegate:NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate
-(void)applicationDidFinishLaunching:(NSNotification*)aNotification {
   
    //MouseInteraction::$()->dragged()->add({.x=50,.y=50},{.x=250,.y=250},1000,true,Ease::OutQuad);
    
/*
    MouseInteraction::$()->add({.x=988,.y=570},1000,true,Ease::OutQuad);
    MouseInteraction::$()->add({.x=885,.y=570},1000,true,Ease::OutQuad);
    MouseInteraction::$()->add({.x=988,.y=570},1000,true,Ease::OutQuad);
    MouseInteraction::$()->add({.x=885,.y=570},1000,true,Ease::OutQuad);
*/

    Bezier b = {
        .p1={100,100},  // 始点
        .p2={100,500},  // 制御点
        .p3={1000,500}, // 制御点
        .p4={1000,100}  // 終点
    };
        
    MouseInteraction::$()->add(b,1000);
    
}

@end

int main (int argc, const char * argv[]) {
    id app = [NSApplication sharedApplication];
    id delegat = [AppDelegate alloc];
    [app setDelegate:delegat];
    [app run];
    return 0;
}