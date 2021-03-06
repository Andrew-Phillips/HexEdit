#pragma once

#include "HexEdit.h"
#include "TaskDialog.h"

// This derives from CTaskDialog solely to add the ability to have an "avoidable dialog".
// If the user turns on the "Don't ask again" checkbox then subsequent use of this dialog returns 
// the same value without any user interaction and without any visible difference to the callee.
// The return values are stored in the registry - once hidden by the user a dialog will not be 
// seen again until the relevant regitry entry is removed.
class CAvoidableDialog : public CTaskDialog
{
public:
	CAvoidableDialog(int id, LPCTSTR content = _T(""), LPCTSTR title = _T(""), MLTASKDIALOG_COMMON_BUTTON_FLAGS buttons = 0, LPCTSTR icon = 0) :
	  CTaskDialog(MAKEINTRESOURCE(id), content, title, buttons, icon)
	{
		id_.Format("%d", id);
		dont_ask_ = false;
		if (buttons == 0 || buttons == MLCBF_OK_BUTTON)
			SetVerificationText("Don't show this message again.");  // It's just informational if only showing the OK button
		else
			SetVerificationText("Don't ask this question again.");  // Multiple options implies a question
	}

	int DoModal(CWnd* pParentWnd = CWnd::GetActiveWindow())
	{
		int retval;

		if ((retval = theApp.GetProfileInt("DialogDontAskValues", id_, -1)) == -1)
		{
			retval = CTaskDialog::DoModal(pParentWnd);
			if (dont_ask_)
				theApp.WriteProfileInt("DialogDontAskValues", id_, retval);
		}

		return retval;
	}

	// CAvoidableDialog::Show() displays an "avoidable dialog" (with "Don't show again" checkbox).
	// Parameters:
	//   id = string id used to display a brief (main) message at the top of the dialog,
	//        also identifies the dialog for the purposes of tracking the "don't ask" flag.
	//   content = more information, defaults to nothing
	//   title = window title, defaults to HexEdit
	//   buttons = flags saying which buttons to show, defaults to OK button only
	//   icon = icon displayed in top right, if 0 then no icon id displayed, if empty string (default) then default icon is displayed
	//          - IDI_QUESTIONMARK if "YES" button is shown
	//          - IDI_EXCLAMATIONMARK if "YES" button is not shown
	// NOTE: An alternative to CAvoidableDialog::Show() which does not require a string resource is ::TaskMessageBox()
	static int Show(int id, LPCTSTR content = _T(""), LPCTSTR title = _T(""), MLTASKDIALOG_COMMON_BUTTON_FLAGS buttons = 0, LPCTSTR icon = 0)
	{
		CAvoidableDialog dlg(id, content, title, buttons, icon);
		return dlg.DoModal(CWnd::GetActiveWindow());
	}

private:
	CString id_;
	bool dont_ask_;

	void OnVerificationClicked(BOOL checked)
	{
		dont_ask_ = checked != 0;
	}
};
