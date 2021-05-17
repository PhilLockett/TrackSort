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


class Used
{
    const Track * tp{};
    bool used{};

public:
    void init(const Track & track) { tp = &track; }
    void setUsed(bool state = true) { used = state; }
    bool isUsed(void) const { return used; }
    size_t getSeconds(void) const { return tp->getSeconds(); }
    const Track * getTp(void) const { return tp; };

};
static std::vector<Side> addTracksToSides(const std::vector<Track> & tracks, const size_t duration, const size_t sideCount)
{
    const auto trackCount{tracks.size()};
    std::unique_ptr<Used[]> tracker = std::make_unique<Used[]>(trackCount);
    auto it = tracks.begin();
    for (int i = 0; i < trackCount; ++i, ++it)
    {
        tracker[i].init(*it);
    }

    // std::cout << "Add tracks to sides\n";
    std::vector<Side> sides;
    sides.reserve(sideCount);
    Side side{};
    for (int i = 0; i < sideCount; ++i)
    {
        const std::string title{"Side " + std::to_string(i+1)};
        // std::cout << "title " << title << "\n";
        side.setTitle(title);
        sides.push_back(side);
    }

    bool forward{true};
    int trackIndex{};
    int sideIndex{};
    for (int attempts = 1; attempts <= sideCount; ++attempts)
    {
        std::cout << "attempts " << attempts << "  trackIndex " << trackIndex << "  sideIndex " << sideIndex << "  forward " << forward << "\n";
        auto tp{tracker[trackIndex].getTp()};
        auto sp{&sides[sideIndex]};
        if (sp->getDuration() + tp->getSeconds() <= duration)
        {
            sp->push(*tp);
            if (++trackIndex >= trackCount)
                break;

            attempts = 0;
        }

        if (forward)
        {
            if (sideIndex == sideCount-1)
                forward = false;
            else
                ++sideIndex;
        }
        else
        {
            if (sideIndex == 0)
                forward = true;
            else
                --sideIndex;
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

    std::cout << "\nThe recommended sides are\n";
    for (const auto & side : sides)
        std::cout << side.toString() << "\n";

    return 0;
}


