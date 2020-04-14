#import <vector>
#import <string>

typedef struct _Point2D {
    int x = 0;
    int y = 0;
} Point2D;


typedef struct _Bezier {
    Point2D p1 = {0,0};
    Point2D p2 = {0,0};
    Point2D p3 = {0,0};
    Point2D p4 = {0,0};
} Bezier;




typedef float (*EasingFunction)(const float);

typedef struct _MousePropety {
    Bezier *bezier = nullptr;
    Point2D *begin = nullptr;
    Point2D end{0,0};
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
        
        bool _init = false;
        bool _dragged = false;
        CGEventRef _mouseDragged = nullptr;

        
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
        
        void _mouseDown(int px,int py) {
            
            NSLog(@"mouseDown");
            
            dispatch_async(dispatch_get_main_queue(),^{
                CGEventRef mouseDown = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseDown,CGPointMake(px,py),kCGMouseButtonLeft);
                CGEventPost(kCGHIDEventTap,mouseDown);
                CFRelease(mouseDown);
            });
        }
        
        void _mouseUp(int px,int py) {
            
            NSLog(@"mouseUp");
            
            dispatch_async(dispatch_get_main_queue(),^{
                CGEventRef mouseUp = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseUp,CGPointMake(px,py),kCGMouseButtonLeft);
                CGEventPost(kCGHIDEventTap,mouseUp);
                CFRelease(mouseUp);
            });
        }
        
        void mouseDown(int px,int py) {
            _mouseDown(px,py);
            _mouseUp(px,py);
        }
                        
        void mouseMove(int px,int py) {
            dispatch_async(dispatch_get_main_queue(),^{
                CGEventRef mouseMoved = CGEventCreateMouseEvent(NULL,kCGEventMouseMoved,CGPointMake(px,py),kCGMouseButtonLeft);
                CGEventPost(kCGHIDEventTap,mouseMoved);
                CFRelease(mouseMoved);
             });
        }
        
        void mouseDragged(int px,int py) {
            dispatch_async(dispatch_get_main_queue(),^{
                
                NSLog(@"mouseDragged");
                
                if(this->_mouseDragged) {
                    CFRelease(this->_mouseDragged);
                    this->_mouseDragged = nullptr;
                }
                this->_mouseDragged = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseDragged,CGPointMake(px,py),kCGMouseButtonLeft);
                CGEventPost(kCGHIDEventTap,this->_mouseDragged);
             });
        }
        
        void updated(int diff) {
            
            if(_prop.size()>0) {
                
                if(_prop[0].begin==nullptr) {
                    
                    CGSize screenSize = [[NSScreen mainScreen] frame].size;
                    NSPoint mouseLocation = [NSEvent mouseLocation];
                    
                    Point2D *p = new Point2D();
                    p->x = (int)mouseLocation.x;
                    p->y = (int)(screenSize.height-mouseLocation.y);;
                    _prop[0].begin = p;    
                
                }
                
                if(this->_init==false) {
                    this->_init = true; 
                    if(this->_dragged) {
                        this->_mouseDown(_prop[0].begin->x,_prop[0].begin->y);
                    }
                }
                
                _prop[0].value += (diff);
                
                if(_prop[0].value>=_prop[0].duration) { // done;
                                            
                    int x = _prop[0].end.x;
                    int y = _prop[0].end.y;
                        
                    
                    if(!this->_dragged) {
                        this->mouseMove(x,y);
                        if(this->_prop[0].click) {
                            this->mouseDown(x,y);
                        }
                    }
                    else {
                        this->mouseDragged(x,y);
                    }
                    
                    if(this->_prop.size()>1) {
                        int t = this->_prop[0].value-this->_prop[0].duration;
                        if(this->_prop[0].bezier) delete this->_prop[0].bezier;
                        delete this->_prop[0].begin;
                        this->_prop[0].ease = nullptr; 
                        this->_prop.erase(this->_prop.begin());
                        if(t<0) {
                            this->updated(t);
                        }
                    }
                    else {
                        
                        if(this->_prop[0].bezier) delete this->_prop[0].bezier;
                        delete this->_prop[0].begin;
                        this->_prop[0].ease = nullptr;
                        
                        if(this->_dragged) {
                            if(this->_mouseDragged) {
                                CFRelease(this->_mouseDragged);
                                this->_mouseDragged = nullptr;
                            }
                            
                            this->_mouseUp(this->_prop[0].end.x,this->_prop[0].end.y);
                        }
                        
                        if(this->_mouseDragged) {
                            CFRelease(this->_mouseDragged);
                            this->_mouseDragged = nullptr;
                        }
                        
                        this->_prop.erase(_prop.begin());
                        dispatch_suspend(this->_timer);
                        
                        this->_suspend = true;
                        this->_init = false;
                        this->_dragged = false;
                        //NSLog(@"suspend");
                    }                    
                }
                else {
                    
                    
                    
                    double x = 0;
                    double y = 0;
                    
                    if(_prop[0].bezier) {
                        
                        Bezier *b = _prop[0].bezier;
                        
                        double target = MouseInteraction::getBezierLength(*b)*(_prop[0].value/(double)_prop[0].duration);//k*(len/(int)(num-1));
                            
                        double px = 0;
                        double py = 0;
                        
                        MouseInteraction::getBezierPoint(
                            0,
                            b->p1.x,b->p1.y,
                            b->p2.x,b->p2.y,
                            b->p3.x,b->p3.y,
                            b->p4.x,b->p4.y,
                            &px,&py
                        );
                        
                        int res = (_prop[0].duration/60.0)*2.0;
                        
                        double t = 0;
                        double sum = 0;

                        for(int n=1; n<res; n++) {
                            
                            double dx = 0;
                            double dy = 0;
                                
                            MouseInteraction::getBezierPoint(
                                n/(double)(res-1),
                                b->p1.x,b->p1.y,
                                b->p2.x,b->p2.y,
                                b->p3.x,b->p3.y,
                                b->p4.x,b->p4.y,
                                &dx,&dy
                            );
                            
                            sum += sqrt((px-dx)*(px-dx)+(py-dy)*(py-dy));
                            
                            if(target-sum<0) {
                                t = (n-1)/(double)(res-1);                                
                                break;
                            }
                        
                            px = dx;
                            py = dy;
                        }
                            
                        this->getBezierPoint(
                            t,
                            b->p1.x,b->p1.y,
                            b->p2.x,b->p2.y,
                            b->p3.x,b->p3.y,
                            b->p4.x,b->p4.y,
                            &x,&y
                        );
                        
                    }
                    
                    else {
                        
                        double t = _prop[0].value/(double)_prop[0].duration;
                        if(_prop[0].ease) t = _prop[0].ease(t);

                        x = this->_prop[0].begin->x + (this->_prop[0].end.x-this->_prop[0].begin->x)*t;
                        y = this->_prop[0].begin->y + (this->_prop[0].end.y-this->_prop[0].begin->y)*t;
                                               
                    }

                    if(!this->_dragged) {
                        this->mouseMove(x,y);
                    }
                    else {
                        this->mouseDragged(x,y);
                    }
                }
            }
        }


        void add(Point2D *begin, Point2D end, int duration=1000, bool click=false, EasingFunction ease=nullptr) {
            
             this->_prop.push_back({
                .bezier = nullptr,
                .begin = begin,
                .end = end,
                .value = 0,
                .duration = duration,
                .click = click,
                .ease = ease
            });
            
            if(this->_suspend) {
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
            
        static void getBezierPoint(double t,int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,double *dx,double *dy) {
            double tp = 1.0-t;
            *dx = t*t*t*x4 + 3*t*t*tp*x3 + 3*t*tp*tp*x2 + tp*tp*tp*x1;
            *dy = t*t*t*y4 + 3*t*t*tp*y3 + 3*t*tp*tp*y2 + tp*tp*tp*y1;
        }
            
        static double getBezierLength(Bezier b,int res=16) {
            
            double len = 0;
                 
            double px = 0;
            double py = 0;
            
            MouseInteraction::getBezierPoint(
                0,
                b.p1.x,b.p1.y,
                b.p2.x,b.p2.y,
                b.p3.x,b.p3.y,
                b.p4.x,b.p4.y,
                &px,&py
            );
            
            
            for(int k=0; k<res; k++) {
                
                double dx = 0;
                double dy = 0;
                    
                MouseInteraction::getBezierPoint(
                    k/(double)(res-1),
                    b.p1.x,b.p1.y,
                    b.p2.x,b.p2.y,
                    b.p3.x,b.p3.y,
                    b.p4.x,b.p4.y,
                    &dx,&dy
                );
                
                len += sqrt((px-dx)*(px-dx)+(py-dy)*(py-dy));
            
                px = dx;
                py = dy;
            }
            
            return len;
                        
        }
        
        void update() {
            double current = CFAbsoluteTimeGetCurrent();            
            this->updated((current-this->_then)*1000.0);
            this->_then = current;
        }
        
        MouseInteraction *dragged(bool b=true) {
            this->_dragged = b;
            if(!this->_dragged&&this->_mouseDragged) {
                CFRelease(this->_mouseDragged);
                this->_mouseDragged = nullptr;
            }
            return this;
        }
        
        
        void add(Bezier bezier,int duration, EasingFunction ease=nullptr) {
                
            Point2D *p = new Point2D();
            p->x = bezier.p1.x;
            p->y = bezier.p1.y;
            
            Bezier *b = new Bezier();
            b->p1.x = bezier.p1.x;
            b->p1.y = bezier.p1.y;
            b->p2.x = bezier.p2.x;
            b->p2.y = bezier.p2.y;
            b->p3.x = bezier.p3.x;
            b->p3.y = bezier.p3.y;
            b->p4.x = bezier.p4.x;
            b->p4.y = bezier.p4.y;
            
            this->_prop.push_back({
                .bezier = b,
                .begin = p,
                .end = {bezier.p4.x,bezier.p4.y},
                .value = 0,
                .duration = duration,
                .click = false,
                .ease = ease
            });
                
            if(this->_suspend) {
                this->_suspend = false;
                this->_then = CFAbsoluteTimeGetCurrent();
                dispatch_resume(this->_timer);
            }
                
        }
        
        
        void add(Point2D end, int duration, EasingFunction ease=nullptr) {
            this->add(nullptr,end,duration,false,ease);            
        }
        
        void add(Point2D end, int duration, bool click, EasingFunction ease=nullptr) {
            this->add(nullptr,end,duration,click,ease);            
        }
        
        void add(Point2D begin,Point2D end, int duration, EasingFunction ease=nullptr) {
                        
            Point2D *p = new Point2D();
            p->x = begin.x;
            p->y = begin.y;
            
            this->add(p,end,duration,false,ease);
        }
        
        void add(Point2D begin,Point2D end, int duration ,bool click=false, EasingFunction ease=nullptr) {
                        
            Point2D *p = new Point2D();
            p->x = begin.x;
            p->y = begin.y;
            
            this->add(p,end,duration,click,ease);
        }
        
};
