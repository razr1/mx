//
//  PathArithmetic.cpp
//  MXGame2
//
//  Created by Jason on 14-5-8.
//
//

#include "PathArithmetic.h"
#include <algorithm>

// 寻路函数
Vector<PointDelegate*> PathArithmetic::getPath(Point from,Point to,vector<Vector<Grid*>> _gridArray){
    // 调用findValidGrid函数递归完成路径查找
    findValidGrid(from, to, _gridArray);
    // 计算路径完成，_pathPoints集合当中存储的是最佳路径的每一个Point点
    // 因为存储时是先加人第一步网格，再加入第二步网格，所以需要反转数组
    _pathPoints.reverse();
    // 返回最佳路径Point点的集合
    return _pathPoints;
}

// 查找有效路径函数（递归搜索）
bool PathArithmetic::findValidGrid(Point from,Point to,vector<Vector<Grid*>> _gridArray){
    // 因为Vector不支持存储Point，所以使用一个代理类PointDelegate存储Point的x和y
    PointDelegate* fromDelegate = PointDelegate::create(from.x, from.y);
    //（1）记录走过的点，之后的点如果存在这个集合当中，则视为无效点，不再重复记录
    _invalidPoints.pushBack(fromDelegate);
    //（2）判断当前点的上、右、左、下、左上、左下、右上、右下八个点是否有效或者是目的点
    // 使用临时Vector集合存储需要检测的8个点
    Vector<PointDelegate*> points;
    points.pushBack(PointDelegate::create(from.x, from.y-1));
    points.pushBack(PointDelegate::create(from.x, from.y+1));
    points.pushBack(PointDelegate::create(from.x-1, from.y));
    points.pushBack(PointDelegate::create(from.x+1, from.y));
    points.pushBack(PointDelegate::create(from.x-1, from.y-1));
    points.pushBack(PointDelegate::create(from.x+1, from.y+1));
    points.pushBack(PointDelegate::create(from.x-1, from.y+1));
    points.pushBack(PointDelegate::create(from.x+1, from.y-1));
    // 使用临时Vector集合存储8个点中有效（是否可以通过）的点
    Vector<PointDelegate*> temp;
    for (int i = 0; i < points.size(); i++) {
        PointDelegate* pd = points.at(i);
        // 判断当前点是不是最终的目地点to，如果是，存储到_pathPoints集合当中，返回true
        Point p = Point(pd->getX(), pd->getY());
        if (p.equals(to)) {
            _pathPoints.pushBack(pd);
            return true;
        }
        // 检查当前点的有效性（前后左右是否可以通过），如果可以通过，添加到临时集合temp中准备排序
        if (isCheck(p, _gridArray)) {
            temp.pushBack(pd);
        }
    }
    //（3）对临时集合中有效点进按最短路径排序。
    std::sort(temp.begin(), temp.end(),
         [=](const Ref* obj1, const Ref* obj2){
             PointDelegate* p1 = (PointDelegate*)obj1;
             PointDelegate* p2 = (PointDelegate*)obj2;
             double r1 = sqrt((p1->getX() - to.x) * (p1->getX() - to.x) + (p1->getY() - to.y) * (p1->getY() - to.y));
             double r2 = sqrt((p2->getX() - to.x) * (p2->getX() - to.x) + (p2->getY() - to.y) * (p2->getY() - to.y));
             return r1 < r2 ? -1 : 0;
         });
	//（5）递归找出所有有效点直到搜索到终点。
    for (int i = 0; i < temp.size(); i++) {
        PointDelegate* pd = temp.at(i);
        Point p = Point(pd->getX(), pd->getY());
        // 注意：temp集合中存储的第一个点[p]就是距离终点最近的有效点，递归调用findValidGrid函数，即再次搜索[p]点的8个点，一直到终点
        bool flag = findValidGrid(p, to, _gridArray);
        if (flag) {
            //（4）把距离最近的有效点的存储到_pathPoints数组，注意：第一个存入_pathPoints的点就是终点，依次类推，即从终点到起点的最佳路径，之后需要调转顺序。
            _pathPoints.pushBack(pd);
            return true;
        }
    }
    return false;
}

// 查检Point点的有效性（是否可以通过）
bool PathArithmetic::isCheck(Point point,vector<Vector<Grid*>> _gridArray){
    // x和y小于0返回false
	if (point.x < 0 || point.y < 0) {
		return false;
	}
    // _invalidPoints中记录已经经过的点，如果该集合中包含这个点，返回false
    PointDelegate* g = PointDelegate::create(point.x,point.y);
    for (int i = 0; i < _invalidPoints.size(); i++) {
        PointDelegate* pp = _invalidPoints.at(i);
        Point t = Point(pp->getX(), pp->getY());
        if (point.equals(t)) {
            return false;
        }
    }
    // x和y超出地图宽度和高度时返回false
	if (point.x >= mapWidth || point.y >= mapHeight) {
		return false;
	}
    // 从二维数组中获取当前点所代表的地图网格，判断x.y代表的点是否可以通过（有障碍物则不能通过）
    Vector<Grid*> tempX = _gridArray.at((int)g->getX());
    Grid* grid = tempX.at((int)g->getY());
    if (point.x >= 0  && point.y >= 0 && grid->getPass()) {
		return true;
	}
    return false;
}
