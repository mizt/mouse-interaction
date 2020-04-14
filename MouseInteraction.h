#import <vector>
#import <string>

typedef struct _MousePosition {
    int x = 0;
    int y = 0;
} MousePosition;

typedef float (*EasingFunction)(const float);

typedef struct _MousePropety {
    MousePosition *begin = nullptr;
    MousePosition end{0,0};
    int value = 0;
    int duration = 1000;
    bool click = false;
    EasingFunction ease = nullptr;
} MousePropety;


namespace Ease {
    inline static float Linear(const float t) { return t; }
    // Quad
    inline static float InQuad(const float t) { return t*t; }
    inline static float OutQuad(const float t) { return 1.-InQuad(1.-t); }
    inline static float InOutQuad(const float t) { return (t<0.5)?InQuad(t*2.0)*0.5:0.5+OutQuad((t-0.5)*2.0)*0.5; }
    //inline static float OutInQuad(const float t) { return (t<0.5)?OutQuad(t*2.0)*0.5:0.5+InQuad((t-0.5)*2.0)*0.5; }
}

class MouseInteraction {
    
    private:
        
        bool _suspend = true;
        dispatch_source_t _timer;
        std::vector<MousePropety> _prop;
        double _then = 0;
        
        MouseInteraction() {
            this->_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,0,dispatch_queue_create("MOUSE_INTERACTION",0));
            dispatch_source_set_timer(this->_timer,dispatch_time(0,0),(1.0/60)*1000000000,0);
            dispatch_source_set_event_handler(this->_timer,^{this->update();});
        }

        MouseInteraction(const MouseInteraction &o) {}
        virtual ~MouseInteraction() {
            if(this->_timer){
                dispatch_source_cancel(this->_timer);
                this->_timer = nullptr;
            }
        }
        
        void mouseDown(int px,int py) {
            dispatch_async(dispatch_get_main_queue(),^{
                CGEventRef mouseDown = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseDown,CGPointMake(px,py),kCGMouseButtonLeft);
                CGEventRef mouseUp = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseUp,CGPointMake(px,py),kCGMouseButtonLeft);
                CGEventPost(kCGHIDEventTap,mouseDown);
                CGEventPost(kCGHIDEventTap,mouseUp);
                CFRelease(mouseDown);
                CFRelease(mouseUp);
            });
        }
                        
        void mouseMove(int px,int py) {
            dispatch_async(dispatch_get_main_queue(),^{
                CGEventRef mouseMoved = CGEventCreateMouseEvent(NULL,kCGEventMouseMoved,CGPointMake(px,py),kCGMouseButtonLeft);
                CGEventPost(kCGHIDEventTap,mouseMoved);
                CFRelease(mouseMoved);
             });
        }
        
        void updated(int diff) {
            if(_prop.size()>0) {
                
                if(_prop[0].begin==nullptr) {
                    
                    CGSize screenSize = [[NSScreen mainScreen] frame].size;
                    NSPoint mouseLocation = [NSEvent mouseLocation];
                    
                    MousePosition *p = new MousePosition();
                    p->x = (int)mouseLocation.x;
                    p->y = (int)(screenSize.height-mouseLocation.y);;
                    _prop[0].begin = p;    
                
                }
                
                _prop[0].value += (diff);
                //
                if(_prop[0].value>=_prop[0].duration) { // done;
                                            
                    int x = _prop[0].begin->x + (_prop[0].end.x-_prop[0].begin->x)*1;
                    int y = _prop[0].begin->y + (_prop[0].end.y-_prop[0].begin->y)*1;
                        
                    //NSLog(@"updated %f,%d,%d",1.0,x,y);
                    
                    this->mouseMove(x,y);
                    if(_prop[0].click) {
                        this->mouseDown(x,y);
                    }
                    
                    if(_prop.size()>1) {
                        int t = _prop[0].value-_prop[0].duration;
                        
                        delete _prop[0].begin;
                        _prop[0].ease = nullptr; 
                        _prop.erase(_prop.begin());
                        if(t<0) {
                            updated(t);
                        }
                    }
                    else {
                        delete _prop[0].begin;
                        _prop[0].ease = nullptr;
                        _prop.erase(_prop.begin());
                        dispatch_suspend(this->_timer);
                        _suspend = true;
                        //NSLog(@"suspend");
                    }                    
                }
                else {
                    
                    double t = _prop[0].value/(double)_prop[0].duration;
                    
                    if(_prop[0].ease) t = _prop[0].ease(t);
                    
                    int x = _prop[0].begin->x + (_prop[0].end.x-_prop[0].begin->x)*t;
                    int y = _prop[0].begin->y + (_prop[0].end.y-_prop[0].begin->y)*t;

                    this->mouseMove(x,y);
                    NSLog(@"updated %f,%d,%d",t,x,y);
                }
            }
        }


        void add(MousePosition *begin, MousePosition end, int duration=1000, bool click=false, EasingFunction ease=nullptr) {
            
             _prop.push_back({
                .begin = begin,
                .end = end,
                .value = 0,
                .duration = duration,
                .click = click,
                .ease = ease
            });
            
            if(_suspend) {
                this->_suspend = false;
                this->_then = CFAbsoluteTimeGetCurrent();
                dispatch_resume(this->_timer);
            }
            
        }
    
    public:
        
        static MouseInteraction *$() {
            static MouseInteraction instance;
            return &instance;
        }
        
        void update() {
            double current = CFAbsoluteTimeGetCurrent();            
            this->updated((current-this->_then)*1000.0);
            this->_then = current;
        }
        
        void add(MousePosition end, int duration, EasingFunction ease=nullptr) {
            this->add(nullptr,end,duration,false,ease);            
        }
        
        void add(MousePosition end, int duration, bool click, EasingFunction ease=nullptr) {
            this->add(nullptr,end,duration,click,ease);            
        }
        
        void add(MousePosition begin,MousePosition end, int duration, EasingFunction ease=nullptr) {
                        
            MousePosition *p = new MousePosition();
            p->x = begin.x;
            p->y = begin.y;
            
            this->add(p,end,duration,false,ease);
        }
        
        void add(MousePosition begin,MousePosition end, int duration ,bool click=false, EasingFunction ease=nullptr) {
                        
            MousePosition *p = new MousePosition();
            p->x = begin.x;
            p->y = begin.y;
            
            this->add(p,end,duration,click,ease);
        }
        
};
