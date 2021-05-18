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
class Finder
{
public:
    using Iterator = std::vector<Side>::const_iterator;

    Finder(const std::vector<Track> &, const size_t, const size_t);

    bool addTracksToSides(void);

    bool isSuccessful(void) const { return success; }

    size_t size(void) const { return sides.size(); }
    Iterator begin(void) { return sides.begin(); }
    Iterator end(void) { return sides.end(); }

private:
    bool look(int track);

    const size_t duration;
    const size_t sideCount;
    const size_t trackCount;

    bool forward;
    int trackIndex;
    int sideIndex;
    bool success;

    std::unique_ptr<Used[]> tracker;
    std::vector<Side> sides;
};

Finder::Finder(const std::vector<Track> & tracks, const size_t dur, const size_t count) :
    duration{dur}, sideCount{count}, trackCount{tracks.size()},
    forward{true}, trackIndex{}, sideIndex{}, success{}
{
    tracker = std::make_unique<Used[]>(trackCount);
    auto it = tracks.begin();
    for (int i = 0; i < trackCount; ++i, ++it)
        tracker[i].init(*it);

    sides.reserve(sideCount);
    Side side{};
    for (int i = 0; i < sideCount; ++i)
    {
        const std::string title{"Side " + std::to_string(i+1)};
        side.setTitle(title);
        sides.push_back(side);
    }
}

bool Finder::look(int track)
{
    if (track == trackCount)
        return true;

    for (int side = 0; side < sideCount; ++side)
    {
        // std::cout << "track " << track << "  side " << side << "\n";

        auto tp{tracker[track].getTp()};
        auto sp{&sides[side]};
        if (sp->getDuration() + tp->getSeconds() <= duration)
        {
            sp->push(*tp);

            if (look(track+1))
                return true;

            sp->pop();
        }
    }

    return false;
}

bool Finder::addTracksToSides(void)
{
    success = look(0);
    success = true;
    return success;
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
    // std::vector<Side> sides{};

    // Calculate number of sides required.
    size_t optimum{total / duration};
    if (total % duration)
        optimum++;
    if ((optimum & 1) && (Configuration::isEven()))
        optimum++;
    std::cout << "Optimum number of sides " << optimum << "\n";



    Finder find{tracks, duration, optimum};
    find.addTracksToSides();
    if ((find.isSuccessful()) && (showDebug))
    {
        std::cout << "Packed sides\n";
        for (const auto & side : find)
            std::cout << side.getTitle() << " - " << side.size() << " tracks " << secondsToTimeString(side.getDuration()) << "\n";

    }


    // Calculate minimum side length.
    size_t length{total/optimum};
    if (showDebug)
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";

    std::cout << "\nThe recommended sides are\n";
    for (const auto & side : find)
        std::cout << side.toString() << "\n";

    return 0;
}


