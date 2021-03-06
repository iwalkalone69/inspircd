/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2013 Sadie Powell <sadie@witchery.services>
 *   Copyright (C) 2012, 2019 Robby <robby@chatbelgie.be>
 *   Copyright (C) 2012 Attila Molnar <attilamolnar@hush.com>
 *   Copyright (C) 2010 Craig Edwards <brain@inspircd.org>
 *   Copyright (C) 2009-2010 Daniel De Graaf <danieldg@inspircd.org>
 *   Copyright (C) 2009 Uli Schlachter <psychon@inspircd.org>
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

class ModulePartMsgBan
	: public Module
	, public ISupport::EventListener
{
 public:
	ModulePartMsgBan()
		: ISupport::EventListener(this)
	{
	}

	Version GetVersion() override
	{
		return Version("Provides extban 'p', part message bans", VF_OPTCOMMON|VF_VENDOR);
	}

	void OnUserPart(Membership* memb, std::string &partmessage, CUList& excepts) override
	{
		if (!IS_LOCAL(memb->user))
			return;

		if (memb->chan->GetExtBanStatus(memb->user, 'p') == MOD_RES_DENY)
			partmessage.clear();
	}

	void OnBuildISupport(ISupport::TokenMap& tokens) override
	{
		tokens["EXTBAN"].push_back('p');
	}
};

MODULE_INIT(ModulePartMsgBan)
