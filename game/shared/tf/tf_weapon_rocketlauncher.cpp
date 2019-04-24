//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Launcher
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_rocketlauncher.h"
#include "tf_fx_shared.h"
#include "tf_weaponbase_rocket.h"
#include "in_buttons.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Rocket Launcher tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFRocketLauncher, DT_WeaponRocketLauncher )

BEGIN_NETWORK_TABLE( CTFRocketLauncher, DT_WeaponRocketLauncher )
#ifndef CLIENT_DLL
	//SendPropBool( SENDINFO(m_bLockedOn) ),
#else
	//RecvPropInt( RECVINFO(m_bLockedOn) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFRocketLauncher )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_rocketlauncher, CTFRocketLauncher );
PRECACHE_WEAPON_REGISTER( tf_weapon_rocketlauncher );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFRocketLauncher )
END_DATADESC()
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncher::CTFRocketLauncher()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncher::~CTFRocketLauncher()
{
}

#ifndef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::Precache()
{
	BaseClass::Precache();
	PrecacheParticleSystem( "rocketbackblast" );
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFRocketLauncher::FireProjectile( CTFPlayer *pPlayer )
{
	m_flShowReloadHintAt = gpGlobals->curtime + 30;
	return BaseClass::FireProjectile( pPlayer );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::ItemPostFrame( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;

	BaseClass::ItemPostFrame();

#ifdef GAME_DLL

	if ( m_flShowReloadHintAt && m_flShowReloadHintAt < gpGlobals->curtime )
	{
		if ( Clip1() < GetMaxClip1() )
		{
			pOwner->HintMessage( HINT_SOLDIER_RPG_RELOAD );
		}
		m_flShowReloadHintAt = 0;
	}

	/*
	Vector forward;
	AngleVectors( pOwner->EyeAngles(), &forward );
	trace_t tr;
	CTraceFilterSimple filter( pOwner, COLLISION_GROUP_NONE );
	UTIL_TraceLine( pOwner->EyePosition(), pOwner->EyePosition() + forward * 2000, MASK_SOLID, &filter, &tr );

	if ( tr.m_pEnt &&
		tr.m_pEnt->IsPlayer() &&
		tr.m_pEnt->IsAlive() &&
		tr.m_pEnt->GetTeamNumber() != pOwner->GetTeamNumber() )
	{
		m_bLockedOn = true;
	}
	else
	{
		m_bLockedOn = false;
	}
	*/

#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFRocketLauncher::Deploy( void )
{
	if ( BaseClass::Deploy() )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFRocketLauncher::DefaultReload( int iClipSize1, int iClipSize2, int iActivity )
{
	m_flShowReloadHintAt = 0;
	return BaseClass::DefaultReload( iClipSize1, iClipSize2, iActivity );
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::CreateMuzzleFlashEffects( C_BaseEntity *pAttachEnt, int nIndex )
{
	BaseClass::CreateMuzzleFlashEffects( pAttachEnt, nIndex );

	// Don't do backblast effects in first person
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( pOwner->IsLocalPlayer() )
		return;

	ParticleProp()->Create( "rocketbackblast", PATTACH_POINT_FOLLOW, "backblast" );
}

/*
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::DrawCrosshair( void )
{
	BaseClass::DrawCrosshair();

	if ( m_bLockedOn )
	{
		int iXpos = XRES(340);
		int iYpos = YRES(260);
		int iWide = XRES(8);
		int iTall = YRES(8);

		Color col( 0, 255, 0, 255 );
		vgui::surface()->DrawSetColor( col );

		vgui::surface()->DrawFilledRect( iXpos, iYpos, iXpos + iWide, iYpos + iTall );

		// Draw the charge level onscreen
		vgui::HScheme scheme = vgui::scheme()->GetScheme( "ClientScheme" );
		vgui::HFont hFont = vgui::scheme()->GetIScheme(scheme)->GetFont( "Default" );
		vgui::surface()->DrawSetTextFont( hFont );
		vgui::surface()->DrawSetTextColor( col );
		vgui::surface()->DrawSetTextPos(iXpos + XRES(12), iYpos );
		vgui::surface()->DrawPrintText(L"Lock", wcslen(L"Lock"));

		vgui::surface()->DrawLine( XRES(320), YRES(240), iXpos, iYpos );
	}
}
*/

#endif


//HACKS FOR DM ROCKET LAUNCHER HOT SWAP WEAPON AND SOUNDS BELOW

#define SOLDIER_VIEW_MODEL			"models/weapons/v_models/v_rocketlauncher_soldier.mdl"
#define SOLDIER_WORLD_MODEL			"models/weapons/w_models/w_rocketlauncher.mdl"
#define QUAKE_VIEW_MODEL			"models/weapons/v_models/v_original.mdl"
#define QUAKE_WORLD_MODEL			"models/weapons/c_models/c_bet_rocketlauncher/c_bet_rocketlauncher.mdl"

IMPLEMENT_NETWORKCLASS_ALIASED(TFOriginal, DT_TFOriginal);

BEGIN_NETWORK_TABLE(CTFOriginal, DT_TFOriginal)
#ifdef CLIENT_DLL
RecvPropInt(RECVINFO(m_iTF2ViewIndex)),
RecvPropInt(RECVINFO(m_iTF2WorldIndex)),
RecvPropInt(RECVINFO(m_iQuakeViewIndex)),
RecvPropInt(RECVINFO(m_iQuakeWorldIndex)),
#else
SendPropInt(SENDINFO(m_iTF2ViewIndex)),
SendPropInt(SENDINFO(m_iTF2WorldIndex)),
SendPropInt(SENDINFO(m_iQuakeViewIndex)),
SendPropInt(SENDINFO(m_iQuakeWorldIndex)),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFOriginal )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_iTF2ViewIndex, FIELD_INTEGER, FTYPEDESC_INSENDTABLE | FTYPEDESC_MODELINDEX ),
	DEFINE_PRED_FIELD( m_iTF2WorldIndex, FIELD_INTEGER, FTYPEDESC_INSENDTABLE | FTYPEDESC_MODELINDEX ),
	DEFINE_PRED_FIELD( m_iQuakeViewIndex, FIELD_INTEGER, FTYPEDESC_INSENDTABLE | FTYPEDESC_MODELINDEX ),
	DEFINE_PRED_FIELD( m_iQuakeWorldIndex, FIELD_INTEGER, FTYPEDESC_INSENDTABLE | FTYPEDESC_MODELINDEX ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_rocketlauncher_dm, CTFOriginal);
PRECACHE_WEAPON_REGISTER(tf_weapon_rocketlauncher_dm);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOriginal::Precache( void )
{
	m_iTF2ViewIndex			= PrecacheModel( SOLDIER_VIEW_MODEL );
	m_iTF2WorldIndex		= PrecacheModel( SOLDIER_WORLD_MODEL );
	m_iQuakeViewIndex		= PrecacheModel( QUAKE_VIEW_MODEL );
	m_iQuakeWorldIndex		= PrecacheModel( QUAKE_WORLD_MODEL );

	BaseClass::Precache();
}

bool CTFOriginal::Deploy( void )
{
	CTFPlayer *pPlayer = ToTFPlayer(GetTFPlayerOwner());
	int iModelToUse = 0;

	if ( pPlayer )
	{
#ifdef GAME_DLL
		iModelToUse = V_atoi(engine->GetClientConVarValue(pPlayer->entindex(), "ofd_use_quake_rl"));
#else
		extern ConVar ofd_use_quake_rl;
		iModelToUse = V_atoi(ofd_use_quake_rl.GetString());
#endif
	}

	if ( iModelToUse )
	{
		ActivateQuakeModel();
	}
	else
	{
		ActivateSoldierModel();
	}

	return BaseClass::Deploy();
}

void CTFOriginal::ActivateSoldierModel( void )
{
	m_iViewModelIndex	= m_iTF2ViewIndex;
	m_iWorldModelIndex	= m_iTF2WorldIndex;
	SetModel( GetViewModel() );
	m_bQuakeRLHack = false;
}

void CTFOriginal::ActivateQuakeModel( void )
{
	m_iViewModelIndex	= m_iQuakeViewIndex;
	m_iWorldModelIndex	= m_iQuakeWorldIndex;
	SetModel( GetViewModel() );
	m_bQuakeRLHack = true;
}

const char *CTFOriginal::GetViewModel( int ) const
{
	if ( m_iViewModelIndex == m_iTF2ViewIndex)
	{
		return SOLDIER_VIEW_MODEL;
	}
	else
	{
		return QUAKE_VIEW_MODEL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFOriginal::GetWorldModel( void ) const
{
	if ( m_iViewModelIndex == m_iTF2ViewIndex)
	{
		return SOLDIER_WORLD_MODEL;
	}
	else
	{
		return QUAKE_WORLD_MODEL;
	}
}
