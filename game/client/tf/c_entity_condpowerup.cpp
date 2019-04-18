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

	~C_CondPowerup();

private:

	CGlowObject		   *m_pGlowEffect;
	void	UpdateGlowEffect( void );
	void	DestroyGlowEffect( void );
	bool	m_bDisableShowOutline;
	bool	m_bRespawning;
	int		iTeamNum;
	bool	m_bShouldGlow;
};

// Inputs.
LINK_ENTITY_TO_CLASS( dm_powerup_spawner, C_CondPowerup );

IMPLEMENT_CLIENTCLASS_DT( C_CondPowerup, DT_CondPowerup, CCondPowerup )
RecvPropBool( RECVINFO( m_bDisableShowOutline ) ),
RecvPropBool( RECVINFO( m_bRespawning ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Set initial team 
//-----------------------------------------------------------------------------
void C_CondPowerup::Spawn( void )
{
	BaseClass::Spawn();
	iTeamNum = TEAM_INVALID;
	m_bShouldGlow = false;

	UpdateGlowEffect();
	
	ClientThink();
}

//-----------------------------------------------------------------------------
// Purpose: Update glow every think
//-----------------------------------------------------------------------------
void C_CondPowerup::ClientThink( void )
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	// If old team does not equal new team then update glow with new glow color
	if (pPlayer &&  pPlayer->GetTeamNumber() != iTeamNum)
	{
		iTeamNum = pPlayer->GetTeamNumber();
		UpdateGlowEffect();
		DevMsg("Updated glow effect on powerup spawner \n");
	}

	bool bShouldGlow = !m_bRespawning;

	//if the shouldglow changed update the glow
	if ( bShouldGlow != m_bShouldGlow ) 
	{
		m_bShouldGlow = bShouldGlow;
		UpdateGlowEffect();
		DevMsg("Updated glow effect on powerup spawner \n");
	}

	SetNextClientThink(CLIENT_THINK_ALWAYS);
}


//-----------------------------------------------------------------------------
// Purpose: Update glow effect
//-----------------------------------------------------------------------------
void C_CondPowerup::UpdateGlowEffect( void )
{
	DestroyGlowEffect();
	
	if ( !m_bDisableShowOutline && m_bShouldGlow )
		m_pGlowEffect = new CGlowObject( this, TFGameRules()->GetTeamGlowColor(GetLocalPlayerTeam()), 1.0, true, true );
/*
	if ( !m_bShouldGlow && m_pGlowEffect )
		m_pGlowEffect->SetAlpha( 0.0f );
*/
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