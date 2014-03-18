// dbflist.h - editable
// Copyright (c) 2007-2014 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATALIST_H__
   #include "datalist.h"
#endif

class DBFListCtrl : public DataModelListCtrl
{
    typedef DataModelListCtrl base;
public:
    DBFListCtrl() : DataModelListCtrl()
    {
    }

    void AssociateModel(DBFModel* model) { base::AssociateModel(model); }
          DBFModel* GetModel()       { return (DBFModel*)base::GetModel(); }
    const DBFModel* GetModel() const { return (DBFModel*)base::GetModel(); }

    bool Edit(long row, long col);
    bool Edit();
    bool AddNew();

    bool IsUndeletedInSelection(void);
    bool IsDeletedInSelection(void);
    void OnUpdateNeedSel_Deleted(wxUpdateUIEvent&);
    void OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent&);

    void DeleteSelection(bool bDelete);
    void DeleteAll(bool bDelete);

    virtual bool IsRecordDeleted(unsigned int index);
    virtual bool DeleteRecord(unsigned int index, bool bDelete);
#if USE_DATALISTVIEW
#else
    virtual wxListItemAttr* OnGetItemAttr(long row) const;
#endif
protected:
    void OnDblClick(wxListCellEvent&);
    DECLARE_EVENT_TABLE()

    wxListItemAttr  m_attr;
};
