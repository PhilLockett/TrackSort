/**
 * @file    TextFile.h
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
 * Template for basic text file read/write handling.
 */

#if !defined(_TEXTFILE_H__20210503_1300__INCLUDED_)
#define _TEXTFILE_H__20210503_1300__INCLUDED_

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>


/**
 * @section text file read/write handling interface.
 *
 */

template<typename T=std::string>
class TextFile
{
public:
    using Iterator = std::vector<T>::const_iterator;

    TextFile(const std::string & file) : fileName{file} {}
    virtual ~TextFile(void) {}

    TextFile(const TextFile & other) : fileName{other.fileName}, data{other.data} {}
    void operator=(const TextFile & other) { fileName = other.fileName; data = other.data; }

    friend std::ostream & operator<<(std::ostream &os, const TextFile &A) { A.display(os); return os; }

    void load(const std::vector<T> & other) { data = other; }
    bool equal(const TextFile & other) const { return std::equal(data.begin(), data.end(), other.data.begin()); }
    void clear(void) { data.clear(); }

    void setFileName(const std::string & file) { fileName = file; }
    std::string getFileName(void) const { return fileName; }
    bool exists(void) const { return std::filesystem::exists(fileName); }

    void reserve(size_t size) { data.reserve(size); }
    size_t size(void) { return data.size(); }
    Iterator begin(void) { return data.begin(); }
    Iterator end(void) { return data.end(); }

    int write(const std::vector<T> & other) { load(other); return write(); }
    int write(void) const;
    int read(int reserve = 100);

private:
    void display(std::ostream &os) const;

    std::filesystem::path fileName; 
    std::vector<T> data;
};


/**
 * @section text file read/write handling implementation.
 *
 */

/**
 * Write the buffer to the named file.
 *
 * @return error value or 0 if no errors.
 */
template<typename T>
int TextFile<T>::write(void) const
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
template<typename T>
int TextFile<T>::read(int res)
{
    if (std::ifstream is{fileName, std::ios::in})
    {
        reserve(res);
        T line;

        while (getline(is, line))
        {
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
template<typename T>
void TextFile<T>::display(std::ostream &os) const
{
    for (const auto & line: data)
        os << line << "\n";
}

#endif // !defined(_TEXTFILE_H__20210503_1300__INCLUDED_)
