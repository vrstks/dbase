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

    void InitColumns();

    bool IsUndeletedInSelection(void);
    bool IsDeletedInSelection(void);
    void OnUpdateNeedSel_Deleted(wxUpdateUIEvent&);
    void OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent&);

    void DeleteSelection(bool bDelete);
    void DeleteAll(bool bDelete);

    virtual bool IsRecordDeleted(unsigned int row);
    virtual bool DeleteRecord(unsigned int row, bool bDelete);
#if !USE_DATALISTVIEW
    virtual wxListItemAttr* OnGetItemAttr(long row) const;
#endif
protected:
#if !USE_DATALISTVIEW
    void OnDblClick(wxListCellEvent&);
#endif
    DECLARE_EVENT_TABLE()

#if !USE_DATALISTVIEW
    wxListItemAttr  m_attr;
#endif
};
