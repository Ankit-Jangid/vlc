/*****************************************************************************
 * iteminfo.cpp : wxWindows plugin for vlc
 *****************************************************************************
 * Copyright (C) 2000-2001 VideoLAN
 * $Id: iteminfo.cpp,v 1.1 2003/10/06 16:23:30 zorglub Exp $
 *
 * Authors: Cl�ment Stenac <zorglub@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#include <stdlib.h>                                      /* malloc(), free() */
#include <errno.h>                                                 /* ENOMEM */
#include <string.h>                                            /* strerror() */
#include <stdio.h>

#include <vlc/vlc.h>

#ifdef WIN32                                                 /* mingw32 hack */
#undef Yield
#undef CreateDialog
#endif

/* Let vlc take care of the i18n stuff */
#define WXINTL_NO_GETTEXT_MACRO

#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>

#include <vlc/intf.h>

#include "wxwindows.h"

#ifndef wxRB_SINGLE
#   define wxRB_SINGLE 0
#endif

/*****************************************************************************
 * Event Table.
 *****************************************************************************/

/* IDs for the controls and the menu commands */
enum
{
    Uri_Event,
    Name_Event,
    Author_Event,
    Enabled_Event,
};

BEGIN_EVENT_TABLE(ItemInfoDialog, wxDialog)
    /* Button events */
    EVT_BUTTON(wxID_OK, ItemInfoDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, ItemInfoDialog::OnCancel)

    /* Events generated by the panels */

END_EVENT_TABLE()

/*****************************************************************************
 * Constructor.
 *****************************************************************************/
ItemInfoDialog::ItemInfoDialog( intf_thread_t *_p_intf,
                                playlist_item_t *_p_item,
                                wxWindow* _p_parent ):
    wxDialog( _p_parent, -1, wxU(_("Playlist Item options")),
             wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE )
{
    /* Initializations */
    p_intf = _p_intf;
    p_parent = _p_parent;
    p_item = _p_item;
    SetIcon( *p_intf->p_sys->p_icon );

    /* Create a panel to put everything in */
    wxPanel *panel = new wxPanel( this, -1 );
    panel->SetAutoLayout( TRUE );

    /* Create the standard info panel */
    wxPanel *info_panel = InfoPanel( panel );

    /* Create the group panel */
    wxPanel *group_panel = GroupPanel( panel );

    /* Separation */
    wxStaticLine *static_line = new wxStaticLine( panel, wxID_OK );

    /* Create the buttons */
    wxButton *ok_button = new wxButton( panel, wxID_OK, wxU(_("OK")) );
    ok_button->SetDefault();
    wxButton *cancel_button = new wxButton( panel, wxID_CANCEL,
                                            wxU(_("Cancel")) );

    /* Place everything in sizers */
    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    button_sizer->Add( ok_button, 0, wxALL, 5 );
    button_sizer->Add( cancel_button, 0, wxALL, 5 );
    button_sizer->Layout();
    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *panel_sizer = new wxBoxSizer( wxVERTICAL );
    panel_sizer->Add( info_panel, 0, wxEXPAND | wxALL, 5 );
    panel_sizer->Add( group_panel, 0, wxEXPAND | wxALL, 5 );
    panel_sizer->Add( static_line, 0, wxEXPAND | wxALL, 5 );
    panel_sizer->Add( button_sizer, 0, wxALIGN_LEFT | wxALIGN_BOTTOM |
                      wxALL, 5 );
    panel_sizer->Layout();
    panel->SetSizerAndFit( panel_sizer );
    main_sizer->Add( panel, 1, wxGROW, 0 );
    main_sizer->Layout();
    SetSizerAndFit( main_sizer );
}

ItemInfoDialog::~ItemInfoDialog()
{
}

/*****************************************************************************
 * Private methods.
 *****************************************************************************/
