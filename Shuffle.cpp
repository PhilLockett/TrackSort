/**
 * @file    Shuffle.cpp
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
 * Basic code for the track shuffling
 */

#include <iostream>
#include <string>
#include <vector>
#include <numeric>

#include "Shuffle.h"
#include "Side.h"
#include "Utilities.h"
#include "Configuration.h"

static std::vector<Side> addTracksToSides(const std::vector<Track> & tracks, size_t duration)
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

static bool leftCondition(size_t required, size_t current)
{
    if (required < current)
        return true;

    return false;
}

static bool rightCondition(const std::vector<Side> & sides)
{
    const auto count{sides.size() - 1};
    if (count <= 0)
        return false;

    // Calculate average length of all previous sides.
    auto lambda = [](size_t a, const Side & b) { return a + b.getDuration(); };
    size_t total = std::accumulate(sides.begin(), std::prev(sides.end()), 0, lambda);
    size_t average{total / count};

    size_t last{sides.back().getDuration()};
    if (last < ((average * 95) / 100))
         return true;

    return false;
}

int shuffleTracksAcrossSides(void)
{
    const auto showDebug{Configuration::isDebug()};

    // Read track list file.
    std::vector<Track> tracks = buildTrackListFromInputFile(Configuration::getInputFile());

    // Calculate total play time.
    size_t total{};
    for (const auto & track : tracks)
        total += track.getSeconds();
    if (showDebug)
        std::cout << "Total duration " << secondsToTimeString(total) << "\n";

    // Get user requested maximum side length.
    const size_t duration{Configuration::getDuration()};
    if (showDebug)
        std::cout << "Required duration " << secondsToTimeString(duration) << "\n";

    // Calculate 'packed' sides -> minimum sides needed.
    std::vector<Side> sides{};
    sides = addTracksToSides(tracks, duration);

    // Calculate number of sides required.
    size_t optimum{sides.size()};
    if ((optimum % 2) && (Configuration::isEven()))
        optimum++;
    if (showDebug)
        std::cout << "Optimum number of sides " << optimum << "\n";

    // Calculate minimum side length.
    size_t length{total/optimum};
    if (showDebug)
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";

    // Home in on optimum side length.
    size_t lim{15};
    size_t minimum{length};
    size_t maximum{duration};
    while (minimum <= maximum)
    {
        size_t median{(minimum + maximum + 1) / 2};
        if (showDebug)
            std::cout << "\nSuggested length " << secondsToTimeString(median) << "\n";

        sides.clear();
        sides = addTracksToSides(tracks, median);

        if (showDebug)
        {
            std::cout << "Suggested sides\n";
            for (const auto & side : sides)
                std::cout << side.getTitle() << " - " << side.size() << " tracks " << secondsToTimeString(side.getDuration()) << "\n";
        }

        if ((median == minimum) || (median == maximum))
        {
            break;
        }
        else
        if (leftCondition(optimum, sides.size()))
        {
            minimum = median;
            if (showDebug)
            {
                std::cout << "Minimum set to " << secondsToTimeString(minimum) << "\n";
                std::cout << "Maximum is " << secondsToTimeString(maximum) << "\n";
            }
        }
        else
        if (rightCondition(sides))
        {
            maximum = median;
            if (showDebug)
            {
                std::cout << "Minimum is " << secondsToTimeString(minimum) << "\n";
                std::cout << "Maximum set to " << secondsToTimeString(maximum) << "\n";
            }
        }
        else
        {
            break;
        }

        if (--lim == 0)
        {
            std::cerr << "Abort!!!\n";
            break;
        }
    }

    std::cout << "\nThe recommended sides are\n";
    for (const auto & side : sides)
        std::cout << side.toString() << "\n";

    return 0;
}


