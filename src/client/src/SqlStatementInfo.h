#pragma once

class SqlStatementInfo
{
public:

	SqlStatementInfo()
		: PreparationTime  (0.0f)
		, ExecutionTime    (0.0f)
		, FinalizationTime (0.0f)
		, UseCount         (0)
	{
	}

	float PreparationTime;
	float ExecutionTime;
	float FinalizationTime;
	int   UseCount;
};
