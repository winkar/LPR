#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <fstream>
#include "settings.hpp"
using namespace cv;
using namespace std;

/*
 * 使用的诸多常量和阈值定义在settings.hpp当中
 */


/*
 * 使用原图生成边缘图像
 * 1. 灰度化
 * 2. Sobel算子边缘检测(为了检测竖直边缘)
 * 3. 二值化(OTSU)
 */
Mat getEdge(Mat& image);


int main()
{
    cout << "Built with OpenCV " << CV_VERSION << endl;


    Mat image = imread(PROJECT_PATH + "test2.BMP");
    if (image.empty()) {
        cout << "image not found!"<<endl;
        return 1;
    }


    Mat edge = getEdge(image);

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

        /*
         * left_bound 线段左边界
         * 右边界由扫描终止时确定
         */
        int left_bound = -1;

        for (int j=0; j<ncol; ++j) {
            int pt = edge.at<uchar>(i, j);
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
                     * 如果与上一个跳变点过远,则判断为不属于该线段, 处理该线段,并启用新线段
                     * 否则以当前点为左端点,查找新的线段
                     */
                    if (j - last_jump > JUMP_INTERVAL) {
                        const int length = last_jump - left_bound;

                        if (length > JUMP_LENGTH_MIN && length < JUMP_LENGTH_MAX) {
                            /*
                             * 将线段涂白色
                             */
                            for (int k=left_bound; k<=last_jump; ++k) {
                                edge.at<uchar>(i, k) = 255;
                            }
                        }

                        last_jump = j;
                        jumps = 1;
                        left_bound = j;
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

            /*
             * 处理线段一直持续到图片右边缘的情况
             */
            if (j == ncol - 1) {
                for (int k = left_bound; k<= last_jump; ++k) {
                    edge.at<uchar>(i, k) = 255;
                }
            }
        }


    }

    imshow("lines found", edge);
    waitKey(0);
    return 0;
}




Mat getEdge(Mat& image) {
    /*
     * generate the edge of origin image
     * use sobel operator to get vertical edge
     */
    Mat gray, edge;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    /*
     * 生成边缘图像
     */
    Sobel(gray, edge, edge.depth(), 2, 0, 3);


    /*
     *  use threshold to binarize the image
     */
    threshold(edge, edge, 100, MAX_VALUE, cv::THRESH_OTSU | cv::THRESH_BINARY);

    return edge;
}