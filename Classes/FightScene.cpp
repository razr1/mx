//
//  FightScene.cpp
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#include "FightScene.h"

Scene* Fight::createScene()
{
    // 创建一个场景对象，该对象将会由自动释放池管理内存的释放
	auto scene = Scene::create();
	// 创建层对象，该对象将会由自动释放池管理内存的释放
	auto layer = Fight::create();
	// 将Fight层作为子节点添加到场景
	scene->addChild(layer);
	// 返回场景对象
	return scene;
}

bool Fight::init()
{
    // 调用父类的init方法
    if ( !Layer::init() )
    {
        return false;
    }
    // 获得设备可见视图大小
    Size visibleSize = Director::getInstance()->getVisibleSize();
    // 获取屏幕宽度和高度
    _screenWidth = visibleSize.width;
    _screenHeight = visibleSize.height;

    // 加载战斗背景
    Sprite* bg = Sprite::create("fight.png");
    bg->setPosition(_screenWidth/2, _screenHeight/2);
    this->addChild(bg);
    
    // 创建玩家并设置位置
    _player = SpriteBase::create("player_stand_2.png");
    _player->setPosition(250, _screenHeight/2);
    _player->setFlippedX(true);
    // 是否可以射箭标志为true
    _isShoot = true;
    
    // 创建怪物并设置位置
    _monster = SpriteBase::create("eagle_stand_2.png");
    _monster->setPosition(_screenWidth-200, _screenHeight/2+30);
    _monster->setLifeValue(20);
    
    // 创建怪物血条
    auto bar = Sprite::create("planeHP.png");
    _monster->setHP(ProgressTimer::create(bar)) ;
    _monster->getHP()->setPercentage(0.0f);
    _monster->getHP()->setScale(0.25f);
    _monster->getHP()->setMidpoint(Point(0, 0.5f));
    _monster->getHP()->setBarChangeRate(Point(1.0f, 0));
    _monster->getHP()->setType(ProgressTimer::Type::BAR);
    _monster->getHP()->setPercentage(60.0f);
    Point pos = Point(_monster->getPosition().x, _monster->getPosition().y);
    _monster->getHP()->setPosition(pos.x+15, pos.y+80);
    this->addChild(_monster->getHP());
    // 设置血条更新量
    _monster->setHPInterval(_monster->getHP()->getPercentage() / (float)_monster->getLifeValue());
    // 将玩家和怪物添加为当前层的子节点
    this->addChild(_player);
    this->addChild(_monster);
    
    // 创建事件监听器
    auto fightListener = EventListenerTouchOneByOne::create();
    // 响应触摸事件函数
    fightListener->onTouchBegan=[](Touch* touch, Event* event){
        return true;
    };
    fightListener->onTouchEnded=[=](Touch* touch, Event* event){
        // 将触摸点坐标转换成Node坐标
        Point touchLocation = touch->getLocation();
        touchLocation = this->convertToNodeSpace(touchLocation);
        // 判断触摸点是否在怪物精灵的boundingBox里
        if (_monster->getBoundingBox().containsPoint(touchLocation)) {
            // 回合制战斗模式，_shoot判断玩家是否可以射箭
            if (_isShoot) {
                // 玩家攻击
                this->playerAction();
            }
        }
    };
    // 添加场景优先事件监听器
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(fightListener, this);
    
    // 游戏主循环,每帧都调用的函数
    this->scheduleUpdate();
    
    return true;
}

void Fight::update(float delta){
   // 如果怪物生命值为0，结束战斗，切换回游戏主画面
    if (_monster->getLifeValue() <= 0) {
        // 当前层删除玩家和怪物
        this->removeChild(_player);
        this->removeChild(_monster);
        // pop之前的游戏场景
        Director::getInstance()->popScene();
    }
}

/**
 * 获得动画动作函数
 * animName：动画帧的名字
 * delay：动画帧与帧之间的间隔时间
 * animNum：动画帧的数量
 */
Animate* Fight::getAnimateByName(std::string animName,float delay,int animNum){
    Animation* animation = Animation::create();
    // 循环从精灵帧缓存中获取与图片名称相对应的精灵帧组成动画
	for(unsigned int i = 1;i<=animNum;i++){
        // 获取动画图片名称，例如plane0.png
        std::string frameName = animName;
        frameName.append(StringUtils::format("%d.png",i));
		// 将单张图片添加为精灵帧（即动画帧）
		animation->addSpriteFrameWithFile(frameName.c_str());
    }
    // 设置动画播放的属性
	animation->setDelayPerUnit(delay);
	// 让精灵对象在动画执行完后恢复到最初状态
	animation->setRestoreOriginalFrame(true);
    // 返回动画动作对象
    Animate* animate = Animate::create(animation);
    return animate;
}

