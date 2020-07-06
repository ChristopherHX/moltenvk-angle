//
// Copyright (c) 2008-2017 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../Precompiled.h"

#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Engine/EngineEvents.h"
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../IO/IOEvents.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "../UI/DropDownList.h"
#include "../UI/Font.h"
#include "../UI/LineEdit.h"
#include "../UI/ListView.h"
#include "../UI/ScrollBar.h"
#include "../UI/Text.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"
#include "../UI/Window.h"
#include "../UI/CheckBox.h"
#include "Urho3DConsole.h"

namespace Urho3D
{

	static const int DEFAULT_CONSOLE_ROWS = 4096;

	const char* Urho3DConsolelogStyles[] =
	{
		"ConsoleDebugText",
		"ConsoleInfoText",
		"ConsoleWarningText",
		"ConsoleErrorText",
		"ConsoleText"
	};

	Urho3DConsole::Urho3DConsole(Context* context) :
		Object(context),
		autoVisibleOnError_(false),
		historyOrAutoCompleteChange_(false),
		printing_(false)
	{

		context_->RegisterSubsystem(this);

		context_->GetSubsystem<Log>()->SetLevel(LOG_DEBUG);


		showDebug_ = true;
		showInfo_ = true;
		showWarning_ = true;
		showError_ = true;

		UI* ui = GetSubsystem<UI>();
		Urho3D::Graphics* graphics = context_->GetSubsystem<Urho3D::Graphics>();
		UIElement* uiRoot = ui->GetRoot();

		// By default prevent the automatic showing of the screen keyboard
		focusOnShow_ = !ui->GetUseScreenKeyboard();

		window_ = uiRoot->CreateChild<Window>(); // new Window(context_);
		window_->SetLayout(LM_VERTICAL);
		window_->SetResizable(true);
		window_->SetMovable(true);
		window_->SetLayoutBorder(IntRect(6, 6, 6, 6));
        float ui_scale = ui->GetScale();
		int width = float(graphics->GetWidth())/ui_scale;
		int height = float(graphics->GetHeight())/ui_scale;
		window_->SetMinSize(width, height);
		//window_->SetMaxSize(300, 300);
		window_->SetSize(width, height);
		window_->BringToFront();
		window_->SetVisible(false);
		
		uiRoot->AddChild(window_);

		/*
		UIElement * titleLayout = new UIElement(context_);
		titleLayout->SetLayout(LM_HORIZONTAL);
		titleLayout->SetStyle("FileSelectorLayout");
		window_->AddChild(titleLayout);

		
		Text * titleText_ = new Text(context_);
		titleText_->SetText("Console Logs");
		titleText_->SetStyle("FileSelectorTitleText");
		titleLayout->AddChild(titleText_);

		closeButton_ = new Button(context_);
		closeButton_->SetName("CloseButton");
		titleLayout->AddChild(closeButton_);
		*/
		BorderImage* divider = new BorderImage(context_);
		divider->SetStyle("EditorDivider");
		window_->AddChild(divider);

		UIElement * buttonLayout_ = new UIElement(context_);
		buttonLayout_->SetStyle("FileSelectorLayout");
		buttonLayout_->SetLayout(LM_HORIZONTAL);
		

		Button *clearButton_ = new Button(context_);
		clearButton_->SetStyle("FileSelectorButton");
		Text * clearButtonText_ = new Text(context_);
		clearButtonText_->SetStyle("FileSelectorButtonText");
		clearButtonText_->SetAlignment(HA_CENTER, VA_CENTER);
		clearButtonText_->SetText("Clear");
		clearButton_->AddChild(clearButtonText_);
		buttonLayout_->AddChild(clearButton_);


		Button *selectAllButton_ = new Button(context_);
		selectAllButton_->SetStyle("FileSelectorButton");
		Text * selectAllButtonText_ = new Text(context_);
		selectAllButtonText_->SetStyle("FileSelectorButtonText");
		selectAllButtonText_->SetAlignment(HA_CENTER, VA_CENTER);
		selectAllButtonText_->SetText("Select all");
		selectAllButton_->AddChild(selectAllButtonText_);
		buttonLayout_->AddChild(selectAllButton_);

		window_->AddChild(buttonLayout_);


		checkBoxDebug = createCheckBoxFilter(buttonLayout_, "Debug");
		checkBoxDebug->SetChecked(showDebug_);
		SubscribeToEvent(checkBoxDebug, "Toggled", URHO3D_HANDLER(Urho3DConsole, ToggleDebug));
		
		checkBoxInfo = createCheckBoxFilter(buttonLayout_, "Info");
		checkBoxInfo->SetChecked(showInfo_);
		SubscribeToEvent(checkBoxInfo, "Toggled", URHO3D_HANDLER(Urho3DConsole, ToggleInfo));

		checkBoxWarning = createCheckBoxFilter(buttonLayout_, "Warning");
		checkBoxWarning->SetChecked(showWarning_);
		SubscribeToEvent(checkBoxWarning, "Toggled", URHO3D_HANDLER(Urho3DConsole, ToggleWarning));

		checkBoxError = createCheckBoxFilter(buttonLayout_, "Error");
		checkBoxError->SetChecked(showError_);
		SubscribeToEvent(checkBoxError, "Toggled", URHO3D_HANDLER(Urho3DConsole, ToggleError));
		
		createSpacer(buttonLayout_);
		createSpacer(buttonLayout_);
		createSpacer(buttonLayout_);
		createSpacer(buttonLayout_);
		createSpacer(buttonLayout_);

		
		rowContainer_ = window_->CreateChild<ListView>();
		rowContainer_->SetHighlightMode(HM_ALWAYS);
		rowContainer_->SetMultiselect(true);
		rowContainer_->SetScrollBarsAutoVisible(true);




		/*ADD at least 1 entry*/
		Text* text = new Text(context_);
		if (window_->GetDefaultStyle())
			text->SetStyle("ConsoleText");
		rowContainer_->InsertItem(0, text);


		SetNumRows(DEFAULT_CONSOLE_ROWS);

		//SubscribeToEvent(closeButton_, E_RELEASED, URHO3D_HANDLER(Urho3DConsole, HandleCloseButtonPressed));
		SubscribeToEvent(clearButton_, E_RELEASED, URHO3D_HANDLER(Urho3DConsole, HandleClearButtonPressed));
		SubscribeToEvent(selectAllButton_, E_RELEASED, URHO3D_HANDLER(Urho3DConsole, HandleSelectAllButtonPressed));
		
		SubscribeToEvent(uiRoot, E_RESIZED, URHO3D_HANDLER(Urho3DConsole, HandleRootElementResized));
		SubscribeToEvent(window_, E_RESIZED, URHO3D_HANDLER(Urho3DConsole, HandleRootElementResized));
		SubscribeToEvent(E_LOGMESSAGE, URHO3D_HANDLER(Urho3DConsole, HandleLogMessage));
		SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Urho3DConsole, HandlePostUpdate));

		
	}

	Urho3DConsole::~Urho3DConsole()
	{
	}

	void Urho3DConsole::SetCheckBox(String name , bool value)
	{
		if (name == "showdebug")
		{
			checkBoxDebug->SetChecked(value);
			showDebug_ = value;
		}
		else if(name == "showinfo")
		{
			checkBoxInfo->SetChecked(value);
			showInfo_ = value;
		}
		else if (name == "showwarning")
		{
			checkBoxWarning->SetChecked(value);
			showWarning_ = value;
		}
		else if (name == "showerror")
		{
			checkBoxError->SetChecked(value);
			showError_ = value;
		}
		
	}

	CheckBox * Urho3DConsole::createCheckBoxFilter(UIElement * parent, Urho3D::String name)
	{
		UIElement *checkboxLayout_ = new UIElement(context_);
		checkboxLayout_->SetStyle("FileSelectorLayout");
		checkboxLayout_->SetLayout(LM_HORIZONTAL);
		parent->AddChild(checkboxLayout_);


		CheckBox *checkBox_ = new CheckBox(context_);
		checkBox_->SetStyleAuto();
		checkboxLayout_->AddChild(checkBox_);

		Text * checkBoxText_ = new Text(context_);
		checkBoxText_->SetStyleAuto();
		checkBoxText_->SetText(name);
		checkboxLayout_->AddChild(checkBoxText_);

		return checkBox_;
	}

	UIElement * Urho3DConsole::createSpacer(UIElement * parent)
	{
		UIElement * spacer = new UIElement(context_);
		spacer->SetStyleAuto();
		parent->AddChild(spacer);
		return spacer;
	}

	void Urho3DConsole::SetDefaultStyle(XMLFile* style)
	{
		if (!style)
			return;


		window_->SetDefaultStyle(style);
		window_->SetStyleAuto();

		rowContainer_->SetStyleAuto();
		for (unsigned i = 0; i < rowContainer_->GetNumItems(); ++i)
			rowContainer_->GetItem(i)->SetStyle("ConsoleText");

	//	closeButton_->SetDefaultStyle(style);
	//	closeButton_->SetStyle("CloseButton");

		UpdateElements();
	}

	void Urho3DConsole::SetVisible(bool enable)
	{
		Input* input = GetSubsystem<Input>();
		UI* ui = GetSubsystem<UI>();
		Cursor* cursor = ui->GetCursor();

		window_->SetVisible(enable);

		if (enable)
		{
			window_->BringToFront();
			rowContainer_->SetFocus(true);
			if (!cursor)
			{
				// Show OS mouse
				//input->SetMouseMode(MM_FREE, true);
				//input->SetMouseVisible(true, true);
			}

			//input->SetMouseGrabbed(false, true);

		}
		else
		{
			rowContainer_->SetFocus(false);
			/*
			if (!cursor)
			{
				// Restore OS mouse visibility
				input->ResetMouseMode();
				input->ResetMouseVisible();
			}

			input->ResetMouseGrabbed();
			*/

		}
	}

	void Urho3DConsole::Toggle()
	{
		SetVisible(!IsVisible());
	}

	void Urho3DConsole::SetNumBufferedRows(unsigned rows)
	{
		if (rows < displayedRows_)
			return;

		rowContainer_->DisableLayoutUpdate();

		int delta = rowContainer_->GetNumItems() - rows;
		if (delta > 0)
		{
			// We have more, remove oldest rows first
			for (int i = 0; i < delta; ++i)
				rowContainer_->RemoveItem((unsigned)0);
		}

		rowContainer_->EnsureItemVisibility(rowContainer_->GetItem(rowContainer_->GetNumItems() - 1));
		rowContainer_->EnableLayoutUpdate();
		rowContainer_->UpdateLayout();

		UpdateElements();
	}

	void Urho3DConsole::SetNumRows(unsigned rows)
	{
		if (!rows)
			return;

		displayedRows_ = rows;
		if (GetNumBufferedRows() > rows)
			SetNumBufferedRows(rows);

		UpdateElements();
	}


	void Urho3DConsole::SetFocusOnShow(bool enable)
	{
		focusOnShow_ = enable;
	}


	void Urho3DConsole::UpdateElements()
	{
		int width = window_->GetWidth();
		const IntRect& border = window_->GetLayoutBorder();
		const IntRect& panelBorder = rowContainer_->GetScrollPanel()->GetClipBorder();
		rowContainer_->SetWidth(width - border.left_ - border.right_);
		if (rowContainer_->GetNumItems() > 0)
		{
			rowContainer_->SetHeight(
				displayedRows_ * rowContainer_->GetItem((unsigned)0)->GetHeight() + panelBorder.top_ + panelBorder.bottom_ +
				(rowContainer_->GetHorizontalScrollBar()->IsVisible() ? rowContainer_->GetHorizontalScrollBar()->GetHeight() : 0));
		}
	}

	XMLFile* Urho3DConsole::GetDefaultStyle() const
	{
		return window_->GetDefaultStyle(false);
	}

	bool Urho3DConsole::IsVisible() const
	{
		return window_ && window_->IsVisible();
	}

	unsigned Urho3DConsole::GetNumBufferedRows() const
	{
		return rowContainer_->GetNumItems();
	}

	void Urho3DConsole::CopySelectedRows() const
	{
		rowContainer_->CopySelectedItemsToClipboard();
	}


	void Urho3DConsole::HandleCloseButtonPressed(StringHash eventType, VariantMap& eventData)
	{
		SetVisible(false);
	}

	void Urho3DConsole::HandleClearButtonPressed(StringHash eventType, VariantMap& eventData)
	{

		rowContainer_->RemoveAllItems();

		/*ADD at least 1 entry*/
		Text* text = new Text(context_);
		if (window_->GetDefaultStyle())
			text->SetStyle("ConsoleText");
		rowContainer_->InsertItem(0, text);

		

		UpdateElements();

	}


	void Urho3DConsole::HandleSelectAllButtonPressed(StringHash eventType, VariantMap& eventData)
	{
		PODVector<unsigned int> selections;

		for (int i = 0; i < rowContainer_->GetNumItems(); i++)
		{
			selections.Push(i);
		}

		if (selections.Size() > 0)
		{
			rowContainer_->SetSelections(selections);
			rowContainer_->SetFocus(true);
			//rowContainer_->UpdateLayout();
		}
	}

	void Urho3DConsole::HandleRootElementResized(StringHash eventType, VariantMap& eventData)
	{
		UpdateElements();
	}

	void Urho3DConsole::HandleLogMessage(StringHash eventType, VariantMap& eventData)
	{
		// If printing a log message causes more messages to be logged (error accessing font), disregard them
		if (printing_)
			return;

		using namespace LogMessage;

		int level = eventData[P_LEVEL].GetInt();

		bool showMessage = false;
		showMessage = ((showDebug_ == true && level == LOG_DEBUG) 
			|| (showInfo_ == true && level == LOG_INFO)
			|| (showWarning_ == true && level == LOG_WARNING)
			|| (showError_ == true && level == LOG_ERROR)) ;

		if (showMessage == true)
		{
			// The message may be multi-line, so split to rows in that case
			Vector<String> rows = eventData[P_MESSAGE].GetString().Split('\n');

			for (unsigned i = 0; i < rows.Size(); ++i)
				pendingRows_.Push(MakePair(level, rows[i]));

			if (autoVisibleOnError_ && level == LOG_ERROR && !IsVisible())
				SetVisible(true);
		}
	}

	void Urho3DConsole::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
	{
		// Ensure UI-elements are not detached
		if (!window_->GetParent())
		{
			UI* ui = GetSubsystem<UI>();
			UIElement* uiRoot = ui->GetRoot();
			uiRoot->AddChild(window_);
		}

		if (!rowContainer_->GetNumItems() || pendingRows_.Empty())
			return;

		printing_ = true;
		rowContainer_->DisableLayoutUpdate();

		Text* text = nullptr;
		for (unsigned i = 0; i < pendingRows_.Size(); ++i)
		{
			if (rowContainer_->GetNumItems() >= displayedRows_)
			{
				rowContainer_->RemoveItem((unsigned)0);
			}
			text = new Text(context_);
			text->SetText(pendingRows_[i].second_);

			switch (pendingRows_[i].first_)
			{
			case LOG_DEBUG:
				text->SetStyle("ConsoleDebugText");
				break;
			case LOG_INFO:
				text->SetStyle("ConsoleInfoText");
				break;
			case LOG_WARNING:
				text->SetStyle("ConsoleWarningText");
				break;
			case LOG_ERROR:
				text->SetStyle("ConsoleErrorText");
				break;
			default:
				text->SetStyle("ConsoleText");
				break;
			}
	
			rowContainer_->AddItem(text);
		}

		pendingRows_.Clear();

		rowContainer_->EnsureItemVisibility(text);
		rowContainer_->EnableLayoutUpdate();
		rowContainer_->UpdateLayout();
		UpdateElements();   // May need to readjust the height due to scrollbar visibility changes
		printing_ = false;
	}

    void Urho3DConsole::ToggleDebug(StringHash eventType, VariantMap& eventData)
    {
        CheckBox* edit = dynamic_cast<CheckBox*>(eventData["Element"].GetPtr());
        showDebug_ = edit->IsChecked();
    }

    void Urho3DConsole::ToggleInfo(StringHash eventType, VariantMap& eventData)
    {
        CheckBox* edit = dynamic_cast<CheckBox*>(eventData["Element"].GetPtr());
        showInfo_ = edit->IsChecked();
    }

    void Urho3DConsole::ToggleWarning(StringHash eventType, VariantMap& eventData)
    {
        CheckBox* edit = dynamic_cast<CheckBox*>(eventData["Element"].GetPtr());
        showWarning_ = edit->IsChecked();
    }

    void Urho3DConsole::ToggleError(StringHash eventType, VariantMap& eventData)
    {
        CheckBox* edit = dynamic_cast<CheckBox*>(eventData["Element"].GetPtr());
        showError_ = edit->IsChecked();
    }

}


