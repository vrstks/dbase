// printout.cpp
// Copyright (c) 2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include <wx/html/htmprint.h>
#include <wx/sstream.h>

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"
#include "datamodel.h"
#include "printout.h"

DataModelPrintout::DataModelPrintout(wxDataModel* model, const std::string& title)
{
    wxString str;
    wxStringOutputStream stream(&str);
    wxHtmlTableWriter writer;
    unsigned int row_count = model->GetRowCount();
    unsigned int col_count = model->GetColumnCount();
    wxHtmlTable* table = new wxHtmlTable();
    unsigned int row, col;

    wxHtmlTableRow row_caption;
    for (col = 0; col < col_count; col++)
    {
        wxDataModelColumnInfo info;

        model->GetColumn(col, &info);

        std::string str(info.Name.utf8_str());
        str = wxHtmlTextParagraph::ToBold(str);
        row_caption.ColumnTextArray.push_back(str);
    }
    table->List.push_back(row_caption);
    for (row = 0; row < row_count; row++)
    {
        wxHtmlTableRow temp;
        for (col = 0; col < col_count; col++)
        {
            wxString str;
            model->GetValueByRow(&str, row, col);

            temp.ColumnTextArray.push_back(std::string(str.utf8_str()));
        }
        table->List.push_back(temp);
    }
    writer.List.push_back(table);
    writer.Title = title;
    writer.SaveFile(&stream);
    SetHtmlText(str);
}
