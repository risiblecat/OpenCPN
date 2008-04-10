/////////////////////////////////////////////////////////////////////////////
// Name:        routeprop.cpp
// Purpose:
// Author:
// Modified by:
// Created:     01/16/08 19:39:35
// RCS-ID:
// Copyright:
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "routeprop.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"

#include "routeprop.h"
#include "navutil.h"                // for Route
#include "georef.h"
#include "chart1.h"
#include "routeman.h"
#include "chcanv.h"


extern "C" float DistGreatCircle(double slat, double slon, double dlat, double dlon);

extern float            gLat, gLon;
extern double           g_PlanSpeed;
extern MyConfig         *pConfig;
extern WayPointman      *pWayPointMan;
extern ChartCanvas      *cc1;
extern Select           *pSelect;
extern Routeman         *pRouteMan;


/*!
 * RouteProp type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RouteProp, wxDialog )

/*!
 * RouteProp event table definition
 */

BEGIN_EVENT_TABLE( RouteProp, wxDialog )


    EVT_TEXT( ID_PLANSPEEDCTL, RouteProp::OnPlanSpeedCtlUpdated )
    EVT_BUTTON( ID_ROUTEPROP_CANCEL, RouteProp::OnRoutepropCancelClick )
    EVT_BUTTON( ID_ROUTEPROP_OK, RouteProp::OnRoutepropOkClick )


END_EVENT_TABLE()

/*!
 * RouteProp constructors
 */

RouteProp::RouteProp( )
{
}

RouteProp::RouteProp( wxWindow* parent, wxWindowID id,
                     const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

RouteProp::~RouteProp( )
{
    delete m_TotalDistCtl;
    delete m_PlanSpeedCtl;
    delete m_TimeEnrouteCtl;

    delete m_RouteNameCtl;
    delete m_RouteStartCtl;
    delete m_RouteDestCtl;

    delete m_wpList;
}


/*!
 * RouteProp creator
 */

bool RouteProp::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RouteProp member initialisation
    m_TotalDistCtl = NULL;
    m_wpList = NULL;
////@end RouteProp member initialisation

////@begin RouteProp creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end RouteProp creation
    return TRUE;
}

/*!
 * Control creation for RouteProp
 */

