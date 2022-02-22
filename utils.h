#include <ctime>
#include <sstream>

using namespace std;

char* formatTime(time_t mtime) {
    char buff[20];
    struct tm* timeinfo;
    timeinfo = localtime(&mtime);
    strftime(buff, sizeof(buff), "%b %d %H:%M", timeinfo);
    return buff;
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