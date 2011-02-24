#pragma once

class HTMLayoutWindow;
class IHtmlDataLoader;

class HtmlLayoutPresenter
{
private:

	IHtmlDataLoader & htmlDataLoader;

public:

	HtmlLayoutPresenter
		( HTMLayoutWindow & aboutView
		, HTMLayoutWindow & credentialsView
		, HTMLayoutWindow & editorView
		, HTMLayoutWindow & noteView
		, HTMLayoutWindow & noteListView
		, HTMLayoutWindow & photoEditorView
		, HTMLayoutWindow & profileView
		, HTMLayoutWindow & voiceEditorView
		, IHtmlDataLoader & htmlDataLoader
		);

private:

	void OnLoadHtmlData(HTMLayoutWindow & view);
};
