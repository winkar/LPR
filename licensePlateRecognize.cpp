#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include "settings.hpp"
#include <vector>
#include "plate_line.hpp"
#include <algorithm>
#include <map>

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


/*
 * 找到图像中竖直跳变密集的线段
 */
void findLines(Mat& edges, map<int, vector<plate_line>>&, vector<plate_line>&);


/*
 * 判断一条线段是否与两个边界足够接近
 */
bool isAvailable(const plate_line& pl, int left_bound, int right_bound);

plateArea findPlate(Mat& edge);

Mat getPlateImage(const Mat& image, const plateArea &plate);

Mat angleAdjustment(const Mat& plate);

int main()
{
    /*
     * 执行流程:
     * 1. 获得图像边缘
     * 2. 扫描一遍获得可能为车牌区域的线段
     * 3. 扫描线段获得可能的车牌区域
     * 4. 提取出车牌区域
     * 5.
     */
    cout << "Built with OpenCV " << CV_VERSION << endl;


    Mat image = imread(PROJECT_PATH + "test2.BMP");
    if (image.empty()) {
        cout << "image not found!"<<endl;
        return 1;
    }

    Mat edge = getEdge(image);


    plateArea plate = findPlate(edge);

//    for (int i =p.top; i<=p.bottom; ++i) {
//        for (int j = p.left; j<=p.right; ++j) {
//            edge.at<uchar>(i,j) =255;
//        }
//    }


    Mat plate_image = getPlateImage(image, plate);
    imshow("origin_plate", plate_image);

    plate_image = angleAdjustment(plate_image);

    imshow("plate", plate_image);

    waitKey(0);
    return 0;
}


Mat angleAdjustment(const Mat& plate){
    Mat new_plate(plate.size(), plate.type());

    Mat dst, color_dst;

    /*
     * 通过hough变换找出车牌上下边缘的直线,以确定倾斜角度
     */
    Canny(plate, dst, 50, 200, 3);
    cvtColor(dst, color_dst, COLOR_GRAY2BGR);
    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/180, LINE_THRESHOLD, MIN_LINE_LENGTH, MAX_LINE_GAP );

    double tanA = 0.0;
    for( size_t i = 0; i < lines.size(); i++ )
    {
//        line( color_dst, Point(lines[i][0], lines[i][1]),
//              Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
        tanA += double(lines[i][3] - lines[i][1]) / double(lines[i][0]-lines[i][2]);
    }


    tanA /= double(lines.size());

    cout << tanA <<endl;

    for (int i=0; i<plate.rows;++i) {
        for (int j=0; j<plate.cols; ++j) {
            new_plate.at<Vec3b>(i,j) = plate.at<Vec3b>(i, cvRound(j+ tanA*i));
        }
    }


    return new_plate;
}


Mat getPlateImage(const Mat& image, const plateArea &plate){
    /*
     * 扩展车牌区域, 否则找到的区域小于车牌区域
     */

    int top = max(0, plate.top - EXPAND_VERTICAL),
        bottom = min(image.rows-1, plate.bottom + EXPAND_VERTICAL),
        left = max(0, plate.left - EXPAND_HORIZONTAL),
        right = min(image.cols-1, plate.right + EXPAND_HORIZONTAL);

    /*
     * 将车牌区域剪切出来
     */
    Mat plate_image = image(Range(top, bottom), Range(left, right));
    plate_image = plate_image.clone();

    return plate_image;
}


plateArea findPlate(Mat& edge) {

    map<int, vector<plate_line>> lines_map;
    vector<plate_line> lines;

    findLines(edge, lines_map, lines);

    sort(lines.begin(), lines.end());

    /*
     * 对于每一条线段
     * 按行往下找出符合边界条件的线段,
     * 记录左边界,右边界,上下界
     */
    for (auto line: lines) {

        int linenum = line.linenum,
                left_bound = line.start,
                right_bound = line.end;

        int next_line = linenum + 1;
        while (next_line < edge.cols) {
            /*
             * 找出出当前要处理的行上的线段中, 符合边界限制的线段
             */
            plate_line candidate;
            for (auto& l: lines_map[next_line]) {
                if (isAvailable(l, left_bound, right_bound)) {
                    candidate = l;
                    break;
                }
            }

            /*
             * 不为空的情况下,继续往下寻找
             */
            if (candidate) {
                /*
                 * 扩展左右边界
                 */
                left_bound = min(left_bound, candidate.start);
                right_bound = max(right_bound, candidate.end);
                next_line ++;
                continue;
            }
            /*
             * 为空时, 若找到的区域符合车牌形状限制,则返回
             * 否则处理以下一条线段为上边界的情况
             */
            else {
                int width = next_line - linenum - 1;
                if (width && width < PLATE_WIDTH_MAX && width > PLATE_WIDTH_MIN) {
                    return plateArea(linenum, next_line-1, left_bound, right_bound);
                }
                break;
            }
        }
    }
    return plateArea();
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

void findLines(Mat& edges, map<int, vector<plate_line>>& lines_map, vector<plate_line>& lines){
    /*
     * 计算跳变以寻找密集竖直边缘
     *
     * 1. 对于每一行, 从左至右扫描, 找到第一个跳变后,后面的每一个跳变需要与上一个足够近,且这段跳变长度合适, 则视为有效线段
     * 2. 扫描下一行, 重复这个过程,找到垂直方向连续可以凑成类似矩形的所有线段
     * 3. 上述找到的线段集合构成我们需要的车牌区域
     *
     * lines_map 存储每一行对应的线段
     * lines 存储所有的线段
     */
    int nrow = edges.rows, ncol = edges.cols;


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
            int pt = edges.at<uchar>(i, j);
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
                            plate_line line(i, left_bound, last_jump);
                            lines_map[i].push_back(line);
                            lines.push_back(line);
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
                    const int length = last_jump - left_bound;

                    if (length > JUMP_LENGTH_MIN && length < JUMP_LENGTH_MAX) {
                        plate_line line(i, left_bound, last_jump);
                        lines_map[i].push_back(line);
                        lines.push_back(line);
                    }

                }
            }
        }
    }
}

bool isAvailable(const plate_line& pl, int left_bound, int right_bound) {
    /*
     * 左边界与线段左端点, 右边界与线段右端点之间的距离不能超出阈值
     */
    return ((abs(pl.start- left_bound) < BOUND_INTERVAL) && abs(pl.end - right_bound) < BOUND_INTERVAL);
}
