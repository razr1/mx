//
//  PathArithmetic.h
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#ifndef __MXGame2__PathArithmetic__
#define __MXGame2__PathArithmetic__

#include <iostream>
#include <vector>
#include "cocos2d.h"
#include "Grid.h"
#include "PointDelegate.h"

USING_NS_CC;
// 使用命名空间
using namespace std;
// 枚举，代表地图的宽和高
enum{
    mapWidth = 26,
    mapHeight = 20
};
 /**
  * cocos2d-x自带的Vector类只能存储继承cocos2d::Ref类的对象，不能存储Vector和Point(Vec2)，
  * 所以笔者使用C++标准的std::vector来存储Vector实现二维数组。
  * 并创建了一个PointDelegate作为Point的代理类存储每一个点的X轴和Y轴数据
  */
class PathArithmetic : public cocos2d::Ref
{
private:
    // 记录已经经过的点
    Vector<PointDelegate*> _invalidPoints;
    // 记录有效路径的点
    Vector<PointDelegate*> _pathPoints;
public:
    // 静态的create函数
    static PathArithmetic* create(){
        PathArithmetic* p = new PathArithmetic();
        if(p){
            p->autorelease();
        }else{
            CC_SAFE_DELETE(p);
        }
        return p;
    }
    /**
     * 寻路函数
     * @param from 开始点
     * @param to 结束点
     * @param gridVector 网格二维数组
     * @return 开始点from到结束点to的最佳（短）路径
     */
    Vector<PointDelegate*> getPath(Point from,Point to,std::vector<Vector<Grid*>> gridVector);
    /**
     * 查找有效路径函数（递归搜索）
     * @param from 开始点
     * @param to 结束点
     * @param gridVector 网格二维数组
     * @return ture 有效路径 false 无效路径
     */
    bool findValidGrid(Point from,Point to,std::vector<Vector<Grid*>> gridVector);
    /**
     * 查检Point点的有效性（是否可以通过）
     * @param point 点
     * @param gridVector 地图二维数组
     * @return true 有效  false 无效
     */
    bool isCheck(Point point,std::vector<Vector<Grid*>> gridVector);
};


#endif /* defined(__MXGame2__PathArithmetic__) */
