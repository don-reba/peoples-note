#pragma once

class IAudioPlayer;
class IAudioRecorder;
class ICredentialsModel;
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
	IUserModel        & userModel;
	IVoiceEditorModel & voiceEditorModel;
	IVoiceEditorView  & voiceEditorView;

	const ICredentialsModel & credentialsModel;

public:

	AudioAttachmentPresenter
		( IAudioPlayer            & audioPlayer
		, IAudioRecorder          & audioRecorder
		, const ICredentialsModel & credentialsModel
		, INoteView               & noteView
		, IUserModel              & userModel
		, IVoiceEditorModel       & voiceEditorModel
		, IVoiceEditorView        & voiceEditorView
		);

private:

	void OnCancel();
	void OnHide();
	void OnPlay();
	void OnPlayAttachment();
	void OnShow();
	void OnStop();
};