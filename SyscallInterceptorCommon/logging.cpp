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
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/expressions.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/locale.hpp>
#include <locale>

namespace logging = boost::log;
namespace expr = boost::log::expressions;
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

	boost::shared_ptr<logging::core> core = logging::core::get();

	locale loc = boost::locale::generator()("en_US.UTF-8");

	// console sink
	{
		boost::shared_ptr<sinks::wtext_ostream_backend> backend = boost::make_shared<sinks::wtext_ostream_backend>();
		backend->add_stream(boost::shared_ptr<std::wostream>(&std::wclog, boost::empty_deleter()));
		backend->auto_flush(true);

		typedef sinks::synchronous_sink<sinks::wtext_ostream_backend> sink_s;
		boost::shared_ptr<sink_s> sink(new sink_s(backend));
		sink->imbue(loc);
		core->add_sink(sink);
	}

	// log file sink
	{
		boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared<sinks::text_file_backend>(
				keywords::file_name = log_file_name
		);
		backend->auto_flush(false);

		typedef sinks::synchronous_sink<sinks::text_file_backend> sink_f;
		boost::shared_ptr<sink_f> sink(new sink_f(backend));
		sink->imbue(loc);
		sink->set_filter(
			expr::is_in_range<int>("Severity", error, fatal)
		);
		core->add_sink(sink);
	}
}




