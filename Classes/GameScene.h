//
//  GameScene.h
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#ifndef __MXGame2__GameScene__
#define __MXGame2__GameScene__

#include <iostream>
#include "cocos2d.h"
#include "SpriteBase.h"
#include "Grid.h"
#include "PointDelegate.h"
#include "PathArithmetic.h"
#include "FightScene.h"

USING_NS_CC;

using namespace std;

// 游戏主场景
class Game : public cocos2d::Layer
{
private:
    TMXTiledMap* _tileMap;  // 地图
    Sprite* _player;  // 玩家精灵
    Vector<SpriteBase*> _monsterVector;  // 怪物数组
    TMXLayer* _collidable;  // 障碍层
    int _screenWidth , _screenHeight;  // 屏幕宽度和高度
    int _count;  // 游戏帧计数器
    vector<Vector<Grid*>> _gridVector; // 网格集合（网格二维数组）
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
    
    // 获得动画函数
    Animate* getAnimateByName(std::string animName,float delay,int animNum);
    // 把玩家作为视觉中心来显示，让地图随玩家一起移动
    void setViewpointCenter(Point position);
    // 玩家根据最佳路径点移动
    void playerMoveWithWayPoints(Vec2 position,Vector<PointDelegate*> path);
    // tile坐标转换为gl坐标
    Vec2 locationForTilePos(Vec2 pos);
    // 将屏幕坐标转换为TileMap坐标
    Vec2 tileCoordForPosition(Vec2 position);
    
    float getMoveTime(Vec2 startPos,Vec2 endPos);
    
    // 随机获得怪物“出现”的坐标
    Vec2 randomPosintion();
    // 随机获得怪物”移动“的坐标
    Vec2 randomMovePosintion(SpriteBase* monster);
    // 随机出现怪物精灵
    void updateMonsterSprite(float delta);
    // 怪物不规则移动
    void monsterRepeatedlyMove(float delta);
    // 怪物根据最佳路径点移动
    void monsterMoveWithWayPoints(Vector<PointDelegate*> pathVector, SpriteBase* monster);
    // 玩家和怪物的碰撞检测方法
    void collisionDetection(float delta);
    
    // 回调更新函数，该函数每一帧都会调用
    void update(float delta);
    /**
     * 使用CREATE_FUNC宏创建当前类的对象，返回的对象将会由自动释放池管理内存的释放
     */
    CREATE_FUNC(Game);
};

#endif /* defined(__MXGame2__GameScene__) */
