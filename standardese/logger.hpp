// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_LOGGER_HPP_INCLUDED
#define STANDARDESE_LOGGER_HPP_INCLUDED

#include <string>

#include "forward.hpp"

namespace spdlog { class logger; }

namespace standardese::logger {

// TODO: Explain why this pattern is necessary, i.e., spdlog is increasing compile times by a lot.

/// Return a logger for all standardese messages.
/// Returns the `"standardese"` logger registered with spdlog and returns it.
/// If none has been registered yet, registers one.
spdlog::logger& get();

// TODO: These should all optionally take a callback producing a string so we
// can check whether we should actually print (only really relevant for debug &
// trace.)

/// Write a critical error to the log.
/// This is identical to `logger::get().critical(...)` but allows to hide away
/// the spdlog headers which increase compilation times a lot.
void critical(const std::string&);

/// Write an error to the log.
void error(const std::string&);

/// Write an warning to the log.
void warn(const std::string&);

/// Write an informational message to the log.
void info(const std::string&);

/// Write an debug message to the log.
void debug(const std::string&);

/// Write an trace message to the log.
void trace(const std::string&);

/// Treat warnings as errors.
void warn_as_error();

/// Return the number of errors and critical errors reported so far.
int errors();

/// Return the number of warnings reported so far.
int warnings();

}

#endif // STANDARDESE_LOGGER_HPP_INCLUDED
