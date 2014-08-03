//
//  SpriteBase.h
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#ifndef __MXGame2__SpriteBase__
#define __MXGame2__SpriteBase__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;
// 扩展精灵类，增加生命值、血条等属性
class SpriteBase : public cocos2d::Sprite
{
public:
    // 宏
    CC_SYNTHESIZE(int, _lifeValue, LifeValue); // 精灵的生命值
    
    CC_SYNTHESIZE(ProgressTimer*, _HP, HP); // 精灵的血条
    
    CC_SYNTHESIZE(float, _HPInterval, HPInterval); // 血条的更新量
    
    CC_SYNTHESIZE(bool, _move, Move); // 是否移动
    
    CC_SYNTHESIZE(int, _moveCount, MoveCount); // 移动的次数
    
    // 静态的create函数
    static SpriteBase* create(const std::string& filename)
    {
        SpriteBase *sprite = new SpriteBase();
        if (sprite && sprite->initWithFile(filename))
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
};

#endif /* defined(__MXGame2__SpriteBase__) */
