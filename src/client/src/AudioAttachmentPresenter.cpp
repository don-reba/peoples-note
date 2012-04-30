#include "stdafx.h"
#include "AudioAttachmentPresenter.h"

#include "IAudioPlayer.h"
#include "IAudioRecorder.h"
#include "IFile.h"
#include "INoteView.h"
#include "IVoiceEditorModel.h"
#include "IVoiceEditorView.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

AudioAttachmentPresenter::AudioAttachmentPresenter
	( IAudioPlayer      & audioPlayer
	, IAudioRecorder    & audioRecorder
	, INoteView         & noteView
	, IVoiceEditorModel & voiceEditorModel
	, IVoiceEditorView  & voiceEditorView
	, IUserModel        & userModel
	)
	: audioPlayer      (audioPlayer)
	, audioRecorder    (audioRecorder)
	, noteView         (noteView)
	, voiceEditorModel (voiceEditorModel)
	, voiceEditorView  (voiceEditorView)
	, userModel        (userModel)
{
	noteView.ConnectPlayAttachment (bind(&AudioAttachmentPresenter::OnPlayAttachment, this));

	voiceEditorView.ConnectCancel (bind(&AudioAttachmentPresenter::OnCancel, this));
	voiceEditorView.ConnectPlay   (bind(&AudioAttachmentPresenter::OnPlay,   this));
	voiceEditorView.ConnectRecord (bind(&AudioAttachmentPresenter::OnRecord, this));
	voiceEditorView.ConnectStop   (bind(&AudioAttachmentPresenter::OnStop,   this));
}

void AudioAttachmentPresenter::OnCancel()
{
	voiceEditorView.Hide();
}

void AudioAttachmentPresenter::OnPlay()
{
	//Blob empty;
	//audio.Play(empty);
}

void AudioAttachmentPresenter::OnPlayAttachment()
{
	voiceEditorView.Show();
	voiceEditorView.SetButtons(IVoiceEditorView::PlayButton | IVoiceEditorView::StopButton);
}

void AudioAttachmentPresenter::OnRecord()
{
	//Blob empty;
	//audio.Record(empty);
}

void AudioAttachmentPresenter::OnStop()
{
	//audio.Stop();
}
