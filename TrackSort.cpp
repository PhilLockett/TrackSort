/**
 * @file    TrackSort.cpp
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
 * Unit test code for the 'tfc' utility.
 *
 * Build using:
 *    g++ -std=c++20 -o TrackSort TrackSort.cpp 
 *
 * Test using:
 *    ./test
 *
 */

#include <iostream>
#include <string>
#include <vector>

#include "TextFile.h"


/**
 * @section basic utility code.
 */

const std::string inputFile{"Tracks.txt"};

class Track
{
public:
    const std::string whitespace{" \t"};
    const std::string digit{"0123456789"};

    Track(std::string line);

    std::string getTitle() const { return title; }
    size_t getSeconds() const { return seconds; }

    std::string toString() const { return std::to_string(seconds) + "s - " + title; }

private:
    std::string title;
    size_t seconds;
};

Track::Track(std::string line)
{
    // Get duration string from the beginning of the line.
    auto pos = line.find_first_of(whitespace);
    if (pos == std::string::npos)
        return;

    std::string duration(line);
    duration.resize(pos);

    // Get track title from whatever is after duration.
    pos = line.find_first_not_of(whitespace, pos);
    if (pos == std::string::npos)
        return;

    title = line.substr(pos);

    // Break duration down to get total number of seconds from duration (H:M:S).
    std::string buffer{duration};
    size_t digits{};
    seconds = 0;
    pos = 0;

    for (int i = 0; i < 3; ++i)
    {
        pos += digits;
        buffer = buffer.substr(pos);
        pos = buffer.find_first_of(digit);
        if (pos == std::string::npos)
            break;

        seconds *= 60;
        buffer = buffer.substr(pos);
        seconds += std::stoi(buffer, &digits);
    }
}

std::vector<Track> tracks;

/**
 * @section check test environment setup.
 *
 */


/**
 * System entry point.
 *
 * @param  argc - command line argument count.
 * @param  argv - command line argument vector.
 * @return error value or 0 if no errors.
 */

int main(int argc, char *argv[])
{
    TextFile input{inputFile};
    input.read();

    for (const auto & line : input)
        std::cout << line << "\n";

    for (const auto & line : input)
        tracks.emplace_back(line);

    for (const auto & track : tracks)
        std::cout << track.toString() << "\n";

    return 0;
}

