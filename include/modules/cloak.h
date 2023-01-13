/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2023 Sadie Powell <sadie@witchery.services>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once

namespace Cloak
{
	class Engine;
	class Method;

	/** A shared pointer to a cloak method. */
	typedef std::shared_ptr<Method> MethodPtr;

	/** Takes a hostname and retrieves the part which should be visible.
	 *
	 * This is usually the last \p hostparts segments but if not enough are
	 * present then all but the most specific segments are used. If the domain
	 * name consists of one label only then none are used.
	 *
	 * Here are some examples for how domain names will be shortened assuming
	 * \p domainparts is set to the default of 3.
	 *
	 *   "this.is.an.example.com"  =>  "an.example.com"
	 *   "an.example.com"          =>  "example.com"
	 *   "example.com"             =>  "com"
	 *   "localhost"               =>  ""
	 *
	 *   "/var/run/inspircd/client.sock"  =>  "run/inspircd/client.sock"
	 *   "/run/inspircd/client.sock"      =>  "inspircd/client.sock"
	 *   "/inspircd/client.sock"          =>  "client.sock"
	 *   "/client.sock"                   =>  ""
	 *
	 * @param host The hostname to cloak.
	 * @param domainparts The number of host labels that should be visible.
	 * @return The visible segment of the hostname.
	 */
	inline std::string VisiblePart(const std::string& host, size_t hostparts, char separator);
}

/** Base class for cloak engines. */
class Cloak::Engine
	: public DataProvider
{
protected:
	Engine(Module* Creator, const std::string& Name)
		: DataProvider(Creator, "cloak/" + Name)
	{
	}

public:
	/** Creates a new cloak method from the specified config.
	 * @param tag The config tag to configure the cloak method with.
	 * @param primary Whether the created cloak method is the primary emthod.
	 */
	virtual MethodPtr Create(const std::shared_ptr<ConfigTag>& tag, bool primary) = 0;
};

/** Base class for cloak methods. */
class Cloak::Method
	: public Cullable
{
private:
	/** The name of the engine that created this method. */
	std::string provname;

protected:
	Method(const Engine* engine) ATTR_NOT_NULL(2)
		: provname(engine->name)
	{
	}

public:
	/** Generates a cloak for the specified user.
	 * @param user The user to generate a cloak for.
	 */
	virtual std::string Generate(LocalUser* user) ATTR_NOT_NULL(2) = 0;

	/** Generates a cloak for the specified hostname, IP address, or UNIX socket path.
	 * @param user The hostname, IP address, or UNIX socket path to generate a cloak for.
	 */
	virtual std::string Generate(const std::string& hostip) = 0;

	/** Retrieves link compatibility data for this cloak method.
	 * @param data The location to store link compatibility data.
	 * @param compatdata The location to store link compatibility data for older protocols.
	 */
	virtual void GetLinkData(Module::LinkData& data, std::string& compatdata) = 0;

	/** Retrieves the name of this cloaking method. */
	std::string_view GetName() const
	{
		return std::string_view(provname.c_str() + 6);
	}

	/** Determines whether this method is provided by the specified service provider.
	 * @param prov The service provider to check.
	 */
	bool IsProvidedBy(const ServiceProvider& prov) const
	{
		return prov.name == provname;
	}
};

inline std::string Cloak::VisiblePart(const std::string& host, size_t hostparts, char separator)
{
	// The position at which we found the last separator.
	std::string::const_reverse_iterator seppos;

	// The number of separators we have seen so far.
	size_t seenseps = 0;

	for (std::string::const_reverse_iterator it = host.rbegin(); it != host.rend(); ++it)
	{
		if (*it != separator)
			continue;

		// We have found a separator!
		seppos = it;
		seenseps += 1;

		// Do we have enough segments to stop?
		if (seenseps >= hostparts)
			break;
	}

	// We only returns a domain part if more than one label is
	// present. See above for a full explanation.
	if (!seenseps)
		return "";

	return std::string(seppos.base(), host.end());
}