#pragma once

#include "../Core/Object.h"

namespace Urho3D
{
	class Window;
	class Button;
	class BorderImage;
	class DropDownList;
	class Engine;
	class Font;
	class LineEdit;
	class ListView;
	class Text;
	class UIElement;
	class XMLFile;
	class CheckBox;
	/// %Console window with log history and command line prompt.
	class Urho3DConsole : public Object
	{
		URHO3D_OBJECT(Urho3DConsole, Object);

	public:
		/// Construct.
		Urho3DConsole(Context* context);
		/// Destruct.
		virtual ~Urho3DConsole() override;

		CheckBox * createCheckBoxFilter(UIElement * parent, Urho3D::String name);
		UIElement * createSpacer(UIElement * parent);

		void ToggleDebug(StringHash eventType, VariantMap& eventData);
		void ToggleInfo(StringHash eventType, VariantMap& eventData);
		void ToggleWarning(StringHash eventType, VariantMap& eventData);
		void ToggleError(StringHash eventType, VariantMap& eventData);

		void SetCheckBox(String name, bool value);
		/// Set UI elements' style from an XML file.
		void SetDefaultStyle(XMLFile* style);
		/// Show or hide.
		void SetVisible(bool enable);
		/// Toggle visibility.
		void Toggle();

		/// Automatically set console to visible when receiving an error log message.
		void SetAutoVisibleOnError(bool enable) { autoVisibleOnError_ = enable; }

		/// Set number of buffered rows.
		void SetNumBufferedRows(unsigned rows);
		/// Set number of displayed rows.
		void SetNumRows(unsigned rows);

		/// Set whether to automatically focus the line edit when showing. Default true on desktops and false on mobile devices, as on mobiles it would pop up the screen keyboard.
		void SetFocusOnShow(bool enable);
		/// Update elements to layout properly. Call this after manually adjusting the sub-elements.
		void UpdateElements();

		/// Return the UI style file.
		XMLFile* GetDefaultStyle() const;

		/// Return the background element.
		BorderImage* GetWindow() const { return window_; }

		/// Return the close butoon element.
	//	Button* GetCloseButton() const { return closeButton_; }

		/// Return whether is visible.
		bool IsVisible() const;

		/// Return true when console is set to automatically visible when receiving an error log message.
		bool IsAutoVisibleOnError() const { return autoVisibleOnError_; }

		/// Return number of buffered rows.
		unsigned GetNumBufferedRows() const;

		/// Return number of displayed rows.
		unsigned GetNumRows() const { return displayedRows_; }

		/// Copy selected rows to system clipboard.
		void CopySelectedRows() const;

		/// Return whether automatically focuses the line edit when showing.
		bool GetFocusOnShow() const { return focusOnShow_; }

		bool showDebug_;
		bool showInfo_;
		bool showWarning_;
		bool showError_;

	private:
		/// Handle close button being pressed.
		void HandleCloseButtonPressed(StringHash eventType, VariantMap& eventData);
		/// Handle clear button being pressed.
		void HandleClearButtonPressed(StringHash eventType, VariantMap& eventData);
		/// Handle select all button being pressed.
		void HandleSelectAllButtonPressed(StringHash eventType, VariantMap& eventData);

		/// Handle UI root resize.
		void HandleRootElementResized(StringHash eventType, VariantMap& eventData);
		/// Handle a log message.
		void HandleLogMessage(StringHash eventType, VariantMap& eventData);
		/// Handle the application post-update.
		void HandlePostUpdate(StringHash eventType, VariantMap& eventData);

		SharedPtr<Window> window_;

		/// Auto visible on error flag.
		bool autoVisibleOnError_;
		/// Container for text rows.
		ListView* rowContainer_;
		/// Close button.
		//SharedPtr<Button> closeButton_;

		/// Pending log message rows.
		Vector<Pair<int, String> > pendingRows_;
		/// Current row being edited.
		String currentRow_;
		/// Maximum displayed rows.
		unsigned displayedRows_;

		/// Flag when printing messages to prevent endless loop.
		bool printing_;
		/// Flag for automatically focusing the line edit on showing the console.
		bool focusOnShow_;
		/// Internal flag whether currently in an autocomplete or history change.
		bool historyOrAutoCompleteChange_;

		CheckBox *  checkBoxDebug;
		CheckBox *  checkBoxInfo;
		CheckBox *  checkBoxWarning;
		CheckBox *  checkBoxError;

	};

}
