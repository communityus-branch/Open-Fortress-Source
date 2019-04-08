//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "tf_dropped_weapon.h"
#include "tf_shareddefs.h"
#include "ammodef.h"
#include "tf_gamerules.h"
#include "explode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//----------------------------------------------

// Network table.
IMPLEMENT_SERVERCLASS_ST( CTFDroppedWeapon, DT_DroppedWeapon )
	SendPropVector( SENDINFO( m_vecInitialVelocity ), -1, SPROP_NOSCALE ),
END_SEND_TABLE()

BEGIN_DATADESC( CTFDroppedWeapon )
	DEFINE_THINKFUNC( FlyThink ),
	DEFINE_ENTITYFUNC( PackTouch ),
END_DATADESC();

LINK_ENTITY_TO_CLASS( tf_dropped_weapon, CTFDroppedWeapon );

PRECACHE_REGISTER( tf_dropped_weapon );

void CTFDroppedWeapon::Spawn( void )
{
	Precache();
	SetModel( STRING( GetModelName() ) );
	BaseClass::Spawn();

	SetNextThink( gpGlobals->curtime + 0.75f );
	SetThink( &CTFDroppedWeapon::FlyThink );

	SetTouch( &CTFDroppedWeapon::PackTouch );

	m_flCreationTime = gpGlobals->curtime;

	// no pickup until flythink
	m_bAllowOwnerPickup = false;

	// no ammo to start
	memset( m_iAmmo, 0, sizeof(m_iAmmo) );

	// Die in 30 seconds
	SetContextThink( &CBaseEntity::SUB_Remove, gpGlobals->curtime + 30, "DieContext" );

	if ( IsX360() )
	{
		RemoveEffects( EF_ITEM_BLINK );
	}
}

void CTFDroppedWeapon::Precache( void )
{
}

CTFDroppedWeapon *CTFDroppedWeapon::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszModelName )
{
	CTFDroppedWeapon *pDroppedWeapon = static_cast<CTFDroppedWeapon*>( CBaseAnimating::CreateNoSpawn( "tf_dropped_weapon", vecOrigin, vecAngles, pOwner ) );
/*	if ( pDroppedWeapon )
	{
		pDroppedWeapon->SetModelName( AllocPooledString( pszModelName ) );
		pDroppedWeapon->SetWeaponID(  )
		DispatchSpawn( pDroppedWeapon );
	}
*/
	return pDroppedWeapon;
}

void CTFDroppedWeapon::SetInitialVelocity( Vector &vecVelocity )
{ 
	m_vecInitialVelocity = vecVelocity;
}

int CTFDroppedWeapon::GiveAmmo( int iCount, int iAmmoType )
{
	if (iAmmoType == -1 || iAmmoType >= TF_AMMO_COUNT )
	{
		Msg("ERROR: Attempting to give unknown ammo type (%d)\n", iAmmoType);
		return 0;
	}

	m_iAmmo[iAmmoType] += iCount;

	return iCount;
}

void CTFDroppedWeapon::FlyThink( void )
{
	m_bAllowOwnerPickup = true;
}

void CTFDroppedWeapon::PackTouch( CBaseEntity *pOther )
{
	Assert( pOther );

	if( !pOther->IsPlayer() )
		return;

	if( !pOther->IsAlive() )
		return;	
	
	//Don't let the person who threw this ammo pick it up until it hits the ground.
	//This way we can throw ammo to people, but not touch it as soon as we throw it ourselves
	if( GetOwnerEntity() == pOther && m_bAllowOwnerPickup == false )
		return;

//	CBasePlayer *pPlayer = ToBasePlayer( pOther );

//	Assert( pPlayer );

//	bool bSuccess = true;
//	CTFWeaponBase *pWeapon = (CTFWeaponBase *)pPlayer->GiveNamedItem( STRING(m_iszWeaponName) );	
/*	for ( int iWeapon = 0; iWeapon < TF_WEAPON_COUNT; ++iWeapon )
	{		
		CTFWeaponBase *pCarriedWeapon = (CTFWeaponBase *)pPlayer->GetWeapon( iWeapon );
		if ( pCarriedWeapon == pWeapon ) 
		{
			bSuccess=false;
		}
	}

	if ( bSuccess )
	{
//		EmitSound( filter, entindex(), STRING(m_iszPickupSound) );
		pWeapon->GiveTo( pPlayer );		
		UTIL_Remove( this );
	}
*/
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
unsigned int CTFDroppedWeapon::PhysicsSolidMaskForEntity( void ) const
{ 
	return BaseClass::PhysicsSolidMaskForEntity() | CONTENTS_DEBRIS;
}