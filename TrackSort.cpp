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
 * System entry point for the track splitter.
 *
 * Build using:
 *    g++ -std=c++20 -c -o TrackSort.o TrackSort.cpp
 *    g++ -std=c++20 -c -o Initialization.o Initialization.cpp
 *    g++ -std=c++20 -c -o Configuration.o Configuration.cpp
 *    g++ -std=c++20 -c -o Utilities.o Utilities.cpp
 *    g++ -std=c++20 -o TrackSort TrackSort.o Initialization.o Configuration.o Utilities.o
 *
 * Test using:
 *    ./TrackSort -i Tracks.txt -d 19:40
 *
 */

#include <iostream>
#include <string>
#include <vector>

#include "Utilities.h"
#include "Configuration.h"
#include "TextFile.h"


/**
 * @section Define Track class.
 *
 */

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



/**
 * @section Define Side class.
 *
 */

class Side
{
public:
    using Iterator = std::vector<Track>::const_iterator;

    Side() : title{}, seconds{} {}

    void setTitle(const std::string & t) { title = t; }
    void push(const Track & track);
    void pop();

    std::string getTitle() const { return title; }
    size_t getDuration() const { return seconds; }

    size_t size(void) const { return tracks.size(); }
    Iterator begin(void) { return tracks.begin(); }
    Iterator end(void) { return tracks.end(); }

    std::string toString() const;

    void clear() { seconds = 0; tracks.clear(); }

private:
    std::string title;
    size_t seconds;
    std::vector<Track> tracks;

};

void Side::push(const Track & track)
{
    tracks.push_back(track);
    seconds += track.getSeconds();
}

void Side::pop()
{
    seconds -= tracks.back().getSeconds();
    tracks.pop_back();
}

std::string Side::toString() const
{
    std::string s{title + " - " + std::to_string(size()) + " tracks\n"};
    
    for (const auto & track : tracks)
        s += secondsToTimeString(track.getSeconds()) + " - " + track.getTitle() + '\n';

    s += secondsToTimeString(seconds) + '\n';

    return s;
}



/**
 * @section process input file.
 *
 */

std::vector<Track> processInputFile()
{
    // std::cout << "Process input file\n";

    TextFile input{Configuration::getInputFile()};
    input.read();

    // std::cout << "Dump input file " << Configuration::getInputFile() <<  "\n";
    // for (const auto & line : input)
    //     std::cout << line << "\n";

    std::vector<Track> tracks{};

    for (const auto & line : input)
        tracks.emplace_back(line);

    // for (const auto & track : tracks)
    //     std::cout << secondsToTimeString(track.getSeconds()) << " | " << track.getTitle() << "\n";

    return tracks;
}

std::vector<Side> process(const std::vector<Track> & tracks, size_t duration)
{
    // std::cout << "Add tracks to sides\n";
    std::vector<Side> sides;
    Side side{};
    for (const auto & track : tracks)
    {
        side.push(track);
        if (side.getDuration() > duration)
        {
            side.pop();
            const auto count{sides.size() + 1};
            const std::string title{"Side " + std::to_string(count)};
            side.setTitle(title);
            sides.push_back(side);
            side.clear();
            side.push(track);
        }
    }
    if (side.size() != 0)
    {
        const auto count{sides.size() + 1};
        const std::string title{"Side " + std::to_string(count)};
        side.setTitle(title);
        sides.push_back(side);
        side.clear();
    }

    return sides;
}

bool leftCondition(size_t required, size_t current)
{
    if (required < current)
        return true;

    return false;
}
bool rightCondition(const std::vector<Side> & sides)
{
    if (sides.size() <= 1)
        return false;

    size_t first{sides.front().getDuration()};
    size_t last{sides.back().getDuration()};
    if (last < ((first * 95) / 100))
         return true;

    return false;
}
int generate()
{
    // Read track list file.
    std::vector<Track> tracks = processInputFile();

    // Calculate total play time.
    size_t total{};
    for (const auto & track : tracks)
        total += track.getSeconds();
    std::cout << "Total duration " << secondsToTimeString(total) << "\n";

    // Get user requested maximum side length.
    const size_t duration{Configuration::getDuration()};
    std::cout << "Required duration " << secondsToTimeString(duration) << "\n";

    // Calculate number of sides required.
    size_t optimum{total / duration};
    if (total % duration)
        optimum++;
    if ((optimum % 2) && (Configuration::isEven()))
        optimum++;
    std::cout << "Optimum number of sides " << optimum << "\n";

    // Calculate minimum side length.
    size_t length{total/optimum};
    std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";

    // Home in on optimum side length.
    size_t lim{15};
    size_t minimum{length};
    size_t maximum{duration};
    while (minimum <= maximum)
    {
        size_t median{(minimum + maximum + 1) / 2};
        std::cout << "\nSuggested length " << secondsToTimeString(median) << "\n";

        std::vector<Side> sides = process(tracks, median);

        std::cout << "Suggested sides\n";
        for (const auto & side : sides)
            std::cout << side.getTitle() << " - " << side.size() << " tracks " << secondsToTimeString(side.getDuration()) << "\n";

        if ((median == minimum) || (median == maximum))
        {
            break;
        }
        else
        if (leftCondition(optimum, sides.size()))
        {
            minimum = median;
            std::cout << "Minimum set to " << secondsToTimeString(minimum) << "\n";
            std::cout << "Maximum is " << secondsToTimeString(maximum) << "\n";
        }
        else
        if (rightCondition(sides))
        {
            maximum = median;
            std::cout << "Minimum is " << secondsToTimeString(minimum) << "\n";
            std::cout << "Maximum set to " << secondsToTimeString(maximum) << "\n";
        }
        else
        {
            break;
        }

        if (--lim == 0)
        {
            std::cout << "Abort!!!\n";
            break;
        }
    }

    return 0;
}



/**
 * @section Process the command line parameters.
 *
 */

/**
 * @brief Process the command line parameters.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int init(int argc, char *argv[])
{
    int i = Configuration::instance().initialise(argc, argv);
    if (i != 0)
    {
        return i;
    }

#if 0
    std::cout << Configuration::instance() << '\n';
#endif

    if (!Configuration::isValid(true))
    {
        return -1;
    }

    return 0;
}


/**
 * @section System entry point.
 *
 */

/**
 * @brief System entry point.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int main(int argc, char *argv[])
{
    int i = init(argc, argv);
    if (i < 0)      // Error?
    {
        return 1;
    }
    else if (i > 0) // No further processing?
    {
        return 0;
    }

//- If all is well, generate the output.
    return generate();
}

