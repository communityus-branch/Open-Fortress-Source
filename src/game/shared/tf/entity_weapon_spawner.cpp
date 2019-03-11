//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: Weapon Spawner.
//
//=============================================================================//
#include "cbase.h"
#include "entity_weapon_spawner.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"

#ifdef CLIENT_DLL
#include <vgui_controls/Panel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui/IScheme.h>
#include "hudelement.h"
#include "iclientmode.h"
#include "hud_numericdisplay.h"
#include "tf_imagepanel.h"
#include "c_tf_player.h"
#include "c_tf_team.h"
#include "tf_hud_objectivestatus.h"
#include "view.h"

ConVar cl_weaponspawner_return_size( "cl_weaponspawner_return_size", "20", FCVAR_CHEAT );

#else
#include "tf_player.h"
#include "tf_team.h"
#include "tf_objective_resource.h"
#include "tf_gamestats.h"
#include "func_respawnroom.h"
#include "datacache/imdlcache.h"

ConVar cl_weaponspawner_return_height( "cl_weaponspawner_return_height", "82", FCVAR_CHEAT );

#endif


#ifdef CLIENT_DLL

static void RecvProxy_IsDisabled( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	CWeaponSpawner *pWeaponSpawner = (CWeaponSpawner *)pStruct;
	bool bIsDisabled = ( pData->m_Value.m_Int > 0 );

	if ( pWeaponSpawner )
	{
		pWeaponSpawner->SetDisabled( bIsDisabled ); 
	}
}

#endif

//=============================================================================
//
// CTF Flag tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSpawner, DT_WeaponSpawner )

BEGIN_NETWORK_TABLE( CWeaponSpawner, DT_WeaponSpawner )

#ifdef GAME_DLL
	SendPropBool( SENDINFO( m_bDisabled ) ),
	SendPropInt( SENDINFO( m_nGameType ), 5, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_nWeaponSpawnerStatus ), 3, SPROP_UNSIGNED ),
	SendPropTime( SENDINFO( m_wsResetTime ) ),
	SendPropTime( SENDINFO( m_wsNeutralTime ) ),
	SendPropTime( SENDINFO( m_wsMaxResetTime ) ),
	SendPropEHandle( SENDINFO( m_hPrevOwner ) ),
#else
	RecvPropInt( RECVINFO( m_bDisabled ), 0, RecvProxy_IsDisabled ),
	RecvPropInt( RECVINFO( m_nGameType ) ),
	RecvPropInt( RECVINFO( m_nWeaponSpawnerStatus ) ),
	RecvPropTime( RECVINFO( m_wsResetTime ) ),
	RecvPropTime( RECVINFO( m_wsNeutralTime ) ),
	RecvPropTime( RECVINFO( m_wsMaxResetTime ) ),
	RecvPropEHandle( RECVINFO( m_hPrevOwner ) ),
#endif
END_NETWORK_TABLE()

BEGIN_DATADESC( CWeaponSpawner )

	// Keyfields.
	DEFINE_KEYFIELD( m_nGameType, FIELD_INTEGER, "GameType" ),

#ifdef GAME_DLL
	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "RoundActivate", InputRoundActivate ),

	// Outputs.
	DEFINE_OUTPUT( m_outputOnReturn, "OnReturn" ),
	DEFINE_OUTPUT( m_outputOnPickUp, "OnPickUp" ),
	DEFINE_OUTPUT( m_outputOnDrop, "OnDrop" ),
	DEFINE_OUTPUT( m_outputOnCapture, "OnCapture" ),
#endif

END_DATADESC();

LINK_ENTITY_TO_CLASS( dm_weapon_spawner, CWeaponSpawner );

//=============================================================================
//
// CTF Flag functions.
//