// 玩家攻击函数
void Fight::playerAction(){
    // 是否可以射箭标志为false
    _isShoot = false;
    // 射箭动画
    auto animAction = getAnimateByName("player_battle_", 0.2f, 5);
    // 创建回调动作
    auto callfunc = CallFunc::create([=]{
        // 射箭动画完成后，在怪物身前播放血花四溅的粒子效果
        this->shootFinish();
		// 玩家攻击完成后，怪物开始攻击
        this->monsterAction();
    });
    // 组合动作
    auto sequence = Sequence::create(animAction,callfunc, NULL);
    // 玩家执行组合动作
    _player->runAction(sequence);
}

// 玩家攻击怪物完成，播放粒子效果
void Fight::shootFinish(){
    // 获得血花四溅的粒子对象
    ParticleSystem *bloodSpurts = getParticleWithName(ParticleType::bloodSpurts);
    // 设置粒子效果播放位置，即怪物面前
    bloodSpurts->setPosition(_monster->getPosition().x-15, _monster->getPosition().y);
    // 添加粒子到当前层
    this->addChild(bloodSpurts);
    // 怪物减少生命值
    _monster->setLifeValue(_monster->getLifeValue() - 7 );
    // 血条减少
    if (_monster->getHP() != nullptr) {
        _monster->getHP()->setPercentage(_monster->getHPInterval() * _monster->getLifeValue());
    }
    // 延迟2秒后删除粒子效果
    auto remove = Sequence::create(
        DelayTime::create(2.0f),
        CallFunc::create([=]{
        // 删除粒子效果
        this->removeChild(bloodSpurts);
    }),NULL);
    // 当前层执行动作
    this->runAction(remove);
}

// 怪物攻击函数
void Fight::monsterAction(){
    // 延迟时间
    float delta = 0.4f;
    // 获取怪物战斗动画
    auto animAction = getAnimateByName("eagle_battle_", delta, 9);
    // 播放怪物战斗动画
    _monster->runAction(animAction);
    // 创建怪物魔法光波
    Sprite* bullet = Sprite::create("bullet.png");
    bullet->setRotation(270);
    bullet->setScale(2);
    bullet->setPosition(Point(_monster->getPosition().x-50, _monster->getPosition().y-40));
    // 魔法光波最初不显示
    bullet->setVisible(false);
    this->addChild(bullet);
    // 创建魔法光波移动动作，移动位置（射向玩家）
    auto moveTo = MoveTo::create(0.5, Point(_player->getPosition().x, _player->getPosition().y+10));
    auto callfunc = CallFunc::create([=]{
        // 设置魔法光波可见
        bullet->setVisible(true);
    });
    // 组合动作，延迟至怪物战斗动画播放完成之后，发射魔法光波
    auto sequence = Sequence::create(DelayTime::create(delta*10),callfunc,moveTo,NULL);
    // 魔法光波执行组合动作
    bullet->runAction(sequence);
    // 获得魔法光波粒子对象
    ParticleSystem *magic = getParticleWithName(ParticleType::magic);
    // 设置魔法粒子效果位置
    magic->setPosition(Point(_player->getPosition().x-50, _player->getPosition().y+28));
    // 设置默认粒子效果不显示
    magic->setVisible(false);
    // 添加魔法光波粒子效果
    this->addChild(magic, 1, 9);
    // 组合动作，停止魔法光波，播放粒子效果，删除粒子效果，修改是否可以射箭标志
    auto remove = Sequence::create(DelayTime::create(delta*11),
                                   CallFunc::create([=]{
        // 停止魔法光波精灵所有动作并删除魔法光波精灵
       bullet->stopAllActions();
       this->removeChild(bullet);
        // 显示魔法光波粒子效果
        this->getChildByTag(9)->setVisible(true);
    }),
                                   DelayTime::create(1.0),
                                   CallFunc::create([=]{
       // 删除魔法光波粒子效果
        this->removeChild(this->getChildByTag(9));
        // 回合结束，玩家可以继续射箭了
        _isShoot = true;
    }),NULL);
    // 怪物执行组合动作
    _monster->runAction(remove);
}

// 根据粒子文件名称获取粒子效果
ParticleSystem* Fight::getParticleWithName(ParticleType type){
    ParticleSystem* particle;
    switch (type) {
        case ParticleType::bloodSpurts:
            // 使用bloodSpurts.plist作为粒子效果的文件
            particle = ParticleSystemQuad::create("bloodSpurts.plist");
            // 设置粒子效果位置独立
            particle->setPositionType(ParticleSystem::PositionType::FREE);
            // 粒子效果完成后自动删除
            particle->setAutoRemoveOnFinish(true);
            break;
        case ParticleType::magic:
            // 使用magic.plist作为粒子效果的文件
            particle = ParticleSystemQuad::create("magic.plist");
//            particle->setBlendAdditive(false);
            // 设置粒子效果位置独立
            particle->setPositionType(ParticleSystem::PositionType::FREE);
            // 粒子效果完成后自动删除
            particle->setAutoRemoveOnFinish(true);
            break;
        default:
            break;
    }
    return particle;
}




