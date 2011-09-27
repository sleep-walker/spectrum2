/**
 * libtransport -- C++ library for easy XMPP Transports development
 *
 * Copyright (C) 2011, Jan Kaluza <hanzz.k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "transport/config.h"
#include <fstream>

using namespace boost::program_options;

namespace Transport {

bool Config::load(const std::string &configfile, boost::program_options::options_description &opts) {
	std::ifstream ifs(configfile.c_str());
	if (!ifs.is_open())
		return false;

	m_file = configfile;
	bool ret = load(ifs, opts);
	ifs.close();

	char path[PATH_MAX] = "";
	if (m_file.find_first_of("/") != 0) {
		getcwd(path, PATH_MAX);
		m_file = std::string(path) + "/" + m_file;
	}

	return ret;
}

bool Config::load(std::istream &ifs, boost::program_options::options_description &opts) {
	m_unregistered.clear();
	opts.add_options()
		("service.jid", value<std::string>()->default_value(""), "Transport Jabber ID")
		("service.server", value<std::string>()->default_value(""), "Server to connect to")
		("service.password", value<std::string>()->default_value(""), "Password used to auth the server")
		("service.port", value<int>()->default_value(0), "Port the server is listening on")
		("service.user", value<std::string>()->default_value(""), "The name of user Spectrum runs as.")
		("service.group", value<std::string>()->default_value(""), "The name of group Spectrum runs as.")
		("service.backend", value<std::string>()->default_value("libpurple_backend"), "Backend")
		("service.protocol", value<std::string>()->default_value(""), "Protocol")
		("service.pidfile", value<std::string>()->default_value("/var/run/spectrum2/spectrum2.pid"), "Full path to pid file")
		("service.working_dir", value<std::string>()->default_value("/var/lib/spectrum2"), "Working dir")
		("service.allowed_servers", value<std::string>()->default_value(""), "Only users from these servers can connect")
		("service.server_mode", value<bool>()->default_value(false), "True if Spectrum should behave as server")
		("service.users_per_backend", value<int>()->default_value(100), "Number of users per one legacy network backend")
		("service.backend_host", value<std::string>()->default_value("localhost"), "Host to bind backend server to")
		("service.backend_port", value<std::string>()->default_value("10000"), "Port to bind backend server to")
		("service.cert", value<std::string>()->default_value(""), "PKCS#12 Certificate.")
		("service.cert_password", value<std::string>()->default_value(""), "PKCS#12 Certificate password.")
		("service.admin_username", value<std::string>()->default_value(""), "Administrator username.")
		("service.admin_password", value<std::string>()->default_value(""), "Administrator password.")
		("service.reuse_old_backends", value<bool>()->default_value(true), "True if Spectrum should use old backends which were full in the past.")
		("service.idle_reconnect_time", value<int>()->default_value(0), "Time in seconds after which idle users are reconnected to let their backend die.")
		("service.memory_collector_time", value<int>()->default_value(0), "Time in seconds after which backend with most memory is set to die.")
		("service.more_resources", value<bool>()->default_value(false), "Allow more resources to be connected in server mode at the same time.")
		("service.enable_privacy_lists", value<bool>()->default_value(true), "")
		("identity.name", value<std::string>()->default_value("Spectrum 2 Transport"), "Name showed in service discovery.")
		("identity.category", value<std::string>()->default_value("gateway"), "Disco#info identity category. 'gateway' by default.")
		("identity.type", value<std::string>()->default_value(""), "Type of transport ('icq','msn','gg','irc', ...)")
		("registration.enable_public_registration", value<bool>()->default_value(true), "True if users should be able to register.")
		("registration.language", value<std::string>()->default_value("en"), "Default language for registration form")
		("registration.instructions", value<std::string>()->default_value(""), "Instructions showed to user in registration form")
		("registration.username_field", value<std::string>()->default_value(""), "Label for username field")
		("registration.username_mask", value<std::string>()->default_value(""), "Username mask")
		("registration.encoding", value<std::string>()->default_value("en"), "Default encoding in registration form")
		("database.type", value<std::string>()->default_value("none"), "Database type.")
		("database.database", value<std::string>()->default_value(""), "Database used to store data")
		("database.server", value<std::string>()->default_value("localhost"), "Database server.")
		("database.user", value<std::string>()->default_value(""), "Database user.")
		("database.password", value<std::string>()->default_value(""), "Database Password.")
		("database.port", value<int>()->default_value(0), "Database port.")
		("database.prefix", value<std::string>()->default_value(""), "Prefix of tables in database")
		("logging.config", value<std::string>()->default_value(""), "Path to log4cxx config file which is used for Spectrum 2 instance")
		("logging.backend_config", value<std::string>()->default_value(""), "Path to log4cxx config file which is used for backends")
		("backend.default_avatar", value<std::string>()->default_value(""), "Full path to default avatar")
		("backend.avatars_directory", value<std::string>()->default_value(""), "Path to directory with avatars")
		("backend.no_vcard_fetch", value<bool>()->default_value(false), "True if VCards for buddies should not be fetched. Only avatars will be forwarded.")
	;

	parsed_options parsed = parse_config_file(ifs, opts, true);

	BOOST_FOREACH(option opt, parsed.options) {
		if (opt.unregistered) {
			m_unregistered[opt.string_key] = opt.value[0];
		}
	}

	store(parsed, m_variables);
	notify(m_variables);

	onConfigReloaded();

	return true;
}

bool Config::load(std::istream &ifs) {
	options_description opts("Transport options");
	return load(ifs, opts);
}

bool Config::load(const std::string &configfile) {
	options_description opts("Transport options");
	return load(configfile, opts);
}

bool Config::reload() {
	if (m_file.empty()) {
		return false;
	}

	return load(m_file);
}

}
