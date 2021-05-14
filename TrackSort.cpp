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

std::vector<Track> tracks;


/**
 * @section Define Disc class.
 *
 */

class Disc
{
public:
    using Iterator = std::vector<Track>::const_iterator;

    Disc() : title{}, seconds{} {}

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

void Disc::push(const Track & track)
{
    tracks.push_back(track);
    seconds += track.getSeconds();
}

void Disc::pop()
{
    seconds -= tracks.back().getSeconds();
    tracks.pop_back();
}

std::string Disc::toString() const
{
    std::string s{title + " - " + std::to_string(size()) + " tracks\n"};
    
    for (const auto & track : tracks)
        s += secondsToTimeString(track.getSeconds()) + " - " + track.getTitle() + '\n';

    s += secondsToTimeString(seconds) + '\n';

    return s;
}

std::vector<Disc> discs;


/**
 * @section process input file.
 *
 */

int process()
{
    TextFile input{Configuration::getInputFile()};
    input.read();

    std::cout << "Dump file\n";
    for (const auto & line : input)
        std::cout << line << "\n";

    std::cout << "Process file\n";
    for (const auto & line : input)
        tracks.emplace_back(line);

    std::cout << "Add tracks to discs\n";
    Disc disc{};
    for (const auto & track : tracks)
    {
        disc.push(track);
        if (disc.getDuration() > Configuration::getDuration())
        {
            disc.pop();
            const auto count{discs.size() + 1};
            const std::string title{"Disc " + std::to_string(count)};
            disc.setTitle(title);
            discs.push_back(disc);
            disc.clear();
            disc.push(track);
        }
    }
    if (disc.size() != 0)
    {
        const auto count{discs.size() + 1};
        const std::string title{"Disc " + std::to_string(count)};
        disc.setTitle(title);
        discs.push_back(disc);
        disc.clear();
    }

    std::cout << "Dump discs\n";
    for (const auto & disc : discs)
        std::cout << disc.toString() << "\n";

    // for (const auto & track : tracks)
    //     std::cout << secondsToTimeString(track.getSeconds()) << " | " << track.getTitle() << "\n";

    return 0;
}


/**
 * @section System entry point.
 *
 */

extern int initialise(int argc, char *argv[]);

/**
 * @brief System entry point.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int main(int argc, char *argv[])
{
//- Process the command line parameters.
    auto i = initialise(argc, argv);
    if (i < 0)
    {
        return 1;
    }
    else if (i > 0)
    {
        return 0;
    }

#if 0
    std::cout << Configuration::instance() << '\n';
#endif


    if (!Configuration::isValid(true))
    {
        return 1;
    }

//- If all is well, generate the output.
    return process();
}

