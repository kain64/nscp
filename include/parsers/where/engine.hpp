#pragma once

#include <parsers/where/node.hpp>
#include <parsers/where.hpp>
#include <parsers/where/dll_defines.hpp>

#include <vector>

namespace parsers {
	namespace where {
		class NSCAPI_EXPORT error_handler_interface {
		public:
			virtual void log_error(const std::string error) = 0;
			virtual void log_warning(const std::string error) = 0;
			virtual void log_debug(const std::string error) = 0;
			virtual bool is_debug() const = 0;
			virtual void set_debug(bool debug) = 0;
		};

		struct NSCAPI_EXPORT engine_filter {
			typedef boost::shared_ptr<error_handler_interface> error_handler;
			typedef parsers::where::evaluation_context execution_context_type;
			parsers::where::parser ast_parser;
			std::string filter_string;
			boost::optional<bool> requires_object;

			engine_filter(const std::string filter_string) : filter_string(filter_string) {}

			bool validate(error_handler error, object_factory context, bool perf_collection, parsers::where::performance_collector &boundries);

			bool require_object(execution_context_type context);

			bool match(error_handler error, execution_context_type context, bool expect_object);

		};

		struct NSCAPI_EXPORT engine {
			typedef boost::shared_ptr<error_handler_interface> error_handler;
			typedef parsers::where::evaluation_context execution_context_type;

			std::list<engine_filter> filters_;
			//parsers::where::parser ast_parser;
			//std::string filter_string;
			bool perf_collection;
			typedef parsers::where::performance_collector::boundries_type boundries_type;
			parsers::where::performance_collector boundries;
			error_handler error;
			boost::optional<bool> requires_object;

			engine(std::vector<std::string> filter, error_handler error);

			boundries_type fetch_performance_data();

			void enabled_performance_collection();

			bool validate(object_factory context);

			bool match(execution_context_type context, bool expect_object);

			std::string get_subject() { return "TODO"; }
		};
	}
}