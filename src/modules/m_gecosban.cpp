/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2015 Renegade334 <contact.caaeed4f@renegade334.me.uk>
 *   Copyright (C) 2013, 2018 Sadie Powell <sadie@witchery.services>
 *   Copyright (C) 2012 Robby <robby@chatbelgie.be>
 *   Copyright (C) 2010 Craig Edwards <brain@inspircd.org>
 *   Copyright (C) 2009 Uli Schlachter <psychon@inspircd.org>
 *   Copyright (C) 2009 Daniel De Graaf <danieldg@inspircd.org>
 *   Copyright (C) 2008 Robin Burchell <robin+git@viroteck.net>
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


#include "inspircd.h"
#include "modules/isupport.h"

class ModuleGecosBan
	: public Module
	, public ISupport::EventListener
{
 public:
	ModuleGecosBan()
		: ISupport::EventListener(this)
	{
	}

	Version GetVersion() override
	{
		return Version("Provides a way to ban users by their real name with the 'a' and 'r' extbans", VF_OPTCOMMON|VF_VENDOR);
	}

	ModResult OnCheckBan(User *user, Channel *c, const std::string& mask) override
	{
		if ((mask.length() > 2) && (mask[1] == ':'))
		{
			if (mask[0] == 'r')
			{
				if (InspIRCd::Match(user->GetRealName(), mask.substr(2)))
					return MOD_RES_DENY;
			}
			else if (mask[0] == 'a')
			{
				// Check that the user actually specified a real name.
				const size_t divider = mask.find('+', 1);
				if (divider == std::string::npos)
					return MOD_RES_PASSTHRU;

				// Check whether the user's mask matches.
				if (!c->CheckBan(user, mask.substr(2, divider - 2)))
					return MOD_RES_PASSTHRU;

				// Check whether the user's real name matches.
				if (InspIRCd::Match(user->GetRealName(), mask.substr(divider + 1)))
					return MOD_RES_DENY;
			}
		}
		return MOD_RES_PASSTHRU;
	}

	void OnBuildISupport(ISupport::TokenMap& tokens) override
	{
		tokens["EXTBAN"].push_back('a');
		tokens["EXTBAN"].push_back('r');
	}
};

MODULE_INIT(ModuleGecosBan)
