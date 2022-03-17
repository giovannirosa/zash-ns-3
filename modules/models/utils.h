#ifndef UTILS
#define UTILS

#include <charconv>

using namespace std;

#include "csv_reader.h"

void printValues(map<const char*, enums::Enum*> map) {
    for (auto const& [key, val] : map) {
        cout << key << " - " << val->weight << endl;
    }
}

string percentage(double number, double total) {
    return total == 0 ? "0.0%" : to_string(number / total * 100.0) + "%";
}

char* formatTime(time_t mtime, const char* format) {
    char* buff = (char*)malloc(sizeof(char) * 20);
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
    strftime(buff, 20, format, timeinfo);
    return buff;
}

char* formatTime(time_t mtime) {
    char* buff = (char*)malloc(sizeof(char) * 20);
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
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
    struct tm tm;
    strptime(str, "%F %T", &tm);
    // need to set daylight saving time to unkown for consistent parse
    tm.tm_isdst = -1;
    time_t t = mktime(&tm);
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
        int s;
        auto result = from_chars(row[i].data(), row[i].data() + row[i].size(), s);
        if (result.ec == errc::invalid_argument) {
            cout << "Could not convert.";
        }
        state.push_back(s);
    }
    return state;
}

#endif