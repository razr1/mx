//
//  FightScene.h
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#ifndef __MXGame2__FightScene__
#define __MXGame2__FightScene__

#include <iostream>
#include "cocos2d.h"
#include "SpriteBase.h"
#include "GameScene.h"

// 定义2种粒子效果
enum class ParticleType
{
    bloodSpurts, // 血花四溅的粒子效果
    magic // 魔法光波的粒子效果
};

class Fight : public cocos2d::Layer
{
private:
    SpriteBase* _player; // 玩家
    SpriteBase* _monster; // 怪物
    bool _isShoot;  // 是否可以射箭
    int _screenWidth , _screenHeight; // 屏幕宽度和高度
public:
    /**
     * 静态函数，创建Scene场景
     * 注意：在Cocos2d-x中没有Cocos2d-iPhone中的“id”,建议返回类实例的指针
     */
    static cocos2d::Scene* createScene();
    /**
     * init函数，完成初始化操作。
     * 注意：init函数在Cocos2d-x中返回bool值，而不是返回Cocos2d-iPhone中的“id”
     */
    virtual bool init();
    
    // 回调更新函数，该函数每一帧都会调用
    void update(float delta);
    // 玩家攻击
    void playerAction();
    // 射箭动画完成后效果
    void shootFinish();
    // 怪物攻击
    void monsterAction();
    // 根据粒子文件名称获取粒子效果
    ParticleSystem* getParticleWithName(ParticleType type);
    // 获得动画函数
    Animate* getAnimateByName(std::string animName,float delay,int animNum);
    
    /**
     * 使用CREATE_FUNC宏创建当前类的对象，返回的对象将会由自动释放池管理内存的释放
     */
    CREATE_FUNC(Fight);
};


#endif /* defined(__MXGame2__FightScene__) */
