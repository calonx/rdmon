#pragma once

#include <string>

struct Datagram
{
	std::string msg;

	template <class Archive>
	void save(Archive & ar) const
	{
		ar(msg);
	}

	template <class Archive>
	void load(Archive & ar)
	{
		ar(msg);
	}
};