void RouteProp::CreateControls()
{
////@begin RouteProp content construction

    RouteProp* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Route Properties"));
    wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer3, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, _("Route Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer3->Add(itemStaticText4, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_RouteNameCtl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(600, -1), 0 );
    itemStaticBoxSizer3->Add(m_RouteNameCtl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxGROW, 5);


    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer3->Add(itemFlexGridSizer6, 1, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("Depart From"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, wxID_STATIC, _("Destination"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText8, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_RouteStartCtl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL2, _T(""), wxDefaultPosition, wxSize(300, -1), 0 );
    itemFlexGridSizer6->Add(m_RouteStartCtl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_RouteDestCtl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxSize(300, -1), 0 );
    itemFlexGridSizer6->Add(m_RouteDestCtl, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);


    wxFlexGridSizer* itemFlexGridSizer6a = new wxFlexGridSizer(2, 3, 0, 0);
    itemStaticBoxSizer3->Add(itemFlexGridSizer6a, 1, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("Total Distance"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6a->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( itemDialog1, wxID_STATIC, _T("Plan Speed"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6a->Add(itemStaticText12, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText12a = new wxStaticText( itemDialog1, wxID_STATIC, _T("Time Enroute"),wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6a->Add(itemStaticText12a, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_TotalDistCtl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL3, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    itemFlexGridSizer6a->Add(m_TotalDistCtl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_PlanSpeedCtl = new wxTextCtrl( itemDialog1, ID_PLANSPEEDCTL, _T(""), wxDefaultPosition, wxSize(100, -1), wxTE_PROCESS_ENTER );
    itemFlexGridSizer6a->Add(m_PlanSpeedCtl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_TimeEnrouteCtl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL4, _T(""), wxDefaultPosition, wxSize(150, -1), wxTE_READONLY );
    itemFlexGridSizer6a->Add(m_TimeEnrouteCtl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);


    wxStaticBox* itemStaticBoxSizer14Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Waypoints"));
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(itemStaticBoxSizer14Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer14, 1, wxGROW|wxALL, 5);

    m_wpList = new wxListCtrl( itemDialog1, ID_LISTCTRL, wxDefaultPosition, wxSize(-1, 100),
        wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
    itemStaticBoxSizer14->Add(m_wpList, 2, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer16, 0, wxALIGN_RIGHT|wxALL, 5);

    wxButton* itemButton17 = new wxButton( itemDialog1, ID_ROUTEPROP_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemButton17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton18 = new wxButton( itemDialog1, ID_ROUTEPROP_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemButton18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton18->SetDefault();

    //  Fill in list control
    //    m_wpList->SetImageList(m_imageListBars, wxIMAGE_LIST_SMALL);

    // note that under MSW for SetColumnWidth() to work we need to create the
    // items with images initially even if we specify dummy image id
    wxListItem itemCol;
    itemCol.SetImage(-1);

    itemCol.SetText(_T("Leg"));
    m_wpList->InsertColumn(0, itemCol);
    m_wpList->SetColumnWidth( 0, 50 );

    itemCol.SetText(_T("To Waypoint"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    m_wpList->InsertColumn(1, itemCol);
    m_wpList->SetColumnWidth( 1, 200 );

    itemCol.SetText(_T("Distance"));
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    m_wpList->InsertColumn(2, itemCol);
    m_wpList->SetColumnWidth( 2, 80 );

    itemCol.SetText(_T("Bearing"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    m_wpList->InsertColumn(3, itemCol);
    m_wpList->SetColumnWidth( 3, 80 );

    itemCol.SetText(_T("Latitude"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    m_wpList->InsertColumn(4, itemCol);
    m_wpList->SetColumnWidth( 4, 100 );

    itemCol.SetText(_T("Longitude"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    m_wpList->InsertColumn(5, itemCol);
    m_wpList->SetColumnWidth( 5, 100 );


//  Fetch any config file values
    m_planspeed = g_PlanSpeed;


//  Set the dynamic fields
//    UpdateProperties();

////@end RouteProp content construction
}

/*!
 * Should we show tooltips?
 */

bool RouteProp::ShowToolTips()
{
    return TRUE;
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL2
 */

void RouteProp::SetRouteAndUpdate(Route *pR)
{
      m_pRoute = pR;
      m_wpList->DeleteAllItems();

//  Iterate on Route Points, inserting blank fields starting with index 0
      if(pR)
      {
            wxRoutePointListNode *pnode = m_pRoute->pRoutePointList->GetFirst();

            int in=0;
            while(pnode)
            {
                  m_wpList->InsertItem(in, _T(""), 0);
                  in++;
                  pnode = pnode->GetNext();
            }

            //  Set User input Text Fields
            m_RouteNameCtl->SetValue(m_pRoute->m_RouteNameString);
            m_RouteStartCtl->SetValue(m_pRoute->m_RouteStartString);
            m_RouteDestCtl->SetValue(m_pRoute->m_RouteEndString);

            m_RouteNameCtl->SetFocus();

            //      Update the plan speed control
            wxString s;
            s.Printf(_T("%5.2f"), m_planspeed);
            m_PlanSpeedCtl->SetValue(s);



      }
}



bool RouteProp::UpdateProperties()
{
    if(m_pRoute)
    {
        m_pRoute->UpdateSegmentDistances();           // get segment and total distance

    //  Total length
        wxString slen;
        slen.Printf(wxT("%5.2f"), m_pRoute->m_route_length);

        m_TotalDistCtl->SetValue(slen);

    //  Time
        if((0.1 < m_planspeed) && (m_planspeed < 20.0))
        {
            double seconds = 3600 * m_pRoute->m_route_length / m_planspeed;     // in seconds
            wxTimeSpan time(0,0, (int)seconds, 0);
            wxString time_form = time.Format(_T("%H Hours %M Minutes"));
            m_TimeEnrouteCtl->SetValue(time_form);

        }

    //  Iterate on Route Points
        wxRoutePointListNode *node = m_pRoute->pRoutePointList->GetFirst();

        int i=0;
        double slat, slon;

        while(node)
        {
                RoutePoint *prp = node->GetData();
                long item_line_index = i;

    //  Leg
                wxString t;
                t.Printf(_T("%d"), i);
                if(i == 0)
                    t = _T("---");
                m_wpList->SetItem(item_line_index, 0, t);


    //  Mark Name
                m_wpList->SetItem(item_line_index, 1, prp->m_MarkName);

    //  Distance
    //  Note that Distance/Bearing for Leg 000 is as from current position

                if(i == 0)
                {
                    slat = gLat;
                    slon = gLon;
                }

                float leg_dist = DistGreatCircle(slat, slon, prp->m_lat, prp->m_lon );
                t.Printf(_T("%6.2f nm"),leg_dist);
                m_wpList->SetItem(item_line_index, 2, t);

    //  Bearing
                double north, east;
                double brg;
                toSM(prp->m_lat, prp->m_lon, slat, slon, &east, &north);
                double a = atan(north / east);
                if(prp->m_lon > slon)
                    brg = 90. - (a * 180/PI);
                else
                    brg = 270. - (a * 180/PI);

                t.Printf(_T("%03.0f Deg. T"),brg);
                m_wpList->SetItem(item_line_index, 3, t);

    //  Lat/Lon
                char tc[50];

                todmm(1, prp->m_lat, tc, 49);
                wxString tlat(tc, wxConvUTF8);
                m_wpList->SetItem(item_line_index, 4, tlat);

                todmm(2, prp->m_lon, tc, 49);
                wxString tlon(tc, wxConvUTF8);
                m_wpList->SetItem(item_line_index, 5, tlon);

    //  Save for distance/bearing calculation
                slat = prp->m_lat;
                slon = prp->m_lon;

                i++;
                node = node->GetNext();
          }
    }
    return true;
}

bool RouteProp::SaveChanges(void)
{

//  Save the current planning speed
    g_PlanSpeed = m_planspeed;

//  Get User input Text Fields
    m_pRoute->m_RouteNameString = m_RouteNameCtl->GetValue();
    m_pRoute->m_RouteStartString = m_RouteStartCtl->GetValue();
    m_pRoute->m_RouteEndString = m_RouteDestCtl->GetValue();

    pConfig->UpdateRoute(m_pRoute);
    pConfig->UpdateSettings();

    return true;

}


void RouteProp::OnPlanSpeedCtlUpdated( wxCommandEvent& event )
{
    //  Fetch the value, and see if it is a "reasonable" speed
    wxString spd = m_PlanSpeedCtl->GetValue();
    double s;
    spd.ToDouble(&s);
    if((0.1 < s) && (s < 20.0))
    {
        m_planspeed = s;

        UpdateProperties();
    }

    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void RouteProp::OnRoutepropCancelClick( wxCommandEvent& event )
{
    Show(false);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
 */

void RouteProp::OnRoutepropOkClick( wxCommandEvent& event )
{
    SaveChanges();              // write changes to globals and update config

    Show(false);
    event.Skip();
}




//-------------------------------------------------------------------------------
//
//    Mark Properties Dialog Implementation
//
//-------------------------------------------------------------------------------
/*!
 * MarkProp type definition
 */

DEFINE_EVENT_TYPE(EVT_LLCHANGE)           // events from LatLonTextCtrl


IMPLEMENT_DYNAMIC_CLASS( MarkProp, wxDialog )

/*!
 * MarkProp event table definition
 */

BEGIN_EVENT_TABLE( MarkProp, wxDialog )

////@begin MarkProp event table entries

    EVT_BUTTON( ID_MARKPROP_CANCEL, MarkProp::OnMarkpropCancelClick )
    EVT_BUTTON( ID_MARKPROP_OK, MarkProp::OnMarkpropOkClick )
    EVT_LIST_ITEM_SELECTED( ID_ICONCTRL, MarkProp::OnIconListSelected)
    EVT_COMMAND(ID_LATCTRL, EVT_LLCHANGE, MarkProp::OnPositionCtlUpdated)
    EVT_COMMAND(ID_LONCTRL, EVT_LLCHANGE, MarkProp::OnPositionCtlUpdated)
    EVT_CHECKBOX( ID_SHOWNAMECHECKBOX1, MarkProp::OnShowNamecheckboxClick )

////@end RouteProp event table entries

END_EVENT_TABLE()

/*!
 * MarkProp constructors
 */

MarkProp::MarkProp( )
{
}

MarkProp::MarkProp(  wxWindow* parent, wxWindowID id,
                     const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

MarkProp::~MarkProp( )
{
    delete m_MarkNameCtl;
    delete m_MarkLatCtl;
    delete m_MarkLonCtl;
}



/*!
 * MarkProp creator
 */

bool MarkProp::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();

    return TRUE;
}

/*!
 * Control creation for MarkProp
 */

void MarkProp::CreateControls()
{
    MarkProp* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Properties"));

    wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer3, 1, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, _("Mark Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer3->Add(itemStaticText4, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_MarkNameCtl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(-1, -1), 0 );
    itemStaticBoxSizer3->Add(m_MarkNameCtl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxGROW, 5);

    m_ShowNameCheckbox = new wxCheckBox( itemDialog1, ID_SHOWNAMECHECKBOX1, _("Show Name"), wxDefaultPosition, wxSize(-1, -1), 0 );
    itemStaticBoxSizer3->Add(m_ShowNameCheckbox, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxGROW, 5);

    wxStaticText* itemStaticText4a= new wxStaticText( itemDialog1, wxID_STATIC, _("Mark Icon"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer3->Add(itemStaticText4a, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_IconList = new wxListCtrl( itemDialog1, ID_ICONCTRL, wxDefaultPosition, wxSize(300, 100),
        wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VRULES );
    itemStaticBoxSizer3->Add(m_IconList, 2, wxGROW|wxALL, 5);


    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Position"));

    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer4, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Latitude"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer4->Add(itemStaticText5, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_MarkLatCtl = new LatLonTextCtrl( itemDialog1, ID_LATCTRL, _T(""), wxDefaultPosition, wxSize(180, -1), 0 );
    itemStaticBoxSizer4->Add(m_MarkLatCtl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxGROW, 5);


    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, _("Longitude"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer4->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_MarkLonCtl = new LatLonTextCtrl( itemDialog1, ID_LONCTRL, _T(""), wxDefaultPosition, wxSize(180, -1), 0 );
    itemStaticBoxSizer4->Add(m_MarkLonCtl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxGROW, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer16, 0, wxALIGN_RIGHT|wxALL, 5);

    wxButton* itemButton17 = new wxButton( itemDialog1, ID_MARKPROP_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemButton17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton18 = new wxButton( itemDialog1, ID_MARKPROP_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemButton18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButton18->SetDefault();

    //  Fill in list control

    m_IconList->Hide();

    int client_x, client_y;
    m_IconList->GetClientSize(&client_x, &client_y);

    m_IconList->SetImageList(pWayPointMan->pmarkicon_image_list, wxIMAGE_LIST_SMALL);

    wxListItem itemCol0;
    itemCol0.SetImage(-1);
    itemCol0.SetText(_T("Icon"));

    m_IconList->InsertColumn(0, itemCol0);
    m_IconList->SetColumnWidth( 0, 40 );

    wxListItem itemCol;
    itemCol.SetText(_T("Description"));
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    m_IconList->InsertColumn(1, itemCol);
    m_IconList->SetColumnWidth( 1, client_x - 56 );


    //      Iterate on the Icon Descriptions, filling in the control

      for(int i = 0 ; i < pWayPointMan->GetNumIcons() ; i++)
      {
            wxString *ps = pWayPointMan->GetIconDescription(i);

            long item_index = m_IconList->InsertItem(i, _T(""), 0);
            m_IconList->SetItem(item_index, 1, *ps);

            m_IconList->SetItemImage(item_index,i);
      }

      m_IconList->Show();
}

bool MarkProp::ShowToolTips()
{
    return TRUE;
}


void MarkProp::SetRoutePoint(RoutePoint *pRP)
{
      m_pRoutePoint = pRP;

      m_lat_save = m_pRoutePoint->m_lat;
      m_lon_save = m_pRoutePoint->m_lon;
      m_IconName_save = m_pRoutePoint->m_IconName;
      m_bShowName_save = m_pRoutePoint->m_bShowName;
}


bool MarkProp::UpdateProperties()
{
      wxString t;
      if(m_pRoutePoint)
      {
//    Name
            m_MarkNameCtl->SetValue(m_pRoutePoint->m_MarkName);
            m_ShowNameCheckbox->SetValue(m_pRoutePoint->m_bShowName);
            m_MarkNameCtl->SetInsertionPoint(0);
            m_MarkNameCtl->SetSelection(-1, -1);
            m_MarkNameCtl->SetFocus();


//  Lat/Lon
            char tc[50];

            todmm(1, m_pRoutePoint->m_lat, tc, 49);
            wxString strt(tc, wxConvUTF8);
            m_MarkLatCtl->SetValue(strt);

            todmm(2, m_pRoutePoint->m_lon, tc, 49);
            wxString strn(tc, wxConvUTF8);
            m_MarkLonCtl->SetValue(strn);

//    Highlite the icon current selection
            m_current_icon_Index = pWayPointMan->GetIconIndex(m_pRoutePoint->m_pbmIcon);

            m_IconList->SetItemState(m_current_icon_Index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_IconList->EnsureVisible(m_current_icon_Index);
    }
    return true;
}

bool MarkProp::SaveChanges(void)
{
//  Get User input Text Fields
      m_pRoutePoint->m_MarkName = m_MarkNameCtl->GetValue();

      //    Here is some logic....
      //    If the Markname is completely numeric, and is part of a route,
      //    Then declare it to be of attribute m_bDynamicName = true
      //    This is later used for re-numbering points on actions like
      //    Insert Point, Delete Point, Append Point, etc

      if(m_pRoutePoint->m_bIsInRoute)
      {
            bool b_name_is_numeric = true;
            for(unsigned int i=0 ; i<m_pRoutePoint->m_MarkName.Len() ; i++)
            {
                  if(wxChar('0') > m_pRoutePoint->m_MarkName[i])
                        b_name_is_numeric = false;
                  if(wxChar('9') < m_pRoutePoint->m_MarkName[i])
                        b_name_is_numeric = false;
            }

            m_pRoutePoint->m_bDynamicName = b_name_is_numeric;
      }
      else
            m_pRoutePoint->m_bDynamicName = false;


      if(m_pRoutePoint->m_bIsInRoute)
      {
            Route *pRoute = pRouteMan->FindRouteContainingWaypoint(m_pRoutePoint);
            pConfig->UpdateRoute(pRoute);
      }
      else
            pConfig->UpdateWayPoint(m_pRoutePoint);

      pConfig->UpdateSettings();

    return true;
}


void MarkProp::OnMarkpropCancelClick( wxCommandEvent& event )
{
      //    Restore saved values for lat/lon and icon
      m_pRoutePoint->m_lat = m_lat_save;
      m_pRoutePoint->m_lon = m_lon_save;
      m_pRoutePoint->m_IconName = m_IconName_save;
      m_pRoutePoint->m_bShowName = m_bShowName_save;

      m_pRoutePoint->m_pbmIcon = pWayPointMan->GetIconBitmap(m_IconName_save);

      Show(false);
      event.Skip();
}


void MarkProp::OnMarkpropOkClick( wxCommandEvent& event )
{
    SaveChanges();              // write changes to globals and update config

    Show(false);
    event.Skip();
}

void MarkProp::OnIconListSelected( wxListEvent& event )
{
      int new_index = event.GetIndex();
      if(new_index != m_current_icon_Index)
      {
           m_current_icon_Index = new_index;

           m_pRoutePoint->m_IconName = *(pWayPointMan->GetIconKey(m_current_icon_Index));
           m_pRoutePoint->m_pbmIcon = pWayPointMan->GetIconBitmap(m_current_icon_Index);

           // dynamically update the icon on the canvas
           cc1->RefreshRect(m_pRoutePoint->CurrentRect_in_DC, false);
      }
}

void MarkProp::OnShowNamecheckboxClick( wxCommandEvent& event )
{
      m_pRoutePoint->m_bShowName = m_ShowNameCheckbox->GetValue();

      // dynamically update the icon on the canvas
      cc1->RefreshRect(m_pRoutePoint->CurrentRect_in_DC, false);
}

void MarkProp::OnPositionCtlUpdated( wxCommandEvent& event )
{
      char str[50];
      wxString l;

      //    Fetch the control values, convert to degrees
      l = m_MarkLatCtl->GetValue();
      strncpy(str, l.mb_str(), 49);
      double lat = fromDMM(str);

      l = m_MarkLonCtl->GetValue();
      strncpy(str, l.mb_str(), 49);
      double lon = fromDMM(str);

      m_pRoutePoint->SetPosition(lat, lon);
      pSelect->ModifySelectablePoint(lat, lon, (void *)m_pRoutePoint, SELTYPE_ROUTEPOINT);

      //    Update the mark position dynamically
      cc1->Refresh();
}


//------------------------------------------------------------------------------
//    LatLonTextCtrl Window Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(LatLonTextCtrl, wxWindow)

      EVT_KILL_FOCUS(LatLonTextCtrl::OnKillFocus)

END_EVENT_TABLE()

// constructor
LatLonTextCtrl::LatLonTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value,
            const wxPoint& pos, const wxSize& size,
            long style, const wxValidator& validator,
            const wxString& name):
wxTextCtrl(parent, id, value, pos, size, style, validator, name)
{
      m_pParentEventHandler = parent->GetEventHandler();
}


void LatLonTextCtrl::OnKillFocus(wxFocusEvent& event)
{
      //    Send an event to the Parent Dialog
      wxCommandEvent up_event( EVT_LLCHANGE,  GetId() );
      up_event.SetEventObject( (wxObject *)this );
      m_pParentEventHandler->AddPendingEvent(up_event);
}










