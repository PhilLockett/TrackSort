/**
 * @file    Side.cpp
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
 * sides.
 *
 * Basic utility code for the track splitter.
 */

#include "Side.h"
#include "Utilities.h"


/**
 * @section Define Track class.
 *
 */

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

