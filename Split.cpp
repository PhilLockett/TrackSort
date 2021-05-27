/**
 * @file    Split.cpp
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
 * Basic code for the track splitter.
 */

#include <iostream>
#include <string>
#include <vector>

#include "Side.h"
#include "Utilities.h"
#include "Configuration.h"

/**
 * @brief Splits a list of tracks across multiple sides using the upper side
 * length limit of 'duration'.
 * 
 * @param tracks to split across sides.
 * @param duration limit of a side.
 * @return std::vector<Side> list of sides containing the tracks.
 */
static std::vector<Side> addTracksToSides(const std::vector<Track> & tracks, size_t duration)
{
    // std::cout << "Add tracks to sides\n";
    std::vector<Side> sides;
    Side side{};
    for (const auto & track : tracks)
    {
        if (side.getValue() + track.getValue() <= duration)
        {
            side.push(track);
        }
        else
        {
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

/**
 * @brief Determine if the minimum side length is too short by checking if the
 * current number of sides exceeds the required number of sides.
 * 
 * @param required number of sides.
 * @param current number of sides.
 * @return true if the current number of sides exceeds the required.
 * @return false otherwise.
 */
static bool isMinimumTooShort(size_t required, size_t current)
{
    if (required < current)
        return true;

    return false;
}

/**
 * @brief Determine if the maximum side length is too long by checking that the
 * length of the last side is close to the average length of all previous
 * sides. If it isn't, then the previous sides are too greedy.
 * 
 * @param sides currently being considered.
 * @return true if last side doesn't hold enough tracks.
 * @return false otherwise.
 */
static bool isMaximumTooLong(const std::vector<Side> & sides)
{
    const auto count{sides.size() - 1};
    if (count <= 0)
        return false;

    // Calculate the standard deviation of all side lengths.
    if (deviation(sides) > 10.0)
         return true;

    return false;
}

/**
 * @brief Reads the track list from the user specified file and optimally
 * splits them across multiple sides so that the sides have similar lengths.
 * 
 * @return int error value of 0.
 */
int splitTracksAcrossSides(void)
{
    const auto showDebug{Configuration::isDebug()};

    // Read track list file.
    std::vector<Track> tracks = buildTrackListFromInputFile(Configuration::getInputFile());

    // Calculate total play time.
    auto lambda = [](size_t a, const Track & b) { return a + b.getValue(); };
    size_t total = std::accumulate(tracks.begin(), tracks.end(), 0, lambda);

    // Get user requested maximum side length.
    const size_t duration{Configuration::getDuration()};

    // Get (user requested) timeout.
    const size_t timeout{Configuration::getTimeout()};

    // Calculate 'packed' sides -> minimum sides needed.
    std::vector<Side> sides{};
    sides = addTracksToSides(tracks, duration);

    // Calculate number of sides required.
    size_t optimum{sides.size()};
    if ((optimum & 1) && (Configuration::isEven()))
        optimum++;

    // Calculate minimum side length.
    size_t length{total/optimum};

    if (showDebug)
    {
        std::cout << "Total duration " << secondsToTimeString(total) << "\n";
        std::cout << "Required duration " << secondsToTimeString(duration) << "\n";
        std::cout << "Required timeout " << secondsToTimeString(timeout) << "\n";
        std::cout << "Optimum number of sides " << optimum << "\n";
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";
    }

    // Home in on optimum side length.
    Timer timer{timeout};
    size_t minimum{length};
    size_t maximum{duration};

    timer.start();
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
                std::cout << side.getTitle() << " - " << side.size() << " tracks " << secondsToTimeString(side.getValue()) << "\n";
        }

        if ((median == minimum) || (median == maximum))
        {
            break;
        }
        else
        if (isMinimumTooShort(optimum, sides.size()))
        {
            minimum = median;
            if (showDebug)
            {
                std::cout << "Minimum set to " << secondsToTimeString(minimum) << "\n";
                std::cout << "Maximum is " << secondsToTimeString(maximum) << "\n";
            }
        }
        else
        if (isMaximumTooLong(sides))
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

        if (!timer.isWorking())
        {
            if (showDebug)
                std::cout << "Abort!!!\n";
            break;
        }
    }
    timer.terminate();

    const auto csv{Configuration::isCSV()};

    if (csv)
    {
        for (const auto & side : sides)
            std::cout << side.toString(Configuration::isPlain(), true);
    }
    else
    {
        std::cout << "\nThe recommended sides are\n";
        for (const auto & side : sides)
            std::cout << side.toString(Configuration::isPlain(), false) << "\n";
    }

    return 0;
}
