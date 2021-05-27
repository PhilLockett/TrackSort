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
 * sides.
 *
 * Implementation of the track splitter command line configuration Singleton.
 */

#include <getopt.h>
#include <future>
#include <iostream>

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
    std::cout << "\t-d --duration <seconds>\tMaximum length of each side.\n";
    std::cout << "\t-t --timeout <seconds> \tThe maximum time to spend looking.\n";
    std::cout << "\t-e --even\t\tRequire an even number of sides.\n";
    std::cout << "\t-s --shuffle\t\tRe-order tracks for optimal fit.\n";
    std::cout << "\t-p --plain\t\tDisplay lengths in seconds instead of hh:mm:ss.\n";
    std::cout << "\t-c --csv\t\tGenerate output as comma separated variables.\n";

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
            {"timeout",  required_argument,0,'t'},
            {"even",    no_argument,0,'e'},
            {"shuffle", no_argument,0,'s'},
            {"plain",    no_argument,0,'p'},
            {"csv", no_argument,0,'c'},
            {"debug",   no_argument,0,'x'},
            {0,0,0,0}
        };

        optchr = getopt_long(argc, argv ,"hi:d:t:espcx", long_options, &option_index);
        if (optchr == -1)
            return 0;

        switch (optchr)
        {
            case 'h': return help(argv[0]);
            case 'i': setInputFile(std::string(optarg)); break;
            case 'd': setDuration(std::string(optarg)); break;
            case 't': setTimeout(std::string(optarg)); break;
            case 'e': enableEven(); break;
            case 's': enableShuffle(); break;
            case 'p': enablePlain(); break;
            case 'c': enableCSV(); break;
            case 'x': enableDebug(); break;

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

/**
 * @brief Display the configuration defined by the command line parameters.
 * 
 * @param os output stream on which to display.
 */
void Configuration::display(std::ostream &os) const
{
    os << "Config is " << std::string{isValid() ? "" : "NOT "} << "valid\n";
    os << "Input file name:  " << getInputFile() << '\n';
    os << "Disc duration: " << getDuration() << "s\n";
    os << "Timeout: " << getTimeout() << "s\n";
    if (isEven())
        os << "An even number of sides requested.\n";
    if (isShuffle())
        os << "Optimal reordering of tracks requested.\n";
    if (isPlain())
        os << "Display lengths in seconds instead of hh:mm:ss.\n";
    if (isCSV())
        os << "Comma separated variable output requested.\n";
}

/**
 * @brief check the validity of the configuration defined by the command line
 * parameters.
 * 
 * @param showErrors if true, send error messages to standard error. 
 * @return true if the configuration is valid.
 * @return false otherwise.
 */
bool Configuration::isValid(bool showErrors)
{
    namespace fs = std::filesystem;

    const auto & inputFile{getInputFile()};

    if (inputFile.string().empty())
    {
        if (showErrors)
            std::cerr << "\nInput file must be specified.\n";

        return false;
    }

    if (!fs::exists(inputFile))
    {
        if (showErrors)
            std::cerr << "\nInput file " << inputFile << " does not exist.\n";

        return false;
    }

    if (getDuration() == 0)
    {
        if (showErrors)
            std::cerr << "\nDisc length must be specified\n";
        
        return false;
    }

    return true;
}

