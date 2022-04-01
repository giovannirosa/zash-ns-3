#ifndef UTILS
#define UTILS

#include <charconv>
#include <iostream>
#include <sstream>

#include "ns3/zash-enums.h"

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

} // namespace ns3

#endif