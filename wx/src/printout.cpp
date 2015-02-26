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

DataModelPrintout::DataModelPrintout(const wxDataModel* model, const wxString& title) : wxHtmlPrintout(title)
{
    wxString str;
    wxStringOutputStream stream(&str);
    MakeHtmlTableFile(model, &stream, std::string(title.utf8_str()));
    SetHtmlText(str);
}

//static
void DataModelPrintout::MakeHtmlTableFile(const wxDataModel* model, wxOutputStream* stream, const std::string& title)
{
    unsigned int row_count = model->GetRowCount();
    unsigned int col_count = model->GetColumnCount();
    HtmlTable* table = new HtmlTable();
    unsigned int row, col;

    HtmlTableRow row_caption;
    for (col = 0; col < col_count; col++)
    {
        wxDataModelColumnInfo info;

        model->GetColumn(col, &info);

        std::string str(info.Name.utf8_str());
        str = HtmlTextParagraph::ToBold(str);
        row_caption.ColumnTextArray.push_back(str);
    }
    table->List.push_back(row_caption);
    for (row = 0; row < row_count; row++)
    {
        HtmlTableRow temp;
        for (col = 0; col < col_count; col++)
        {
            wxString str;
            model->GetValueByRow(&str, row, col);

            std::string value(str.utf8_str());
            value = HtmlTextWriter::Escape(value);
            temp.ColumnTextArray.push_back(value);
        }
        table->List.push_back(temp);
    }
    wxHtmlTableWriter writer;
    writer.List.push_back(table);
    writer.Title = title;
    writer.SaveFile(stream);
}

//static
void DataModelPrintout::MakeXmlFile(const wxDataModel* model, wxOutputStream* stream, const std::string& tableName)
{
    unsigned int row_count = model->GetRowCount();
    unsigned int col_count = model->GetColumnCount();
    unsigned int row, col;
    const wxDataModelColumnInfoVector col_vec = model->GetColumns();
    wxXmlWriter writer;

    writer.Open(tableName, stream);
    for (row = 0; row < row_count; row++)
    {
        writer.WriteStartElement("record");
            for (col = 0; col < col_count; col++)
            {
                const std::string elmName(col_vec[col].Name.utf8_str());

                wxString str;
                model->GetValueByRow(&str, row, col);

                std::string value(str.utf8_str());
                value = HtmlTextWriter::Escape(value);
                writer.WriteElement(elmName, value);
            }
        writer.WriteEndElement();
    }
    writer.Close();
}
