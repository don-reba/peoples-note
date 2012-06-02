#include "stdafx.h"
#include "AudioAttachmentPresenter.h"

#include "IAudioPlayer.h"
#include "IAudioPlayerView.h"
#include "IAudioRecorder.h"
#include "ICredentialsModel.h"
#include "IFile.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "WaitCursor.h"

using namespace boost;
using namespace std;

AudioAttachmentPresenter::AudioAttachmentPresenter
	( IAudioPlayer            & audioPlayer
	, IAudioPlayerView        & audioPlayerView
	, const ICredentialsModel & credentialsModel
	, INoteView               & noteView
	, IUserModel              & userModel
	)
	: audioPlayer      (audioPlayer)
	, audioPlayerView  (audioPlayerView)
	, credentialsModel (credentialsModel)
	, noteView         (noteView)
	, userModel        (userModel)
{
	noteView.ConnectPlayAttachment (bind(&AudioAttachmentPresenter::OnPlayAttachment, this));

	audioPlayerView.ConnectCancel (bind(&AudioAttachmentPresenter::OnCancel, this));
	audioPlayerView.ConnectHide   (bind(&AudioAttachmentPresenter::OnHide,   this));
	audioPlayerView.ConnectPlay   (bind(&AudioAttachmentPresenter::OnPlay,   this));
	audioPlayerView.ConnectShow   (bind(&AudioAttachmentPresenter::OnShow,   this));
	audioPlayerView.ConnectStop   (bind(&AudioAttachmentPresenter::OnStop,   this));
}

void AudioAttachmentPresenter::OnCancel()
{
	audioPlayerView.Hide();
}

void AudioAttachmentPresenter::OnHide()
{
	audioPlayer.Stop();
	userModel.Unload();
}

void AudioAttachmentPresenter::OnPlay()
{
	Guid guid(noteView.GetSelectedAttachmentGuid());
	if (guid.IsEmpty())
		return;
	audioPlayer.Play(userModel.GetResourceData(guid));
}

void AudioAttachmentPresenter::OnPlayAttachment()
{
	audioPlayerView.Show();
	audioPlayerView.SetFileName(noteView.GetSelectedAttachmentName());
}

void AudioAttachmentPresenter::OnShow()
{
	userModel.Load(credentialsModel.GetUsername());
}

void AudioAttachmentPresenter::OnStop()
{
	audioPlayer.Stop();
}
