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

int process(const std::vector<Track> & tracks, size_t duration)
{
    std::cout << "Add tracks to sides\n";
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

    std::cout << "Dump sides\n";
    for (const auto & side : sides)
        std::cout << side.toString() << "\n";

    return 0;
}

int generate()
{
    std::vector<Track> tracks = processInputFile();
    auto duration = Configuration::getDuration();
    process(tracks, duration);

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

