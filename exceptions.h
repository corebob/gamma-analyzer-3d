//  gamma-analyzer-3d - 3d visualization of sessions generated by gamma-analyzer
//  Copyright (C) 2017  Dag Robole
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <QString>

struct Exception : public std::runtime_error
{
    explicit Exception(QString arg) noexcept
        : std::runtime_error(arg.toStdString()) {}
};

struct Exception_IndexOutOfBounds : public Exception
{
    explicit Exception_IndexOutOfBounds(QString source) noexcept
        : Exception("Index out of bounds: " + source) {}
};

struct Exception_NumericRangeError : public Exception
{
    explicit Exception_NumericRangeError(QString source) noexcept
        : Exception("Numeric range error: " + source) {}
};

struct Exception_InvalidPointer : public Exception
{
    explicit Exception_InvalidPointer(QString source) noexcept
        : Exception("Invalid pointer: " + source) {}
};

struct Exception_DirDoesNotExist : public Exception
{
    explicit Exception_DirDoesNotExist(QString dir) noexcept
        : Exception("Directory does not exist: " + dir) {}
};

struct Exception_FileDoesNotExist : public Exception
{
    explicit Exception_FileDoesNotExist(QString filename) noexcept
        : Exception("File does not exist: " + filename) {}
};

struct Exception_UnableToLoadFile : public Exception
{
    explicit Exception_UnableToLoadFile(QString filename) noexcept
        : Exception("Unable to load file: " + filename) {}
};

struct Exception_MissingJsonValue : public Exception
{
    explicit Exception_MissingJsonValue(QString source) noexcept
        : Exception("Missing JSON value: " + source) {}
};

#endif // EXCEPTIONS_H
