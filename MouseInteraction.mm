#import <Cocoa/Cocoa.h>
#import "MouseInteraction.h"

@interface AppDelegate:NSObject <NSApplicationDelegate>
@end

@implementation AppDelegate
-(void)applicationDidFinishLaunching:(NSNotification*)aNotification {

    MouseInteraction::$()
        ->drag()
        ->add({.x=50,.y=50},{.x=250,.y=250},1000,true,Ease::OutQuad)
        ->undrag()
        ->add({.x=450,.y=450},1000,true,Ease::OutQuad);
 
/*
    Bezier b = {
        .p1={100,100},  // the first anchor point
        .p2={100,500},  // the first control point
        .p3={1000,500}, // the second control point
        .p4={1000,100}  // the second anchor point
    };
        
    MouseInteraction::$()->drag()->add(b,1000);

 */
    
}

@end

int main (int argc, const char * argv[]) {
    id app = [NSApplication sharedApplication];
    id delegat = [AppDelegate alloc];
    [app setDelegate:delegat];
    [app run];
    return 0;
}
