//
//  GameScene.cpp
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#include "GameScene.h"

Scene* Game::createScene()
{
    // 创建一个场景对象，该对象将会由自动释放池管理内存的释放
	auto scene = Scene::create();
	// 创建层对象，该对象将会由自动释放池管理内存的释放
	auto layer = Game::create();
	// 将Game层作为子节点添加到场景
	scene->addChild(layer);
	// 返回场景对象
	return scene;
}

// on "init" you need to initialize your instance
bool Game::init()
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
    
    // ①调用TMXTiledMap的函数读取TileMap.tmx文件创建一个瓦片地图对象
    _tileMap = TMXTiledMap::create("map.tmx");
    // 把地图的锚点和位置都设置为原点，这样可以使地图的左下角与屏幕的左下角对齐
    _tileMap->setAnchorPoint(Vec2::ZERO);
    _tileMap->setPosition(Vec2::ZERO);
    // 将地图添加为当前层的子节点
    this->addChild(_tileMap);
    
     // ②获取障碍层，并设置障碍层为不可见
    _collidable = _tileMap->getLayer("collidable");
    _collidable->setVisible(false);
    
    /********③初始化读取地图所有网格，并确定网格对象是否是障碍物，将信息保存到网格二维数组**************/
    for (int i = 0; i < _tileMap->getMapSize().width; i++) {
        // 内部网格集合（[x,0]-[x-20]），存储网格
        Vector<Grid*> inner;
        for (int j = 0; j < _tileMap->getMapSize().height; j++) {
            // 设置网格对象的x轴和y轴以及是否可通过变量值
            Grid *o = Grid::create(i,j);
            // 将网格加入到集合
            inner.pushBack(o);
        }
        // 将内部集合加入到网格集合
        _gridVector.push_back(inner);
    }
    // 循环保存根据每个网格的x轴和y轴查找对应的地图的GID，判断是否可通过
    for (int i = 0; i < _gridVector.size(); i++) {
        Vector<Grid*> inner = _gridVector.at(i);
        // 循环内部网格集合
        for (int j = 0;  j < inner.size(); j++) {
            // 获取每一个网格对象
            Grid *grid = inner.at(j);
            // 获取每一个网格对象对应的的坐标
            Vec2 tileCoord = Vec2(grid->getX(), grid->getY());
            // 使用TMXLayer类的tileGIDAt函数获取TileMap坐标系里的“全局唯一标识”GID
            int tileGid = _collidable->getTileGIDAt(tileCoord);
            if (tileGid) {
                // 使用GID来查找指定tile的属性，返回一个Value
                Value properties = _tileMap->getPropertiesForGID(tileGid);
                // 返回的Value实际是一个ValueMap
                ValueMap map = properties.asValueMap();
                // 查找ValueMap，判断是否有”可碰撞的“物体，如果有，设置网格对象的isPass变量为false
                std::string value = map.at("collidable").asString();
                if (value.compare("true") == 0) {
                    grid->setPass(false);
                }
            }
        }
    }
    /****************二维数组设置结束**************************************************/
    // ④创建玩家精灵
    _player = Sprite::create("player_stand_1.png");
    // 玩家x轴镜像反转
    _player->setFlippedX(true);
    // 设置玩家精灵屏幕位置并添加为地图的子节点
    _player->setPosition(100, _screenHeight/2+240);
    // 将玩家添加为地图的子节点最上层，之后会调整节点的zOrder值实现遮挡效果
    _tileMap->addChild(_player, 2);
    
    // ⑤创建事件监听器
    auto gameListener = EventListenerTouchOneByOne::create();
    // 响应触摸事件函数
    gameListener->onTouchBegan=[](Touch* touch, Event* event){return true;};
    gameListener->onTouchEnded = [=](Touch *touch, Event *event){
        // 将触摸点坐标转换成Node坐标
        Vec2 touchLocation = touch->getLocation();
        touchLocation = this->convertToNodeSpace(touchLocation);
        // 玩家镜像反转
        if (_player->getPosition().x > touchLocation.x) {
            if(_player->isFlippedX() == true)
                _player->setFlippedX(false);
        }else{
            if(_player->isFlippedX() == false)
                _player->setFlippedX(true);
        }
        // 用玩家位置作为起点，触摸点作为终点，转换为网格坐标，在地图上查找最佳到达路径
        Vec2 from = tileCoordForPosition(_player->getPosition());
        Vec2 to = tileCoordForPosition(touchLocation);
        // 如果终点是不可通过（即有障碍物）的位置，则直接return
        int tileGid = _collidable->getTileGIDAt(to);
        if (tileGid) {
            // 使用GID来查找指定tile的属性，返回一个Value
            Value properties = _tileMap->getPropertiesForGID(tileGid);
            // 返回的Value实际是一个ValueMap
            ValueMap map = properties.asValueMap();
            // 查找ValueMap，判断是否有”可碰撞的“物体，如果有，直接返回
            std::string value = map.at("collidable").asString();
            if (value.compare("true") == 0) {
                return;
            }
        }
        // 调用PathArithmetic对象getPath函数获得起点到终点的最佳路径点的集合
        PathArithmetic* pa = PathArithmetic::create();
        Vector<PointDelegate*> pathVector = pa->getPath(from , to, _gridVector);
        
        if (_player->getNumberOfRunningActions() == 0
                && this->getNumberOfRunningActions() == 0) {
            // 玩家根据最佳路径点移动
            playerMoveWithWayPoints(touchLocation,pathVector);
            // 地图随玩家一起移动
            setViewpointCenter(touchLocation);
        }
        else{
            _player->stopAllActions();
            this->stopAllActions();
            playerMoveWithWayPoints(touchLocation,pathVector);
            setViewpointCenter(touchLocation);
        }
    };
    // 添加场景优先事件监听器
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(gameListener, this);
    
    // 游戏主循环,每帧都调用的函数
    this->scheduleUpdate();
    
    return true;
}

