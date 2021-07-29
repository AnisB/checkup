// SDK includes
#include <bento_base/security.h>
#include <bento_memory/common.h>
#include <bento_memory/system_allocator.h>
#include <bento_collection/dynamic_string.h>

int main()
{
	bento::SystemAllocator allocator;
	{
		bento::DynamicString str(allocator);
		str = "MM_bob_MM_bob_MMM_bob_M_bob";
		bento::Vector<uint32_t> occurences(allocator);
		bento::string::find_all_occurences(str.c_str(), str.size(), "MM", 2, occurences);
		assert(occurences.size() == 3);
		assert(occurences[0] == 0);
		assert(occurences[1] == 7);
		assert(occurences[2] == 14);
	}

	{
		bento::DynamicString source(allocator);
		source = "C:\\Bijour\\Je\\Mappelle\\Robert";
		bento::DynamicString destination(allocator);
		bento::string::replace_substring(source, "\\", "//", destination);
		assert(destination == "C://Bijour//Je//Mappelle//Robert");
	}

	{
		bento::DynamicString source(allocator);
		source = "C:\\Bijour\\Je\\Mappelle\\Robert";
		bento::DynamicString destination(allocator);
		bento::string::replace_substring(source, "\\", "/", destination);
		assert(destination == "C:/Bijour/Je/Mappelle/Robert");
	}

	{
		bento::DynamicString source(allocator);
		source = "C:_Bijour_Je_Mappelle_Robert";
		bento::DynamicString destination(allocator);
		bento::string::replace_substring(source, "_", "//", destination);
		assert(destination == "C://Bijour//Je//Mappelle//Robert");
	}

	{
		bento::DynamicString source(allocator);
		source = "C:\\Bijour\\Je\\Mappelle\\";
		bento::DynamicString destination(allocator);
		bento::string::replace_substring(source, "\\", "/", destination);
		assert(destination == "C:/Bijour/Je/Mappelle/");
	}

	{
		bento::DynamicString source(allocator);
		source = "C:\\\Bijour\\Je\\Mappelle\\";
		bento::DynamicString destination(allocator);
		bento::string::replace_substring(source, "\\", "/", destination);
		assert(destination == "C:/\Bijour/Je/Mappelle/");
	}

	return 0;
}