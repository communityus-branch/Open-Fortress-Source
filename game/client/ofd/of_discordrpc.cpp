//=============================================================================
//
// Purpose: Discord and Steam Rich Presence support.
//
//=============================================================================

#include "cbase.h"
#include "of_discordrpc.h"
#include "c_team_objectiveresource.h"
#include "tf_gamerules.h"
#include "c_tf_team.h"
#include "c_tf_playerresource.h"
#include <inetchannelinfo.h>
#include "discord_rpc.h"
#include "discord_register.h"
#include "tf_gamerules.h"
#include <ctime>
#include "steam/isteammatchmaking.h"
#include "steam/isteamgameserver.h"
#include "steam/isteamfriends.h"
#include "steam/steam_api.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_richpresence_printmsg( "cl_richpresence_printmsg", "0", FCVAR_ARCHIVE, "" );

ConVar of_enable_rpc("of_enable_rpc", "1", FCVAR_ARCHIVE, "Enables/Disables Discord Rich Presence. Requires a game restart.");

//#define DISCORD_LIBRARY_DLL "discord-rpc.dll"
#define DISCORD_APP_ID	"558662173736566794"

// update once every 10 seconds. discord has an internal rate limiter of 15 seconds as well
#define DISCORD_UPDATE_RATE 10.0f

CTFDiscordRPC g_discordrpc;

CTFDiscordRPC::CTFDiscordRPC()
{
	Q_memset(m_szLatchedMapname, 0, MAX_MAP_NAME);
	m_bInitializeRequested = false;
}

CTFDiscordRPC::~CTFDiscordRPC()
{
}

void CTFDiscordRPC::Shutdown()
{
	Discord_Shutdown();

	if ( steamapicontext->SteamFriends() )
		steamapicontext->SteamFriends()->ClearRichPresence();
}

void CTFDiscordRPC::Init()
{
	InitializeDiscord();
	m_bInitializeRequested = true;

	// make sure to call this after game system initialized
	ListenForGameEvent( "server_spawn" );
}

void CTFDiscordRPC::RunFrame()
{
	if (m_bErrored)
		return;

	// NOTE: we want to run this even if they have use_discord off, so we can clear
	// any previous state that may have already been sent
	UpdateRichPresence();
	Discord_RunCallbacks();

	// always run this, otherwise we will chicken & egg waiting for ready
	//if (Discord_RunCallbacks)
	//	Discord_RunCallbacks();
}

void CTFDiscordRPC::OnReady( const DiscordUser* user )
{
	if (!of_enable_rpc.GetBool())
	{
		Discord_Shutdown();

		if ( steamapicontext->SteamFriends() )
			steamapicontext->SteamFriends()->ClearRichPresence();

		return;
	}

	ConColorMsg( Color( 114, 137, 218, 255 ), "[Rich Presence] Ready!\n" );
	ConColorMsg( Color( 114, 137, 218, 255 ), "[Rich Presence] User %s#%s - %s\n", user->username, user->discriminator, user->userId );
	
	g_discordrpc.Reset();
}

void CTFDiscordRPC::OnDiscordError(int errorCode, const char *szMessage)
{
	g_discordrpc.m_bErrored = true;
	char buff[1024];
	Q_snprintf(buff, 1024, "[Rich Presence] Init failed. code %d - error: %s\n", errorCode, szMessage);
	Warning(buff);
}


void CTFDiscordRPC::OnJoinGame( const char *joinSecret )
{
	ConColorMsg( Color( 114, 137, 218, 255 ), "[Rich Presence] Join Game: %s\n", joinSecret );

	char szCommand[128];
	Q_snprintf( szCommand, sizeof( szCommand ), "connect %s\n", joinSecret );
	engine->ExecuteClientCmd( szCommand );
}

void CTFDiscordRPC::OnSpectateGame( const char *spectateSecret )
{
	ConColorMsg( Color( 114, 137, 218, 255 ), "[Rich Presence] Spectate Game: %s\n", spectateSecret );
}

