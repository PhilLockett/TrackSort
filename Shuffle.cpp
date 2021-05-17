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
#include <iterator>
#include <vector>
#include <numeric>

#include "Shuffle.h"
#include "Side.h"
#include "Utilities.h"
#include "Configuration.h"

#if 0
static std::vector<Side> addTracksToSides(const std::vector<Track> & tracks, size_t duration, size_t count)
{
    // std::cout << "Add tracks to sides\n";
    std::vector<Side> sides;
    sides.reserve(count);
    Side side{};
    for (int i = 0; i < count; ++i)
    {
        const std::string title{"Side " + std::to_string(i+1)};
        side.setTitle(title);
        sides.push_back(side);
    }

    bool forward{true};
    int direction{1};
    std::vector<Side>::iterator forth{sides.begin()};
    std::vector<Side>::reverse_iterator back{sides.rbegin()};
    std::vector<Track>::const_iterator track{tracks.begin()};
    for (int attempts = 0; attempts < count; ++attempts)
    {
        if (forward)
        {
            (*forth).push(*track);
            if ((*forth).getDuration() > duration)
            {
                (*forth).pop();
                continue;
            }

            std::advance(track, 1);
            if (track == tracks.end())
                break;

            std::advance(forth, 1);
            attempts = 0;

            if (forth == sides.end())
            {
                forward = false;
                forth = sides.begin();
            }
        }
        else
        {
            (*back).push(*track);
            if ((*back).getDuration() > duration)
            {
                (*back).pop();
                continue;
            }

            std::advance(track, 1);
            if (track == tracks.end())
                break;

            std::advance(back, 1);
            attempts = 0;

            if (back == sides.rend())
            {
                forward = true;
                back = sides.rbegin();
            }
        }
    }
    // if (side.size() != 0)
    // {
    //     const auto count{sides.size() + 1};
    //     const std::string title{"Side " + std::to_string(count)};
    //     side.setTitle(title);
    //     sides.push_back(side);
    //     side.clear();
    // }

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
#endif

struct Used
{
    const Track * tp{};
    bool used{};
};
static std::vector<Side> addTracksToSides(const std::vector<Track> & tracks, size_t duration, size_t count)
{
    const auto trackCount{tracks.size()};
    std::unique_ptr<Used[]> tracker = std::make_unique<Used[]>(trackCount);
    auto it = tracks.begin();
    for (int i = 0; i < trackCount; ++i, ++it)
    {
        tracker[i].tp = &(*it);
    }

    // std::cout << "Add tracks to sides\n";
    std::vector<Side> sides;
    sides.reserve(count);
    Side side{};
    for (int i = 0; i < count; ++i)
    {
        const std::string title{"Side " + std::to_string(i+1)};
        side.setTitle(title);
        sides.push_back(side);
    }

    bool forward{true};
    int direction{1};
    std::vector<Side>::iterator forth{sides.begin()};
    std::vector<Side>::reverse_iterator back{sides.rbegin()};
    // std::vector<Track>::const_iterator track{tracks.begin()};
    int index{};
    for (int attempts = 0; attempts < count; ++attempts)
    {
        auto tp{tracker[index].tp};
        if (forward)
        {
            if ((*forth).getDuration() + tp->getSeconds() > duration)
            {
                continue;
            }

            (*forth).push(*tp);
            if (++index >= trackCount)
                break;

            std::advance(forth, 1);
            attempts = 0;

            if (forth == sides.end())
            {
                forward = false;
                forth = sides.begin();
            }
        }
        else
        {
            if ((*back).getDuration() + tp->getSeconds() > duration)
            {
                continue;
            }

            (*back).push(*tp);
            if (++index >= trackCount)
                break;

            std::advance(back, 1);
            attempts = 0;

            if (back == sides.rend())
            {
                forward = true;
                back = sides.rbegin();
            }
        }
    }
    // if (side.size() != 0)
    // {
    //     const auto count{sides.size() + 1};
    //     const std::string title{"Side " + std::to_string(count)};
    //     side.setTitle(title);
    //     sides.push_back(side);
    //     side.clear();
    // }

    return sides;
}

int shuffleTracksAcrossSides(void)
{
    const auto showDebug{Configuration::isDebug()};

    // Read track list file and sort it, longest to shortest.
    std::vector<Track> tracks = buildTrackListFromInputFile(Configuration::getInputFile());
    std::sort(tracks.begin(), tracks.end(), [](const Track & a, const Track & b) {
        return a.getSeconds() > b.getSeconds();
    });

    // for (const auto & track : tracks)
    //     std::cout << track.toString() << "\n";

    // Calculate total play time.
    auto lambda = [](size_t a, const Track & b) { return a + b.getSeconds(); };
    size_t total = std::accumulate(tracks.begin(), tracks.end(), 0, lambda);
    if (showDebug)
        std::cout << "Total duration " << secondsToTimeString(total) << "\n";

    // Get user requested maximum side length.
    const size_t duration{Configuration::getDuration()};
    if (showDebug)
        std::cout << "Required duration " << secondsToTimeString(duration) << "\n";

    // Calculate 'packed' sides -> minimum sides needed.
    std::vector<Side> sides{};

    // Calculate number of sides required.
    size_t optimum{total / duration};
    if (total % duration)
        optimum++;
    if ((optimum & 1) && (Configuration::isEven()))
        optimum++;
    std::cout << "Optimum number of sides " << optimum << "\n";

    sides = addTracksToSides(tracks, duration, optimum);
    if (showDebug)
    {
        std::cout << "Packed sides\n";
        for (const auto & side : sides)
            std::cout << side.getTitle() << " - " << side.size() << " tracks " << secondsToTimeString(side.getDuration()) << "\n";
    }

    // Calculate minimum side length.
    size_t length{total/optimum};
    if (showDebug)
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";
#if 0
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
#endif
    std::cout << "\nThe recommended sides are\n";
    for (const auto & side : sides)
        std::cout << side.toString() << "\n";

    return 0;
}


