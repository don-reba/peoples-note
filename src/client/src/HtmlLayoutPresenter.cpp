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
	, HTMLayoutWindow & profileView
	, IHtmlDataLoader & htmlDataLoader
	)
	: aboutView       (aboutView)
    , credentialsView (credentialsView)
    , editorView      (editorView)
    , htmlDataLoader  (htmlDataLoader)
    , noteView        (noteView)
    , noteListView    (noteListView)
	, profileView     (profileView)
{
	aboutView       .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(aboutView)));
	credentialsView .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(credentialsView)));
	editorView      .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(editorView)));
	noteView        .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(noteView)));
	noteListView    .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(noteListView)));
	profileView     .ConnectLoadHtmlData(bind(&HtmlLayoutPresenter::OnLoadHtmlData, this, ref(profileView)));
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
