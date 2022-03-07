#ifndef UTILS
#define UTILS

#include <charconv>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

#include "csv_reader.h"

char* formatTime(time_t mtime) {
    char* buff;
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
    strftime(buff, sizeof(buff), "%b %d %H:%M", timeinfo);
    return buff;
}

time_t strToTime(string str) {
    struct tm tm;
    istringstream ss(str);
    ss >> get_time(&tm, "%YYYY-mm-dd %H:%M:%S");  // 2016-02-01 08:00:00
    return mktime(&tm);
}

int extractHour(time_t mtime) {
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
    return timeinfo->tm_hour;
}

string vecToStr(vector<int> v) {
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i != 0)
            ss << ",";
        ss << v[i];
    }
    ss << "]";
    return ss.str();
}

vector<int> rowToState(CSVRow row, int devNum) {
    vector<int> state;
    for (int i = 0; i < devNum; ++i) {
        // cout << row[i] << " ";
        int s;
        auto result = from_chars(row[i].data(), row[i].data() + row[i].size(), s);
        if (result.ec == errc::invalid_argument) {
            cout << "Could not convert.";
        }
        state.push_back(s);
    }
    // cout << endl;
    return state;
}

#endif