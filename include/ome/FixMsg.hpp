#pragma once

#include <string_view>

class FixField
{
    public:
        int tag() const;
        std::string_view value() const;
};

class FixGroup
{
    private:
        std::vector<FixField>   _fields;
    public:
        FixField get(int tag) const;
        FixGroup group(int countTag, size_t index) const;
};

class FixMessage
{
    private:
        std::vector<Fix
    public:
        FixField get(int tag) const;
        FixGroup group(int countTag, size_t index) const;
};