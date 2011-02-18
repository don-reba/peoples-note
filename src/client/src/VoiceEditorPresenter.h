#pragma once

class IFile;
class INoteListView;
class IVoiceEditorModel;
class IVoiceEditorView;
class IUserModel;

class VoiceEditorPresenter
{
private:

	IFile             & file;
	INoteListView     & noteListView;
	IVoiceEditorModel & voiceEditorModel;
	IVoiceEditorView  & voiceEditorView;
	IUserModel        & userModel;

public:

	VoiceEditorPresenter
		( IFile             & file
		, INoteListView     & noteListView
		, IVoiceEditorModel & voiceEditorModel
		, IVoiceEditorView  & voiceEditorView
		, IUserModel        & userModel
		);

private:

	void OnCancel();
	void OnNewVoiceNote();
};