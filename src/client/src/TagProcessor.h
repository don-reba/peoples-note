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

	void Add(const Tag & remote);

	void Delete(const Tag & local);

	void RenameAdd
		( const Tag & local
		, const Tag & remote
		);

	void Upload
		( const Tag  & local
		, INoteStore & noteStore
		);

	void Merge
		( const Tag & local
		, const Tag & remote
		);
};
