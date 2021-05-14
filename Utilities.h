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

/**
 * @section basic utility code.
 */

extern const std::string whitespace;
extern const std::string digit;

extern size_t timeStringToSeconds(std::string buffer);
extern std::string secondsToTimeString(size_t seconds, const std::string & sep = ":");

#endif //!defined _UTILITIES_H_INCLUDED_
