#include "stdafx.h"
#include "VoiceEditorPresenter.h"

#include "IFile.h"
#include "INoteListView.h"
#include "IVoiceEditorModel.h"
#include "IVoiceEditorView.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

VoiceEditorPresenter::VoiceEditorPresenter
	( IFile             & file
	, INoteListView     & noteListView
	, IVoiceEditorModel & voiceEditorModel
	, IVoiceEditorView  & voiceEditorView
	, IUserModel        & userModel
	)
	: file             (file)
	, noteListView     (noteListView)
	, voiceEditorModel (voiceEditorModel)
	, voiceEditorView  (voiceEditorView)
	, userModel        (userModel)
{
	noteListView.ConnectNewVoiceNote(bind(&VoiceEditorPresenter::OnNewVoiceNote, this));

	voiceEditorView.ConnectCancel (bind(&VoiceEditorPresenter::OnCancel, this));
	voiceEditorView.ConnectPlay   (bind(&VoiceEditorPresenter::OnPlay,   this));
	voiceEditorView.ConnectRecord (bind(&VoiceEditorPresenter::OnRecord, this));
	voiceEditorView.ConnectStop   (bind(&VoiceEditorPresenter::OnStop,   this));
}

void VoiceEditorPresenter::OnCancel()
{
	voiceEditorView.Hide();
}

void VoiceEditorPresenter::OnNewVoiceNote()
{
	voiceEditorView.Show();
}

void VoiceEditorPresenter::OnPlay()
{
}

void VoiceEditorPresenter::OnRecord()
{
}

void VoiceEditorPresenter::OnStop()
{
}
