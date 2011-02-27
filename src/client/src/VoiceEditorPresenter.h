#pragma once

class IAudio;
class INoteListView;
class IVoiceEditorModel;
class IVoiceEditorView;
class IUserModel;

class VoiceEditorPresenter
{
private:

	IAudio            & audio;
	INoteListView     & noteListView;
	IVoiceEditorModel & voiceEditorModel;
	IVoiceEditorView  & voiceEditorView;
	IUserModel        & userModel;

public:

	VoiceEditorPresenter
		( IAudio            & audio
		, INoteListView     & noteListView
		, IVoiceEditorModel & voiceEditorModel
		, IVoiceEditorView  & voiceEditorView
		, IUserModel        & userModel
		);

private:

	void OnCancel();
	void OnNewVoiceNote();
	void OnPlay();
	void OnRecord();
	void OnStop();
};