#ifndef UTILS
#define UTILS

#include <charconv>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ns3/zash-enums.h"
// #include "ns3/zash-audit.h"

using namespace std;

namespace ns3 {

/*
 * Print weight of enums
 * \param map the map of enums
 */
void printValues(map<const char *, enums::Enum *> map);

/*
 * Format to percentage text
 * \param number dividend (numerator)
 * \param total divisor (denominator)
 * \returns formatted percentage string
 */
string percentage(double number, double total);

/*
 * Format from time_t to string using specific format
 * \param mtime time
 * \param format time format
 * \returns formatted time string
 */
string formatTime(time_t mtime, const char *format);

/*
 * Format from time_t to string using "%F %T" format
 * \param mtime time
 * \returns formatted time string
 */
string formatTime(time_t mtime);

/*
 * Print formatted time string to specific output stream
 * \param mtime time
 * \param out output stream
 */
void printFormattedTime(time_t mtime, ostream &out);

/*
 * Print formatted time string to standard output stream (cout)
 * \param mtime time
 */
void printFormattedTime(time_t mtime);

/*
 * Parse from text to time
 * \param str time text
 * \returns time
 */
time_t strToTime(const char *str);

/*
 * Extract hour from time
 * \param mtime time
 * \returns hour
 */
int extractHour(time_t mtime);

/*
 * Extract day of month from time
 * \param mtime time
 * \returns day of month
 */
int extractDay(time_t mtime);

/*
 * Format from vector of enums to text
 * \param v vector of enums
 * \returns formatted text
 */
string vecToStr(vector<enums::Enum *> v);

/*
 * Format from vector of integers to text
 * \param v vector of integers
 * \returns formatted text
 */
string vecToStr(vector<int> v);

/*
 * Format from vector of string to text
 * \param v vector of strings
 * \returns formatted text
 */
string vecToStr(vector<string> v);

/*
 * Format from string to vector of strings using comma as delimiter
 * \param buffer string
 * \returns vector of strings
 */
vector<string> strTokenize(string buffer);

/*
 * Get current local time formatted to string
 * \returns formatted current time
 */
string getTimeOfSimulationStart();

/*
 * Create file and store its content
 * \param fileName string
 * \param simDate string
 * \param content string
 */
void createFile(string fileName, string simDate, string content);

} // namespace ns3

#endif
