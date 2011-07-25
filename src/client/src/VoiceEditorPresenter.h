#pragma once

class IAudioPlayer;
class IAudioRecorder;
class INoteListView;
class IVoiceEditorModel;
class IVoiceEditorView;
class IUserModel;

class VoiceEditorPresenter
{
private:

	IAudioPlayer      & audioPlayer;
	IAudioRecorder    & audioRecorder;
	INoteListView     & noteListView;
	IVoiceEditorModel & voiceEditorModel;
	IVoiceEditorView  & voiceEditorView;
	IUserModel        & userModel;

public:

	VoiceEditorPresenter
		( IAudioPlayer      & audioPlayer
		, IAudioRecorder    & audioRecorder
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