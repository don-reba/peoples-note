#pragma once

class IAudioPlayer;
class IAudioRecorder;
class INoteView;
class IVoiceEditorModel;
class IVoiceEditorView;
class IUserModel;

class AudioAttachmentPresenter
{
private:

	IAudioPlayer      & audioPlayer;
	IAudioRecorder    & audioRecorder;
	INoteView         & noteView;
	IVoiceEditorModel & voiceEditorModel;
	IVoiceEditorView  & voiceEditorView;
	IUserModel        & userModel;

public:

	AudioAttachmentPresenter
		( IAudioPlayer      & audioPlayer
		, IAudioRecorder    & audioRecorder
		, INoteView         & noteView
		, IVoiceEditorModel & voiceEditorModel
		, IVoiceEditorView  & voiceEditorView
		, IUserModel        & userModel
		);

private:

	void OnCancel();
	void OnPlay();
	void OnPlayAttachment();
	void OnRecord();
	void OnStop();
};