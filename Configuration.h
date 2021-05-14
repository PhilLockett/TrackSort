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

#include <iostream>
#include <string>
#include <filesystem>

#if !defined ELEMENTS
#define ELEMENTS(A) (sizeof(A)/sizeof(A[0]))
#endif


/**
 * @section tfc configuration data.
 *
 * Implementation of the tfc comand line configuration Singleton.
 */

class Configuration
{
private:
//- Hide the default constructor and destructor.
    Configuration(void) : inputFile{}, seconds{}, even{} {  }
    virtual ~Configuration(void) {}

    void display(std::ostream &os) const;

    std::filesystem::path inputFile;
    size_t seconds;
    bool even;

    void setInputFile(std::string name) { Configuration::instance().inputFile = name; }
    void setDuration(std::string time) { Configuration::instance().seconds = std::stoi(time); }
    void setEven() { Configuration::instance().even = true; }

public:
//- Delete the copy constructor and assignement operator.
    Configuration(const Configuration &) = delete;
    void operator=(const Configuration &) = delete;

    friend int parseCommandLine(int argc, char *argv[], Configuration &config);
    friend std::ostream & operator<<(std::ostream &os, const Configuration &A) { A.display(os); return os; }

    static Configuration & instance() { static Configuration neo; return neo; }

    static std::filesystem::path & getInputFile(void) { return Configuration::instance().inputFile; }
    static size_t getDuration(void) { return Configuration::instance().seconds; }
    static bool isEven(void) { return Configuration::instance().even; }

    static bool isValid(bool showErrors = false);

};

#endif //!defined _CONFIGURATION_H_INCLUDED_