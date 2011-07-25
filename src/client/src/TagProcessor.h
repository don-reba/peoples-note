#pragma once

class INoteStore;
class IUserModel;
class Tag;

class TagProcessor
{
private:

	IUserModel & userModel;

public:

	TagProcessor(IUserModel & userModel);

	void AddLocal(const Tag & remote);

	void CreateRemote
		( const Tag  & local
		, INoteStore & noteStore
		);

	void DeleteLocal(const Tag & local);

	void MergeLocal
		( const Tag & local
		, const Tag & remote
		);

	void UpdateRemote
		( const Tag  & local
		, INoteStore & noteStore
		);
};
