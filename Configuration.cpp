/**
 * @file    Configuration.cpp
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
 * Common interfaces for the track splitter.
 */

#include "Configuration.h"


/**
 * @section tfc configuration data.
 *
 * Implementation of the tfc comand line configuration Singleton.
 */

void Configuration::display(std::ostream &os) const
{
    os << "Config is " << std::string{Configuration::isValid() ? "" : "NOT "} << "valid\n";
    os << "Input file name:  " << Configuration::getInputFile() << '\n';
    os << "Disc duration: " << Configuration::getDuration() << "s\n";
    if (Configuration::isEven())
        os << "An even number of side requested.\n";
}

bool Configuration::isValid(bool showErrors)
{
    namespace fs = std::filesystem;

    const auto & inputFile{Configuration::getInputFile()};

    if (inputFile.string().empty())
    {
        if (showErrors)
            std::cerr << "\nInput (or replacement) file must be specified.\n";

        return false;
    }

    if (!fs::exists(inputFile))
    {
        if (showErrors)
        {
            std::cerr << "\nInput file " << inputFile << " does not exist.\n";
        }

        return false;
    }

    if (Configuration::getDuration() == 0)
    {
        if (showErrors)
        {
            std::cerr << "\nDisc length must be specified (in seconds)\n";
        }
        
        return false;
    }

    return true;
}

