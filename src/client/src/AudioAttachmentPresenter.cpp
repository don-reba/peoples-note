#include "stdafx.h"
#include "AudioAttachmentPresenter.h"

#include "IAudioPlayer.h"
#include "IAudioRecorder.h"
#include "ICredentialsModel.h"
#include "IFile.h"
#include "INoteView.h"
#include "IVoiceEditorModel.h"
#include "IVoiceEditorView.h"
#include "IUserModel.h"
#include "WaitCursor.h"

using namespace boost;
using namespace std;

AudioAttachmentPresenter::AudioAttachmentPresenter
	( IAudioPlayer            & audioPlayer
	, IAudioRecorder          & audioRecorder
	, const ICredentialsModel & credentialsModel
	, INoteView               & noteView
	, IUserModel              & userModel
	, IVoiceEditorModel       & voiceEditorModel
	, IVoiceEditorView        & voiceEditorView
	)
	: audioPlayer      (audioPlayer)
	, audioRecorder    (audioRecorder)
	, credentialsModel (credentialsModel)
	, noteView         (noteView)
	, userModel        (userModel)
	, voiceEditorModel (voiceEditorModel)
	, voiceEditorView  (voiceEditorView)
{
	noteView.ConnectPlayAttachment (bind(&AudioAttachmentPresenter::OnPlayAttachment, this));

	voiceEditorView.ConnectHide   (bind(&AudioAttachmentPresenter::OnHide,  this));
	voiceEditorView.ConnectCancel (bind(&AudioAttachmentPresenter::OnCancel, this));
	voiceEditorView.ConnectPlay   (bind(&AudioAttachmentPresenter::OnPlay,   this));
	voiceEditorView.ConnectShow   (bind(&AudioAttachmentPresenter::OnShow,   this));
	voiceEditorView.ConnectStop   (bind(&AudioAttachmentPresenter::OnStop,   this));
}

void AudioAttachmentPresenter::OnCancel()
{
	voiceEditorView.Hide();
}

void AudioAttachmentPresenter::OnHide()
{
	userModel.Unload();
}

void AudioAttachmentPresenter::OnPlay()
{
	Guid guid(noteView.GetSelecteAttachmentGuid());
	if (guid.IsEmpty())
		return;
	audioPlayer.Play(*userModel.GetResourceData(guid));
}

void AudioAttachmentPresenter::OnPlayAttachment()
{
	voiceEditorView.Show();
	voiceEditorView.SetButtons(IVoiceEditorView::PlayButton | IVoiceEditorView::StopButton);
}

void AudioAttachmentPresenter::OnShow()
{
	//userModel.Load(credentialsModel.GetUsername());
}

void AudioAttachmentPresenter::OnStop()
{
	audioPlayer.Stop();
}
