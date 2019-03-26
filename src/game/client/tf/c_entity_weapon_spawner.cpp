//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//

#include "cbase.h"
#include "glow_outline_effect.h"
#include "c_tf_player.h"
#include "view.h"

#include "tier0/memdbgon.h"
/*
//-----------------------------------------------------------------------------
// Purpose: Spawn function for the ammopack
//-----------------------------------------------------------------------------
class C_WeaponSpawner : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_WeaponSpawner, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	C_WeaponSpawner();
	~C_WeaponSpawner();

	virtual void OnDataChanged( DataUpdateType_t type );

	void	ClientThink( void );
	void	UpdateGlowEffect( void );

private:

	CGlowObject		   *m_pGlowEffect;
	bool				m_bShouldGlow;
	bool				m_bShowOutline;
};

// Inputs.
LINK_ENTITY_TO_CLASS( dm_weapon_spawner, C_WeaponSpawner );

IMPLEMENT_CLIENTCLASS_DT( C_WeaponSpawner, DT_WeaponSpawner, CWeaponSpawner )
	RecvPropBool( RECVINFO( m_bShowOutline ) ),
END_RECV_TABLE()

C_WeaponSpawner::C_WeaponSpawner()
{
	m_pGlowEffect = NULL;
	m_bShouldGlow = false;
}

C_WeaponSpawner::~C_WeaponSpawner()
{
	delete m_pGlowEffect;
}

void C_WeaponSpawner::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}

void C_WeaponSpawner::ClientThink( void )
{
	// Outline disabled, so don't show
	if ( !m_bShowOutline )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
		return;
	}

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	bool m_bShowOutline = true;
	bool bTouchingPlayer = false;

	if ( pPlayer )
	{
		Vector vecPlayerOrigin = pPlayer->GetAbsOrigin();
		Vector vecPlayerMins = vecPlayerOrigin + pPlayer->GetPlayerMins();
		Vector vecPlayerMaxs = vecPlayerOrigin + pPlayer->GetPlayerMaxs();

		bTouchingPlayer = IsBoxIntersectingBox( GetAbsOrigin() + WorldAlignMins(), GetAbsOrigin() + WorldAlignMaxs(), vecPlayerMins, vecPlayerMaxs );

		// When the player picks it up, stop glowing
		if ( !m_bRespawning && !m_bDisabled )
		{
			trace_t tr;
			UTIL_TraceLine( GetAbsOrigin(), pPlayer->EyePosition(), MASK_OPAQUE, this, COLLISION_GROUP_NONE, &tr );
			if ( tr.fraction == 1.0f )
			{
				bShouldGlow = true;
			}
		}
	}

	if ( m_bShouldGlow != bShouldGlow || m_bTouchingPlayer != bTouchingPlayer )
	{
		m_bShouldGlow = bShouldGlow;
		m_bTouchingPlayer = bTouchingPlayer;
		UpdateGlowEffect();
	}

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}
*/