//
// Created by 陈钧衍 on 15/5/20.
//
#include <string>
#ifndef LPR_SETTINGS_HPP
#define LPR_SETTINGS_HPP
using namespace std;


const bool WRITING_OUTPUT = true;
/*
 * 项目路径
 */
const string PROJECT_PATH =  "/Users/WinKaR/ClionProjects/LPR/";
/*
 * 有效连续跳变的最大长度, 最小长度
 */
const int JUMP_LENGTH_MAX = 80;
const int JUMP_LENGTH_MIN = 50;



/*
 * 两个连续跳变之间的最长距离
 */
const int JUMP_INTERVAL = 10;


/*
 * 二值化后的最大值
 */
const int MAX_VALUE = 255;

/*
 * 竖直连续跳变线段的同侧端点之间允许的最大距离
 */
const int BOUND_INTERVAL = 20;

/*
 * 车牌可能的竖直高度上下限
 */
const int PLATE_WIDTH_MIN = 10;
const int PLATE_WIDTH_MAX = 20;

/*
 * 找到车牌之后, 竖直方向和水平方向的扩展长度.
 */
const int EXPAND_TOP = 5;
const int EXPAND_BOTTOM = 10;
const int EXPAND_HORIZONTAL = 0;

/*
 * hough变换中使用的常量
 * 参考opencv文档
 */
const int LINE_THRESHOLD = 50;
const double MIN_LINE_LENGTH = 60;
const double MAX_LINE_GAP = 30;


/*
 * 车牌水平投影阈值
 * 车牌竖直投影阈值
 * 切分字符时的阈值
 * 对字符1的扩展
 */
const int HORIZONTAL_THRESHOLD = 15;
const int VERTICAL_THRESHOLD = 10;
const int CHAR_THRESHOLD = 1;
const int CHAR_ONE_WIDTH = 3;
const int EXPAND_ONE = 2;


#endif //LPR_SETTINGS_HPP
