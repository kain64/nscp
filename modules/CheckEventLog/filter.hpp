#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#endif

#include <map>
#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <error.hpp>
#include <format.hpp>

#include <parsers/where.hpp>
#include <parsers/where/node.hpp>
#include <parsers/where/engine.hpp>
#include <parsers/filter/modern_filter.hpp>
#include <parsers/where/filter_handler_impl.hpp>

#include <buffer.hpp>
#include <handle.hpp>

#include "eventlog_record.hpp"
#include "modern_eventlog.hpp"

namespace eventlog_filter {
	struct filter_obj : boost::noncopyable {

		filter_obj() {}
		virtual ~filter_obj() {}

		virtual long long get_id() const = 0;
		virtual std::string get_source() const = 0;
		virtual std::string get_guid() const = 0;
		virtual std::string get_computer() const = 0;
		virtual long long get_el_type() const = 0;
		virtual std::string get_task() = 0;
		virtual std::string get_keyword() = 0;
		virtual std::string get_el_type_s() const = 0;
		virtual long long get_severity() const = 0;
		virtual std::string get_message() = 0;
		virtual std::string get_strings() = 0;
		virtual std::string get_log() const = 0;
		virtual long long get_written() const = 0;
		virtual long long get_category() const = 0;
		virtual long long get_facility() const = 0;
		virtual long long get_customer() const = 0;
		virtual long long get_raw_id() const = 0;
		virtual long long get_generated() const = 0;
		virtual bool is_modern() const = 0;
		virtual std::string get_written_s() const {
			return format::itos_as_time(get_written());
		}
		virtual std::string to_string() const = 0;
	};

	struct old_filter_obj : filter_obj {
		EventLogRecord record;
		const int truncate_message;

		old_filter_obj(std::string file, const EVENTLOGRECORD *pevlr, __int64 currentTime, const int truncate_message)
			: record(file, pevlr, currentTime)
			, truncate_message(truncate_message) {}

		long long get_id() const {
			return record.eventID();
		}
		std::string get_source() const {
			return utf8::cvt<std::string>(record.get_source());
		}
		std::string get_computer() const {
			return utf8::cvt<std::string>(record.get_computer());
		}
		std::string get_task() {
			return "";
		}
		std::string get_keyword() {
			return "";
		}
		long long get_el_type() const {
			return record.eventType();
		}
		std::string get_el_type_s() const;
		long long get_severity() const {
			return record.severity();
		}
		std::string get_message() {
			return utf8::cvt<std::string>(record.render_message(truncate_message));
		}
		std::string get_strings() {
			return utf8::cvt<std::string>(record.enumStrings());
		}
		std::string get_log() const {
			return utf8::cvt<std::string>(record.get_log());
		}
		std::string get_guid() const {
			return "";
		}
		long long get_written() const {
			return record.written();
		}
		long long get_category() const {
			return record.category();
		}
		long long get_facility() const {
			return record.facility();
		}
		long long get_customer() const {
			return record.customer();
		}
		long long get_raw_id() const {
			return record.raw_id();
		}
		long long get_generated() const {
			return record.generated();
		}
		bool is_modern() const { return false; }
		
		virtual std::string to_string() const { 
			return get_log() + ":" + strEx::s::xtos(get_id()) + "=" + get_el_type_s();
		}

	};

	struct new_filter_obj : filter_obj {
		const std::string logfile;
		eventlog::evt_handle hEvent;
		hlp::buffer<wchar_t, eventlog::api::PEVT_VARIANT> buffer;
		const int truncate_message;
		eventlog::evt_handle hProviderMetadataHandle;

		new_filter_obj(const std::string &logfile, eventlog::api::EVT_HANDLE hEvent, eventlog::evt_handle &hContext, const int truncate_message);
		virtual ~new_filter_obj() {}

		long long get_id() const {
			return buffer.get()[eventlog::api::EvtSystemEventID].UInt16Val;
		}
		std::string get_source() const;
		std::string get_guid() const;
		std::string get_computer() const;
		long long get_el_type() const;
		std::string get_task();
		std::string get_keyword();
		std::string get_el_type_s() const;
		long long get_severity() const {
			return 0;
		}
		std::string get_message();
		std::string get_strings() {
			return get_message();
		}
		std::string get_log() const;
		long long get_written() const;
		long long get_category() const;
		long long get_facility() const {
			return 0;
		}
		long long get_customer() const {
			return 0;
		}
		long long get_raw_id() const {
			return 0;
		}
		long long get_generated() const {
			return 0;
		}
		bool is_modern() const { return true; }
		eventlog::evt_handle& get_provider_handle();
		virtual std::string to_string() const { return logfile + ":" + strEx::s::xtos(get_id()) + "=" + get_el_type_s(); }
	};

	typedef parsers::where::filter_handler_impl<boost::shared_ptr<filter_obj> > native_context;
	struct filter_obj_handler : public native_context {
		static const parsers::where::value_type type_custom_severity = parsers::where::type_custom_int_1;
		static const parsers::where::value_type type_custom_type = parsers::where::type_custom_int_2;
		filter_obj_handler();
	};
	typedef modern_filter::modern_filters<filter_obj, filter_obj_handler> filter;
}