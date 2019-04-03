#include "cbase.h"
#include "c_tf_player.h"
//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Powerup spawner
//
//=============================================================================//

#include "cbase.h"
#include "c_tf_player.h"
#include "view.h"
#include "tier0/memdbgon.h"
#include "glow_outline_effect.h"
#include "tf_gamerules.h"

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the Powerup Spawner
//-----------------------------------------------------------------------------
class C_CondPowerup : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_CondPowerup, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	void	ClientThink( void );
	void	Spawn( void );

private:

	QAngle absAngle;
	~C_CondPowerup();

private:

	CGlowObject		   *m_pGlowEffect;
	void	UpdateGlowEffect( void );
	void	DestroyGlowEffect( void );
	bool	m_bDisableShowOutline;
	bool	m_bShouldGlow;
};

// Inputs.
LINK_ENTITY_TO_CLASS( dm_powerup_spawner, C_CondPowerup );

IMPLEMENT_CLIENTCLASS_DT( C_CondPowerup, DT_CondPowerup, CCondPowerup )
RecvPropBool( RECVINFO( m_bDisableShowOutline ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Set initial angles 
//-----------------------------------------------------------------------------
void C_CondPowerup::Spawn( void )
{
	BaseClass::Spawn();

	UpdateGlowEffect();

	ClientThink();
}

//-----------------------------------------------------------------------------
// Purpose: Update angles every think
//-----------------------------------------------------------------------------
void C_CondPowerup::ClientThink( void )
{
	bool bShouldGlow = false;

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( pPlayer )
	{
		trace_t tr;
		UTIL_TraceLine( GetAbsOrigin(), pPlayer->EyePosition(), MASK_OPAQUE, this, COLLISION_GROUP_NONE, &tr );
		if ( tr.fraction == 1.0f )
		{
			bShouldGlow = true;
		}
	}

	if ( m_bShouldGlow != bShouldGlow )
	{
		m_bShouldGlow = bShouldGlow;
		UpdateGlowEffect();
	}


	SetNextClientThink( CLIENT_THINK_ALWAYS );
}


//-----------------------------------------------------------------------------
// Purpose: Update glow effect
//-----------------------------------------------------------------------------
void C_CondPowerup::UpdateGlowEffect( void )
{
	DestroyGlowEffect();

	if ( !m_bDisableShowOutline )
		m_pGlowEffect = new CGlowObject( this, TFGameRules()->GetTeamGlowColor(GetLocalPlayerTeam()), 1.0, true, true );

	if ( !m_bShouldGlow )
		m_pGlowEffect->SetAlpha( 0.0f );
}

void C_CondPowerup::DestroyGlowEffect( void )
{
	if ( m_pGlowEffect )
	{
		delete m_pGlowEffect;
		m_pGlowEffect = NULL;
	}
}

C_CondPowerup::~C_CondPowerup()
{
	DestroyGlowEffect();
}