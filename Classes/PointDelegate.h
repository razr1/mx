//
//  PointDelegate.h
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#ifndef __MXGame2__PointDelegate__
#define __MXGame2__PointDelegate__

#include <iostream>
#include "cocos2d.h"

// Point的代理类，继承cocos2d::Ref类
class PointDelegate : public cocos2d::Ref
{
public:
    
    CC_SYNTHESIZE(float, _x, X);
    
    CC_SYNTHESIZE(float, _y, Y);
    
    static PointDelegate* create(float x, float y){
        PointDelegate* p = new PointDelegate();
        if(p && p->initPoint(x, y)){
            p->autorelease();
            return p;
        }
        CC_SAFE_DELETE(p);
        return nullptr;
    }
    bool initPoint(float x, float y){
        this->setX(x);
        this->setY(y);
        return true;
    }

};

#endif /* defined(__MXGame2__PointDelegate__) */
