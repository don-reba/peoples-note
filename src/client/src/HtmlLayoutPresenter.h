#pragma once

class HTMLayoutWindow;
class IHtmlDataLoader;

class HtmlLayoutPresenter
{
private:

	HTMLayoutWindow & aboutView;
	HTMLayoutWindow & credentialsView;
	HTMLayoutWindow & editorView;
	HTMLayoutWindow & noteView;
	HTMLayoutWindow & noteListView;
	HTMLayoutWindow & profileView;
	IHtmlDataLoader & htmlDataLoader;

public:

	HtmlLayoutPresenter
		( HTMLayoutWindow & aboutView
		, HTMLayoutWindow & credentialsView
		, HTMLayoutWindow & editorView
		, HTMLayoutWindow & noteView
		, HTMLayoutWindow & noteListView
		, HTMLayoutWindow & profileView
		, IHtmlDataLoader & htmlDataLoader
		);

private:

	void OnLoadHtmlData(HTMLayoutWindow & view);
};
