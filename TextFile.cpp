/**
 * @file    TextFile.cpp
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
 * Basic text file read/write handling.
 */

#include "TextFile.h"


/**
 * @section text file read/write handling implementation.
 *
 */

/**
 * Write the buffer to the named file.
 *
 * @return error value or 0 if no errors.
 */
int TextFile::write(void) const
{
    if (std::ofstream os{fileName, std::ios::out})
    {
        for (auto & line : data)
            os << line << '\n';

        return 0;
    }

    return 1;
}

/**
 * Read the named file into the buffer.
 *
 * @param  res - reserve the number of lines in the buffer.
 * @return error value or 0 if no errors.
 */
int TextFile::read(int res)
{
    if (std::ifstream is{fileName, std::ios::in})
    {
        reserve(res);
        std::string line;

        while (getline(is, line))
        {
            const auto pos{line.find_first_of("\r\n")};
            if (pos != std::string::npos)
                line = line.substr(0, pos);
            if (!is.eof() && line.length())
                data.push_back(std::move(line));
        }

        return 0;
    }

    return 1;
}


/**
 * Send the current lines to the output stream.
 *
 * @param  os - Output stream.
 */
void TextFile::display(std::ostream &os) const
{
    for (const auto & line: data)
        os << line << "\n";
}

