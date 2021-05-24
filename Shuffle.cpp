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
#include <cmath>
#include <limits>
#include <thread>
#include <chrono>

#include "Shuffle.h"
#include "Side.h"
#include "Utilities.h"
#include "Configuration.h"


/**
 * @section Define Indexer class.
 *
 */

template<typename T=int>
class Indexer
{
public:
    Indexer(T first, T limit);
    T operator()(void) const { return index; }
    T inc();

private:
    T step, index, start, end;
};

template<typename T>
Indexer<T>::Indexer(T first, T limit) :
    step{1}, index{(first / 2) % limit}, start{0}, end{limit-1}
{
    if (first & 1)
    {
        step = -1;
        index = limit - 1 - index;
        std::swap(start, end);
    }
}

template<typename T>
T Indexer<T>::inc()
{
    if (index == end)
        index = start;
    else
        index += step;

    return index;
}


/**
 * @section Define SideRef class.
 *
 */

class SideRef
{
public:
    SideRef(const std::vector<Track> & trackList) : title{}, seconds{}, tracks{trackList} {}

    void setTitle(const std::string & t) { title = t; }
    void push(size_t);
    void pop();
    std::string toString() const;

    std::string getTitle() const { return title; }
    size_t getDuration() const { return seconds; }

    size_t size(void) const { return trackRefs.size(); }
    void clear() { seconds = 0; trackRefs.clear(); }

    std::vector<size_t> getRefs() const { return trackRefs; }

private:
    std::string title;
    size_t seconds;
    const std::vector<Track> & tracks;
    std::vector<size_t> trackRefs;

};

void SideRef::push(size_t track)
{
    trackRefs.push_back(track);
    seconds += tracks[track].getSeconds();
}

void SideRef::pop()
{
    seconds -= tracks[trackRefs.back()].getSeconds();
    trackRefs.pop_back();
}

std::string SideRef::toString() const
{
    std::string s{title + " - " + std::to_string(size()) + " tracks\n"};
    
    for (const auto track : trackRefs)
        s += secondsToTimeString(tracks[track].getSeconds()) + " - " + tracks[track].getTitle() + '\n';

    s += secondsToTimeString(seconds) + '\n';

    return s;
}


/**
 * @section Define Finder class.
 *
 */

class Finder
{
public:
    using Iterator = std::vector<SideRef>::const_iterator;

    Finder(const std::vector<Track> &, const size_t, const size_t);

    bool addTracksToSides(void);
    bool isSuccessful(void) const { return success; }
    bool show(std::ostream & os) const;
    bool showAll(std::ostream & os) const;

    size_t size(void) const { return sides.size(); }
    Iterator begin(void) { return sides.begin(); }
    Iterator end(void) { return sides.end(); }

private:
    bool look(int track);
    bool snapshot(double latest);
    static void waiter(void);

    const size_t duration;
    const size_t sideCount;
    const size_t trackCount;

    bool forward;
    int trackIndex;
    int sideIndex;
    bool success;
    size_t lim;

    const std::vector<Track> & tracks;
    std::vector<SideRef> sides;

    static size_t working;
    double dev;
    std::vector<std::vector<size_t>> best;
};

Finder::Finder(const std::vector<Track> & trackList, const size_t dur, const size_t count) :
    duration{dur}, sideCount{count}, trackCount{trackList.size()},
    forward{true}, trackIndex{}, sideIndex{}, success{}, lim{150},  tracks{trackList}, sides{},
    dev{std::numeric_limits<double>::max()}, best{}
{
    sides.reserve(sideCount);
    best.reserve(sideCount);
    SideRef side{trackList};
    for (int i = 0; i < sideCount; ++i)
    {
        const std::string title{"Side " + std::to_string(i+1)};
        side.setTitle(title);
        sides.push_back(side);
    }
}
size_t Finder::working;


bool Finder::snapshot(double latest)
{
    dev = latest;
    best.clear();
    for (const auto & side : sides)
        best.push_back(side.getRefs());

    return true;
}

bool Finder::look(int track)
{
    if ((!working) || (lim == 0) || (dev < 30.0))
        return true;

    if (track == trackCount)
    {
        // show(std::cout);
        const auto latest{deviation<SideRef>(sides)};
        if (latest < dev)
        {
            // std::cout << latest << "\n";
            snapshot(latest);

            --lim;

    // for (const auto & side : sides)
    //     std::cout << side.getTitle() << " - " << side.size() << " tracks " <<
    //         secondsToTimeString(side.getDuration()) << "\n";
        }

        return true;
    }

    Indexer side{track, (int)sideCount};

    for (int i = 0; i < sideCount; ++i, side.inc())
    // for (int side = 0; side < sideCount; ++side)
    {
        // std::cout << "track " << track << "  side " << side << "\n";

        auto & tp{tracks[track]};
        auto & sp{sides[side()]};
        if (sp.getDuration() + tp.getSeconds() <= duration)
        {
            sp.push(track);

            look(track+1);
            // if (look(track+1))
            //     return true;

            sp.pop();
        }
    }

    return false;
}

void Finder::waiter()
{
    using namespace std::literals::chrono_literals;

    while (working)
    {
        std::this_thread::sleep_for(1s);
        --working;
    }
}
bool Finder::addTracksToSides(void)
{
    working = 600;
    std::thread worker(waiter);

    success = look(0);
    success = true;
    working = 1;

    worker.join();

    return success;
}

bool Finder::show(std::ostream & os) const
{
    os << "deviation " << dev << "\n";
    for (const auto & side : best)
    {
        size_t total{};
        for (const auto & track : side)
            total += tracks[track].getSeconds();
        os << " - " << side.size() << " tracks " << secondsToTimeString(total) << "\n";
    }

    return success;
}

bool Finder::showAll(std::ostream & os) const
{
    for (const auto & side : best)
    {
        os << " - " << side.size() << " tracks " << "\n";
        for (const auto & track : side)
            os << secondsToTimeString(tracks[track].getSeconds()) << " - " << tracks[track].getTitle() << "\n";
        os << "\n";
    }

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

    // Calculate total play time.
    auto lambda = [](size_t a, const Track & b) { return a + b.getSeconds(); };
    size_t total = std::accumulate(tracks.begin(), tracks.end(), 0, lambda);
    if (showDebug)
        std::cout << "Total duration " << secondsToTimeString(total) << "\n";

    // Get user requested maximum side length.
    const size_t duration{Configuration::getDuration()};
    if (showDebug)
        std::cout << "Required duration " << secondsToTimeString(duration) << "\n";

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
        find.show(std::cout);
    }


    // Calculate minimum side length.
    size_t length{total/optimum};
    if (showDebug)
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";

    if (find.isSuccessful())
    {
        std::cout << "\nThe recommended sides are\n";
        find.showAll(std::cout);
    }

    return 0;
}


