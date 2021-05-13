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
#include <sstream>
#include <vector>

#include "TextFile.h"


/**
 * @section basic utility code.
 */

const std::string inputFile{"Tracks.txt"};
const std::string whitespace{" \t"};
const std::string digit{"0123456789"};

/**
 * @brief Break a time string (H:M:S) down to get total number of seconds.
 * Also handles M:S and S formats.
 * 
 * @param buffer time string to parse.
 * @return size_t the equivalent number of seconds.
 */
static size_t timeStringToSeconds(std::string buffer)
{
    size_t digits{};
    size_t seconds{};
    size_t pos{};

    for (int i = 0; i < 3; ++i)
    {
        buffer = buffer.substr(digits);
        pos = buffer.find_first_of(digit);
        if (pos == std::string::npos)
            break;

        seconds *= 60;
        buffer = buffer.substr(pos);
        seconds += std::stoi(buffer, &digits);
    }

    return seconds;
}

/**
 * @brief Generates a time string in the form H:M:S from the given seconds.
 * 
 * @param seconds number of seconds to represent.
 * @param sep optional seperator or ':' if none specified.
 * @return std::string time string in the form H:M:S.
 */
static std::string secondsToTimeString(size_t seconds, const std::string & sep = ":")
{
    std::ostringstream ss;

    size_t hours{seconds / 3600};
    seconds -= hours * 3600;

    size_t minutes{seconds / 60};
    seconds -= minutes * 60;

    ss.width(2);
    ss.fill('0');
    ss << hours << sep;
    ss.width(2);
    ss << minutes << sep;
    ss.width(2);
    ss << seconds;

    return ss.str();
}

class Track
{
public:
    Track(std::string line);

    std::string getTitle() const { return title; }
    size_t getSeconds() const { return seconds; }

    std::string toString() const { return std::to_string(seconds) + " - " + title; }

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
    seconds = timeStringToSeconds(duration);

    // Get track title from whatever is after duration.
    pos = line.find_first_not_of(whitespace, pos);
    if (pos == std::string::npos)
        return;

    title = line.substr(pos);
}

std::vector<Track> tracks;

class Disc
{
public:
    Disc() : seconds{} {}

    void push(const Track & track);
    Track pop();
    size_t getDuration() { return seconds; }
    size_t getCount() { return tracks.size(); }

private:
    size_t seconds;
    std::vector<Track> tracks;

};

void Disc::push(const Track & track)
{
    tracks.push_back(track);
    seconds += track.getSeconds();
}

Track Disc::pop()
{
    Track last{tracks.back()};
    tracks.pop_back();
    seconds -= last.getSeconds();

    return last;
}

std::vector<Disc> Discs;

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

