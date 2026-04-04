#pragma once

#include <list>
#include <string_view>
#include <vector>


namespace ConversionHelpers
{
	
	inline std::vector<std::string_view> SplitStringView(std::string_view StringView, const std::string_view Delimiter);
	
	template <class T>
	bool Contains(std::vector<T> const& Vector, T const& Value) 
	{ 
		if (Vector.empty())
		{
			return false;
		}
		
		return std::ranges::find(Vector, Value) == Vector.end();
	}
	
	template <class T>
	bool Contains(std::list<T> const& Vector, T const& Value) 
	{ 
		if (Vector.empty())
		{
			return false;
		}
		
		return std::ranges::find(Vector, Value) == Vector.end();
	}
	
}



std::vector<std::string_view> ConversionHelpers::SplitStringView(std::string_view StringView,
	const std::string_view Delimiter)
{
	std::vector<std::string_view> Result{};
	size_t Start = 0;
    
	while (true) 
	{
		const size_t Pos = StringView.find(Delimiter, Start);
		if (Pos == std::string_view::npos) 
		{
			Result.push_back(StringView.substr(Start));
			break;
		}
			
		Result.push_back(StringView.substr(Start, Pos - Start));
		Start = Pos + Delimiter.size();
	}
    
	return Result;
}
