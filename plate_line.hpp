//
// Created by 陈钧衍 on 15/5/20.
//

#ifndef LPR_LINE_HPP
#define LPR_LINE_HPP

class plateArea {
public:
    int top, bottom, left, right;
    plateArea(int top, int bottom, int left, int right):
            top(top), bottom(bottom), left(left), right(right) {}
    plateArea() {top=bottom=left=right=0;}


    operator bool() {
        return top || bottom || left || right;
    }
};

class plate_line {
public:
    /*
     * 行号
     * 左端点,右端点
     */
    int linenum;
    int start, end;

    plate_line(int linenum, int start, int end): linenum(linenum), start(start), end(end) {}
    plate_line() {linenum=start=end=0;}
    friend bool operator<(const plate_line& p1, const plate_line& p2);


    operator bool() {
        return linenum || start || end;
    }
};

bool operator<(const plate_line& p1, const plate_line& p2) {
    if (p1.linenum != p2.linenum) return (p1.linenum < p2.linenum);
    return (p1.start < p2.start);
}

#endif //LPR_LINE_HPP