void Game::update(float delta){
    _count++;
    updateMonsterSprite(delta);
    monsterRepeatedlyMove(delta);
    collisionDetection(delta);
}

/***********玩家精灵移动相关函数*********************************/

// 玩家精灵根据最佳路径点移动函数
void Game::playerMoveWithWayPoints(Vec2 position,Vector<PointDelegate*> path){
    // 调用getAnimateByName函数获得玩家精灵移动动画
    auto animate = getAnimateByName("player_move_", 0.4f, 4);
    // 创建一个动作，重复执行Animate动画
    auto repeatanimate = RepeatForever::create(animate);
    // 玩家精灵重复执行动画动作
    _player->runAction(repeatanimate);
    
    float duration = 0.4f;
    // 定义动作集合，用来保存多个moveTo动作
    Vector<FiniteTimeAction*> actionVector;
    // 循环最佳路径点集合，创建多个MoveTo动作，玩家将执行多个MoveTo动作完成移动
    for (int i = 0; i < path.size(); i++) {
        // 获得要移动的每一个点
        PointDelegate* pd = path.at(i);
        Vec2 p = Vec2(pd->getX(), pd->getY());
        // 将tileMap的网格转成gl坐标，即玩家需要移动的位置
        Vec2 glPoint = locationForTilePos(p);
        // 创建MoveTo一个动作，让精灵对象移动到指定的位置
        MoveTo* moveTo = MoveTo::create(duration, glPoint);
        // 将该动作添加到临时数组
        actionVector.pushBack(moveTo);
    }
    // 创建回调动作，当MoveTo动作完成后精灵恢复最初站立状态
    auto callfunc = CallFunc::create([=]{
        // 停止动画
        _player->stopAction(repeatanimate);
        _player->setTexture("player_stand_1.png");
    });
    // 将回调动作加入到动作集合
    actionVector.pushBack(callfunc);
    // 按顺序执行动作集合中的动作
    auto sequence = Sequence::create(actionVector);
    // 执行一系列的动作
    _player->runAction(sequence);

}



