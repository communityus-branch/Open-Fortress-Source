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
	int		DrawModel( int flags );

private:

	~C_CondPowerup();

	private:

		CGlowObject		   *m_pGlowEffect;
		void	UpdateGlowEffect( void );
		void	DestroyGlowEffect( void );
		bool	m_bDisableShowOutline;
		bool	m_bRespawning;
		bool	bInitialDelay;
		int		iTeamNum;
		bool	m_bShouldGlow;
		float				fl_RespawnTime;
		float				m_flRespawnTick;
		float				fl_RespawnDelay;
		
		
	IMaterial	*m_pReturnProgressMaterial_Empty;		// For labels above players' heads.
	IMaterial	*m_pReturnProgressMaterial_Full;
};

ConVar hud_powerup_height( "hud_powerup_height", "35", FCVAR_CHEAT );

extern ConVar cl_flag_return_size;
extern ConVar ofd_color_r;
extern ConVar ofd_color_g;
extern ConVar ofd_color_b;

// Inputs.
LINK_ENTITY_TO_CLASS( dm_powerup_spawner, C_CondPowerup );

IMPLEMENT_CLIENTCLASS_DT( C_CondPowerup, DT_CondPowerup, CCondPowerup )
RecvPropBool( RECVINFO( m_bDisableShowOutline ) ),
RecvPropBool( RECVINFO( m_bRespawning ) ),
RecvPropBool( RECVINFO( bInitialDelay ) ),
RecvPropTime( RECVINFO( fl_RespawnTime ) ),
RecvPropTime( RECVINFO( m_flRespawnTick ) ),
RecvPropTime( RECVINFO( fl_RespawnDelay ) ),
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

typedef struct
{
	float maxProgress;

	float vert1x;
	float vert1y;
	float vert2x;
	float vert2y;

	int swipe_dir_x;
	int swipe_dir_y;
} progress_powerup_segment_t;


// This defines the properties of the 8 circle segments
// in the circular progress bar.
progress_powerup_segment_t Segments[8] = 
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
int C_CondPowerup::DrawModel( int flags )
{
	int nRetVal = BaseClass::DrawModel( flags );
	
	if ( !m_bRespawning )
		return nRetVal;
	
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
		return nRetVal;
	}

	CMatRenderContextPtr pRenderContext( materials );

	Vector vOrigin = GetAbsOrigin() + Vector(0,0,hud_powerup_height.GetFloat());
	QAngle vAngle = vec3_angle;

	// Align it towards the viewer
	Vector vUp = CurrentViewUp();
	Vector vRight = CurrentViewRight();
	if ( fabs( vRight.z ) > 0.95 )	// don't draw it edge-on
		return nRetVal;

	vRight.z = 0;
	VectorNormalize( vRight );

	float flSize = cl_flag_return_size.GetFloat();

	unsigned char ubColor[4];
	ubColor[3] = 255;
	float r, g, b;
	r = ofd_color_r.GetFloat();
	g = ofd_color_g.GetFloat();
	b = ofd_color_b.GetFloat();
	if ( r < TF_GLOW_COLOR_CLAMP && g < TF_GLOW_COLOR_CLAMP && b < TF_GLOW_COLOR_CLAMP )
	{
		float maxi = max(max(r, g), b);
		maxi = TF_GLOW_COLOR_CLAMP - maxi;
		r += maxi;
		g += maxi;
		b += maxi;
	}
	
	switch( GetLocalPlayerTeam() )
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
		ubColor[0] = r;
		ubColor[1] = g;
		ubColor[2] = b;
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
	float RespawnTime =( bInitialDelay ) ? fl_RespawnDelay : fl_RespawnTime;
	float flProgress = ( m_flRespawnTick - gpGlobals->curtime ) / RespawnTime;
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
		if ( flProgress < Segments[i].maxProgress )
		{
			CMeshBuilder meshBuilder_Full;

			meshBuilder_Full.Begin( pMesh, MATERIAL_TRIANGLES, 3 );

			// vert 0 is ( 0.5, 0.5 )
			meshBuilder_Full.Color4ubv( ubColor );
			meshBuilder_Full.TexCoord2f( 0, 0.5, 0.5 );
			meshBuilder_Full.Position3fv( vOrigin.Base() );
			meshBuilder_Full.AdvanceVertex();

			// Internal progress is the progress through this particular slice
			float internalProgress = RemapVal( flProgress, Segments[i].maxProgress - 0.125, Segments[i].maxProgress, 0.0, 1.0 );
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

	return nRetVal;
}