#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;


/*
 *  Define all the threshold used in license plate recognize
 */

/*
 * 边缘检测的阈值(Canny算子用)
 */
const int EDGE_THRESHOLD = 30;

/*
 * 测试用图片路径
 */
const string PROJECT_PATH = "/Users/WinKaR/ClionProjects/LPR/";

/*
 * 两个连续跳变之间的最长距离
 */
const int JUMP_INTERVAL = 10;

/*
 * 有效连续跳变的最大数量, 最小数量
 */
const int JUMP_COUNT_MAX = 20;
const int JUMP_COUNT_MIN = 10;

/*
 * 二值化后的最大值
 */
const int MAX_VALUE = 255;

/*
 * 连续跳变线段端点之间的最大距离
 */
const int BOUND_INTERVAL = 10;


int main()
{
    cout << "Built with OpenCV " << CV_VERSION << endl;

    Mat image = imread(PROJECT_PATH + "test2.BMP");
    if (image.empty()) {
        cout << "image not found!"<<endl;
        return 1;
    }

    /*
     * generate the edge of origin image
     * use sobel operator to get vertical edge
     */
    Mat gray, edge;
    cvtColor(image, gray, COLOR_BGR2GRAY);

//    imshow("gray", gray);
    Sobel(gray, edge, edge.depth(), 2, 0, 3);


    /*
     *  use threshold to binarize the image
     */
    threshold(edge, edge, 100, MAX_VALUE, cv::THRESH_OTSU | cv::THRESH_BINARY);

    if (! imwrite(PROJECT_PATH + "edge.png", edge))  {
        cout << "write image failed" << endl;
    }

    /*
     * 计算跳变以寻找密集竖直边缘
     *
     * 1. 对于每一行, 从左至右扫描, 找到第一个跳变后,后面的每一个跳变需要与上一个足够近,且这段跳变长度合适, 则视为有效线段
     * 2. 扫描下一行, 重复这个过程,找到垂直方向连续可以凑成类似矩形的所有线段
     * 3. 上述找到的线段集合构成我们需要的车牌区域
     */
    int nrow = edge.rows,
        ncol = edge.cols;


    int leftSide = -1,
        rightSide = -1,
        top = -1,
        bottom = -1;



    for (int i =0; i<nrow; ++i) {

        /*
         * 跳变点判断条件: 连续的0和255之间的分界点
         *
         * current_status 存储当前为0或为1
         * last_jump 存储上一个跳变点
         * jumps 当前线段跳变数
         */
        int current_status = -1;
        int last_jump = -1;
        int jumps = 0;

        int left_bound = -1;

        for (int j=0; j<ncol; ++j) {
            int pt = image.at<uchar>(i, j);
            /*
             * 每行的第一个点
             */
            if (current_status == -1) {
                current_status = pt;
                continue;
            }

            /*
             * 找到一个跳变点
             */
            if (current_status != pt) {
                current_status = pt;

                /*
                 * 如果为本条线段的第一个跳变点
                 * 更新左边界
                 * 设置本线段计数
                 */
                if (last_jump == -1) {
                    jumps = 1;
                    last_jump = j;
                    left_bound = j;
                }
                /*
                 * 如果非第一个跳变点
                 */
                else {
                    /*
                     * 如果与上一个跳变点过远,则判断为不属于该线段
                     * 更新右边界
                     * 如果线段的长度已经足够, 且与当前车牌区域的左右边界接近, 则将用该线段更新左右边界, 退出本行
                     * 否则以当前点为左端点,查找新的线段
                     */
                    if (j - last_jump > JUMP_INTERVAL) {
                        if (jumps > JUMP_COUNT_MIN && jumps < JUMP_COUNT_MAX
                            && abs(left_bound - leftSide) < BOUND_INTERVAL
                            && abs(last_jump - rightSide) < BOUND_INTERVAL) {
                            leftSide = min(leftSide, left_bound);
                            rightSide = max(last_jump, rightSide);
                            if (top == -1) {
                                top = i;
                            }
                            bottom = i;
                            break;
                        } else {
                            last_jump = j;
                            jumps = 1;
                            left_bound = j;
                        }
                    }
                    /*
                     * 与上一个跳变点接近, 则将点加入该线段
                     */
                    else {
                        jumps ++ ;
                        last_jump = j;
                    }
                }
            }
        }
    }

    for (int i = top; i<=bottom; ++i) {
        for (int j = leftSide; j<= rightSide; ++j) {
            edge.at<uchar>(i, j) = 255;
        }
    }

    imshow("license plate", edge);
    waitKey(0);
    return 0;
}
