#include "cbase.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IScheme.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "ofd_imagecoloredhudpanel.h"
#include "c_tf_player.h"
#include "tf_gamerules.h"
#include "c_playerresource.h"
#include "c_tf_playerresource.h"
#include "tf_shareddefs.h"

using namespace vgui;

DECLARE_BUILD_FACTORY( CTFImageColoredHudPanel );

extern ConVar ofd_coloredhud;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFImageColoredHudPanel::CTFImageColoredHudPanel(Panel *parent, const char *name) :ScalableImagePanel(parent, name)
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFImageColoredHudPanel::PaintBackground()
{
	C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>(g_PR);

	if ( !tf_PR || !ofd_coloredhud.GetBool() || ( TFGameRules() && !TFGameRules()->IsDMGamemode() ))
	{
		SetDrawColor(Color(0, 0, 0, 0));
	}
	else
	{
		int pTeam=GetLocalPlayerTeam();
		switch ( pTeam )
		{
			case TF_TEAM_BLUE:
				{
					SetDrawColor(Color(77, 116, 139, 255));
					break;
				}
			case TF_TEAM_RED:
				{
					SetDrawColor(Color(184, 59, 59, 255));
					break;
				}		
			case TF_TEAM_MERCENARY:
			{
				int iIndex = GetLocalPlayerIndex();
				SetDrawColor(tf_PR->GetPlayerColor(iIndex));
				break;
			}
		}
	}
	BaseClass::PaintBackground();
};