void CTFDiscordRPC::OnJoinRequest( const DiscordUser *joinRequest )
{
	ConColorMsg( Color( 114, 137, 218, 255 ), "[Rich Presence] Join Request: %s#%s\n", joinRequest->username, joinRequest->discriminator );
	ConColorMsg( Color( 114, 137, 218, 255 ), "[Rich Presence] Join Request Accepted\n" );
	Discord_Respond( joinRequest->userId, DISCORD_REPLY_YES );
}

void CTFDiscordRPC::SetLogo( void )
{
	const char *pszGameType = "";
	const char *pszImageLarge = "ico";
	
	if ( engine->IsConnected() )
	{
		if (TFGameRules()->GetGameType() == TF_GAMETYPE_UNDEFINED)
		{
			pszGameType = "";
		}
		else if (TFGameRules()->GetGameType() == TF_GAMETYPE_CTF)
		{
			pszGameType = "Capture The Flag";
		}
		else if (TFGameRules()->GetGameType() == TF_GAMETYPE_CP)
		{
			pszGameType = "Control Point";
		}
		else if (TFGameRules()->IsDMGamemode())
		{
			pszGameType = "Deathmatch";
		}
		else if (TFGameRules()->GetGameType() == TF_GAMETYPE_TDM)
		{
			pszGameType = "Team Deathmatch";
		}		
		else if (TFGameRules()->GetGameType() == TF_GAMETYPE_ESC)
		{
			pszGameType = "Escort";
		}
	}
	
	m_sDiscordRichPresence.largeImageKey = pszImageLarge;
	m_sDiscordRichPresence.largeImageText = pszGameType;

	// we can have class icon here like tf2c discord
	//m_sDiscordRichPresence.smallImageKey = "logo-small";
	//m_sDiscordRichPresence.smallImageText = "";
}

void CTFDiscordRPC::InitializeDiscord()
{
	DiscordEventHandlers handlers;
	Q_memset(&handlers, 0, sizeof(handlers));
	handlers.ready			= &CTFDiscordRPC::OnReady;
	handlers.errored		= &CTFDiscordRPC::OnDiscordError;
	handlers.joinGame		= &CTFDiscordRPC::OnJoinGame;
	//handlers.spectateGame = &CTFDiscordRPC::OnSpectateGame;
	handlers.joinRequest	= &CTFDiscordRPC::OnJoinRequest;

	char command[512];
	V_snprintf( command, sizeof( command ), "%s -game \"%s\" -novid -steam\n", CommandLine()->GetParm( 0 ), CommandLine()->ParmValue( "-game" ) );
	Discord_Register( DISCORD_APP_ID, command );
	Discord_Initialize( DISCORD_APP_ID, &handlers, false, "" );
	Reset();
}

bool CTFDiscordRPC::NeedToUpdate()
{
	if ( m_bErrored || m_szLatchedMapname[0] == '\0')
		return false;

	return gpGlobals->curtime >= m_flLastUpdatedTime + DISCORD_UPDATE_RATE;
}

void CTFDiscordRPC::Reset()
{
	Q_memset( &m_sDiscordRichPresence, 0, sizeof( m_sDiscordRichPresence ) );
	m_sDiscordRichPresence.details = "Main Menu";
	const char *pszState = "";
	
	m_sDiscordRichPresence.state = pszState;

	m_sDiscordRichPresence.endTimestamp;

	if ( steamapicontext->SteamFriends() )
	{
		steamapicontext->SteamFriends()->SetRichPresence( "status", "Main Menu" );
		steamapicontext->SteamFriends()->SetRichPresence( "connect", NULL );
		steamapicontext->SteamFriends()->SetRichPresence( "steam_display", "Main Menu" );
		steamapicontext->SteamFriends()->SetRichPresence( "steam_player_group", NULL );
		steamapicontext->SteamFriends()->SetRichPresence( "steam_player_group_size", NULL );
	}

	SetLogo();
	Discord_UpdatePresence( &m_sDiscordRichPresence );
}