wxPanel *ItemInfoDialog::InfoPanel( wxWindow* parent )
{
    wxPanel *panel = new wxPanel( parent, -1, wxDefaultPosition,
                                  wxDefaultSize );

    wxStaticBox *panel_box = new wxStaticBox( panel, -1,
                                   wxU(_("Item informations")) );
    wxStaticBoxSizer *panel_sizer = new wxStaticBoxSizer( panel_box,
                                                          wxVERTICAL );

    info_subpanel = new wxPanel( panel, -1 );

    wxFlexGridSizer *subpanel_sizer =
                    new wxFlexGridSizer( 3, 1 , 0 , 0 );

    /* URI Textbox */
    wxStaticText *uri_label =
           new wxStaticText(info_subpanel, -1, wxU(_("URI")) );

    uri_text =  new wxTextCtrl( info_subpanel, Uri_Event,
                                wxT(p_item->psz_uri),
                                wxDefaultPosition, wxSize( 300, -1 ),
                                wxTE_PROCESS_ENTER);

    subpanel_sizer->Add( uri_label, 0, wxALIGN_LEFT |
                         wxALIGN_CENTER_VERTICAL );
    subpanel_sizer->Add( uri_text, 0, wxALIGN_RIGHT |
                         wxALIGN_CENTER_VERTICAL );


    /* Name Textbox */
    wxStaticText *name_label =
           new wxStaticText(info_subpanel, -1, wxU(_("Name")) );

    name_text =
                   new wxTextCtrl( info_subpanel, Uri_Event,
                                   wxT(p_item->psz_name),
                                   wxDefaultPosition, wxSize( 300, -1 ),
                                   wxTE_PROCESS_ENTER);

    subpanel_sizer->Add( name_label, 0, wxALIGN_LEFT |
                         wxALIGN_CENTER_VERTICAL );
    subpanel_sizer->Add( name_text, 0, wxALIGN_RIGHT |
                         wxALIGN_CENTER_VERTICAL );

    /* Author Textbox */
    wxStaticText *author_label =
           new wxStaticText(info_subpanel, -1, wxU(_("Author")) );

    author_text =
                   new wxTextCtrl( info_subpanel, Uri_Event,
                                   wxT(p_item->psz_author),
                                   wxDefaultPosition, wxSize( 300, -1 ),
                                   wxTE_PROCESS_ENTER);

    subpanel_sizer->Add( author_label, 0, wxALIGN_LEFT |
                         wxALIGN_CENTER_VERTICAL );
    subpanel_sizer->Add( author_text, 0, wxALIGN_RIGHT |
                         wxALIGN_CENTER_VERTICAL );

    info_subpanel->SetSizerAndFit( subpanel_sizer );

    /* Stuff everything into the main panel */
    panel_sizer->Add( info_subpanel, 1,
                      wxEXPAND | wxALIGN_LEFT |
                      wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    panel->SetSizerAndFit( panel_sizer );

    return panel;
}

wxPanel *ItemInfoDialog::GroupPanel( wxWindow* parent )
{
    wxPanel *panel = new wxPanel( parent, -1, wxDefaultPosition,
                                  wxDefaultSize );

    wxStaticBox *panel_box = new wxStaticBox( panel, -1,
                                   wxU(_("Group Info")) );

    wxStaticBoxSizer *panel_sizer = new wxStaticBoxSizer( panel_box,
                                                         wxVERTICAL);

    wxBoxSizer *subpanel_sizer;

    group_subpanel = new wxPanel( panel, -1 );

    subpanel_sizer = new wxBoxSizer( wxVERTICAL) ;

    enabled_checkbox = new wxCheckBox( group_subpanel,
                                     -1,
                                     wxU(_("Item enabled")) );

    enabled_checkbox->SetValue( p_item->b_enabled);

    wxStaticText *group_label = new wxStaticText( group_subpanel,
                                        -1, wxU(_("Group")) );
    group_spin = new wxSpinCtrl( group_subpanel,
                                             -1 );
    group_spin->SetValue( p_item->i_group > 0 ? p_item->i_group : 0);

    subpanel_sizer->Add( enabled_checkbox, 0, wxALIGN_RIGHT|
                         wxALIGN_CENTER_VERTICAL );
    subpanel_sizer->Add( group_label, 0, wxALIGN_LEFT |
                         wxALIGN_CENTER_VERTICAL );
    subpanel_sizer->Add( group_spin, 0, wxALIGN_RIGHT );

    group_subpanel->SetSizerAndFit( subpanel_sizer );

    /* Stuff everything into the main panel */
     panel_sizer->Add( group_subpanel, 0,
                      wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    panel->SetSizerAndFit( panel_sizer );

    /* Update panel */
    return panel;
}

/*****************************************************************************
 * Events methods.
 *****************************************************************************/
void ItemInfoDialog::OnOk( wxCommandEvent& WXUNUSED(event) )
{
    p_item->psz_name = strdup( name_text->GetLineText(0) );
    p_item->psz_uri = strdup( uri_text->GetLineText(0) );
    p_item->psz_author = strdup( author_text->GetLineText(0) );
    vlc_bool_t b_old_enabled = p_item->b_enabled;

    playlist_t * p_playlist =
          (playlist_t *)vlc_object_find( p_intf, VLC_OBJECT_PLAYLIST,
                                       FIND_ANYWHERE );
    if( p_playlist != NULL )
    {
        if( b_old_enabled == VLC_FALSE && enabled_checkbox->IsChecked() )
            p_playlist->i_enabled ++;
        else if( b_old_enabled == VLC_TRUE && !enabled_checkbox->IsChecked() )
            p_playlist->i_enabled --;

        vlc_object_release( p_playlist );
    }

    p_item->b_enabled = enabled_checkbox->IsChecked() ? VLC_TRUE : VLC_FALSE ;
    p_item->i_group = group_spin->GetValue();
    EndModal( wxID_OK );
}

void ItemInfoDialog::OnCancel( wxCommandEvent& WXUNUSED(event) )
{
    EndModal( wxID_CANCEL );
}

/******************************************************************************
 * Info panel event methods.
 ******************************************************************************/
