/**
 * @file    Utilities.h
 * @author  Phil Lockett <phillockett65@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * 'TrackSort' is a command-line utility for splitting tracks across multiple
 * discs.
 *
 * Basic utility code for the track splitter.
 */

#if !defined _UTILITIES_H_INCLUDED_
#define _UTILITIES_H_INCLUDED_

#include <string>
#include <filesystem>
#include <vector>
#include <cmath>
#include <numeric>

#include "Side.h"

/**
 * @section basic utility code.
 */

extern const std::string whitespace;
extern const std::string digit;

extern size_t timeStringToSeconds(std::string buffer);
extern std::string secondsToTimeString(size_t seconds, const std::string & sep = ":");
extern std::vector<Track> buildTrackListFromInputFile(const std::filesystem::path & inputFile);

/**
 * @brief Calculate the standard deviation of the lengths of the given list of
 * sides.
 * 
 * @param list to use as data.
 * @return double the calculated the standard deviation.
 */
#include <iostream>
template<typename T=std::vector<Side>>
double deviation(const T & list)
{
    // Calculate total play time.
    auto lambdaSum = [](size_t a, const Side & b) { return a + b.getDuration(); };
    size_t total = std::accumulate(list.begin(), list.end(), 0, lambdaSum);
	// std::cout << "total " << total << "\n";

    double mean{(double)total / list.size()};
	// std::cout << "mean " << mean << "\n";

    auto lambdaVariance = [mean](double a, const Side & b) { return a + std::pow((mean - b.getDuration()), 2); };
    double variance = std::accumulate(list.begin(), list.end(), 0.0, lambdaVariance);
	// std::cout << "variance " << variance << "\n";
    variance /= list.size();
	// std::cout << "variance " << variance << "\n";

    return std::sqrt(variance);
}

#endif //!defined _UTILITIES_H_INCLUDED_
