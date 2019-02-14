//
// Created by limo on 2/14/2019.
//

#include "logger.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h" // or "../stdout_sinks.h" if no colors needed
#include "spdlog/sinks/daily_file_sink.h" // or "../stdout_sinks.h" if no colors needed

namespace limo_ns {

    namespace log {

        constexpr const char *k_default_logger_name = "GLOBAL_LOGGER";

        static bool create_and_register_logger(std::string const &logger_name, bool log_to_console,
                bool log_to_file, std::string const &log_file, log_level_e log_level) {

            std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;
            if (log_to_console) {
                sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
            }
            if (log_to_file) {
                sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file, 23, 59));
            }

            auto const logger = std::make_shared<spdlog::logger>(
                    logger_name, sinks.begin(), sinks.end());
            logger->set_level(log_level);
            register_logger(logger);

            return true;
        }

        static std::shared_ptr<spdlog::logger> logger(std::string const &logger_name) {
            return spdlog::get(logger_name);
        }

        std::shared_ptr<spdlog::logger> logger() {
            static bool initialized = false;
            if (not initialized) {
                create_and_register_logger(k_default_logger_name, true, false, "", spdlog::level::trace);
                initialized = true;
            }
            return ::limo_ns::log::logger(k_default_logger_name);
        }

    }

}
