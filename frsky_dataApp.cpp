
/*
 Copyright (c) 2012 - 2013 Andy Little 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "frsky_dataApp.h"
#include "frsky_dataMain.h"


IMPLEMENT_APP(frsky_dataApp);

bool frsky_dataApp::OnInit()
{
    
    frsky_dataDialog* dlg = new frsky_dataDialog(0L, _("Mavlink -> FrSky"));
    this->SetTopWindow(dlg);

    dlg->Show();

    return true;
}
