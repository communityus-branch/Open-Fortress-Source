//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: TF's custom CPlayerResource
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tf_player.h"
#include "player_resource.h"
#include "tf_player_resource.h"
#include "tf_gamestats.h"
#include "tf_gamerules.h"
#include <coordsize.h>

// Datatable
IMPLEMENT_SERVERCLASS_ST( CTFPlayerResource, DT_TFPlayerResource )
	SendPropArray3( SENDINFO_ARRAY3( m_iTotalScore ), SendPropInt( SENDINFO_ARRAY( m_iTotalScore ), 12, SPROP_UNSIGNED ) ),
	SendPropArray3( SENDINFO_ARRAY3( m_iMaxHealth ), SendPropInt( SENDINFO_ARRAY( m_iMaxHealth ), 10, SPROP_UNSIGNED ) ),
	SendPropArray3( SENDINFO_ARRAY3( m_iPlayerClass ), SendPropInt( SENDINFO_ARRAY( m_iPlayerClass ), 5, SPROP_UNSIGNED ) ),
	SendPropArray3( SENDINFO_ARRAY3( m_vecColors ), SendPropVector( SENDINFO_ARRAY3( m_vecColors ), 12, SPROP_COORD ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tf_player_manager, CTFPlayerResource );

CTFPlayerResource::CTFPlayerResource( void )
{
	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFPlayerResource::UpdatePlayerData( void )
{
	int i;

	BaseClass::UpdatePlayerData();

	for ( i = 1 ; i <= gpGlobals->maxClients; i++ )
	{
		CTFPlayer *pPlayer = (CTFPlayer*)UTIL_PlayerByIndex( i );
		
		if ( pPlayer && pPlayer->IsConnected() )
		{			
			PlayerStats_t *pPlayerStats = CTF_GameStats.FindPlayerStats( pPlayer );
			if ( pPlayerStats ) 
			{
			m_iMaxHealth.Set( i, pPlayer->GetPlayerClass()->GetMaxHealth() );
			m_iPlayerClass.Set( i, pPlayer->GetPlayerClass()->GetClassIndex() );
				int iTotalScore = CTFGameRules::CalcPlayerScore( &pPlayerStats->statsAccumulated );
				m_iTotalScore.Set( i, iTotalScore );
				
				m_vecColors.Set( i, pPlayer->m_vecPlayerColor );
			}					
		}
	}
}

void CTFPlayerResource::Spawn( void )
{
	int i;

	for ( i = 0; i < MAX_PLAYERS + 1; i++ )
	{
		m_iTotalScore.Set( i, 0 );
		m_iMaxHealth.Set( i, TF_HEALTH_UNDEFINED );
		m_iPlayerClass.Set( i, TF_CLASS_UNDEFINED );
		m_vecColors.Set( i, Vector( 0.0, 0.0, 0.0 ) );
	}

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Gets a value from an array member
//-----------------------------------------------------------------------------
int CTFPlayerResource::GetTotalScore( int iIndex )
{
	CTFPlayer *pPlayer = (CTFPlayer*)UTIL_PlayerByIndex( iIndex );

	if ( pPlayer && pPlayer->IsConnected() )
	{	
		return m_iTotalScore[iIndex];
	}

	return 0;
}

Color CTFPlayerResource::GetPlayerColor( int iIndex )
{
	if ( m_vecColors[iIndex].x < 0 || m_vecColors[iIndex].y < 0 || m_vecColors[iIndex].z < 0 )
	{
		return Color( RandomFloat( 0.0f, 1.0f ) * 255.0, RandomFloat( 0.0f, 1.0f ) * 255.0, RandomFloat( 0.0f, 1.0f ) * 255.0, 255 );
	}
	
	return Color( m_vecColors[iIndex].x * 255.0, m_vecColors[iIndex].y * 255.0, m_vecColors[iIndex].z * 255.0, 255 );
}