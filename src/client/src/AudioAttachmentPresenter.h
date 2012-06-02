#pragma once

class IAudioPlayer;
class IAudioPlayerView;
class IAudioRecorder;
class ICredentialsModel;
class INoteView;
class IVoiceEditorModel;
class IUserModel;

class AudioAttachmentPresenter
{
private:

	IAudioPlayer     & audioPlayer;
	IAudioPlayerView & audioPlayerView;
	INoteView        & noteView;
	IUserModel       & userModel;

	const ICredentialsModel & credentialsModel;

public:

	AudioAttachmentPresenter
		( IAudioPlayer            & audioPlayer
		, IAudioPlayerView        & audioPlayerView
		, const ICredentialsModel & credentialsModel
		, INoteView               & noteView
		, IUserModel              & userModel
		);

private:

	void OnCancel();
	void OnHide();
	void OnPlay();
	void OnPlayAttachment();
	void OnShow();
	void OnStop();
};