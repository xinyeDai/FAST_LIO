#include <iostream>
#include <fstream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
using namespace std;
// 要转换的文件名
// 注意！！！
// 文件和代码要放在同一级目录
// 在.csv文件中加一行，放在开头：：：：   0,0,0,0,0,0,0
string fname = "/home/xyd/PycharmProjects/ncl2rosbag_ws/raw_data/2013-01-10/ground_truth/groundtruth_2013-01-10.csv";
int getLong(string line)
{
    int numsize = 0;
    for (auto a : line)
        if (a == ',')
            numsize++;
    return numsize + 1;
}
// 读取真实值 .cs格式
vector<vector<long double>> csvRead(string filename)
{
    vector<vector<long double>> result;
    ifstream infile(filename, ios::in);
    string line;
    getline(infile, line);
    int sizex = getLong(line);
    while (getline(infile, line))
    {
        stringstream ss(line);
        string str;
        vector<long double> lineReader;
        for (int i = 0; i < sizex; i++)
        {
            getline(ss, str, ',');
            lineReader.push_back(stod(str));
        }
        result.push_back(lineReader);
    }
    return result;
}
// 输出真实值 tum格式
bool csvWrite(vector<vector<long double>> data, string output_filename)
{
    ofstream outFile(output_filename, ios::out);
    if (!outFile)
    {
        // out part
        cout << "打开文件失败！" << endl;

        return false;
    }
    for (auto i : data)
    {
        i[0] /= 1000000.0;
        int count = 0;
        for (auto j : i)
        {
            outFile << std::fixed << std::setprecision(6) << j;
            count++;
            if (count != i.size())
            {
                outFile << " ";
            }
        }
        outFile << endl;
    }
    outFile.close();
    return true;
}

int main()
{
    long double x, y, z, w, roll, pitch, yaw;
    // 用于存放读取文件的容器
    vector<vector<long double>> result1;
    // 用于输出的容器
    vector<vector<long double>> result2;
    result1 = csvRead(fname);
    result2.resize(result1.size(), std::vector<long double>(8));
    // 将欧拉角转换为四元素形式
    for (int i = 0; i < result1.size(); i++)
    {
        roll = result1[i][4];
        pitch = result1[i][5];
        yaw = result1[i][6];
        yaw = yaw * 3.14 / 180;
        roll = roll * 3.14 / 180;
        pitch = pitch * 3.14 / 180;
        double cy = cos(yaw * 0.5);
        double sy = sin(yaw * 0.5);
        double cp = cos(pitch * 0.5);
        double sp = sin(pitch * 0.5);
        double cr = cos(roll * 0.5);
        double sr = sin(roll * 0.5);
        w = cy * cp * cr + sy * sp * sr;
        x = cy * cp * sr - sy * sp * cr;
        y = sy * cp * sr + cy * sp * cr;
        z = sy * cp * cr - cy * sp * sr;
        for (int j = 0; j < 4; j++)
        {
            result2[i][j] = result1[i][j];
        }
        result2[i][4] = x;
        result2[i][5] = y;
        result2[i][6] = z;
        result2[i][7] = w;
    }

    csvWrite(result2, "/home/xyd/Project/fastlio_ws/src/FAST_LIO/trajectory/gd.txt");
    return 0;
}
