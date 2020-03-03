#pragma once

#define ASSERT(expression, message) \
if(!expression)\
	throw message