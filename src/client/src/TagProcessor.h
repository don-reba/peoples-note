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

	void Create
		( const Tag  & local
		, INoteStore & noteStore
		);

	void Delete(const Tag & local);

	void Merge
		( const Tag & local
		, const Tag & remote
		);

	void RenameAdd
		( const Tag & local
		, const Tag & remote
		);

	void Update
		( const Tag  & local
		, INoteStore & noteStore
		);
};