CWeaponSpawner::CWeaponSpawner()
{
#ifdef CLIENT_DLL
	m_pGlowTrailEffect = NULL;
#else
	m_hReturnIcon = NULL;
#endif	
	UseClientSideAnimation();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
unsigned int CWeaponSpawner::GetItemID( void )
{
	return TF_ITEM_CAPTURE_FLAG;
}

//-----------------------------------------------------------------------------
// Purpose: Precache the model and sounds.
//-----------------------------------------------------------------------------
void CWeaponSpawner::Precache( void )
{
	PrecacheModel( TF_WEAPON_MODEL );

	PrecacheScriptSound( TF_CTF_FLAGSPAWN );
	PrecacheScriptSound( TF_CTF_ENEMY_STOLEN );
	PrecacheScriptSound( TF_CTF_ENEMY_DROPPED );
	PrecacheScriptSound( TF_CTF_ENEMY_CAPTURED );
	PrecacheScriptSound( TF_CTF_ENEMY_RETURNED );
	PrecacheScriptSound( TF_CTF_TEAM_STOLEN );
	PrecacheScriptSound( TF_CTF_TEAM_DROPPED );
	PrecacheScriptSound( TF_CTF_TEAM_CAPTURED );
	PrecacheScriptSound( TF_CTF_TEAM_RETURNED );

	PrecacheScriptSound( TF_AD_CAPTURED_SOUND );
	PrecacheScriptSound( TF_AD_ENEMY_STOLEN );
	PrecacheScriptSound( TF_AD_ENEMY_DROPPED );
	PrecacheScriptSound( TF_AD_ENEMY_CAPTURED );
	PrecacheScriptSound( TF_AD_ENEMY_RETURNED );
	PrecacheScriptSound( TF_AD_TEAM_STOLEN );
	PrecacheScriptSound( TF_AD_TEAM_DROPPED );
	PrecacheScriptSound( TF_AD_TEAM_CAPTURED );
	PrecacheScriptSound( TF_AD_TEAM_RETURNED );

	PrecacheScriptSound( TF_INVADE_ENEMY_STOLEN );
	PrecacheScriptSound( TF_INVADE_ENEMY_DROPPED );
	PrecacheScriptSound( TF_INVADE_ENEMY_CAPTURED );
	PrecacheScriptSound( TF_INVADE_TEAM_STOLEN );
	PrecacheScriptSound( TF_INVADE_TEAM_DROPPED );
	PrecacheScriptSound( TF_INVADE_TEAM_CAPTURED );
	PrecacheScriptSound( TF_INVADE_FLAG_RETURNED );
}

#ifndef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSpawner::OnPreDataChanged( DataUpdateType_t updateType )
{
	m_nOldWeaponSpawnerStatus = m_nWeaponSpawnerStatus;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSpawner::OnDataChanged( DataUpdateType_t updateType )
{
	if ( m_nOldWeaponSpawnerStatus != m_nWeaponSpawnerStatus )
	{
		IGameEvent *pEvent = gameeventmanager->CreateEvent( "weaponspawnerstatus_update" );
		if ( pEvent )
		{
			gameeventmanager->FireEventClientSide( pEvent );
		}
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::Spawn( void )
{
	// Precache the model and sounds.  Set the flag model.
	Precache();
	SetModel( TF_WEAPON_MODEL );

	// Set the flag solid and the size for touching.
	SetSolid( SOLID_BBOX );
	SetSolidFlags( FSOLID_NOT_SOLID | FSOLID_TRIGGER );
	SetSize( vec3_origin, vec3_origin );

	// Bloat the box for player pickup
	CollisionProp()->UseTriggerBounds( true, 24 );

	// use the initial dynamic prop "m_bStartDisabled" setting to set our own m_bDisabled flag
#ifdef GAME_DLL
	m_bDisabled = m_bStartDisabled;
	m_bStartDisabled = false;

	// Don't allow the intelligence to fade.
	m_wsFadeScale = 0.0f;
#else
	m_bDisabled = false;
#endif

	// Base class spawn.
	BaseClass::Spawn();

#ifdef GAME_DLL
	// Save the starting position, so we can reset the flag later if need be.
	m_vecResetPos = GetAbsOrigin();
	m_vecResetAng = GetAbsAngles();

	SetWeaponSpawnerStatus( TF_WEAPONSPAWNERINFO_NONE );
	ResetWeaponSpawnerReturnTime();
	ResetWeaponSpawnerNeutralTime();

	m_bAllowOwnerPickup = true;
	m_hPrevOwner = NULL;

	m_bCaptured = false;

#else


#endif

	if ( m_bDisabled )
	{
		SetDisabled( true );
	}
	else
	{
		SetDisabled( false );
	}
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Activate( void )
{
	BaseClass::Activate();

	m_iOriginalTeam = GetTeamNumber();
	m_nSkin = ( GetTeamNumber() == TEAM_UNASSIGNED ) ? 2 : (GetTeamNumber() - 2);
}
#endif


//-----------------------------------------------------------------------------
// Purpose: Reset the flag position state.
//-----------------------------------------------------------------------------
void CWeaponSpawner::Reset( void )
{
#ifdef GAME_DLL
	// Set the flag position.
	SetAbsOrigin( m_vecResetPos );
	SetAbsAngles( m_vecResetAng );

	// No longer dropped, if it was.
	SetWeaponSpawnerStatus( TF_WEAPONSPAWNERINFO_NONE );
	ResetWeaponSpawnerReturnTime();
	ResetWeaponSpawnerNeutralTime();

	m_bAllowOwnerPickup = true;
	m_hPrevOwner = NULL;

	if ( m_nGameType == TF_WEAPONSPAWNERTYPE_INVADE )
	{
		ChangeTeam( m_iOriginalTeam );
		m_nSkin = ( GetTeamNumber() == TEAM_UNASSIGNED ) ? 2 : (GetTeamNumber() - 2);
	}

	SetMoveType( MOVETYPE_NONE );
#endif 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSpawner::ResetMessage( void )
{
#ifdef GAME_DLL

		// Returned sound
		CPASAttenuationFilter filter( this, TF_CTF_WEAPONSPAWNERSPAWN );
		EmitSound( filter, entindex(), TF_CTF_WEAPONSPAWNERSPAWN );
	}
	else if ( m_nGameType == TF_WEAPONSPAWNERTYPE_ATTACK_DEFEND )
	{

	}
	else if ( m_nGameType == TF_WEAPONSPAWNERTYPE_INVADE )
	{
		
	}

	// Output.
	m_outputOnReturn.FireOutput( this, this );

	if ( m_hReturnIcon.Get() )
	{
		UTIL_Remove( m_hReturnIcon );
		m_hReturnIcon = NULL;
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::WeaponSpawnerTouch( CBaseEntity *pOther )
{
	// Is the Weapon Spawner enabled?
	if ( IsDisabled() )
	{
		return;
	}

	// The the touch from a live player.
	if ( !pOther->IsPlayer() || !pOther->IsAlive() )
	{
		return;
	}

#ifdef GAME_DLL
	// Don't let the person who threw this flag pick it up until it hits the ground.
	// This way we can throw the flag to people, but not touch it as soon as we throw it ourselves
	if(  m_hPrevOwner.Get() && m_hPrevOwner.Get() == pOther && m_bAllowOwnerPickup == false )
	{
		return;
	}
#endif

	// Does my team own this flag? If so, no touch.
	if ( m_nGameType == pOther->GetTeamNumber() == GetTeamNumber() )
	{
		return;
	}

	// Can't pickup flags during WaitingForPlayers
	if ( TFGameRules()->IsInWaitingForPlayers() )
		return;

	// Get the touching player.
	CTFPlayer *pPlayer = ToTFPlayer( pOther );
	if ( !pPlayer )
	{
		return;
	}

#ifdef GAME_DLL
	if ( PointInRespawnRoom(pPlayer,pPlayer->WorldSpaceCenter()) )
		return;
#endif

	// Pick up the flag.
	PickUp( pPlayer, true );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::PickUp( CTFPlayer *pPlayer, bool bInvisible )
{
	// Is the flag enabled?
	if ( IsDisabled() )
		return;

	if (!TFGameRules()->WeaponSpawnersMayBeUsed())
		return;

#ifdef GAME_DLL
	if ( !m_bAllowOwnerPickup )
	{
		if ( m_hPrevOwner.Get() && m_hPrevOwner.Get() == pPlayer ) 
		{
			return;
		}
	}

#endif

	// Call into the base class pickup.
	BaseClass::PickUp( pPlayer, false );

	pPlayer->TeamFortress_SetSpeed();

#ifdef GAME_DLL
	
	// Update the parent to set the correct place on the model to attach the flag.
	int iAttachment = pPlayer->LookupAttachment( "flag" );
	if( iAttachment != -1 )
	{
		SetParent( pPlayer, iAttachment );
		SetLocalOrigin( vec3_origin );
		SetLocalAngles( vec3_angle );
	}

	// Remove the touch function.
	SetTouch( NULL );

	m_hPrevOwner = pPlayer;
	m_bAllowOwnerPickup = true;

	SetWeaponSpawnerStatus( TF_WEAPONSPAWNERINFO_STOLEN );
	ResetWeaponSpawnerReturnTime();

	IGameEvent *event = gameeventmanager->CreateEvent( "teamplay_weaponspawner_event" );
	if ( event )
	{
		event->SetInt( "player", pPlayer->entindex() );
		event->SetInt( "eventtype", TF_WEAPONSPAWNEREVENT_PICKUP );
		event->SetInt( "priority", 8 );
		gameeventmanager->FireEvent( event );
	}

//	pPlayer->SpeakConceptIfAllowed( MP_CONCEPT_FLAGPICKUP );

	// Output.
	m_outputOnPickUp.FireOutput( this, this );

	if ( m_hReturnIcon.Get() )
	{
		UTIL_Remove( m_hReturnIcon );
		m_hReturnIcon = NULL;
	}

#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::Capture( CTFPlayer *pPlayer, int nCapturePoint )
{
	// Is the flag enabled?
	if ( IsDisabled() )
		return;

#ifdef GAME_DLL

	IGameEvent *event = gameeventmanager->CreateEvent( "teamplay_weaponspawner_event" );
	if ( event )
	{
		event->SetInt( "player", pPlayer->entindex() );
		event->SetInt( "eventtype", TF_WEAPONSPAWNEREVENT_CAPTURE );
		event->SetInt( "priority", 9 );
		gameeventmanager->FireEvent( event );
	}

	SetWeaponSpawnerStatus( TF_WEAPONSPAWNERINFO_NONE );
	ResetWeaponSpawnerReturnTime();
	ResetWeaponSpawnerNeutralTime();

	// Reset the flag.
	BaseClass::Drop( pPlayer, true );

	Reset();

	pPlayer->TeamFortress_SetSpeed();
	
	// Output.
	m_outputOnCapture.FireOutput( this, this );

	m_bCaptured = true;
	SetNextThink( gpGlobals->curtime + TF_WEAPONSPAWNER_THINK_TIME );

	if ( TFGameRules()->InStalemate() )
	{
		// whoever capped the flag is the winner, give them enough caps to win
		CTFTeam *pTeam = pPlayer->GetTFTeam();
		if ( !pTeam )
			return;

		// if we still need more caps to trigger a win, give them to us
		if ( pTeam->GetWeaponSpawnerCaptures() < tf_weaponspawner_caps_per_round.GetInt() )
		{
			pTeam->SetWeaponSpawnerCaptures( tf_weaponspawner_caps_per_round.GetInt() );
		}	
	}

#endif
}

//-----------------------------------------------------------------------------
// Purpose: A player drops the flag.
//-----------------------------------------------------------------------------
void CWeaponSpawner::Drop( CTFPlayer *pPlayer, bool bVisible,  bool bThrown /*= false*/, bool bMessage /*= true*/ )
{
	// Is the flag enabled?
	if ( IsDisabled() )
		return;

	// Call into the base class drop.
	BaseClass::Drop( pPlayer, bVisible );

	pPlayer->TeamFortress_SetSpeed();

#ifdef GAME_DLL

	if ( bThrown )
	{
		m_bAllowOwnerPickup = false;
		m_wpOwnerPickupTime = gpGlobals->curtime + TF_WEAPONSPAWNER_OWNER_PICKUP_TIME;
	}

	Vector vecStart = GetAbsOrigin();
	Vector vecEnd = vecStart;
	vecEnd.z -= 8000.0f;
	trace_t trace;
	UTIL_TraceHull( vecStart, vecEnd, WorldAlignMins(), WorldAlignMaxs(), MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &trace );
	SetAbsOrigin( trace.endpos );

	if ( m_nGameType == TF_WEAPONSPAWNERTYPE_CTF )
	{
		SetWeaponSpawnerReturnIn( TF_CTF_RESET_TIME );
	}
	else if ( m_nGameType == TF_WEAPONSPAWNERTYPE_INVADE )
	{
		SetWeaponSpawnerReturnIn( TF_INVADE_RESET_TIME );
		SetWeaponSpawnerNeutralIn( TF_INVADE_NEUTRAL_TIME );
	}
	else if ( m_nGameType == TF_WEAPONSPAWNERTYPE_ATTACK_DEFEND )
	{
		SetWeaponSpawnerReturnIn( TF_AD_RESET_TIME );
	}

	// Reset the flag's angles.
	SetAbsAngles( m_vecResetAng );

	// Reset the touch function.
	SetTouch( &CWeaponSpawner::WeaponSpawnerTouch );

	SetWeaponSpawnerStatus( TF_WEAPONSPAWNERINFO_DROPPED );

	// Output.
	m_outputOnDrop.FireOutput( this, this );

	m_hReturnIcon = CBaseEntity::Create( "dm_weaponspawner_return_icon", GetAbsOrigin() + Vector(0,0,cl_weaponspawner_return_height.GetFloat()), vec3_angle, this );
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponSpawner::IsDropped( void )
{ 
	return ( m_nWeaponSpawnerStatus == TF_WEAPONSPAWNERINFO_DROPPED ); 
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponSpawner::IsHome( void )
{ 
	return ( m_nWeaponSpawnerStatus == TF_WEAPONSPAWNERINFO_NONE ); 
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponSpawner::IsStolen( void )
{ 
	return ( m_nWeaponSpawnerStatus == TF_WEAPONSPAWNERINFO_TAKEN ); 
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponSpawner::IsDisabled( void )
{
	return m_bDisabled;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::SetDisabled( bool bDisabled )
{
	m_bDisabled = bDisabled;

	if ( bDisabled )
	{
		AddEffects( EF_NODRAW );

		SetTouch( NULL );
		SetThink( NULL );
	}
	else
	{
		RemoveEffects( EF_NODRAW );

		SetTouch( &CWeaponSpawner::WeaponSpawnerTouch );
		SetThink( &CWeaponSpawner::Think );
		SetNextThink( gpGlobals->curtime );
	}
}

//-----------------------------------------------------------------------------------------------
// GAME DLL Functions
//-----------------------------------------------------------------------------------------------
#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::Think( void )
{
	// Is the flag enabled?
	if ( IsDisabled() )
		return;

	if ( !TFGameRules()->WeaponSpawnerMayBeCapped() )
	{
		SetNextThink( gpGlobals->curtime + TF_WEAPONSPAWNER_THINK_TIME );
		return;
	}

	if ( m_bCaptured )
	{
		m_bCaptured = false;
		SetTouch( &CWeaponSpawner::WeaponSpawnerTouch );
	}

	if ( IsDropped() )
	{

	}

	SetNextThink( gpGlobals->curtime + TF_WEAPONSPAWNER_THINK_TIME );
}

//-----------------------------------------------------------------------------
// Purpose: Sets the flag status
//-----------------------------------------------------------------------------
void CWeaponSpawner::SetWeaponSpawnerStatus( int iStatus )
{ 
	MDLCACHE_CRITICAL_SECTION();

	m_nWeaponSpawnerStatus = iStatus; 

	switch ( m_nWeaponSpawnerStatus )
	{
	case TF_WEAPONSPAWNERINFO_NONE:
	case TF_WEAPONSPAWNERINFO_DROPPED:
		ResetSequence( LookupSequence("spin") );	// set spin animation if it's not being held
		break;
	case TF_WEAPONSPAWNERINFO_STOLEN:
		ResetSequence( LookupSequence("idle") );	// set idle animation if it is being held
		break;
	default:
		AssertOnce( false );	// invalid stats
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::InputEnable( inputdata_t &inputdata )
{
	SetDisabled( false );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSpawner::InputDisable( inputdata_t &inputdata )
{
	SetDisabled( true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSpawner::InputRoundActivate( inputdata_t &inputdata )
{
	CTFPlayer *pPlayer = ToTFPlayer( m_hPrevOwner.Get() );

	// If the player has a capture flag, drop it.
	if ( pPlayer && pPlayer->HasItem() && ( pPlayer->GetItem() == this ) )
	{
		Drop( pPlayer, true, false, false );
	}

	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: Always transmitted to clients
//-----------------------------------------------------------------------------
int CWeaponSpawner::UpdateTransmitState()
{
	// ALWAYS transmit to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}


#else

float CWeaponSpawner::GetReturnProgress()
{
	float wsEventTime = max( m_wsResetTime.m_Value, m_wsNeutralTime.m_Value );

	return ( 1.0 - ( ( wsEventTime - gpGlobals->curtime ) / m_wsMaxResetTime ) );
}


void CWeaponSpawner::Simulate( void )
{
	BaseClass::Simulate();

}

#endif


LINK_ENTITY_TO_CLASS( dm_weapon_spawner_return_icon, CWeaponSpawnerReturnIcon );

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSpawnerReturnIcon, DT_WeaponSpawnerReturnIcon )

BEGIN_NETWORK_TABLE( CWeaponSpawnerReturnIcon, DT_WeaponSpawnerReturnIcon )
END_NETWORK_TABLE()

CWeaponSpawnerReturnIcon::CWeaponSpawnerReturnIcon()
{
#ifdef CLIENT_DLL
	m_pReturnProgressMaterial_Empty = NULL;
	m_pReturnProgressMaterial_Full = NULL;
#endif
}

#ifdef GAME_DLL

void CWeaponSpawnerReturnIcon::Spawn( void )
{
	BaseClass::Spawn();

	UTIL_SetSize( this, Vector(-8,-8,-8), Vector(8,8,8) );

	CollisionProp()->SetCollisionBounds( Vector( -50, -50, -50 ), Vector( 50, 50, 50 ) );
}

int CWeaponSpawnerReturnIcon::UpdateTransmitState( void )
{
	return SetTransmitState( FL_EDICT_PVSCHECK );
}
#endif

#ifdef CLIENT_DLL

typedef struct
{
	float maxProgress;

	float vert1x;
	float vert1y;
	float vert2x;
	float vert2y;

	int swipe_dir_x;
	int swipe_dir_y;
} weapon_progress_segment_t;


// This defines the properties of the 8 circle segments
// in the circular progress bar.
weapon_progress_segment_t Segments[8] = 
{
	{ 0.125, 0.5, 0.0, 1.0, 0.0, 1, 0 },
	{ 0.25,	 1.0, 0.0, 1.0, 0.5, 0, 1 },
	{ 0.375, 1.0, 0.5, 1.0, 1.0, 0, 1 },
	{ 0.50,	 1.0, 1.0, 0.5, 1.0, -1, 0 },
	{ 0.625, 0.5, 1.0, 0.0, 1.0, -1, 0 },
	{ 0.75,	 0.0, 1.0, 0.0, 0.5, 0, -1 },
	{ 0.875, 0.0, 0.5, 0.0, 0.0, 0, -1 },
	{ 1.0,	 0.0, 0.0, 0.5, 0.0, 1, 0 },
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
RenderGroup_t CWeaponSpawnerReturnIcon::GetRenderGroup( void ) 
{	
	return RENDER_GROUP_TRANSLUCENT_ENTITY;	
}

void CWeaponSpawnerReturnIcon::GetRenderBounds( Vector& theMins, Vector& theMaxs )
{
	theMins.Init( -20, -20, -20 );
	theMaxs.Init(  20,  20,  20 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CWeaponSpawnerReturnIcon::DrawModel( int flags )
{
	int nRetVal = BaseClass::DrawModel( flags );
	
	DrawReturnProgressBar();

	return nRetVal;
}

//-----------------------------------------------------------------------------
// Purpose: Draw progress bar above the flag indicating when it will return
//-----------------------------------------------------------------------------
void CWeaponSpawnerReturnIcon::DrawReturnProgressBar( void )
{
	CWeaponSpawner *pWeaponSpawner = dynamic_cast< CWeaponSpawner * > ( GetOwnerEntity() );

	if ( !pWeaponSpawner )
		return;

	// Don't draw if this flag is not going to reset
	if ( pWeaponSpawner->GetMaxResetTime() <= 0 )
		return;

	if ( !m_pReturnProgressMaterial_Full )
	{
		m_pReturnProgressMaterial_Full = materials->FindMaterial( "VGUI/flagtime_full", TEXTURE_GROUP_VGUI );
	}

	if ( !m_pReturnProgressMaterial_Empty )
	{
		m_pReturnProgressMaterial_Empty = materials->FindMaterial( "VGUI/flagtime_empty", TEXTURE_GROUP_VGUI );
	}

	if ( !m_pReturnProgressMaterial_Full || !m_pReturnProgressMaterial_Empty )
	{
		return;
	}

	CMatRenderContextPtr pRenderContext( materials );

	Vector vOrigin = GetAbsOrigin();
	QAngle vAngle = vec3_angle;

	// Align it towards the viewer
	Vector vUp = CurrentViewUp();
	Vector vRight = CurrentViewRight();
	if ( fabs( vRight.z ) > 0.95 )	// don't draw it edge-on
		return;

	vRight.z = 0;
	VectorNormalize( vRight );

	float flSize = cl_weaponspawner_return_size.GetFloat();

	unsigned char ubColor[4];
	ubColor[3] = 255;

	switch( pWeaponSpawner->GetTeamNumber() )
	{
	case TF_TEAM_RED:
		ubColor[0] = 255;
		ubColor[1] = 0;
		ubColor[2] = 0;
		break;
	case TF_TEAM_BLUE:
		ubColor[0] = 0;
		ubColor[1] = 0;
		ubColor[2] = 255;
		break;
	case TF_TEAM_MERCENARY:
		ubColor[0] = 128;
		ubColor[1] = 0;
		ubColor[2] = 128;
		break;
	default:
		ubColor[0] = 100;
		ubColor[1] = 100;
		ubColor[2] = 100;
		break;
	}

	// First we draw a quad of a complete icon, background
	CMeshBuilder meshBuilder;

	pRenderContext->Bind( m_pReturnProgressMaterial_Empty );
	IMesh *pMesh = pRenderContext->GetDynamicMesh();

	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	meshBuilder.Color4ubv( ubColor );
	meshBuilder.TexCoord2f( 0,0,0 );
	meshBuilder.Position3fv( (vOrigin + (vRight * -flSize) + (vUp * flSize)).Base() );
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv( ubColor );
	meshBuilder.TexCoord2f( 0,1,0 );
	meshBuilder.Position3fv( (vOrigin + (vRight * flSize) + (vUp * flSize)).Base() );
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv( ubColor );
	meshBuilder.TexCoord2f( 0,1,1 );
	meshBuilder.Position3fv( (vOrigin + (vRight * flSize) + (vUp * -flSize)).Base() );
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv( ubColor );
	meshBuilder.TexCoord2f( 0,0,1 );
	meshBuilder.Position3fv( (vOrigin + (vRight * -flSize) + (vUp * -flSize)).Base() );
	meshBuilder.AdvanceVertex();

	meshBuilder.End();

	pMesh->Draw();

	float wsProgress = pWeaponSpawner->GetReturnProgress();

	pRenderContext->Bind( m_pReturnProgressMaterial_Full );
	pMesh = pRenderContext->GetDynamicMesh();

	vRight *= flSize * 2;
	vUp *= flSize * -2;

	// Next we're drawing the circular progress bar, in 8 segments
	// For each segment, we calculate the vertex position that will draw
	// the slice.
	int i;
	for ( i=0;i<8;i++ )
	{
		if ( wsProgress < Segments[i].maxProgress )
		{
			CMeshBuilder meshBuilder_Full;

			meshBuilder_Full.Begin( pMesh, MATERIAL_TRIANGLES, 3 );

			// vert 0 is ( 0.5, 0.5 )
			meshBuilder_Full.Color4ubv( ubColor );
			meshBuilder_Full.TexCoord2f( 0, 0.5, 0.5 );
			meshBuilder_Full.Position3fv( vOrigin.Base() );
			meshBuilder_Full.AdvanceVertex();

			// Internal progress is the progress through this particular slice
			float internalProgress = RemapVal( wsProgress, Segments[i].maxProgress - 0.125, Segments[i].maxProgress, 0.0, 1.0 );
			internalProgress = clamp( internalProgress, 0.0, 1.0 );

			// Calculate the x,y of the moving vertex based on internal progress
			float swipe_x = Segments[i].vert2x - ( 1.0 - internalProgress ) * 0.5 * Segments[i].swipe_dir_x;
			float swipe_y = Segments[i].vert2y - ( 1.0 - internalProgress ) * 0.5 * Segments[i].swipe_dir_y;

			// vert 1 is calculated from progress
			meshBuilder_Full.Color4ubv( ubColor );
			meshBuilder_Full.TexCoord2f( 0, swipe_x, swipe_y );
			meshBuilder_Full.Position3fv( (vOrigin + (vRight * ( swipe_x - 0.5 ) ) + (vUp *( swipe_y - 0.5 ) ) ).Base() );
			meshBuilder_Full.AdvanceVertex();

			// vert 2 is ( Segments[i].vert1x, Segments[i].vert1y )
			meshBuilder_Full.Color4ubv( ubColor );
			meshBuilder_Full.TexCoord2f( 0, Segments[i].vert2x, Segments[i].vert2y );
			meshBuilder_Full.Position3fv( (vOrigin + (vRight * ( Segments[i].vert2x - 0.5 ) ) + (vUp *( Segments[i].vert2y - 0.5 ) ) ).Base() );
			meshBuilder_Full.AdvanceVertex();

			meshBuilder_Full.End();

			pMesh->Draw();
		}
	}
}

#endif
