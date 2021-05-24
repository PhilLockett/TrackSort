/**
 * @file    Configuration.h
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

#if !defined _CONFIGURATION_H_INCLUDED_
#define _CONFIGURATION_H_INCLUDED_

#include <string>
#include <filesystem>

#include "Utilities.h"

/**
 * @section track splitter configuration data.
 *
 * Implementation of the track splitter comand line configuration Singleton.
 */

class Configuration
{
private:
//- Hide the default constructor and destructor.
    Configuration(void) : inputFile{}, seconds{}, timeout{60}, even{}, shuffle{}, debug{} {  }
    virtual ~Configuration(void) {}

    void display(std::ostream &os) const;

    std::filesystem::path inputFile;
    size_t seconds;
    size_t timeout;
    bool even;
    bool shuffle;
    bool debug;

    void setInputFile(std::string name) { inputFile = name; }
    void setDuration(std::string time) { seconds = timeStringToSeconds(time); }
    void setTimeout(std::string time) { timeout = timeStringToSeconds(time); }
    void enableEven() { even = true; }
    void enableShuffle() { shuffle = true; }
    void enableDebug(void) { debug = true; }

    int help(const char * const name);
    int parseCommandLine(int argc, char *argv[]);

public:
//- Delete the copy constructor and assignement operator.
    Configuration(const Configuration &) = delete;
    void operator=(const Configuration &) = delete;

    int initialise(int argc, char *argv[]);
    friend std::ostream & operator<<(std::ostream &os, const Configuration &A) { A.display(os); return os; }

    static Configuration & instance() { static Configuration neo; return neo; }

    static std::filesystem::path & getInputFile(void) { return Configuration::instance().inputFile; }
    static size_t getDuration(void) { return Configuration::instance().seconds; }
    static size_t getTimeout(void) { return Configuration::instance().timeout; }
    static bool isEven(void) { return Configuration::instance().even; }
    static bool isShuffle(void) { return Configuration::instance().shuffle; }
    static bool isDebug(void) { return Configuration::instance().debug; }

    static bool isValid(bool showErrors = false);

};

#endif //!defined _CONFIGURATION_H_INCLUDED_
