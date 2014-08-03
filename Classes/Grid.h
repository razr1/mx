//
//  Grid.h
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#ifndef __MXGame2__Grid__
#define __MXGame2__Grid__

#include <iostream>
#include "cocos2d.h"

// 网格类，继承cocos2d::Ref类
class Grid : public cocos2d::Ref
{
public:
    
    CC_SYNTHESIZE(int, _x, X); // 在地图二维数组中的x坐标
    CC_SYNTHESIZE(int, _y, Y); // 在地图二维数组中的y坐标
    CC_SYNTHESIZE(bool, _pass, Pass); // 是否可以通过变量（有障碍物则不能通过）
    
    // 静态的create函数
    static Grid* create(int x,int y){
        Grid* g = new Grid();
        if(g && g->initWithPoint(x,y))
        {
            g->autorelease();
            return g;
        }
        CC_SAFE_DELETE(g);
        return nullptr;
    }
    bool initWithPoint(int x,int y){
        this->setX(x);
        this->setY(y);
        this->setPass(true);
        return true;
    }

};

#endif /* defined(__MXGame2__Grid__) */
