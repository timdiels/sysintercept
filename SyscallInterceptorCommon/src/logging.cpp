/*
 * Copyright 2012 Tim Diels
 *
 * This file is part of sysintercept.
 *
 * sysintercept is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sysintercept is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sysintercept.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "logging.h"

#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

using namespace std;

src::wseverity_logger<> _lg;

// Note: libs/log/examples are especially useful to figure out what to include, etc.
void init_logging(string log_file_name /*, verbosity*/) {
	// TODO: format log messages as: timestamp, severity, message
	// TODO: send everything to log file, additionally send >= verbosity to wcerr
	// TODO prefix program name to wcerr
	// TODO: if verbosity is set to 'none', set_logging_enabled to false, it gives better performance that way
	// TODO: if verbosity more verbose than ... set auto_flush to true, false otherwise

	boost::shared_ptr<logging::wcore> core = logging::wcore::get();

	// console sink
	{
		boost::shared_ptr<sinks::wtext_ostream_backend> backend = boost::make_shared<sinks::wtext_ostream_backend>();
		backend->add_stream(boost::shared_ptr<std::wostream>(&std::wclog, logging::empty_deleter()));
		backend->auto_flush(true);

		typedef sinks::synchronous_sink<sinks::wtext_ostream_backend> sink_s;
		boost::shared_ptr<sink_s> sink(new sink_s(backend));
		core->add_sink(sink);
	}

	// log file sink
	{
		boost::shared_ptr<sinks::wtext_file_backend> backend = boost::make_shared<sinks::wtext_file_backend>(
				keywords::file_name = log_file_name
		);
		backend->auto_flush(false);

		typedef sinks::synchronous_sink<sinks::wtext_file_backend> sink_f;
		boost::shared_ptr<sink_f> sink(new sink_f(backend));
		sink->set_filter(
			flt::attr<int>(L"Severity") >= error
		);
		core->add_sink(sink);
	}
}




