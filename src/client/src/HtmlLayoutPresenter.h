#pragma once

class IAboutView;
class ICredentialsView;
class IEditorView;
class IHtmlDataLoader;
class INoteView;
class INoteListView;

class HTMLayoutWindow;

class HtmlLayoutPresenter
{
private:

	HTMLayoutWindow & aboutView;
	HTMLayoutWindow & credentialsView;
	HTMLayoutWindow & editorView;
	HTMLayoutWindow & noteView;
	HTMLayoutWindow & noteListView;
	IHtmlDataLoader & htmlDataLoader;

public:

	HtmlLayoutPresenter
		( HTMLayoutWindow & aboutView
		, HTMLayoutWindow & credentialsView
		, HTMLayoutWindow & editorView
		, HTMLayoutWindow & noteView
		, HTMLayoutWindow & noteListView
		, IHtmlDataLoader & htmlDataLoader
		);

private:

	void OnLoadHtmlData(HTMLayoutWindow & view);
};
