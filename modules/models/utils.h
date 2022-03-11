#ifndef UTILS
#define UTILS

#include <stdio.h>
#include <time.h>

#include <charconv>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

#include "csv_reader.h"

void printValues(map<const char*, enums::Enum*> map) {
    for (auto const& [key, val] : map) {
        cout << key << " - " << val->weight << endl;
    }
}

string percentage(double number, double total) {
    // return "0.0%" if total == 0 else "{}%".format(str(round(number / total * 100, 2)))
    // cout << number / total << endl;
    return total == 0 ? "0.0%" : to_string(number / total * 100.0) + "%";
}

char* formatTime(time_t mtime, const char* format) {
    char* buff = (char*)malloc(sizeof(char) * 20);
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
    // cout << "year = " << timeinfo->tm_year << endl
    //      << "month = " << timeinfo->tm_mon << endl
    //      << "day = " << timeinfo->tm_mday << endl
    //      << "hour = " << timeinfo->tm_hour << endl
    //      << "minute = " << timeinfo->tm_min << endl
    //      << "second = " << timeinfo->tm_sec << endl;
    // cout << sizeof(buff) << endl;
    strftime(buff, 20, format, timeinfo);
    return buff;
}

char* formatTime(time_t mtime) {
    char* buff = (char*)malloc(sizeof(char) * 20);
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
    // cout << "year = " << timeinfo->tm_year << endl
    //      << "month = " << timeinfo->tm_mon << endl
    //      << "day = " << timeinfo->tm_mday << endl
    //      << "hour = " << timeinfo->tm_hour << endl
    //      << "minute = " << timeinfo->tm_min << endl
    //      << "second = " << timeinfo->tm_sec << endl;
    // cout << sizeof(buff) << endl;
    strftime(buff, 20, "%F %T", timeinfo);
    return buff;
}

void printFormattedTime(time_t mtime, ostream &out) {
    char* str = formatTime(mtime);
    out << str;
    delete str;
}

void printFormattedTime(time_t mtime) {
    char* str = formatTime(mtime);
    cout << str;
    delete str;
}

time_t strToTime(const char* str) {
    // cout << "strToTime = " << str << endl;
    struct tm tm;
    strptime(str, "%F %T", &tm);

    // int d, m, y, hh, mm, ss;
    // sscanf(str, "%d-%d-%d %d:%d:%d", &y, &m, &d, &hh, &mm, &ss);

    // // cout << d << " " << m << " " << y << " " << hh << " " << mm << " " << ss << endl;

    // tm.tm_hour = hh;
    // tm.tm_min = mm;
    // tm.tm_sec = ss;
    // tm.tm_year = y;
    // tm.tm_mon = m;
    // tm.tm_mday = d;

    // istringstream ss(str);
    // ss >> get_time(&tm, "%YYYY-mm-dd %H:%M:%S");  // 2016-02-01 08:00:00
    time_t t = mktime(&tm);
    // cout << "strToTime = " << tm.tm_mday << endl;
    // cout << "strToTime = " << t << endl;
    // cout << "strToTime = " << formatTime(t) << endl;
    return t;
}

int extractHour(time_t mtime) {
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
    return timeinfo->tm_hour;
}

string vecToStr(vector<enums::Enum*> v) {
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i != 0)
            ss << ",";
        ss << v[i]->key;
    }
    ss << "]";
    return ss.str();
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