void CTFDiscordRPC::UpdatePlayerInfo()
{
	C_TF_PlayerResource *pResource = GetTFPlayerResource();
	if ( !pResource )
		return;

	int maxPlayers = gpGlobals->maxClients;
	int curPlayers = 0;

	const char *pzePlayerName = NULL;

	m_sDiscordRichPresence.details = m_szLatchedMapname;
	m_sDiscordRichPresence.startTimestamp;

	for (int i = 1; i < maxPlayers; i++)
	{
		if ( pResource->IsConnected( i ) )
		{
			
			curPlayers++;
			if ( pResource->IsLocalPlayer( i ) )
			{
				pzePlayerName = pResource->GetPlayerName( i );
			}
		}
	}

	//int iTimeLeft = TFGameRules()->GetTimeLeft();

	if ( m_szLatchedHostname[0] != '\0' )
	{
		if ( cl_richpresence_printmsg.GetBool() )
		{
			ConColorMsg( Color( 114, 137, 218, 255 ), "[Discord] sending details of\n '%s'\n", m_szServerInfo );
		}
		m_sDiscordRichPresence.partySize = curPlayers;
		m_sDiscordRichPresence.partyMax = maxPlayers;
		m_sDiscordRichPresence.state = m_szLatchedHostname;
	}

	if ( steamapicontext->SteamFriends() )
	{
		//steamapicontext->SteamFriends()->SetRichPresence( "connect", pSteamGameServer->GetSteamID() );
		steamapicontext->SteamFriends()->SetRichPresence( "connect", NULL );
		steamapicontext->SteamFriends()->SetRichPresence( "steam_player_group", NULL );
		steamapicontext->SteamFriends()->SetRichPresence( "steam_player_group_size", NULL );
		steamapicontext->SteamFriends()->SetRichPresence( "status", m_szServerInfo );
		steamapicontext->SteamFriends()->SetRichPresence( "steam_display", "In-Game" );
	}
}

void CTFDiscordRPC::FireGameEvent( IGameEvent *event )
{
	const char * type = event->GetName();

	if ( Q_strcmp( type, "server_spawn" ) == 0 )
	{
		Q_strncpy( m_szLatchedHostname, event->GetString( "hostname" ), 255 );
	}
}

void CTFDiscordRPC::UpdateRichPresence()
{
	if (!NeedToUpdate())
		return;

	m_flLastUpdatedTime = gpGlobals->curtime;

	if ( engine->IsConnected() )
	{
		UpdatePlayerInfo();
		UpdateNetworkInfo();
	}
	else
	{
		m_sDiscordRichPresence.state = "";
		m_sDiscordRichPresence.details = "Currently is loading...";
	}

	SetLogo();

	Discord_UpdatePresence(&m_sDiscordRichPresence);
}


void CTFDiscordRPC::UpdateNetworkInfo()
{
	INetChannelInfo *ni = engine->GetNetChannelInfo();

	char partyId[128];
	sprintf( partyId, "%s-party", ni->GetAddress()); // adding -party here because secrets cannot match the party id

	m_sDiscordRichPresence.partyId = partyId;

	m_sDiscordRichPresence.joinSecret = ni->GetAddress();
	m_sDiscordRichPresence.spectateSecret = "Spectate";
}

void CTFDiscordRPC::LevelInit( const char *szMapname )
{
	Reset();
	// we cant update our presence here, because if its the first map a client loaded,
	// discord api may not yet be loaded, so latch
	Q_strcpy(m_szLatchedMapname, szMapname);
	// important, clear last update time as well
	m_flLastUpdatedTime = max(0, gpGlobals->curtime - DISCORD_UPDATE_RATE);
}