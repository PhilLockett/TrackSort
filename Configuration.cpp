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
 * Implementation of the track splitter command line configuration Singleton.
 */

#include <getopt.h>
#include <future>

#include "Configuration.h"


/**
 * @section Command line parameter initialisation.
 */

/**
 * @brief Display help message.
 * 
 * @param name of application.
 * @return auto indicate processing should be aborted.
 */
int Configuration::help(const char * const name)
{
    std::cout << "Usage: " << name << " [Options]\n";
    std::cout << '\n';
    std::cout << "  Splits a list of tracks across multiple discs of a given length.\n";
    std::cout << '\n';
    std::cout << "  Options:\n";
    std::cout << "\t-h --help \t\tThis help page and nothing else.\n";
    std::cout << "\t-i --input <file> \tInput file name containing the track listing.\n";
    std::cout << "\t-d --duration <seconds>\t\tMaximum length of each disc.\n";
    std::cout << "\t-e --even\t\tRequire an even number of discs (sides).\n";

    return 1;
}


/**
 * @brief Process command line parameters with help from getopt_long() and
 * update global variables.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int Configuration::parseCommandLine(int argc, char *argv[])
{
    if (argc < 2)
    {
        help(argv[0]);

        return -1;
    }

    while (1)
    {
        int option_index = 0;
        int optchr;

        static struct option long_options[] = {
            {"help",    no_argument,0,'h'},
            {"input",   required_argument,0,'i'},
            {"duration",  required_argument,0,'d'},
            {"even",    no_argument,0,'e'},
            {0,0,0,0}
        };

        optchr = getopt_long(argc, argv ,"hi:d:e", long_options, &option_index);
        if (optchr == -1)
            return 0;

        switch (optchr)
        {
            case 'h': return help(argv[0]);

            case 'i': setInputFile(std::string(optarg)); break;

            case 'd': setDuration(std::string(optarg)); break;

            case 'e': setEven();    break;

            default:
                help(argv[0]);

                return -1;
        }//end switch
    }//end while

    return 0;
}


/**
 * @brief Initialise 'TrackSort' using command line input and ensure we only
 * do it once.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int Configuration::initialise(int argc, char *argv[])
{
    static std::once_flag virgin;
    int ret = 0;

    std::call_once(virgin, [&](){ ret = parseCommandLine(argc, argv); });

    return ret;
}


/**
 * @section track splitter configuration data.
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

