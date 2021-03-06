/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2013, 2017 Sadie Powell <sadie@witchery.services>
 *   Copyright (C) 2012-2013, 2016 Attila Molnar <attilamolnar@hush.com>
 *   Copyright (C) 2012, 2019 Robby <robby@chatbelgie.be>
 *   Copyright (C) 2010 Craig Edwards <brain@inspircd.org>
 *   Copyright (C) 2009-2010 Daniel De Graaf <danieldg@inspircd.org>
 *   Copyright (C) 2009 Matt Smith <dz@inspircd.org>
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

class ModuleAllowInvite
	: public Module
	, public ISupport::EventListener
{
 private:
	SimpleChannelModeHandler ni;

 public:
	ModuleAllowInvite()
		: ISupport::EventListener(this)
		, ni(this, "allowinvite", 'A')
	{
	}

	void OnBuildISupport(ISupport::TokenMap& tokens) override
	{
		tokens["EXTBAN"].push_back('A');
	}

	ModResult OnUserPreInvite(User* user,User* dest,Channel* channel, time_t timeout) override
	{
		if (IS_LOCAL(user))
		{
			ModResult res = channel->GetExtBanStatus(user, 'A');
			if (res == MOD_RES_DENY)
			{
				// Matching extban, explicitly deny /invite
				user->WriteNumeric(ERR_CHANOPRIVSNEEDED, channel->name, "You are banned from using INVITE");
				return res;
			}
			if (channel->IsModeSet(ni) || res == MOD_RES_ALLOW)
			{
				// Explicitly allow /invite
				return MOD_RES_ALLOW;
			}
		}

		return MOD_RES_PASSTHRU;
	}

	Version GetVersion() override
	{
		return Version("Provides channel mode +A to allow /INVITE freely on a channel, and extban 'A' to deny specific users it", VF_VENDOR);
	}
};

MODULE_INIT(ModuleAllowInvite)
