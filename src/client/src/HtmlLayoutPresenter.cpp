#include "stdafx.h"
#include "HtmlLayoutPresenter.h"

#include "HTMLayoutWindow.h"
#include "IHtmlDataLoader.h"

using namespace boost;

HtmlLayoutPresenter::HtmlLayoutPresenter
	( HTMLayoutWindow & aboutView
	, HTMLayoutWindow & credentialsView
	, HTMLayoutWindow & editorView
	, HTMLayoutWindow & noteView
	, HTMLayoutWindow & noteListView
	, HTMLayoutWindow & photoEditorView
	, HTMLayoutWindow & profileView
	, HTMLayoutWindow & voiceEditorView
	, IHtmlDataLoader & htmlDataLoader
	)
	: htmlDataLoader  (htmlDataLoader)
{
	aboutView       .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(aboutView)));
	credentialsView .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(credentialsView)));
	editorView      .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(editorView)));
	noteView        .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(noteView)));
	noteListView    .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(noteListView)));
	photoEditorView .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(photoEditorView)));
	profileView     .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(profileView)));
	voiceEditorView .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(voiceEditorView)));
}

void HtmlLayoutPresenter::OnLoadHtmlData(HTMLayoutWindow & view)
{
	if (htmlDataLoader.LoadFromUri(view.GetHtmlUri()))
	{
		view.SetHtmlData
			( htmlDataLoader.GetData()
			, htmlDataLoader.GetDataSize()
			);
	}
}