/**
 * 获得动画动作函数
 * animName：动画帧的名字
 * delay：动画帧与帧之间的间隔时间
 * animNum：动画帧的数量
 */
Animate* Game::getAnimateByName(std::string animName,float delay,int animNum){
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

// 将屏幕Point的坐标转换为TileMap坐标
Vec2 Game::tileCoordForPosition(Vec2 position){
    // CC_CONTENT_SCALE_FACTOR 在iPhone上视网膜显示返回2。否则返回1
	// 玩家位置的x除以地图的宽，得到的是地图横向的第几个格子（tile）
    int x = (int)(position.x / (_tileMap->getTileSize().width / CC_CONTENT_SCALE_FACTOR()));
    // 玩家位置的y除以地图的高，得到的是地图纵向第几个格子（tile），
	// 因为cocos2d-x的y轴和TileMap的y轴相反，所以使用地图的高度减去玩家位置的y
    float pointHeight = _tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR();
    int y = (int)((_tileMap->getMapSize().height * pointHeight - position.y) / pointHeight);
    return Vec2(x,y);
}

// tile坐标转换为gl坐标
Vec2 Game::locationForTilePos(Vec2 pos){
    int x = (int)(pos.x*(_tileMap->getTileSize().width/CC_CONTENT_SCALE_FACTOR()));
	float pointHeight = _tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR();
	int y = (int)((_tileMap->getMapSize().height * pointHeight) - (pos.y * pointHeight));
	return Point(x,y);
}

// 把玩家作为视觉中心来显示，让地图随玩家一起移动
void Game::setViewpointCenter(Vec2 position){
    // 不让显示区域超出地图的边界
    int x = MAX(position.x, _screenWidth/2);
    int y = MAX(position.y, _screenHeight/2);
    x = MIN(x, (_tileMap->getMapSize().width *
                _tileMap->getTileSize().width-_screenWidth/2));
    y = MIN(y, (_tileMap->getMapSize().height *
                _tileMap->getTileSize().height-_screenHeight/2));
    // 计算中心位置和玩家位置两点之间的差值
    Vec2 actualPosition = Vec2(x, y);
    Vec2 centerOfView = Vec2(_screenWidth/2, _screenHeight/2);
    Vec2 viewPoint = centerOfView - actualPosition;
    // 重新设置屏幕位置
    float duration = getMoveTime(_player->getPosition(), position);
    auto moveTo = MoveTo::create(duration, viewPoint);
    this->runAction(moveTo);
}

// 根据移动的位置计算移动的时间
float Game::getMoveTime(Vec2 startPos,Vec2 endPos)
{
    Vec2 startGrid = this->tileCoordForPosition(startPos);
    Vec2 endGrid = this->tileCoordForPosition(endPos);
    // 移动一个网格的时间
    float duration = 0.15f;
    // 根据移动网格计算移动时间,横纵坐标差的和计算平方根
    duration = duration * sqrtf((startGrid.x - endGrid.x) * (startGrid.x - endGrid.x)
                                + (startGrid.y - endGrid.y) * (startGrid.y - endGrid.y));
    return duration;
}

/***********怪物相关函数*********************************/
// 获取怪物随机出现的坐标
Vec2 Game::randomPosintion(){
    // 使用随机数来获取怪物的X和Y坐标
    int randX = arc4random() % (int)(_tileMap->getMapSize().width *
                                _tileMap->getTileSize().width) + 100;
    int randY = arc4random() % (int)(_tileMap->getMapSize().height *
                                     _tileMap->getTileSize().height) + 120;
    Vec2 position = Vec2(randX, randY);
    // 将屏幕的怪物坐标转换为TileMap坐标
    Vec2 tileCoord = tileCoordForPosition(position);
    // 如果获取坐标错误，重新获取
    if (tileCoord.x < 0 || tileCoord.x >= _tileMap->getMapSize().width
		|| tileCoord.y < 0 || tileCoord.y >= _tileMap->getMapSize().height) {
		return randomPosintion();
	}
    // 获取TileMap坐标系里的“全局唯一标识”GID
    int tileGid = _collidable->getTileGIDAt(tileCoord);
    // 如果随机坐标是不可通过的网格位置，则重新获取
    if (tileGid) {
        // 使用GID来查找指定tile的属性，返回一个Value
        Value properties = _tileMap->getPropertiesForGID(tileGid);
        // 返回的Value实际是一个ValueMap
        ValueMap map = properties.asValueMap();
        // 查找ValueMap，判断是否有”可碰撞的“物体，如果有，设置网格对象的isPass变量为false
        std::string value = map.at("collidable").asString();
        if (value.compare("true") == 0) {
            return randomPosintion();
        }else{
            return position;
        }
    }
    // 如果坐标正常直接返回
    return position;
}

// 随机获得怪物移动的坐标
Vec2 Game::randomMovePosintion(SpriteBase* monster){
    Vec2 position;
    // 随机获得移动的X值和Y值
	int randX = arc4random() % 200;
	int randY = arc4random() % 100;
	// _moveCount单数向左移动，双数向右移动
    if (monster->getMoveCount() % 2 == 0) {
        position = Vec2(monster->getPosition().x+randX, monster->getPosition().y+randY);
        if (!monster->isFlippedX()) {
            monster->setFlippedX(true);
        }
    }else{
        position = Vec2(monster->getPosition().x-randX, monster->getPosition().y-randY);
        if (monster->isFlippedX()) {
            monster->setFlippedX(false);
        }
    }
    // 将屏幕的怪物坐标转换为TileMap坐标
    Vec2 tileCoord = tileCoordForPosition(position);
    // 如果获取坐标错误，重新获取
    if (tileCoord.x < 0 || tileCoord.x >= _tileMap->getMapSize().width
		|| tileCoord.y < 0 || tileCoord.y >= _tileMap->getMapSize().height) {
		return randomPosintion();
	}
    // 获取TileMap坐标系里的“全局唯一标识”GID
    int tileGid = _collidable->getTileGIDAt(tileCoord);
    // 如果随机坐标是不可通过的网格位置，则重新获取
    if (tileGid) {
        // 使用GID来查找指定tile的属性，返回一个Value
        Value properties = _tileMap->getPropertiesForGID(tileGid);
        // 返回的Value实际是一个ValueMap
        ValueMap map = properties.asValueMap();
        // 查找ValueMap，判断是否有”可碰撞的“物体，如果有，设置网格对象的isPass变量为false
        std::string value = map.at("collidable").asString();
        if (value.compare("true") == 0) {
            return randomPosintion();
        }else{
            return position;
        }
    }
    // 如果坐标正常直接返回
    return position;
}

// 随机出现怪物精灵
void Game::updateMonsterSprite(float delta) {
    // 当_count取模200时并且怪物集合<=2（控制屏幕怪物总数）时创建一个怪物精灵
    if (_count % 200 == 0 && _monsterVector.size() <= 2) {
        SpriteBase* monster = SpriteBase::create("eagle_move_1.png");
        monster->setScale(0.7);
        // 设置怪物生命值
        monster->setLifeValue(9);
        // 怪物移动次数，用来控制怪物移动方向（左右）
        monster->setMoveCount(1);
        // 设置随机坐标
        monster->setPosition(randomPosintion());
        // 添加到怪物集合
        _monsterVector.pushBack(monster);
        // 添加到地图，并设置zOrder
        _tileMap->addChild(monster,2);
        // 设置怪物移动标记为false
        monster->setMove(false);
    }
}

// 怪物不规则移动
void Game::monsterRepeatedlyMove(float delta){
    // 遍历怪物集合
    for (int i = 0; i < _monsterVector.size(); i++) {
        // 获取每一个怪物精灵对象
        SpriteBase* monster = _monsterVector.at(i);
        // 如果怪物的_move标记为false，代表可以移动
        if (!monster->getMove()) {
            // 设置怪物移动标记为true，意味着当前改为正在移动，则不会进入if重复移动
            monster->setMove(true);
            // 随机获得怪物需要移动到的目标位置
            Vec2 position = randomMovePosintion(monster);
            // 用怪物位置作为起点，触摸点作为终点，转换为网格坐标，查找最佳路径
            Vec2 from = tileCoordForPosition(monster->getPosition());
            Vec2 to = tileCoordForPosition(position);
            // 获得起点到终点的最佳路径
            PathArithmetic* pa = PathArithmetic::create();
            Vector<PointDelegate*> pathVector = pa->getPath(from , to, _gridVector);
            // 怪物根据最佳路径点移动
             monsterMoveWithWayPoints(pathVector, monster);
        }
    }
}

// 怪物根据最佳路径点移动
void Game::monsterMoveWithWayPoints(Vector<PointDelegate*> pathVector, SpriteBase* monster){
    // 定义动作集合，用来保存多个moveTo动作
    Vector<FiniteTimeAction*> actionVector;
    // 循环最佳路径点集合，创建多个MoveTo动作，玩家将执行多个MoveTo动作完成移动
    for (int i = 0; i < pathVector.size(); i++) {
        // 获得需要移动的每一个点
        PointDelegate* pd = pathVector.at(i);
        Vec2 p = Point(pd->getX(), pd->getY());
        // 将tileMap的网格转成gl坐标，即怪物需要移动的位置
        Vec2 glPoint = locationForTilePos(p);
        // 创建MoveTo一个动作，让精灵对象移动到指定的位置
        MoveTo* moveTo = MoveTo::create(1.5f, glPoint);
        // 将该动作添加到临时数组
        actionVector.pushBack(moveTo);
    }
    // 调用getAnimateByName函数获得玩家精灵移动动画
    auto animate = getAnimateByName("eagle_move_", 0.5f, 4);
    // 创建一个动作，重复执行Animate动画
    auto repeatanimate = RepeatForever::create(animate);
    // 玩家精灵重复执行动画动作
    monster->runAction(repeatanimate);
    // 创建回调动作，当MoveTo动作完成后精灵恢复最初站立状态
    auto callfunc = CallFunc::create([=]{
        // 停止动画
        monster->stopAction(repeatanimate);
        // 设置怪物移动标记为false
        monster->setMove(false);
        // moveCount自增，用于控制方向
        monster->setMoveCount(monster->getMoveCount()+1);
    });
    actionVector.pushBack(callfunc);
    // 按顺序执行动作集合中的动作
    auto sequence = Sequence::create(actionVector);
    // 执行一系列的动作
    monster->runAction(sequence);
}

// 玩家和怪物的碰撞检测方法
void Game::collisionDetection(float delta){
    // 遍历怪物精灵集合
    for (int i = 0; i < _monsterVector.size(); i++) {
        SpriteBase* monster = _monsterVector.at(i);
        // 检测玩家精灵和怪物精灵是否发生碰撞
        if (monster->getBoundingBox().intersectsRect(_player->getBoundingBox())) {
            // TransitionSplitRows 左右滑动剧场效果
            auto transition = TransitionSplitRows::create(2.0, Fight::createScene());
            // push战斗场景
            Director::getInstance()->pushScene(transition);
            // 从当前场景和怪物数组中删除怪物
            _tileMap->removeChild(monster);
            _monsterVector.eraseObject(monster);
        }
    }
}






